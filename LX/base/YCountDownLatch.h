#ifndef YBASE_COUNTDOWNLATCH_H
#define YBASE_COUNTDOWNLATCH_H

#include <LX/base/YCondition.h>
#include <LX/base/YMutex.h>
#include <LX/base/YNoncopyable.h>

namespace YBASE
{
    class CountDownLatch : noncopyable
    {
    public:
        explicit    CountDownLatch(int count);
        void        wait();
        void        countDown();
        int         getCount() const;
    private:
        mutable     MutexLock mutex_;
        Condition   condition_;
        int         count_;
    };
}

#endif //YBASE_COUNTDOWNLATCH_H