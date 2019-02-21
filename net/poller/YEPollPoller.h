#ifndef YBASE_NET_POLLER_EPOLLPOLLER_H
#define YBASE_NET_POLLER_EPOLLPOLLER_H

#include <LX/net/YPoller.h>

#include <vector>

struct epoll_event;
/*
typedef union epoll_data {
    void *ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;

struct epoll_event {
    __uint32_t events;      //Epoll events
    epoll_data_t data;      //User data variable
};
*/

namespace YBASE
{
    namespace net
    {
        class EPollPoller : public Poller
        {
        public:
            EPollPoller(EventLoop* loop);
            virtual ~EPollPoller();

            virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
            virtual void updateChannel(Channel* channel);
            virtual void removeChannel(Channel* channel);
        private:
            static const int kInitEventListSize = 16;
            static const char* operationToString(int op);
            void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
            void update(int operation, Channel* channel);

            typedef std::vector<struct epoll_event> EventList;

            int epollfd_;
            EventList events_;
        };
    }
}

#endif