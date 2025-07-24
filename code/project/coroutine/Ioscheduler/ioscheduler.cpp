#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>

#include "ioscheduler.h"

static bool debug = true;

namespace sylar {

IOManager* IOManager::GetThis() 
{
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}

IOManager::FdContext::EventContext& IOManager::FdContext::getEventContext(Event event)
{
    assert(event == READ || event == WRITE);
    switch (event)
    {
        case READ:
            return read;
        case WRITE:
            return write;
        default:
            throw std::invalid_argument("Invalid event type");
    }
}

void IOManager::FdContext::resetEventContext(EventContext& ctx)
{
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(Event event)
{
    // 触发事件,判断事件是否存在
    assert(events & event);

    // delete event
    events = static_cast<Event>(events & ~event);

    // trigger the callback
    EventContext& ctx = getEventContext(event);
    if (ctx.cb)
    {
        // 如果有回调函数，则执行回调函数
        ctx.scheduler->scheduleLock(&ctx.cb);
    }
    else
    {
        ctx.scheduler->scheduleLock(&ctx.fiber);
    }

    // 重置事件上下文
    resetEventContext(ctx);

}


IOManager::IOManager(size_t threads, bool use_caller, const std::string & name):
Scheduler(threads, use_caller, name),TimerManager()
{
    // create epoll file descriptor

    m_epfd = epoll_create(5000);
    assert(m_epfd > 0);

    // create pipe for tickle
    int rt = pipe(m_tickleFds);
    assert(rt == 0);

    // add read event to epoll
    epoll_event event;
    event.events = EPOLLIN | EPOLLET; // Edge Triggered
    event.data.fd = m_tickleFds[0]; // read end of the pipe

    // 设置非阻塞模式
    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    assert(rt == 0);

    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    assert(rt == 0);

    // 初始化FdContext数组
    contextResize(32); // 初始大小为32

    start(); // 启动调度器

}

IOManager::~IOManager() 
{
    stop(); // 停止调度器
    close(m_epfd); // 关闭epoll文件描述符
    close(m_tickleFds[0]); // 关闭管道读端
    close(m_tickleFds[1]); // 关闭管道写端

    for (auto& fd_ctx : m_fdContexts) {
        delete fd_ctx; // 释放FdContext数组
    }
    m_fdContexts.clear(); // 清空FdContext数组
}

// 调整FdContext数组大小
void IOManager::contextResize(size_t size)
{
    m_fdContexts.resize(size);

    for (size_t i = 0; i < size; ++i) {
        if (!m_fdContexts[i]) {
            m_fdContexts[i] = new FdContext();
            m_fdContexts[i]->fd = i; // 设置文件描述符
        }
    }
}
    
int IOManager::addEvent(int fd, Event event, std::function<void()> cb)
{
    // 检查文件描述符是否有效
    FdContext* fd_ctx = nullptr;

    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    if ((int)m_fdContexts.size() > fd)
    {
        fd_ctx = m_fdContexts[fd];
        read_lock.unlock();
    }
    else
    {
        read_lock.unlock();
        std::unique_lock<std::shared_mutex> write_lock(m_mutex);
        contextResize(fd * 1.5); // 扩展FdContext数组
        fd_ctx = m_fdContexts[fd];
    }

    std::lock_guard<std::mutex> lock(fd_ctx->mutex); // 锁定FdContext

    // 检查事件是否已经存在
    if (fd_ctx->events & event) 
    {
        if (debug) {
            std::cout << "IOManager::addEvent() fd=" << fd << " event=" << event
                      << " already exists, ignoring." << std::endl;
        }
        return -1; // 事件已存在，返回错误
    }

    // 设置事件
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->events | event; // Edge Triggered
    epevent.data.ptr = fd_ctx; // 使用FdContext指针作为数据

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt)
    {
        std::cerr << "IOManager::addEvent() epoll_ctl failed, fd=" << fd 
                  << ", event=" << event << ", error=" << strerror(errno) << std::endl;
        return -1; // 添加事件失败
    }

    ++m_pendingEventCount; // 增加待处理事件数量

    // 更新FdContext
    fd_ctx->events = static_cast<Event>(fd_ctx->events | event);

    // update fdcontext
    FdContext::EventContext& event_ctx = fd_ctx->getEventContext(event);
    assert(!event_ctx.scheduler && !event_ctx.fiber && !event_ctx.cb);
    event_ctx.scheduler =  Scheduler::GetThis(); // 设置调度器
    if (cb) 
    {
        event_ctx.cb.swap(cb); // 设置回调函数
    }
    else
    {
        event_ctx.fiber = Fiber::GetThis(); // 设置回调协程
        assert(event_ctx.fiber->getState() == Fiber::RUNNING);
    }

    return 0; // 成功添加事件


}

bool IOManager::delEvent(int fd, Event event)
{
    // attempt to find FdContext
    FdContext* fd_ctx = nullptr;

    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    if ((int)m_fdContexts.size() > fd)
    {
        fd_ctx = m_fdContexts[fd];
        read_lock.unlock();
    
    }
    else
    {
        read_lock.unlock();
        return false; // 文件描述符无效
    }

    std::lock_guard<std::mutex> lock(fd_ctx->mutex); // 锁定FdContext

    // 检查事件是否存在
    if (!(fd_ctx->events & event))
    {
        if (debug) {
            std::cout << "IOManager::delEvent() fd=" << fd << " event=" << event
                      << " does not exist, ignoring." << std::endl;
        }
        return false; // 事件不存在，返回false
    }

    // 删除事件
    Event new_events = static_cast<Event>(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL; // 如果没有其他事件，则删除
    epoll_event epevent;
    epevent.events = EPOLLET | new_events; // Edge Triggered
    epevent.data.ptr = fd_ctx; // 使用FdContext指针作为数据

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt)
    {
        std::cerr << "IOManager::delEvent() epoll_ctl failed, fd=" << fd 
                  << ", event=" << event << ", error=" << strerror(errno) << std::endl;
        return false; // 删除事件失败
    }

    --m_pendingEventCount; // 减少待处理事件数量

    // 更新FdContext
    fd_ctx->events = new_events;

    // 重置事件上下文
    FdContext::EventContext& event_ctx = fd_ctx->getEventContext(event);
    fd_ctx->resetEventContext(event_ctx);

    return true; // 成功删除事件
}

bool IOManager::cancelEvent(int fd, Event event)
{
    // attempt to find FdContext
    FdContext* fd_ctx = nullptr;

    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    if ((int)m_fdContexts.size() > fd)
    {
        fd_ctx = m_fdContexts[fd];
        read_lock.unlock();
    
    }
    else
    {
        read_lock.unlock();
        return false; // 文件描述符无效
    }

    std::lock_guard<std::mutex> lock(fd_ctx->mutex); // 锁定FdContext

    // 检查事件是否存在
    if (!(fd_ctx->events & event))
    {
        if (debug) {
            std::cout << "IOManager::cancelEvent() fd=" << fd << " event=" << event
                      << " does not exist, ignoring." << std::endl;
        }
        return false; // 事件不存在，返回false
    }

    
    // 删除事件
    Event new_events = static_cast<Event>(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL; // 如果没有其他事件，则删除
    epoll_event epevent;
    epevent.events = EPOLLET | new_events; // Edge Triggered
    epevent.data.ptr = fd_ctx; // 使用FdContext指针作为数据

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt)
    {
        std::cerr << "IOManager::cancelEvent() epoll_ctl failed, fd=" << fd 
                  << ", event=" << event << ", error=" << strerror(errno) << std::endl;
        return false; // 删除事件失败
    }

    --m_pendingEventCount; // 减少待处理事件数量
    // update FdContext
    fd_ctx->triggerEvent(event); // 触发事件

    return true; // 成功取消事件
}

bool IOManager::cancelAll(int fd)
{
    // attempt to find FdContext
    FdContext* fd_ctx = nullptr;

    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    if ((int)m_fdContexts.size() > fd)
    {
        fd_ctx = m_fdContexts[fd];
        read_lock.unlock();
    
    }
    else
    {
        read_lock.unlock();
        return false; // 文件描述符无效
    }

    std::lock_guard<std::mutex> lock(fd_ctx->mutex); // 锁定FdContext

    // 检查是否有事件
    if (!fd_ctx->events)
    {
        if (debug) {
            std::cout << "IOManager::cancelAll() fd=" << fd << " has no events, ignoring." << std::endl;
        }
        return false; // 没有事件，返回false
    }

    // 删除所有事件
    int op = EPOLL_CTL_DEL; 
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_ctx; // 使用FdContext指针作为数据

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt)
    {
        std::cerr << "IOManager::cancelAll() epoll_ctl failed, fd=" << fd 
                  << ", error=" << strerror(errno) << std::endl;
        return false; // 删除事件失败
    }

