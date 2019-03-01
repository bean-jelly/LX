#include <LX/net/YEventLoopThread.h>
#include <LX/net/YEventLoop.h>

using namespace LX;
using namespace LX::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
                                 const string& name)
        :loop_(NULL),
        exiting_(false),
        thread_(std::bind(&EventLoopThread::threadFunc, this), name),
        mutex_(),
        cond_(mutex_),
        callback_(cb_)
{

}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if(loop_ != NULL)
    {
        loop_.quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    assert(!thread_.started());
    thread_.start();

    {
        MutexLockGuard lock(mutex_);
        while(loop_ == NULL)
        {
            cond_.wait();
        }
    }

    return loop_;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    if(callback_)
    {
        callback_(&loop);
    }

    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    loop.loop();
    loop_ = NULL;
}