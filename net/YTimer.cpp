#include <LX/net/YTimer.h>

using namespace LX;
using namespace LX::net;

AtomicInt64 Timer::s_numCreated_;

void Timer::restart(Timestamp now)
{
    if(repeat_)
    {
        expiration_ = addTime(now, interval_);
    }
    else
    {
        expiration_ = Timestamp::invalid();
    }
}