    // 更新FdContext
    if (fd_ctx->events & READ) {
        fd_ctx->triggerEvent(READ); // 触发可读事件
        --m_pendingEventCount; // 减少待处理事件数量
    }

    if (fd_ctx->events & WRITE) {
        fd_ctx->triggerEvent(WRITE); // 触发可写事件
        --m_pendingEventCount; // 减少待处理事件数量
    }

    assert(fd_ctx->events == 0); // 确保事件已清空
    return true;
}

void IOManager::tickle() 
{
    // no idle thread
    if (!hasIdleThreads())
    {
        return;
    }
    // write to the pipe to wake up the idle thread
    int rt = write(m_tickleFds[1], "T", 1);
    assert(rt == 1);
}

bool IOManager::stopping() 
{
    uint64_t timeout = getNextTimer();

    return timeout == ~0ull && m_pendingEventCount == 0 && Scheduler::stopping();
}

void IOManager::idle() 
{
    static uint64_t MAX_EVENTS = 256; // 最大事件数
    std::unique_ptr<epoll_event[]> events(new epoll_event[MAX_EVENTS]);

    while (true)
    {
        if (debug)
        {
            std::cout << "IOManager::idle(), run in thread: " << Thread::GetThreadId() << std::endl;
        }

        if (stopping())
        {
            if (debug)
            {
                std::cout << "name = " << getName() 
                          << ", idle exits in thread: " << Thread::GetThreadId() << std::endl;
            }
            break; // 退出idle协程
        }

        // block until there are events or timeout
        int rt = 0;
        while (true)
        {
            static const uint64_t MAX_TIMEOUT = 5000; // 最大超时时间
            uint64_t next_timeout = getNextTimer();
            next_timeout = std::min(next_timeout, MAX_TIMEOUT);

            rt = epoll_wait(m_epfd, events.get(), MAX_EVENTS, (int)next_timeout);

            // EINTR -> interrupted by a signal
            if (rt < 0 && errno == EINTR) 
            {
                continue; // 继续等待
            }
            else
            {
                break; // 成功获取事件或超时
            }

        };

        // collect all timers overdue
        std::vector<std::function<void()>> cbs;
        listExpiredCb(cbs);

        if (!cbs.empty())
        {
            for (const auto& cb : cbs)
            {
                for (const auto& cb : cbs)
                {
                    if (cb)
                    {
                        scheduleLock(cb); // 调度执行超时回调函数
                    }
                }
                cbs.clear(); // 清空回调函数列表
            }
    
        }

        // collext all events ready
        for (int i = 0; i < rt; ++i)
        {
            epoll_event& event = events[i];

            // tickle event
            if (event.data.fd == m_tickleFds[0])
            {
                uint8_t dummy[256];
                // edge triggered, read until the pipe is empty
                while (read(m_tickleFds[0], dummy, sizeof(dummy)) > 0);
                continue; // 继续处理其他事件 
            }

            // other events
            FdContext* fd_ctx = static_cast<FdContext*>(event.data.ptr);
            std::lock_guard<std::mutex> lock(fd_ctx->mutex); // 锁定FdContext

            // convert EPOLLERR or EPOLLHUP to READ or WRITE
            if (event.events & (EPOLLERR | EPOLLHUP))
            {
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events; // 将错误或挂起事件转换为可读或可写事件
            }

            // events happening during the wait
            int real_events = NONE;
            if (event.events & EPOLLIN)
            {
                real_events |= READ; // 可读事件
            }

            if (event.events & EPOLLOUT)
            {
                real_events |= WRITE; // 可写事件
            }

            if ((fd_ctx->events & real_events) == NONE)
            {
                if (debug) {
                    std::cout << "IOManager::idle() fd=" << fd_ctx->fd 
                              << " real_events=" << real_events 
                              << " does not match fd_ctx events=" << fd_ctx->events 
                              << ", ignoring." << std::endl;
                }
                continue; // 事件不匹配，忽略
            }

            // delete the events that have been processed
            int left_events = static_cast<Event>(fd_ctx->events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL; // 如果还有其他事件，则修改，否则删除
            event.events = EPOLLET | left_events; // Edge Triggered

            int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if (rt2)
            {
                std::cerr << "IOManager::idle() epoll_ctl failed, fd=" << fd_ctx->fd 
                          << ", error=" << strerror(errno) << std::endl;
                continue; // 处理失败，继续下一个事件
            }

            // schedule the callback and update FdContext and event context
            if (real_events & READ)
            {
                fd_ctx->triggerEvent(READ); // 触发可读事件
                --m_pendingEventCount; // 减少待处理事件数量
            }

            if (real_events & WRITE)
            {
                fd_ctx->triggerEvent(WRITE); // 触发可写事件
                --m_pendingEventCount; // 减少待处理事件数量
            }
        }

        Fiber::GetThis()->yield(); // 将执行权归还给调度器
    }

        
}

void IOManager::onTimerInsertedAtFront() 
{
    tickle(); // 通知调度器有任务
}

}