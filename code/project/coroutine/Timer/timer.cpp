#include "timer.h"

namespace sylar {

bool Timer::cancel()
{
    std::unique_lock<std::shared_mutex> write_lock(m_manager->m_mutex);

    if (m_cb == nullptr) 
    {
        return false; // 定时器已被取消或不存在
    }
    else
    {
        m_cb = nullptr;
    }

    auto it = m_manager->m_timers.find(shared_from_this());
    if (it != m_manager->m_timers.end()) 
    {
        m_manager->m_timers.erase(it);
    }
    return true;
}

bool Timer::refresh()
{
    std::unique_lock<std::shared_mutex> write_lock(m_manager->m_mutex);

    if(!m_cb) 
    {
        return false; // 定时器已被取消或不存在
    }

    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) 
    {
        return false; // 定时器不存在
    }

    m_manager->m_timers.erase(it);
    m_next = std::chrono::system_clock::now() + std::chrono::milliseconds(m_ms);
    m_manager->m_timers.insert(shared_from_this());
    return true;
}

bool Timer::reset(uint64_t ms, bool from_now)
{
    if (ms == m_ms && !from_now) 
    {
        return true; // 没有变化
    }

    {
        std::unique_lock<std::shared_mutex> write_lock(m_manager->m_mutex);
        if (!m_cb) 
        {
            return false; // 定时器已被取消或不存在
        }

        auto it = m_manager->m_timers.find(shared_from_this());
        if (it == m_manager->m_timers.end()) 
        {
            return false; // 定时器不存在
        }
        m_manager->m_timers.erase(it);
    }

    // 更新定时器参数
    auto start = from_now ? std::chrono::system_clock::now() : m_next - std::chrono::milliseconds(m_ms);
    m_ms = ms;
    m_next = start + std::chrono::milliseconds(m_ms);
    m_manager->addTimer(shared_from_this());
    return true;
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager):
m_recurring(recurring), m_ms(ms), m_cb(cb), m_manager(manager)
{
    auto now = std::chrono::system_clock::now();
    m_next = now + std::chrono::milliseconds(m_ms);
}

bool Timer::Comparator::operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs) const 
{
    assert(lhs && rhs);
    return lhs->m_next < rhs->m_next; // 按照下次执行时间排序
}

TimerManager::TimerManager()
{
    m_previousTime = std::chrono::system_clock::now();
}

TimerManager::~TimerManager() {}

std::shared_ptr<Timer> TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring)
{
    std::shared_ptr<Timer> timer(new Timer(ms, cb, recurring, this));
    addTimer(timer);
    return timer;
}

void TimerManager::addTimer(std::shared_ptr<Timer> timer)
{
    bool at_front = false;
    {
        std::unique_lock<std::shared_mutex> write_lock(m_mutex);

        auto it = m_timers.insert(timer).first;
        at_front = (it == m_timers.begin()) && !m_tickled;

        if (at_front) {
            m_tickled = true;
            onTimerInsertedAtFront();
        }
    }
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb)
{
    std::shared_ptr<void> cond = weak_cond.lock();
    if (cond) {
        cb();
    }
}

std::shared_ptr<Timer> TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring)
{
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer()
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);

    // reset tickled state
    m_tickled = false;

    if (m_timers.empty()) 
    {
        return ~0ull; // 没有定时器
    }

    auto now = std::chrono::system_clock::now();
    auto time = (*m_timers.begin())->m_next;

    if (time <= now) {
        return 0; // 有定时器超时
    }
    else{
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(time - now);
        return static_cast<uint64_t>(duration.count());
    }

}

void TimerManager::listExpiredCb(std::vector<std::function<void()>> & cbs)
{
    auto now = std::chrono::system_clock::now();

    std::unique_lock<std::shared_mutex> write_lock(m_mutex);

    bool rollover = detectClockRollover();

    // 回退 -> 清空所有定时器 || 超时 -> 清空所有超时定时器
    while (!m_timers.empty() && rollover || !m_timers.empty() && (*m_timers.begin())->m_next <= now) 
    {
        std::shared_ptr<Timer> temp = *m_timers.begin();
        m_timers.erase(m_timers.begin());

        cbs.push_back(temp->m_cb);

        if (temp->m_recurring) 
        {
            temp->m_next = now + std::chrono::milliseconds(temp->m_ms);
            m_timers.insert(temp);
        }
        else 
        {
            temp->m_cb = nullptr; // 清空回调函数
        }
    }
}

bool TimerManager::hasTimer()
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    return !m_timers.empty();
}

bool TimerManager::detectClockRollover() 
{
    bool rollover = false;
    auto now = std::chrono::system_clock::now();
    if (now < (m_previousTime - std::chrono::milliseconds(60 * 60 * 1000))) {
        rollover = true; // 系统时间回退
    }
    m_previousTime = now;
    return rollover;
}

}