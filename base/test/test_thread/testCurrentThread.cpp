#include "YCurrentThread.h"

int main()
{
    __thread LX::CurrentThread::t_cachedTid = 0;
    return 0;
}
