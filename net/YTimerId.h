#ifndef YBASE_NET_TIMERID_H
#define YBASE_NET_TIMERID_H

#include <LX/base/copyable.h>

namespace YBASE
{
    namespace net
    {
        class Timer;

        class TimerId : public YBASE::copyable
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