#ifndef YBASE_COUNTDOWNLATCH_H
#define YBASE_COUNTDOWNLATCH_H

#include "YCondition.h"
#include "YMutex.h"
#include "YNoncopyable.h"

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