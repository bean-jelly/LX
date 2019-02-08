#include <iostream>
#include <thread>
#include <chrono>
#include "ltimer.h"

void TimerHandler()
{
    std::cout << "TimerHandler" << std::endl;
}

void TimerHandler1()
{
    std::cout << "TimerHanler1" << std::endl;
}

int main()
{
    TimerManager tm;
    Timer t(tm);
    t.Start(&TimerHandler, 2000);
    Timer t1(tm);
    t1.Start(&TimerHandler1, 1500);
    while(true)
    {
	std::cout << "waiting..." << std::endl;
        tm.DetectTimers();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cin.get();
    return 0;
}
