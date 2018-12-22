#ifndef LTIMER_H_
#define LTIMER_H_

#include <vector>

typedef void (*pFun)(void);

class TimerManager;

class Timer
{
public:
    enum TimerType { ONCE, CIRCLE};
    Timer(TimerManager& manager);
    ~Timer();
    template<typename Fun>
    void Start(Fun fun, unsigned interval, TimerType timeType = CIRCLE);
    void Stop();
private:
    void OnTimer(unsigned long long now);
private:
    friend class        TimerManager;
    TimerManager&       m_manager;
    TimerType           m_timerType;
    pFun                m_timerFun;
    unsigned int        m_interval;
    unsigned long long  m_expires;
    size_t              m_heapIndex;
};

class TimerManager
{
public:
    static unsigned long long GetCurrentMillisecs();
    void DetectTimers();
private:
    friend class Timer;
    void AddTimer(Timer* timer);
    void RemoveTimer(Timer* timer);
    void UpHeap(size_t index);
    void DownHeap(size_t index);
    void SwapHeap(size_t index1, size_t index2);
private:
    struct HeapEntry
    {
        unsigned long long time;
        Timer* timer;
    };
    std::vector<HeapEntry> m_heap;
};

template<typename Fun>
inline void Timer::Start(Fun fun, unsigned interval, TimerType timeType)
{
    Stop();
    m_interval  = interval;
    m_timerFun  = fun;
    m_timerType = timeType;
    m_expires   = m_interval + TimerManager::GetCurrentMillisecs();
    m_manager.AddTimer(this);
}

#endif