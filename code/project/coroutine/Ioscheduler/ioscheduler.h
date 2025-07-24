#ifndef _IOMANGER_H_
#define _IOMANGER_H_

#include "Scheduler/scheduler.h"
#include "Timer/timer.h"

namespace sylar {
// workflow
// 1. register an event -> 2. wait for it to ready -> 3. schedule the callback -> 4. unregister the event -> 5. run the callback

class IOManager : public Scheduler, public TimerManager
{

public:

enum Event
{
    NONE = 0x0, // 无事件
    READ = 0x1, // 可读事件
    WRITE = 0x4 // 可写事件
};

// threads 指定线程池数量，use_caller指定是否将主线程作为工作线程,name调度器名称
IOManager(size_t threads = 1, bool use_caller = true, const std::string & name = "IOManager");
// 析构函数
virtual ~IOManager();

// add an event to the IOManager
// fd 文件描述符，event 事件类型，cb 回调函数
int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
// delete event
bool delEvent(int fd, Event event);
// cancel event and trigger the callback
bool cancelEvent(int fd, Event event);
// cancel all events and trigger the callback
bool cancelAll(int fd);

static IOManager* GetThis();


protected:
// 通知调度器有任务
virtual void tickle() override;

// 判断是否可以停止
virtual bool stopping() override;

// idle 协程负责收集已经触发的事件并将其加入到任务队列中
// 在idle协程退出后，调度器调度执行
virtual void idle() override;

virtual void onTimerInsertedAtFront() override;

void contextResize(size_t size);// 调整FdContext数组大小


private:

// 描述一个文件描述符的事件
struct FdContext 
{
    struct EventContext
    {
        // scheduler
        Scheduler* scheduler = nullptr; // 调度器
        // callback fiber
        std::shared_ptr<Fiber> fiber; // 回调协程
        // callback function
        std::function<void()> cb; // 回调函数
    };

    // read event context
    EventContext read; // 可读事件上下文
    // write event context
    EventContext write; // 可写事件上下文
    int fd = 0; // 文件描述符
    Event events = NONE; // 当前事件
    std::mutex mutex; // 互斥锁，保护事件上下文
    EventContext& getEventContext(Event event); // 获取事件上下文
    void resetEventContext(EventContext& ctx); // 重置事件上下文
    void triggerEvent(Event event); // 触发事件
};

int m_epfd = 0; // epoll文件描述符
// fd[0] read, fd[1] write
int m_tickleFds[2]; // 用于线程间通信的管道文件描述符
std::atomic<size_t> m_pendingEventCount{0}; // 待处理事件数量
std::shared_mutex m_mutex; // 互斥锁，保护FdContext数组
std::vector<FdContext*> m_fdContexts; // 文件描述符上下文数组

};


}

#endif