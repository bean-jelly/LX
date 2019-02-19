#ifndef YBASE_NET_EVENTLOOP_H
#define YBASE_NET_EVENTLOOP_H

#include <vector>
#include <atomic>
#include <functional>

#include <boost/any.hpp>
#include <LX/base/YMutex.h>
#include <LX/base/YCurrentThread.h>
#include <LX/base/YTimestamp.h>
#include <LX/net/YCallbacks.h>
#include <LX/net/YTimerId.h>

namespace YBASE
{
    namespace net
    {
        class Channel;
        class Poller;
        class TimerQueue;

        class EventLoop : noncopyable
        {
        public:
            typedef std::function<void()> Functor;

            EventLoop();
            ~EventLoop();

            void loop();

            void quit();

            Timestamp pollReturnTime() const { return pollReturnTime_; }

            int64_t iteration() const {return iteration_;}

            //Runs callback immediately in the loop thread
            //It wakes up the loop, and run the cb
            //If in the same loop thread, cb is run within the function
            //Safe to call from other threds
            void runInLoop(functor cb);

            //Queues callback in the loop thread
            //Runs after finish pooling
            void queueInLoop(Functor cb);

            size_t queueSize() const;

            TimerId runAt(Timestamp time, TimerCallback cb);

            TimerId runAfter(double delay, TimerCallback cb);

            TImerId runEvery(double interval, TimerCallback cb);

            void cancel(TimerId timerId);

            void wakeup();
            void updateChannel(Channel* channel);
            void removeChannel(Channel* channel);
            bool hasChannel(Channel* channel);

            void assertInLoopThread()
            {
                if(!isInLoopThread())
                {
                    abortNotInLoopThread();
                }
            }

            bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
            bool eventHandling() const { return eventHandling_; }

            void setContext(const boost::any& context)
            {
                context_ = context;
            }

            const boost::any& getContext() const
            {
                return context_;
            }

            boost::any* getMutableContext()
            {
                return &context_;
            }

            static EventLoop* getEventLoopOfCurrentThread();

        private:
            void abortNotInLoopThread();
            void handleRead();
            void doPendingFunctors();
            void printActiveChannels() const;

            typedef std::vecor<Channel*> ChannelList;

            bool                        looping_;
            std::atomic<boo>            quit_;
            bool                        eventHandling_;
            bool                        callingPendingFunctors_;
            int64_t                     iteration_;
            const pid_t                 threadId_;
            Timestamp                   pollReturnTime_;
            std::unique_ptr<Poller>     poller_;
            std::unique_ptr<TimerQueue> timerQueue_;
            int                         wakeupFd_;
            std::unique_ptr<Channel>    wakeupChannel_;
            boost::any                  context_;

            ChannelList                 activeChannels_;
            Channel*                    currentActiveChannel_;

            mutable MutexLock           mutex_;
            std::vector<Functor>        pendingFunctors_;
        };
    }
}