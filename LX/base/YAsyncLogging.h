#ifndef YBASE_ASYNCLOGGING_H
#define YBASE_ASYNCLOGGING_H

#include <LX/base/YBlockingQueue.h>
#include <LX/base/YBoundedBlockingQueue.h>
#include <LX/base/YCountDownLatch.h>
#include <LX/base/YMutex.h>
#include <LX/base/YThread.h>
#include <LX/base/YLogStream.h>

#include <atomic>
#include <vector>

namespace YBASE
{
    class AsyncLogging:noncopyable
    {
    public:
        AsyncLogging(const string& basename, off_t rollSize, int flushInterval = 3);
        ~AsyncLogging()
        {
            if(running_)
                stop();
        }

        void append(const char* logline, int len);

        void start()
        {
            running_ = true;
            thread_.start();
            latch_.wait();
        }

        void stop()
        {
            running_ = false;
            cond_.notify();
            thread_.join();
        }

    private:
        void threadFunc();

        typedef YBASE::detail::FixedBuffer<YBASE::detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        const int               flushInterval_;
        std::atomic<bool>       running_;
        std::string             basename_;
        off_t                   rollSize_;
        YBASE::Thread           thread_;
        YBASE::MutexLock        mutex_;
        YBASE::CountDownLatch   latch_;
        YBASE::Condition        cond_;
        BufferPtr               currentBuffer_;
        BufferPtr               nextBuffer_;
        BufferVector            buffers_;
    };
}


#endif