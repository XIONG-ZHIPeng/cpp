#include "scheduler.h"

static bool debug = false;

namespace sylar {
// 设置正在运行的调度器
static thread_local Scheduler* t_scheduler = nullptr;

Scheduler* Scheduler::GetThis() {
    return t_scheduler;
}
// 设置调度器
void Scheduler::SetThis() {
    t_scheduler = this;
}

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string & name):
m_useCaller(use_caller), m_name(name){
    // 判断线程数是否大于0，以及当前调度器是否为空
    assert(threads > 0 && Scheduler::GetThis() == nullptr);

    SetThis();
    // 设置当前线程的名称为调度器名称
    Thread::SetName(m_name);

    // 使用主线程作为工作线程，创建协程的原因是为了实现高效任务调度和管理
    if (use_caller){
        threads--;

        // 创建主协程
        Fiber::GetThis();

        //创建调度协程, false -> 该调度协程退出后返回主协程
        m_schedulerFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, false));
        Fiber::SetSchedulerFiber(m_schedulerFiber.get());//设置协程调度器对象

        m_rootThreadId = Thread::GetThreadId(); // 获取主线程ID
        m_threadIds.push_back(m_rootThreadId); // 将主线程ID添加到线程ID列表中
    }

    m_threadCount = threads; // 设置需要创建的线程数量
    if (debug) std::cout << "Scheduler::Scheduler() threads=" << threads
              << " use_caller=" << use_caller
              << " name=" << m_name << std::endl;
}

Scheduler::~Scheduler(){
    assert(stopping()==true);
    if(GetThis() == this){
        t_scheduler = nullptr; // 清除当前调度器
    }
    if (debug) std::cout << "Scheduler::~Scheduler() name=" << m_name << std::endl;
}

//start函数是启动调度器的核心方法之一。负责初始化和启动调度器管理所有工作线程
void Scheduler::start(){
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stopping){
        std::cerr << "Scheduler is stopping, cannot start again." << std::endl;
        return; // 如果调度器已经停止，则直接返回
    }

    assert(m_threads.empty());// 确保线程池为空
    m_threads.resize(m_threadCount);
    // 创建线程池和启动工作线程
    for (size_t i = 0; i < m_threadCount; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }

    if (debug) {
        std::cout << "Scheduler::start() name=" << m_name
                  << " thread_count=" << m_threadCount
                  << " root_thread_id=" << m_rootThreadId
                  << " thread_ids=";
        for (const auto& id : m_threadIds) {
            std::cout << id << " ";
        }
        std::cout << std::endl;
    }

}

// 调度器核心，取出任务并通过协程执行
void Scheduler::run(){
    int thread_id = Thread::GetThreadId(); // 获取当前线程ID
    if (debug) std::cout << "Scheduler::run() thread_id=" << thread_id
              << " name=" << m_name << std::endl;

    SetThis(); // 设置当前调度器

    // 运行正在创建的线程
    if (thread_id != m_rootThreadId) {
        Fiber::GetThis();
    }

    // 创建空闲协程
    std::shared_ptr<Fiber> idle_fiber = std::make_shared<Fiber>(std::bind(&Scheduler::idle, this));
    ScheduleTask task;

    while (true){
        task.reset(); // 重置任务
        bool tickle_me = false;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_tasks.begin();
            // 查找任务队列中是否有任务
            while (it != m_tasks.end()) {
                if (it->thread != -1 && it->thread != thread_id) {
                    it++;
                    tickle_me = true; // 标记需要唤醒线程
                    continue; // 跳过当前任务
                }
                if (it->fiber && it->fiber->getState() != Fiber::READY) {
                    it = m_tasks.erase(it); // 如果协程已经终止，跳过当前任务
                    continue;
                }

                // 取出任务
                assert(it->fiber || it->cb);
                task = *it;
                m_tasks.erase(it); // 从任务队列中删除任务
                m_activeThreadCount++; // 活动线程数量加1
                break;
            }

            tickle_me = tickle_me || (it != m_tasks.end()); // 如果有任务被取出，则需要唤醒线程
        
        }

        if (tickle_me) {
            tickle(); // 唤醒一个线程
        }

        // 执行任务
        if (task.fiber){
            {
                std::lock_guard<std::mutex> lock(task.fiber->m_mutex);
                if (task.fiber->getState() != Fiber::TERM){
                    task.fiber->resume();
                }
            }
            m_activeThreadCount--; // 活动线程数量减1
            task.reset(); // 重置任务
        }
        else if(task.cb){
            std::shared_ptr<Fiber> cb_fiber = std::make_shared<Fiber>(task.cb);
            {
                std::lock_guard<std::mutex> lock(cb_fiber->m_mutex);
                cb_fiber->resume();
            }
            m_activeThreadCount--; // 活动线程数量减1
            task.reset(); // 重置任务
        }
        else{
            if (idle_fiber->getState() == Fiber::TERM){
                if (debug){
                    std::cout << "Scheduler::run() idle fiber is terminated, thread_id=" << thread_id
                              << " name=" << m_name << std::endl;
                }
                break;
            }

            m_idleThreadCount++; // 空闲线程数量加1
            idle_fiber->resume(); // 恢复空闲协程执行
            m_idleThreadCount--; // 空闲线程数量减1

            }
        }
    }

// 停止调度器
void Scheduler::stop(){
    if (debug) std::cout << "Scheduler::stop() Thread ID=" << Thread::GetThreadId() << std::endl;

    if (stopping()) {
        return; // 如果已经停止，则直接返回
    }

    m_stopping = true; // 设置停止标志

    if (m_useCaller){
        assert(GetThis() == this);
    }
    else{
        assert(GetThis() != this);
    }

    for (size_t i = 0; i < m_threadCount; ++i){
        tickle();
    }

    if (m_schedulerFiber){
        tickle();
    }

    if (m_schedulerFiber){
        m_schedulerFiber->resume(); // 恢复调度协程执行
        if (debug) std::cout << "m_schedulerFiber ends in thread: " << Thread::GetThreadId() << std::endl;
    }

    std::vector<std::shared_ptr<Thread>> threads;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        threads.swap(m_threads); // 交换线程池，清空当前线程池
    }

    for (auto& thread : threads) {
        if (thread) {
            thread->join(); // 等待所有线程结束
        }
    }

    if (debug) {
        std::cout << "Scheduler::stop() name=" << m_name
                  << " stopped, active_thread_count=" << m_activeThreadCount.load()
                  << " idle_thread_count=" << m_idleThreadCount.load() << std::endl;
    }
}

void Scheduler::tickle(){}

void Scheduler::idle() {
    while (!stopping()) {
        if (debug) std::cout << "Scheduler::idle(), sleeping in thread: "
                  << Thread::GetThreadId() << std::endl;

        sleep(1);
        Fiber::GetThis()->yield(); // 将执行权归还给调度器
    }
}

bool Scheduler::stopping() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_stopping && m_activeThreadCount == 0 && m_tasks.empty();
}


}