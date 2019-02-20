#ifndef YBASE_NET_TIMER_H
#define YBASE_NET_TIMER_H

#include <LX/base/YAtomic.h>
#include <LX/base/YTimestamp.h>
#include <LX/net/YCallbacks.h>

namespace YBASE
{
    namespace net
    {
        class Timer : noncopyable
        {
        public:
            Timer(TimerCallback cb, Timestamp when, double interval)
                : callback_(std::move(cb)),
                  expiration_(when),
                  interval_(interval),
                  repeat_(interval > 0.0),
                  sequence_(s_numCreated_.incrementAndGet())
            {}

            void run() const
            {
                callback_();
            }

            Timestamp expiration() const { return expiration_; }
            bool repeat() const { return repeat_; }
            int64_t sequence() const {return sequence_; }

            void restart(Timestamp now);

            static int64_t numCreated() { return s_numCreated_.get(); }

        private:
            const TimerCallback callback_;      //定时器回调函数
            Timestamp expiration_;              //下一次的超时时刻
            const double interval_;             //超时时间间隔，如果是一次性定时器，该值为0
            const bool repreat_;                //是否重复
            const int64_t sequence_;            //定时器序号

            static AtomicInt64 s_numCreated_;   //定时器计时，当前已经创建的定时器数量
        };
    }
}

#endif 