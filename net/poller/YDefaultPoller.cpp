#include <LX/net/YPoller.h>
//#include <LX/net/poller/YPollPoller.h>
#include <LX/net/poller/YEPollPoller.h>

#include <stdlib.h>

using namespace LX::net;

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    return new EPollPoller(loop);
}
