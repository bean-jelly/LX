#ifndef LX_NET_EVENTLOOPTHREAD_H
#define LX_NET_EVENTLOOPTHREAD_H

#include <LX/base/YCondition.h>
#include <LX/base/YMutex.h>
#include <LX/base/YThread.h>

namespace LX
{
    namespace net
    {
        class EventLoop;
        class EventLoopThread : noncopyable
        {
        public:
            typedef std::function<void(EventLoop*)> ThreadInitCallback;

            EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                            const string& name = string());
            ~EventLoopThread();
            EventLoop* startLoop();

        private:
            void threadFunc();
            EventLoop*  loop_;
            bool        exiting_;
            Thread      thread_;
            MutexLock   mutex_;
            Condition   cond_;
            ThreadInitCallback callback_;
        };
    }
}

#endif