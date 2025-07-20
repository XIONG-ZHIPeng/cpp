#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "Fiber/fiber.h"
#include "Thread/thread.h"

#include <mutex>
#include <vector>

namespace sylar {

class Scheduler{
public:
    // threads指定线程池数量，use_caller指定是否将主线程作为工作线程,name调度器名称
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name="Scheduler");
    // 析构函数 防止出现内存泄漏，基类指针删除派生类对象问题
    virtual ~Scheduler();

    // 获取调度器名称
    const std::string& getName() const { return m_name; }

    // 获取当前调度器
    static Scheduler* GetThis();


    // 添加任务到任务队列
    // FiberOrCb可以是协程或回调函数
    template<class FiberOrCb>
    void scheduleLock(FiberOrCb fc, int thread = -1){
        // 标记任务队列是否为空，判断是否需要唤醒线程
        bool need_tickle;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            // empty -> all threads are idle -> need to wake up one thread
            need_tickle = m_tasks.empty();
            // 创建Task对象
            ScheduleTask task(fc, thread);
            if (task.fiber || task.cb){
                m_tasks.push_back(task);
            }
        }

        // 如果任务队列之前为空，则需要唤醒一个线程
        if (need_tickle){
            tickle();
        }
    }
    // 启动线程池，启动调度器
    virtual void start();
    // 停止线程池，停止调度器
    virtual void stop();

protected:
    // 设置正在运行的调度器
    void SetThis();

    // 唤醒一个线程
    virtual void tickle();

    // 线程函数
    virtual void run();

    // 空闲线程函数
    virtual void idle();

    // 是否可以停止
    virtual bool stopping();

    // 返回是否有空闲线程
    // 当调度协程进入idle时空闲线程数+1，从idle返回时空闲线程数-1
    bool hasIdleThreads() const {
        return m_idleThreadCount > 0;
    }

private:
    // 任务
    struct ScheduleTask {
        std::shared_ptr<Fiber> fiber; // 协程
        std::function<void()> cb; // 回调函数
        int thread; // 指定运行任务需要运行的线程ID

        ScheduleTask():fiber(nullptr), cb(nullptr), thread(-1) {}

        ScheduleTask(std::shared_ptr<Fiber> f, int thr)
            : fiber(f), thread(thr) {}

        ScheduleTask(std::shared_ptr<Fiber>* f, int thr){
            fiber.swap(*f);
            thread = thr;
        }

        ScheduleTask(std::function<void()> c, int thr)
            : cb(c), thread(thr) {}

        ScheduleTask(std::function<void()>* c, int thr){
            cb.swap(*c);
            thread = thr;
        }

        void reset(){
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

    std::string m_name; // 调度器名称
    std::mutex m_mutex; // 互斥锁，保护任务队列
    std::vector<std::shared_ptr<Thread>> m_threads; // 线程池
    std::vector<ScheduleTask> m_tasks; // 任务队列
    std::vector<int> m_threadIds; // 线程ID列表
    size_t m_threadCount=0; // 需要另外创建线程数量
    std::atomic<size_t> m_activeThreadCount{0}; // 活动线程数量
    std::atomic<size_t> m_idleThreadCount{0}; // 空闲线程数量
    bool m_useCaller; // 是否使用主线程作为工作线程，参与调度
    // 如果是 -> 需要另外创建调度协程
    std::shared_ptr<Fiber> m_schedulerFiber; // 调度协程
    // 如果是 -> 记录主线程的线程id
    int m_rootThreadId = -1; // 主线程ID
    // 是否关闭
    bool m_stopping = false; // 是否停止调度器



};

}


#endif