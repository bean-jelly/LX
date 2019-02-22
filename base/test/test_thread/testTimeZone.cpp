#include "YTimeZone.h"
#include "YTimestamp.h"

int main()
{
    LX::Timestamp time_ = LX::Timestamp::now();
    int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / LX::Timestamp::kMicroSecondsPerSecond);
    LX::TimeZone test;
    test.toLocalTime(seconds);
    return 0;
}
