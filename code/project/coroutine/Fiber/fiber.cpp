#include "fiber.h"

static bool debug = false;

namespace sylar {

    // 当前线程上的协程控制信息

    //线程局部变量 正在运行的协程
    static thread_local Fiber* t_fiber = nullptr;
    //线程局部变量 主协程
    static thread_local std::shared_ptr<Fiber> t_thread_fiber = nullptr;
    // 线程局部变量 调度器协程
    static thread_local Fiber* t_scheduler_fiber = nullptr;

    // 全局变量 协程ID计数器
    static std::atomic<uint64_t> s_fiber_id{0};
    // 全局变量 活跃协程数量计数器
    static std::atomic<uint64_t> s_fiber_count{0};

    // 设置当前运行的协程
    void Fiber::SetThis(Fiber* f) {
        t_fiber = f;
    }

    // 获取当前运行的协程
    std::shared_ptr<Fiber> Fiber::GetThis() {
        if (t_fiber){
            return t_fiber->shared_from_this();
        }

        // 如果当前线程没有协程，则创建一个主协程
        std::shared_ptr<Fiber> main_fiber(new Fiber());
        t_thread_fiber = main_fiber;
        t_scheduler_fiber = main_fiber.get();// 默认设置调度器协程为主协程
        
        // 用于判断当前运行的协程t_fiber是否为主协程，不是则程序终止
        assert(t_fiber == main_fiber.get());

        return t_fiber->shared_from_this();
    }   

    // 设置当前的调度器协程
    void Fiber::SetSchedulerFiber(Fiber* f) {
        t_scheduler_fiber = f;
    }

    // 获取当前运行的协程的ID
    uint64_t Fiber::GetFiberId(){
        if (t_fiber) {
            return t_fiber->m_id;
        }
        return (uint64_t)-1;
    }

    // 创建主协程，设置状态，初始化上下文并分配ID
    Fiber::Fiber(){
        // 把当前协程设置为主协程
        SetThis(this);
        m_state = RUNNING; // 主协程状态为运行中

        if (getcontext(&m_ctx) == -1) {
            std::cerr << "getcontext error" << std::endl;
            pthread_exit(NULL); // 获取上下文失败，退出线程
        }

        m_id = s_fiber_id++; // 分配一个唯一的协程ID
        s_fiber_count++; // 活跃协程数量加1
        if (debug) {
            std::cout << "Fiber::Fiber() main id=" << m_id << std::endl;
        }
    }

    //创建一个新协程，初始化状态、上下文、栈大小和回调函数。分配栈空间，并通过makecontext设置上下文。
    Fiber::Fiber(std::function<void()> cb, size_t stack_size, bool run_in_scheduler)
        : m_run_in_scheduler(run_in_scheduler), m_cb(cb){
            m_state = READY; // 初始状态为就绪

            // 分配协程栈空间
            m_stack_size = stack_size ? stack_size : 128 * 1024; // 默认栈大小为128KB
            m_stack = malloc(m_stack_size);

            if (getcontext(&m_ctx) == -1) {
                std::cerr << "getcontext error" << std::endl;
                pthread_exit(NULL); // 获取上下文失败，退出线程
            }

            m_ctx.uc_link = nullptr; // 执行完返回主协程，因此设置下一个上下文链接为nullptr
            m_ctx.uc_stack.ss_sp = m_stack; // 设置协程栈指针
            m_ctx.uc_stack.ss_size = m_stack_size; // 设置协程栈大小
            makecontext(&m_ctx, &Fiber::MainFunc, 0); // 设置协程入口函数为MainFunc

            m_id = s_fiber_id++; // 分配一个唯一的协程ID
            s_fiber_count++; // 活跃协程数量加1
            if (debug) {
                std::cout << "Fiber::Fiber() child id=" << m_id << std::endl;
            }
        }


    Fiber::~Fiber(){
        s_fiber_count--;
        if (m_stack) {
            assert(m_state == TERM);
            free(m_stack);
        }
        else{
            assert(m_state == RUNNING);
            Fiber *cur = t_fiber;
            if (cur == this) {
                SetThis(nullptr);
            }
        }
    }

    // 重置协程状态和入口函数
    void Fiber::reset(std::function<void()> cb) {
        assert(m_stack != nullptr && m_state == TERM); // 确保协程栈已分配且状态为终止

        m_state = READY; // 重置状态为就绪
        m_cb = cb; // 设置新的回调函数

        if (getcontext(&m_ctx) == -1) {
            std::cerr << "reset() failed" << std::endl;
            pthread_exit(NULL); // 重置上下文失败，退出线程
        }

        m_ctx.uc_link = nullptr; // 执行完返回主协程，因此设置下一个上下文链接为nullptr
        m_ctx.uc_stack.ss_sp = m_stack; // 设置协程栈指针
        m_ctx.uc_stack.ss_size = m_stack_size; // 设置协程栈大小
        makecontext(&m_ctx, &Fiber::MainFunc, 0); // 设置协
    }

    // 恢复协程执行
    void Fiber::resume() {
        assert(m_state == READY); // 确保协程状态为就绪

        m_state = RUNNING; // 设置状态为运行中

        if (m_run_in_scheduler) {
            SetThis(this); // 设置当前协程为正在运行的协程
            if (swapcontext(&t_scheduler_fiber->m_ctx, &m_ctx) == -1) {
                std::cerr << "swapcontext error" << std::endl;
                pthread_exit(NULL); // 切换上下文失败，退出线程
            }
        }   
        else{
            SetThis(this); // 设置当前协程为正在运行的协程
            if (swapcontext(&t_thread_fiber->m_ctx, &m_ctx) == -1) {
                std::cerr << "resume() to t_thread_fiber failed\n" << std::endl;
                pthread_exit(NULL); // 切换上下文失败，退出线程
            }
        }
    }

    // 将执行权归还给调度器
    void Fiber::yield() {
        assert(m_state == RUNNING || m_state == TERM); // 确保协程状态为运行中或终止

        if (m_state != TERM){
            m_state = READY; // 如果不是终止状态，则设置为就绪状态
        }

        if (m_run_in_scheduler){
            SetThis(t_scheduler_fiber); // 设置当前协程为调度器协程
            if (swapcontext(&m_ctx, &t_scheduler_fiber->m_ctx) == -1) {
                std::cerr << "yield() to t_scheduler_fiber failed\n" << std::endl;
                pthread_exit(NULL); // 切换上下文失败，退出线程
            }
        }
        else{
            SetThis(t_thread_fiber.get()); // 设置当前协程为主协程
            if (swapcontext(&m_ctx, &t_thread_fiber->m_ctx) == -1) {
                std::cerr << "yield() to t_thread_fiber failed\n" << std::endl;
                pthread_exit(NULL); // 切换上下文失败，退出线程
            }
        }
    }




    // 协程的主函数，入口点
    void Fiber::MainFunc(){
        std::shared_ptr<Fiber> cur = GetThis(); // 获取当前协程对象
        assert(cur!=nullptr); // 确保当前协程对象不为空

        cur->m_cb(); // 执行协程的回调函数
        cur->m_cb = nullptr; // 清空回调函数，避免内存泄漏
        cur->m_state = TERM; // 设置协程状态为终止


        // 协程执行完毕后，自动将执行权归还给调度器或主协程
        auto raw_ptr = cur.get(); // 获取当前协程的原始指针
        cur.reset(); // 重置当前协程对象，释放资源
        raw_ptr->yield(); // 将执行权归还给调度器或主协程
    }
}