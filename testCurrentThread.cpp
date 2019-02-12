#include "YCurrentThread.h"

int main()
{
    __thread YBASE::CurrentThread::t_cachedTid = 0;
    return 0;
}
