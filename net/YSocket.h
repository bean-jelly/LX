#ifndef YBASE_NET_SOCKET_H
#define YBASE_NET_SOCKET_H

#include "../YNoncopyable.h"

struct tcp_info;

namespace YBASE
{
    namespace net
    {
        class InetAddress;

        class Socket : noncopyable
        {
        public:
            explicit Socket(int sockfd) : sockfd_(sockfd){}
            ~Socket();

            int fd() const {return sockfd_;}
            bool getTcpInfo(struct tcp_info*) const;
            bool getTcpInfoString(char* buf, int len) const;
            void bindAddress(const InetAddress& localaddr);
            void listen();
            int accept(InetAddress* peeraddr);
            void shutdownWrite();
            
            //enable/disable TCP_NODELAY
            void setTcpNoDelay(bool on);
            //enable/disable SO_REUSEADDR
            void setReuseAddr(bool on);
            //enable/disable SO_REUSEPORT
            void setReusePort(bool on);
            //enable/disable SO_KEEPALIVE
            void setKeepAlive(bool on);

        private:
            const int sockfd_;
        };
    }
}

#endif