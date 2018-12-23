#include <iostream>
#include <thread>
#include <chrono>
#include "ltimer.h"

void TimerHandler()
{
    std::cout << "TimerHandler" << std::endl;
}

int main()
{
    TimerManager tm;
    Timer t(tm);
    t.Start(&TimerHandler, 1000);
    while(true)
    {
        tm.DetectTimers();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cin.get();
    return 0;
}