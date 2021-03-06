#ifndef YAN_BASE_MUTEX_H
#define YAN_BASE_MUTEX_H

#include "YCurrentThread.h"
#include "YNoncopyable.h"
#include <assert.h>
#include <pthread.h>

#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail(int errnum,`
                                 const char* file,
                                 unsigned int line,
                                 const char* function)
    __THROW __attribute__ ((__noreturn__));
__END_DECLS
#endif

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);     \
                     if (__builtin_expect(errnum != 0, 0))  \
                     __assert_perror_fail (errnum, __FILE__, __LINE__, __func__);})

#else   // CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret);     \
                     assert(errnum == 0); (void) errnum;})

#endif  // CHECK_PTHREAD_RETURN_VALUE

//以下两个类均不提供拷贝构造函数

namespace YBASE
{
    class MutexLock : noncopyable
    {
    public:
        MutexLock() : holder_(0)
        {
            //if pthread_mutex_init words,it return 0,else means error.
            MCHECK(pthread_mutex_init(&mutex_, NULL));
        }
        ~MutexLock()
        {
            assert(holder_ == 0);
            MCHECK(pthread_mutex_destroy(&mutex_));
        }

        bool isLockedByThisThread() const
        {
            //系统内唯一的pid
            return holder_ == CurrentThread::tid();
        }

        void assertLocked() const
        {
            assert(isLockedByThisThread());
        }

        void lock()                             //仅供MutexLockGuard调用
        {
            MCHECK(pthread_mutex_lock(&mutex_));
            assignHolder();
        }

        void unlock()
        {
            unassignHolder();
            MCHECK(pthread_mutex_unlock(&mutex_));
        }

        pthread_mutex_t* getPthreadMutex()      //仅供Condition调用，严禁用户代码调用
        {
            return &mutex_;
        }
    private:
        friend class Condition;
        class UnassignGuard : noncopyable
        {
        public:
            UnassignGuard(MutexLock& owner) : owner_(owner)
            {
                owner_.unassignHolder();
            }
            ~UnassignGuard()
            {
                owner_.assignHolder();
            }
        private:
            MutexLock& owner_;
        };

        void unassignHolder()
        {
            holder_ = 0;
        }

        void assignHolder()
        {
            holder_ = CurrentThread::tid();
        }

    private:
        pthread_mutex_t mutex_;
        pid_t holder_;
    };

    class MutexLockGuard : noncopyable
    {
    public:
        explicit MutexLockGuard(MutexLock& mutex):mutex_(mutex)
        {
            mutex_.lock();
        }
        ~MutexLockGuard ()
        {
            mutex_.unlock();
        }
        private:
            MutexLock& mutex_;
    };
}

//该宏是为了防止程序里出现MutexLockGuard(mutex)的错误编写情况
//如MutexLockGuard(mutex) ，应该为MutexLockGuard lock(mutex)
#define MutexLockGuard(x) error "Missing guard object name"

#endif