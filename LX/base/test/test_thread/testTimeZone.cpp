#include "YTimeZone.h"
#include "YTimestamp.h"

int main()
{
    YBASE::Timestamp time_ = YBASE::Timestamp::now();
    int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / YBASE::Timestamp::kMicroSecondsPerSecond);
    YBASE::TimeZone test;
    test.toLocalTime(seconds);
    return 0;
}
