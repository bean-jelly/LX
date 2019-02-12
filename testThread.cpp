#include "YThread.h"
#include "YCurrentThread.h"

#include <string>
#include <stdio.h>
#include <unistd.h>

void mysleep(int seconds)
{
    timespec t = {seconds, 0};
    nanosleep(&t, NULL);
}

void threadFunc()
{
    printf("threadFunc tid=%d\n", YBASE::CurrentThread::tid());
}

void ThreadFunc2(int x)
{
    printf("ThreadFunc2 tid=%d, x=%d\n", YBASE::CurrentThread::tid(), x);
}

void threadFunc3()
{
    printf("threadFunc3 tid=%d\n", muduo::CurrentThread::tid());
    mysleep(1);
}

class Foo
{
public:
    explicit Foo(double x) : x_(x){}
    void memberFunc()
    {
        printf("Foo memberFunc tid=%d, Foo::x_=%f\n", YBASE::CurrentThread::tid(), x_);
    }
    void memberFunc2(const std::string& text)
    {
        printf("Foo memberFunc2 tid=%d, Foo::x_=%f, text=%s\n", YBASE::CurrentThread::tid(), x_, text.c_str());
    }
private:
    double x_;
};

int main()
{
    printf("pid=%d, tid=%d\n", ::getpid(), muduo::CurrentThread::tid());

    YBASE::Thread t1(threadFunc);
    t1.start();
    printf("t1.tid=%d\n", t1.tid());
    t1.join();

    YBASE::Thread t2(std::bind(threadFunc2, 42), "thread for free function with argument");
    t2.start();
    printf("t2.tid=%d\n", t2.tid());
    t2.join();

    Foo foo(87.53);
    YBASE::Thread t3(std::bind(&Foo::memberFunc, &foo), "thread for member function without argument");
    t3.start();
    t3.join();

    YBASE::Thread t4(std::bind(&Foo::memberFunc2, std::ref(foo), std::string("Yan")));
    t4.start();
    t4.join();

    {
        YBASE::Thread t5(threadFunc3);
        t5.start();
    }
    mysleep(2);
    {
        muduo::Thread t6(threadFunc3);
        t6.start();
        mysleep(2);
    }
    sleep(2);
    printf("number of created threads %d\n", muduo::Thread::numCreated());
}