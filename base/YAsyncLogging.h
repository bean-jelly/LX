#ifndef LX_ASYNCLOGGING_H
#define LX_ASYNCLOGGING_H

#include <LX/base/YBlockingQueue.h>
#include <LX/base/YBoundedBlockingQueue.h>
#include <LX/base/YCountDownLatch.h>
#include <LX/base/YMutex.h>
#include <LX/base/YThread.h>
#include <LX/base/YLogStream.h>

#include <atomic>
#include <vector>

namespace LX
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

        typedef LX::detail::FixedBuffer<LX::detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        const int               flushInterval_;
        std::atomic<bool>       running_;
        std::string             basename_;
        off_t                   rollSize_;
        LX::Thread           thread_;
        LX::MutexLock        mutex_;
        LX::CountDownLatch   latch_;
        LX::Condition        cond_;
        BufferPtr               currentBuffer_;     //当前缓冲区
        BufferPtr               nextBuffer_;        //预备缓冲
        BufferVector            buffers_;           //待写入文件的已填满的缓冲（供后端写入的buffer）
    };
}


#endif