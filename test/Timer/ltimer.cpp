#define _CRT_SECURE_NO_WARNINGS

#include "ltimer.h"
#ifdef _WIN32
# include   <time.h>
#else
# include   <sys/time.h>
#endif

Timer::Timer(TimerManager& manager) : m_manager(manager), m_heapIndex(-1)
{

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
    timer->m_heapIndex = m_heap.size();
    HeapEntry entry = {timer->m_expires, timer};
    m_heap.push_back(entry);
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
        size_t minChild = (child + 1 == m_heap.size() || )
    }
}