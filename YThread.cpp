#include "YThread.h"
#include "YCurrentThread.h"
#include "YException.h"
#include "YLogging.h"

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>         //unix类系统定义符号常量的头文件，也包含了read(),write(),geepid(),sleep(),fork()等
#include <sys/types.h>      //unix/Linux系统的基本系统数据类型的头文件，含有size_t，time_t，pid_t等类型
//#include <windows.h>
#include <linux/unistd.h>
#include <sys/prctl.h>      //进程相关
#include <sys/syscall.h>    //syscall()执行一个系统调用，根据指定的参数number和所有系统调用的汇编语言接口来确定调用哪个系统调用。

/*
进程pid: getpid()                 
线程tid: pthread_self()         //进程内唯一，但是在不同进程则不唯一。
线程pid: syscall(SYS_gettid)    //系统内是唯一的
*/
namespace YBASE
{
    namespace CurrentThread
    {
        __thread int t_cachedTid;
        __thread char t_tidString[32];
        __thread int t_tidStringLength;
        __thread const char* t_threadName = "unknown";
        static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");
    }
    /*detail -----------------------------------------------------------------------------*/
    namespace detail
    {
        pid_t gettid()
        {
            #ifndef __linux__
            return static_cast<pid_t>(GetCurrentThreadId());
            #else
            return static_cast<pid_t>(::syscall(SYS_gettid));
            #endif
        }
        
        //parent fork创建了子进程以后，但在fork返回之前在父进程的进程环境中调用的
        void afterFork()
        {
            YBASE::CurrentThread::t_cachedTid = 0;
            YBASE::CurrentThread::t_threadName = "main";
            CurrentThread::tid();
        }
    
        class ThreadNameInitializer
        {
        public:
            ThreadNameInitializer(){
                YBASE::CurrentThread::t_threadName = "main";
                /*
                pthread_atfork(void (*prepare)(void），void (*parent)(void）, void(*child)(void))
                prepare在父进程fork创建子进程之前调用，这里可以获取父进程定义的所有锁；
                child fork返回之前在子进程环境中调用，在这里unlock prepare获得的锁；
                parent fork创建了子进程以后，但在fork返回之前在父进程的进程环境中调用的，在这里对prepare获得的锁进行解锁；
                */
                CurrentThread::tid();
                pthread_atfork(NULL, NULL, &afterFork);
            }
        };
        
        ThreadNameInitializer init;
        
        struct ThreadData
        {
            typedef YBASE::Thread::ThreadFunc ThreadFunc;
            ThreadFunc func_;
            std::string name_;
            pid_t* tid_;
            CountDownLatch* latch_;

            ThreadData(const ThreadFunc& func, const std::string& name, pid_t* tid, CountDownLatch* latch) :func_(func), name_(name), tid_(tid), latch_(latch){}

            void runInThread()
            {
                *tid_ = YBASE::CurrentThread::tid();
                tid_ = NULL;
                latch_->countDown();
                latch_ = NULL;

                YBASE::CurrentThread::t_threadName = name_.empty() ? "YThread" : name_.c_str();
                ::prctl(PR_SET_NAME,YBASE::CurrentThread::t_threadName);
                try
                {
                    func_();
                    YBASE::CurrentThread::t_threadName = "finished";
                }
                catch(const Exception& ex)
                {
                    YBASE::CurrentThread::t_threadName = "crashed";
                    abort();
                }
                catch(const std::exception& ex)
                {
                    YBASE::CurrentThread::t_threadName = "crashed";
                    abort();
                }
                catch(...)
                {
                    YBASE::CurrentThread::t_threadName = "crashed";
                    throw;
                }
            }
        };

        void* startThread(void* obj)
        {
            ThreadData* data = static_cast<ThreadData*>(obj);
            data->runInThread();
            delete data;
            return NULL;
        }
    }
    /*detail -----------------------------------------------------------------------------*/
}

using namespace YBASE;

void CurrentThread::cacheTid()
{
    if(t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
    ::nanosleep(&ts, NULL);
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const std::string& name)
:started_(false), joined_(false), pthreadId_(0), tid_(0), func_(std::move(func)), name_(name), latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = numCreated_.incrementAndGet();
    if(name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;

    detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
    if(pthread_create(&pthreadId_, NULL, &detail::startThread, data))
    {
        started_ = false;
        delete data;
        abort();
    }
    else
    {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}

void say()
{
    std::cout << "hello world" << std::endl;
}

int main()
{
    std::string name = "yan";
    YBASE::Thread thread_t(say, name);
    thread_t.start();
    thread_t.join();

    return 0;
}
