#ifndef __TIMER_H__
#define __TIMER_H__

#include <memory>
#include <vector>
#include <set>
#include <shared_mutex>
#include <assert.h>
#include <functional>
#include <mutex>

namespace sylar{
//定时器管理类
class TimerManager;


// 定时器类，继承自enable_shared_from_this以支持shared_ptr的自引用
class Timer : public std::enable_shared_from_this<Timer> 
{
// 设置成友元函数访问timerManager的私有成员
friend class TimerManager;

public:
    // 删除timer任务
    bool cancel();
    // 刷新timer任务
    bool refresh();
    // 重新设置timer超时时间
    // ms 定时器执行间隔时间，from_now表示从现在开始计算
    bool reset(uint64_t ms, bool from_now = true);

private:

    // 构造函数
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manger);
    // 是否循环
    bool m_recurring = false;
    // 定时器超时时间
    uint64_t m_ms = 0;
    // 绝对超时时间，该定时器下次执行的时间点
    std::chrono::time_point<std::chrono::system_clock> m_next;
    // 超时时触发的回调函数、
    std::function<void()> m_cb;
    // 定时器管理器
    TimerManager* m_manager = nullptr;

    // 比较函数，用于定时器排序
    struct Comparator 
    {
        bool operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs) const; 
    };
};

// 定时器管理类
class TimerManager 
{
friend class Timer;
public:
    // 构造函数
    TimerManager();
    // 析构函数
    virtual ~TimerManager();

    // 添加定时器
    // ms 定时器执行间隔时间，from_now表示从现在开始计算，cb 定时器超时触发的回调函数, recurring 是否循环
    std::shared_ptr<Timer> addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);

    // 添加条件定时器
    std::shared_ptr<Timer> addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring = false);

    // 拿到堆中最近的超时时间
    uint64_t getNextTimer();

    // 取出所有超时定时器的回调函数
    void listExpiredCb(std::vector<std::function<void()>>& cbs);

    // 堆中是否有定时器
    bool hasTimer();

protected:
    // 当最早的timer加入到堆中时，调用该函数
    virtual void onTimerInsertedAtFront() {};

    // 添加timer
    void addTimer(std::shared_ptr<Timer> timer);

private:
    // 当系统时间发生变化时->调用该函数
    bool detectClockRollover();

    std::shared_mutex m_mutex; // 互斥锁，保护定时器的访问
    // 时间堆
    std::set<std::shared_ptr<Timer>, Timer::Comparator> m_timers;

    // 在下次getNextTimer()调用前, onTimerInsertedAtFront()是否被触发
    bool m_tickled = false;
    // 上次检查系统时间是否回退的绝对时间
    std::chrono::time_point<std::chrono::system_clock> m_previousTime;
    

};

}

#endif // __TIMER_H__