#include "YThreadPool.h"
#include "YCountDownLatch.h"
#include "YCurrentThread.h"
#include "YLogging.h"

#include <stdio.h>
#include <unistd.h>

void printf()
{
    printf("tid = %d\n", YBASE::CurrentThread::tid());
}

void printString(const std::string& str)
{
    LOG_INFO << str;
    usleep(100 * 1000);
}

void test(int maxSize)
{
    LOG_WARN << "Test ThreadPool with max queue size = " << maxSize;
    YBASE::ThreadPool pool("MainThreadPool");
    pool.setMaxQueueSize(maxSize);
    pool.start(5);

    LOG_WARN << "Adding";
    pool.run(print);
    pool.run(print);
    for(int i = 0; i < 100; i++)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "task %d", i);
        pool run(std::bind(printString, std::string(buf)));
    }
    LOG_WARN << "Done";

    YBASE::CountDownLatch latch(1);
    pool.run(std::bind(&YBASE::CountDownLatch::countDown, &latch));
    latch.wait();
    pool.stop();
}

int main()
{
    test(0);
    test(1);
    test(2);
    test(5);
    test(10);
    test(50);
}