#ifndef YBASE_CURRENTTHREAD_H
#define YBASE_CURRENTTHREAD_H

#include <stdint.h>

/*
进程pid: getpid()                 
线程tid: pthread_self()         //进程内唯一，但是在不同进程则不唯一。
线程pid: syscall(SYS_gettid)    //系统内是唯一的
*/

namespace YBASE
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
            if(__builtin_expect(t_cachedTid == 0, 0))
            {
                cacheTid();
            }
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