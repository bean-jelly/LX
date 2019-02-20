#ifndef YBASE_NET_TIMERQUEUE_H
#define YABSE_NET_TIMERQUEUE_H

#include <set>
#include <vector>

#include <LX/base/YMutex.h>
#include <LX/base/YTimestamp.h>
#include <LX/net/YCallbacks.h>
#include <LX/net/YChannel.h>

namespace YBASE
{
    namespace net
    {
        class EventLoop;
        class Timer;
        class TimerId;

        class TimerQueue:noncopyable
        {
        public:
            explicit TimerQueue(EventLoop* loop);
            ~TimerQueue();

            TimerId addTimer(TimerCallback cb, Timestamp when, double interval);
            void cancel(TimerId timerId);
        private:
            typedef std::pair<Timestamp, Timer*> Entry;
            typedef std::set<Entry> TimerList;
            typedef std::pair<Timer*, int64_t> ActiveTimer;
            typedef std::set<ActiveTimer> ActiveTimerSet;

            void addTimerInLoop(Timer* timer);
            void cancelInLoop(TimerId timerId);
            void handleRead();
            void reset(const std::vector<Entry>& expired, Timestamp now);
            bool insert(Timer* timer);
            std::vector<Entry> getExpired(Timestamp now);

            EventLoop* loop_;
            const int timerfd_;
            Channel timerfdChannel_;
            TimerList timers_;

            ActiveTimerSet activeTimres_;
            bool callingExpiredTimers_;
            ActiveTimerSet cancelingTimers_;
        };
    }
}

#endif