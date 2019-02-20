#ifndef YBASE_NET_POLLER_H
#define YBASE_NET_POLLER_H

#include <map>
#include <vector>

#include <LX/base/YTimestamp.h>
#include <LX/net/YEventLoop.h>

namespace YBASE
{
    namespace net
    {
        class Channel;

        class Poller:noncopyable
        {
        public:
            typedef std::vector<Channel*> ChannelList;

            Poller(EventLoop* loop);
            virtual ~Poller();

            virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

            virtual void updateChannel(Channel* channel) = 0;

            virtual void removeChannel(Channel* channel) = 0;
            
            virtual bool hasChannel(Channel* channel) const;

            virtual Poller* newDefaultPoller(EventLoop* loop);

            void assertInLoopThread() const
            {
                ownerLoop->assertInLoopThread();
            }

        protected:
            typedef std::map<int, Channel*> ChannelMap;
            ChannelMap channels_;

        private:
            EventLoop* ownerLoop_;
        };
    }
}

#endif