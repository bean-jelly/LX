#include "Socker.h"

vector<int> CSocker::sessions;
int CSocker::maxnums = 0;
extern char localip[];

CSocker::CSocker()
{

}

CSocker::~CSocker()
{

}

int CSocker::ServerListen(int fd, int port)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(0 != bind(fd, (struct sockaddr*)&addr, sizeof addr))
    {
        return -1;
    }

    if(0 != listen(fd, MAX_LISTEN_QUEUE))
    {
        return -1;
    }

    return 0;
}

int CSocker::ServerAccept(int fd)
{
    struct sockaddr_in clientaddr;
    socklen_t clilen = sizeof clientaddr;
    int connfd;

    connfd = accept(fd, (struct sockaddr*)&clientaddr, &clilen);
    if(connfd < 0)
    {
        return -1;
    }
    else
    {
        return connfd;
    }
}

int CSocker::SocketSend(int fd, void* buf, size_t len)
{
    int ret;
    ret = send(fd, buf, len, 0);
    if(ret < 0)
    {
        return ret;
    }
    else if(ret > 0)
    {
        return ret;
    }
    return ret;
}

int CSocker::SocketRecv(int fd, void* buf, size_t len)
{
    int ret = recv(fd, buf, len, 0);
    if(ret < 0)
    {
        return ret;
    }
    else if(ret == 0)
    {
        return ret;
    }
    else if(ret > 0)
    {
        return ret;
    }
    return ret;
}

void CSocker::SocketClose(int fd)
{
    close(fd);
}

int CSocker::ClientConnect(int fd, const char* ip, int port)
{
    struct sockaddr_in remote;
    bzero(&remote, sizeof remote);
    remote.sin_family = AF_INET;
    remote.sin_port = htons(port);
    remote.sin_addr.s_addr = inet_addr(ip);

    if(0 != connect(fd, (struct sockaddr*)&remote, sizeof remote))
    {
        return -1;
    }
    else
    {
        ;
    }

    struct sockaddr_in host;
    socklen_t llen = sizeof host;
    bzero(&host, sizeof host);
    if(0 == getpeername(fd, (struct sockaddr*)&host, &llen))
    {
        strncpy(localip, inet_ntoa(host.sin_addr), 19);
    }
    return 0;
}

int CSocker::SocketNoBlock(int fd)
{
    int opts = fcntl(fd, F_GETFL);
    if(opts < 0)
    {
        return -1;
    }
    opts = opts | O_NONBLOCK;
    if(fcntl(fd, F_SETFL, opts) < 0)
    {
        return -1;
    }
    return 0;
}

int CSocker::SocketReUse(int fd)
{
    int opt = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
}

int CSocker::SockerInit()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > fd)
    {
        exit(EXIT_FAILURE);
    }
    return fd;
}

int CSocker::ConnNoBlock(int fd, const char* ip, int port)
{
    struct sockaddr_in remote;
    bzero(&remote, sizeof remote);
    remote.sin_family = AF_INET;
    remote.sin_port = htons(port);
    remote.sin_addr.s_addr = inet_addr(ip);

    if(fd > 0)
    {
        if(0 == CSocker::SocketNoBlock(fd))
        {
            if(0 > connect(fd, (struct sockaddr*)&remote, sizeof(remote)))
            {
                if(errno != EINPROGRESS)
                {
                    return -1;
                }
            }
            CSocker::sessions.push_back(fd);
            CSocker::maxnums++;
            return 0;
        }
        else
        {
            ;
        }
    }
    else
    {
        ;
    }
    return -1;
}

int CSocker::WaitForConnect(int seconds)
{
    if(CSocker::maxnums <= 0)
    {
        CSocker::maxnums = 0;
        return 0;
    }
    int maxfd = 0;
    int error = 0;
    socklen_t errlen = sizeof error;

    fd_set readset;
    fd_set writeset;

    FD_ZERO(&readset);
    FD_ZERO(&writeset);

    vector<int>::iterator iter = CSocker::sessions.begin();
    for(; iter != CSocker::sessions.end(); ++iter)
    {
        FD_SET(*iter, &readset);
        FD_SET(*iter, &writeset);
        if(maxfd < *iter)
        {
            maxfd = *iter;
        }
    }

    struct timeval tval;
    tval.tv_sec = seconds;
    tval.tv_usec = 0;

    int n = 0;
    if(0 == (n = select(maxfd + 1, &readset, &writeset, NULL, seconds ? &tval : NULL)))
    {
        for(iter = CSocker::sessions.begin(); iter != CSocker::sessions.end(); ++iter)
		{
			close(*iter);
		}
		CSocker::sessions.clear();
		CSocker::maxnums = 0;
		return 0;
    }
    else if(n < 0)	//如果小于零
	{
		return -1;
	}

    for(iter = CSocker::sessions.begin(); iter != CSocker::sessions.end();)
	{
		if(FD_ISSET(*iter, &readset) || FD_ISSET(*iter, &writeset))
		{
			//如果错误
			if(getsockopt(*iter, SOL_SOCKET, SO_ERROR, &error, &errlen) < 0)
			{
				iter = CSocker::sessions.erase(iter);
				--CSocker::maxnums;
				close(*iter);
			}
			else
			{
				int rtfd = *iter;
				iter = CSocker::sessions.erase(iter);
				--CSocker::maxnums;
				if(error != 0)
				{
					;
				}
				else
				{
					return rtfd;
				}				
			}
			//此时已经处理了并更新了该迭代器，不可以让迭代器自增
			continue;
		}
		
		++iter;
	}

	return 0;
}