#ifndef LX_THREADPOOL_H
#define LX_THREADPOOL_H

#include <LX/base/YCondition.h>
#include <LX/base/YMutex.h>
#include <LX/base/YThread.h>
#include <LX/base/YTypes.h>

#include <deque>
#include <vector>

namespace LX
{
    class ThreadPool : noncopyable
    {
    public:
        typedef std::function<void()> Task;
        
        explicit ThreadPool(const string& nameArg = string("ThreadPool"));
        ~ThreadPool();

        void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
        void setThreadInitCallback(const Task& cb) { threadInitCallback_ = cb; }

        void start(int numThreads);
        void stop();

        const string& name() const { return name_; }
        size_t queueSize() const;

        void run(Task f);
    private:
        bool isFull() const;
        void runInThread();
        Task take();

        mutable MutexLock mutex_;
        Condition notEmpty_;
        Condition notFull_;
        string name_;
        Task threadInitCallback_;
        std::vector<std::unique_ptr<LX::Thread> > threads_;
        std::deque<Task> Taskqueue_;
        size_t maxQueueSize_;
        bool running_;
    };
}

#endif
