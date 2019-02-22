#include <LX/net/YEventLoop.h>
#include <LX/base YThread.h>

#include <boost/bind>

#include <stdio.h>
#include <unistd.h>

using namespace LX;
using namespace LX::net;

int cnt = 0;
EventLoop *g_loop;

void PrintTid()
{
    printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    printf("now %s\n", Timestamp::now().toString().c_str());
}

void print(const char *msg)
{
    printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
    if (++cnt == 20)
    {
        g_loop->quit();
    }
}

void cancel(TimerId timer)
{
    g_loop->cancel(timer);
    printf("cancelled at %s\n", Timestamp::now().toString().c_str());
}

int main()
{
    PrintTid();
    sleep(1);
    {
        EventLoop loop;
        g_loop = &loop;

        printf("main");
        loop.runAfter(1, boost::bind(print, "once1"));
        loop.runAfter(1.5, boost::bind(print, "once1.5"));
        loop.runAfter(2.5, boost::bind(print, "once2.5"));
        loop.runAfter(3.5, boost::bind(print, "once3.5"));
        TimerId t45 = loop.runAfter(4.5, boost::bind(print, "once4.5"));
        loop.runAfter(4.2, boost::bind(cancel, t45));
        loop.runAfter(4.8, boost::bind(cancel, t45));
        loop.runEvery(2, boost::bind(print, "every2"));
        TimerId t3 = loop.runEvery(3, boost::bind(print, "every3"));
        loop.runAfter(9.001, boost::bind(cancel, t3));

        loop.loop();
        print("main loop exits");
    }
}