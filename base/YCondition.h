#ifndef YBASE_CONDITION_H
#define YBASE_CONDITION_H

#include <LX/base/YMutex.h>
#include <pthread.h>

namespace YBASE
{
    class Condition : noncopyable
    {
    public:
        explicit Condition(MutexLock& mutex): mutex_(mutex)
        {
            MCHECK(pthread_cond_init(&pcond_, NULL));
        }
        ~Condition()
        {
            MCHECK(pthread_cond_destroy(&pcond_));
        }

        void wait()
        {
            MutexLock::UnassignGuard ug(mutex_);
            MCHECK(pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()));
        }

        bool waitForSeconds(double seconds);

        void notify()
        {
            MCHECK(pthread_cond_signal(&pcond_));
        }
        void notifyAll()
        {
            MCHECK(pthread_cond_broadcast(&pcond_));
        }
    private:
        //如果一个class需要包含MutexLock 和Condition，请注意他们的声明顺序和初始化顺序
        //mutex_应该先于condition_构造，并作为后者的构造参数
        MutexLock& mutex_;
        pthread_cond_t pcond_;
    };
}

#endif