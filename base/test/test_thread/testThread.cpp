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
    printf("threadFunc tid=%d\n", LX::CurrentThread::tid());
}

void threadFunc2(int x)
{
    printf("threadFunc2 tid=%d, x=%d\n", LX::CurrentThread::tid(), x);
}

void threadFunc3()
{
    printf("before threadFunc3 tid=%d\n", LX::CurrentThread::tid());
    mysleep(1);
    printf("after threadFunc3 tid=%d\n", LX::CurrentThread::tid());
}

class Foo
{
public:
    explicit Foo(double x) : x_(x){}
    void memberFunc()
    {
        printf("Foo memberFunc tid=%d, Foo::x_=%f\n", LX::CurrentThread::tid(), x_);
    }
    void memberFunc2(const std::string& text)
    {
        printf("Foo memberFunc2 tid=%d, Foo::x_=%f, text=%s\n", LX::CurrentThread::tid(), x_, text.c_str());
    }
private:
    double x_;
};

int main()
{
    printf("pid=%d, tid=%d\n", ::getpid(), LX::CurrentThread::tid());

    LX::Thread t1(threadFunc);
    t1.start();
    printf("t1.tid=%d\n", t1.tid());
    t1.join();

    LX::Thread t2(std::bind(threadFunc2, 42), "thread for free function with argument");
    t2.start();
    printf("t2.tid=%d\n", t2.tid());
    t2.join();

    Foo foo(87.53);
    LX::Thread t3(std::bind(&Foo::memberFunc, &foo), "thread for member function without argument");
    t3.start();
    printf("t3.name=%s\n", t3.name().c_str());
    t3.join();

    LX::Thread t4(std::bind(&Foo::memberFunc2, std::ref(foo), std::string("Yan")));
    t4.start();
    printf("t4.name=%s\n", t4.name().c_str());
    t4.join();

    {
        LX::Thread t5(threadFunc3);
        t5.start();
        printf("t5.start()\n");
    }
    mysleep(2);
    {
        LX::Thread t6(threadFunc3);
        t6.start();
        printf("t6.start()\n");
        mysleep(2);
    }
    printf("prepare over\n");
    sleep(2);
    printf("number of created threads %d\n", LX::Thread::numCreated());
}