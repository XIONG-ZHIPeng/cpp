#include <mutex>
#include <condition_variable>
#include <string>
#include <functional>

namespace sylar  // 命名空间sylar，避免与其他库或项目的命名冲突
{
// 用于线程方法的同步
class Semaphore{
    private:
        std::mutex mtx;
        std::condition_variable cv;
        int count;
    public:
        // 构造函数，初始计数值为0，显示转换防止把整数转换成为Semaphore对象
        // 可以通过传入初始计数值来设置信号量的初始
        explicit Semaphore(int initial_count = 0) : count(initial_count) {}

        // P操作（等待操作）
        // 当计数为0时，线程将阻塞，直到有其他线程调用
        void wait() {
            std::unique_lock<std::mutex> lock(mtx);
            while (count == 0){
                cv.wait(lock);
            }

            --count;
        }

        // V操作（信号操作）
        // 增加计数值，并通知一个等待的线程（如果有的话）
        void signal() {
            std::unique_lock<std::mutex> lock(mtx);
            ++count;
            cv.notify_one();
        }
};

class Thread{
    public:
        Thread(std::function<void()> cb, const std::string& name);
        ~Thread();

        pid_t getId() const {return m_id;}
        const std::string& getName() const {return m_name;}

        void join();

        // 获取当前系统分配的线程ID
        static pid_t GetThreadId();
        // 获取当前线程的Thread对象
        static Thread* GetThis();
        // 获取当前线程的名称
        static const std::string& GetName();
        // 设置当前线程的名称
        static void SetName(const std::string& name);

    private:
        // 线程入口函数
        static void* run(void* arg);

        pid_t m_id = -1;  // 线程ID
        pthread_t m_thread = 0; // 线程句柄

        std::function<void()> m_cb; // 线程执行的回调函数
        std::string m_name; // 线程名称

        Semaphore m_semaphore; // 用于线程同步的信号量
};

}