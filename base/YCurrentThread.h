#ifndef LX_CURRENTTHREAD_H
#define LX_CURRENTTHREAD_H

#include <LX/base/YTypes.h>
#include <stdint.h>

/*
进程pid: getpid()                 
线程tid: pthread_self()         //进程内唯一，但是在不同进程则不唯一。
线程pid: syscall(SYS_gettid)    //系统内是唯一的
*/

#define LIKELY(x)     __builtin_expect(!!(x), 1) //x很可能为真
#define UNLIKELY(x)   __builtin_expect(!!(x), 0) //x很可能为假

namespace LX
{
    namespace CurrentThread
    {
        extern __thread int     t_cachedTid;
        extern __thread char    t_tidString[32];
        extern __thread int     t_tidStringLength;
        extern __thread const char* t_threadName;

        void cacheTid();

        inline int tid()
        {
            cacheTid();
            return t_cachedTid;
        }

        inline const char* tidString()
        {
            return t_tidString;
        }

        inline int tidStringLength()
        {
            return t_tidStringLength;
        }

        inline const char* name()
        {
            return t_threadName;
        }

        bool isMainThread();

        void sleepUsec(int64_t usec);
    }
}

#endif
