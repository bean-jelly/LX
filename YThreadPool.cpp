#include "YThreadPool.h"
#include "YException.h"

#include <assert.h>
#include <stdio.h>

using namespace YBASE;

ThreadPool::ThreadPool(const string& nameArg):mutex_(),
                                              notEmpty_(mutex_),
                                              notFull_(mutex_),
                                              name_(nameArg),
                                              maxQueueSize_(0),
                                              running_(false)
{

}

ThreadPool::~ThreadPool()
{
    if(running_)
    {
        stop();
    }
}

void ThreadPool::stop()
{
    {
        MutexLockGuard lock(mutex_);
        running_ = false;
        notEmpty_.notifyAll();
    }
    for(auto& thr : threads_)
        thr->join();
}

size_t ThreadPool::queueSize() const
{
    MutexLockGuard lock(mutex_);
    return Taskqueue_.size();
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);
    for(int i = 0; i < numThreads; ++i)
    {
        char id[32];
        snprintf(id, sizeof id, "%d", i+1);
        threads_.emplace_back(new Thread(std::bind(&ThreadPool::runInThread, this), name_+id));
        threads_[i]->start();
    }
    if(numThreads == 0 && threadInitCallback_)
    {
        threadInitCallback_();
    }
}

void ThreadPool::runInThread()
{
    try
    {
        if(threadInitCallback_)
        {
            threadInitCallback_();
        }
        while(running_)
        {
            Task task(take());
            if(task)
            {
                task();
            }
        }
    }
    catch(const Exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        abort();
    }
    catch(const std::exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch(...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw;
    }
}

ThreadPool::Task ThreadPool::take()
{
    MutexLockGuard lock(mutex_);
    while(Taskqueue_.empty() && running_)
    {
        notEmpty_.wait();
    }
    Task task;
    if(!Taskqueue_.empty())
    {
        task = Taskqueue_.front();
        Taskqueue_.pop_front();
        if(maxQueueSize_ > 0)
        {
            notFull_.notify();
        }
    }
    return task;
}

void ThreadPool::run(Task task)
{
    if(threads_.empty())
    {
        task();
    }
    else
    {
        MutexLockGuard lock(mutex_);
        while(isFull())
        {
            notFull_.wait();
        }
        assert(!isFull());
        Taskqueue_.push_back(std::move(task));
        notEmpty_.notify();
    }
}

bool ThreadPool::isFull() const
{
    mutex_.assertLocked();
    return maxQueueSize_ > 0 && Taskqueue_.size() >= maxQueueSize_;
}
