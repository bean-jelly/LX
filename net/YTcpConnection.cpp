#include <net/YTcpConnection.h>
#include <YLogging.h>
#include <YWeakCallback.h>
#include <net/YChannel.h>
#include <net/YEventLoop.h>
#include <net/YSocket.h>
#include <net/YSocketsOps.h>

#include <errno.h>

using namespace YBASE;
using namespace YBASE::net;

void YBASE::net::defaultConnectionCallback(const TcpConnectionPtr& conn)
{
    LOG_TRACE << conn->localAddress().toIpPort() << " ->"
              << conn->perrAddress().toIpPort() << " is"
              << (conn->connected() ? "UP" : "DOWN");
}

void YBASE::net::defaultMessageCallback(const TcpConnectionPtr&, Buffer* buf, Timestamp)
{
    buf->retrieveAll();
}

TcpConnection::TcpConnection(EventLoop* loop, const string& nameArg,
                             int sockfd, const InetAddress& localAddr
                             const InetAddress& peerAddr)
    : loop_(CHECK_NOTNULL(loop)),
    state_(kConnecting),
    reading_(true),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr),
    highWaterMark_(64*1024*1024)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));

    LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this << " fd=" << sockfd;
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
            << " fd=" << channel_->fd()
            << " state=" << stateToString();
    assert(state_ == kDisconnected);
}

bool TcpConnection::getTcpInfo(struct tcp_info* tcpi) const
{
    return socket_->getTcpInfo(tcpi);
}

string TcpConnection::getTcpInfoString() const
{
    char buf[1024];
    buf[0] = '\0';
    socket_->getTcpInfoString(buf, sizeof buf);
    return buf;
}

void TcpConnection::send(const void* data, int len)
{
    if(state_ ==)
}