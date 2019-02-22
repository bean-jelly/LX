#ifndef LX_NET_TIMERID_H
#define LX_NET_TIMERID_H

#include <LX/base/YCopyable.h>

namespace LX
{
    namespace net
    {
        class Timer;

        class TimerId : public LX::copyable
        {
        public:
            TimerId():timer_(NULL), sequence_(0){}

            TimerId(Timer* timer, int64_t seq):timer_(timer), sequence_(seq){}

            friend class TimerQueue;

        private:
            Timer* timer_;
            int64_t sequence_;      //时钟序号
        };
    }
}

#endif