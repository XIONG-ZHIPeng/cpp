#ifndef _COROUTINE_H_
#define _COROUTINE_H_
#include <iostream>
#include <memory>
#include <functional>
#include <atomic>
#include <cassert>
#include <ucontext.h>
#include <unistd.h>
#include <mutex>

namespace sylar{
    class Fiber: public std::enable_shared_from_this<Fiber> {
        public:
            enum State{
                READY,
                RUNNING,
                TERM
            };

            std::mutex m_mutex; // 互斥锁，用于保护协程状态的修改
            //创建指定回调函数、栈大小和run_in_scheduler标志本协程是否参与调度器调度的协程对象
            Fiber(std::function<void()> cb, size_t stack_size = 0, bool run_in_scheduler = true);
            ~Fiber();

            //重置协程状态和入口函数
            void reset(std::function<void()> cb);
            //恢复协程执行
            void resume();
            //将执行权归还给调度器
            void yield();
            //获取唯一标识
            uint64_t getId() const { return m_id; }
            State getState() const { return m_state; }

            //设置当前运行的协程
            static void SetThis(Fiber *f);
            //获取当前运行的协程
            static std::shared_ptr<Fiber> GetThis();
            //设置调度协程
            static void SetSchedulerFiber(Fiber *f);
            //获取当前运行的协程ID
            static uint64_t GetFiberId();
            //协程的主函数，入口点
            static void MainFunc();
        private:
            Fiber();// Fiber::Fiber()是私有的，不能直接创建Fiber对象，只能通过GetThis()方法获取当前协程对象，用于创建主协程

            uint64_t m_id; // 协程唯一标识
            uint32_t m_stack_size; // 协程栈大小
            State m_state = READY; // 协程状态
            ucontext_t m_ctx; // 协程上下文
            void* m_stack = nullptr; // 协程栈指针
            std::function<void()> m_cb; // 协程执行的回调函数
            bool m_run_in_scheduler; // 是否参与调度器调度
        };
}




#endif