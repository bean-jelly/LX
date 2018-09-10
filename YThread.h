#ifndef YAN_BASE_THREAD_H
#define YAN_BASE_THREAD_H

#include "Atomic.h"

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <pthread.h>
#include <string>

namespace YBASE
{
    class Thread : boost::noncopyable
    {
    public:
        typedef boost::function<void()> ThreadFunc;
        explicit Thread(const ThreadFunc&, const std::string& name = std::string());
        ~Thread();

        void start();
        void join();

        bool started() const {return started_;}
        pid_t tid() const {return *tid_;}

    private:
        bool started_;
        bool joined_;
        //pid_t用于表示进程号的类型
        boost::shared_ptr<pid_t> tid_;
        pthread_t pthreadId_;
        ThreadFunc func_;
        std::string name_;

        static AtomicInt32 numCreated_;
    };
}

#endif