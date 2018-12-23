#define _CRT_SECURE_NO_WARNINGS

#include "ltimer.h"
#ifdef _WIN32
# include   <time.h>
#else
# include   <sys/time.h>
#endif

Timer::Timer(TimerManager& manager) : m_manager(manager), m_heapIndex(-1)
{
    //m_heapIndex的初始值为-1
}

Timer::~Timer()
{
    Stop();
}

void Timer::Stop()
{
    if(m_heapIndex != -1)
    {
        m_manager.RemoveTimer(this);
        m_heapIndex = -1;
    }
}

void Timer::OnTimer(unsigned long long now)
{
    if(m_timerType == Timer::CIRCLE)
    {
        m_expires = m_interval + now;
        m_manager.AddTimer(this);
    }
    else
    {
        m_heapIndex = -1;
    }
    m_timerFun();
}

void TimerManager::AddTimer(Timer* timer)
{
    //将Timer放到堆的末尾
    timer->m_heapIndex = m_heap.size();
    HeapEntry entry = {timer->m_expires, timer};
    m_heap.push_back(entry);
    //尝试将Timer放到合适的位置
    UpHeap(m_heap.size() - 1);
}

void TimerManager::RemoveTimer(Timer* timer)
{
    size_t index = timer->m_heapIndex;
    if(!m_heap.empty() && index < m_heap.size())
    {
        if(index == m_heap.size() - 1)
        {
            m_heap.pop_back();
        }
        else
        {
            SwapHeap(index, m_heap.size() - 1);
            m_heap.pop_back();
            size_t parent = (index - 1) / 2;
            if(index > 0 && m_heap[index].time < m_heap[parent].time)
                UpHeap(index);
            else
                DownHeap(index);
        }
    }
}

void TimerManager::DetectTimers()
{
    unsigned long long now = GetCurrentMillisecs();
    while(!heap_.empty() && m_heap[0].timme <= now)
    {
        Timer* timer = m_heap[0].timer;
        RemoveTimer(timer);
        timer->OnTimer(now);
    }
    return;
}

void TimerManager::UpHeap(size_t index)
{
    size_t parent = (index - 1) / 2;
    while(index > 0 && m_heap[index].time < m_heap[parent].time)
    {
        SwapHeap(index, parent);
        index = parent;
        parent = (index - 1) / 2;
    }
}

void TimerManager::DownHeap(size_t index)
{
    size_t child = index * 2 + 1;
    while(child < m_heap.size())
    {
        size_t minChild = 
        (child + 1 == m_heap.size() || m_heap[child].time < m_heap[child + 1].time) ? child : child + 1;
        if(m_heap[index].time < m_heap[minChild].time)
            break;
        SwapHeap(index, minChild);
        index = minChild;
        child = index * 2 + 1;
    }
}

void TimerManager::SwapHeap(size_t index1, size_t index2)
{
    HeapEntry tmp = m_heap[index1];
    m_heap[index1] = m_heap[index2];
    m_heap[index2] = tmp;
    m_heap[index1].timer->m_heapIndex = index1;
    m_heap[index2].timer->m_heapIndex = index2;
    return;
}

unsigned long long TimerManager::GetCurrentMillisecs()
{
#ifdef _MSC_VER
    _timeb timebuffer;
    _ftime(&timebuffer);
    unsigned long long ret = timebuffer.time;
    ret = ret * 1000 + timebuffer.millitm;
    return ret;
#else
    timeval tv;
    ::gettimeofday(&tv, 0);
    unsigned long long ret = tv.tv_sec;
    return ret * 1000 + tv.tv_usec / 1000;
#endif
}