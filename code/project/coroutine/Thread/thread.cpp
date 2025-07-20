#include "thread.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <iostream>

/*
两种方式给线程设置名称：
1. 使用构造函数传入名称
2. 使用SetName静态方法设置名称

Thread类中构造函数有一个std::function<void()>类型的回调函数cb
通过pthread_create创建线程时，将run函数作为线程入口，并使用Semaphore来同步线程的执行。
*/ 

namespace sylar{ // 命名空间sylar，避免与其他库或项目的命名冲突
    // 线程局部存储，保存当前线程的Thread对象和名称
    static thread_local Thread* t_thread = nullptr; // 当前线程的Thread对象
    static thread_local std::string t_thread_name = "UNKNOW"; // 当前线程的名称

    pid_t Thread::GetThreadId() {
        return syscall(SYS_gettid); // 获取当前线程的系统分配ID
    }

    Thread* Thread::GetThis() {
        return t_thread; // 返回当前线程的Thread对象
    }

    const std::string& Thread::GetName() {
        return t_thread_name; // 返回当前线程的名称
    }

    void Thread::SetName(const std::string& name) {
        if (t_thread) {
            t_thread->m_name = name; // 设置当前线程的名称
        }
        t_thread_name = name; // 更新线程局部存储的名称
    }

    Thread::Thread(std::function<void()> cb, const std::string& name):
        m_cb(cb), m_name(name) {
        int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
        if (rt) {
            std::cerr << "pthread_create error: " << rt << " thread name: " << m_name << std::endl;
            throw std::runtime_error("pthread_create error");
        }

        m_semaphore.wait(); // 等待线程完成初始化
    }

    Thread::~Thread() {
        if (m_thread) {
            pthread_detach(m_thread); // 分离线程
            m_thread = 0;
        }
    }


    void Thread::join() {
        if (m_thread) {
            int rt = pthread_join(m_thread, nullptr);
            if (rt) {
                std::cerr << "pthread_join error: " << rt << " thread name: "
                            << m_name << std::endl;
                throw std::runtime_error("pthread_join error");
                }
            m_thread = 0; // 清除线程句柄
        }   
    }

    void* Thread::run(void*arg){ // void* arg 的设计是为了让线程函数能够处理任意类型的参数
        Thread* thread = static_cast<Thread*>(arg);

        t_thread = thread; // 设置当前线程的Thread对象
        t_thread_name = thread->m_name; // 设置当前线程的名称

        thread->m_id = GetThreadId(); // 获取当前线程的ID
        pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str()); // 设置线程名称, linux下最多15个字符

        std::function<void()> cb;
        cb.swap(thread->m_cb); // 交换回调函数

        thread->m_semaphore.signal(); // 通知主线程，线程已准备就绪

        cb(); // 执行线程的回调函数

        return nullptr; // 线程执行完毕
    }

}


