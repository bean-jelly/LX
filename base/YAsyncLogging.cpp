#include <LX/base/YAsyncLogging.h>
#include <LX/base/YLogFile.h>
#include <LX/base/YTimestamp.h>

#include <stdio.h>

using namespace LX;

AsyncLogging::AsyncLogging(const string& basename, off_t rollSize, int flushInterval)
                :flushInterval_(flushInterval),
                running_(false),
                basename_(basename),
                rollSize_(rollSize),
                thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
                latch_(1),
                mutex_(),
                cond_(mutex_),
                currentBuffer_(new Buffer),
                nextBuffer_(new Buffer),
                buffers_()
{
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

void AsyncLogging::append(const char* logline, int len)
{
    LX::MutexLockGuard lock(mutex_);
    if(currentBuffer_->avail() > len)
    {
        currentBuffer_->append(logline, len);
    }
    else
    {
        buffers_.push_back(std::move(currentBuffer_));
        if(nextBuffer_)
        {
            //如果备用nextBuffer_有空间，将nextBuffers_空间分配给currentBuffer_
            currentBuffer_ = std::move(nextBuffer_);
        }
        else
        {
            //如果备用nextBuffer_没空间，代表currentBuffer_和nextBuffer_都用光了，只好重新分配一个Buffer
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logline, len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc()
{
    assert(running_ == true);
    latch_.countDown();
    LogFile output(basename_, rollSize_, false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();

    //用来和前台线程的buffers_进行swap
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {
            LX::MutexLockGuard lock(mutex_);
            if(buffers_.empty())
            {
                cond_.waitForSeconds(flushInterval_);
            }

            /* 
            无论cond是因何种原因醒来，都要将currentBuffer_放到buffers_中。如果是因为时间
            到而醒，那么就算currentBuffer_还没写满，此时也要将它写入LogFile中。如果前台
            currentBuffer_已经满了，那么在前台线程中就已经把一个前台currentBuffer_放到
            buffers_中了。此时，还是需要把currentBuffer_放到buffers_中（注意，前后放置是不同            
            的buffer，因为在前台线程中，currentBuffer_已经被换成nextBuffer_指向的buffer了） 
            */

            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);         //归还currentBuffer
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_)                                //如果需要，归还nextBuffer
            {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if(buffersToWrite.size() > 25)
        {
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n", Timestamp::now().toFormattedString().c_str(), buffersToWrite.size() - 2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }

        // 将已经满了的buffer写入到LogFile中。由LogFile进行IO操作
        for (size_t i = 0; i < buffersToWrite.size(); ++i)
        {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }

        if (buffersToWrite.size() > 2)
        {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        //前台buffer是由newBuffer1 2 归还的。现在把buffersToWrite的buffer归还给后台buffer
        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}