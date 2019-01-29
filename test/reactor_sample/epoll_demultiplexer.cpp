#include <vector>
#include <sys/epoll.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include "epoll_demultiplexer.h"
/*
EPOLLIN：触发该事件，表示对应的文件描述符上有可读数据。(包括对端SOCKET正常关闭)；
EPOLLOUT：触发该事件，表示对应的文件描述符上可以写数据；
EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
EPOLLERR：表示对应的文件描述符发生错误；
EPOLLHUP：表示对应的文件描述符被挂断；
EPOLLET：将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监
*/
EpollDemultiplexer::EpollDemultiplexer() : _max_fd(0){
    _epoll_fd = epoll_create(1);
}

EpollDemultiplexer::~EpollDemultiplexer()
{
    close(_epoll_fd);
}

int EpollDemultiplexer::wait_event(std::map<Handle, EventHandler*>& handlers, int timeout)
{
    std::vector<struct epoll_event> events(_max_fd);
    //_epoll_fd是epoll_create产生的epoll fd
    //&events[0]为回传事件的数组
    //_max_fd为回传事件数组的最大值
    //timeout是超时返回,-1表示阻塞,0表示非阻塞,>0表示超时时间
    //返回值表示返回事件的多少
    //events是一个vector,原来容器是可以这样用，直接赋值首个元素的地址
    int num = epoll_wait(_epoll_fd, &events[0], _max_fd, timeout);
    if(num < 0)
    {
        return num;
    }
    for(int i = 0; i < num; ++i)
    {
        Handle handle = events[i].data.fd;
        if((EPOLLHUP|EPOLLERR) & events[i].events)
        {
            return -1;
        }
        else
        {
            if((EPOLLIN) & events[i].events)
            {
                assert(handlers[handle] != NULL);
                (handlers[handle])->handle_read();
            }
            if(EPOLLOUT & events[i].events)
            {
                (handlers[handle])->handle_write();
            }
        }
    }
    return num;
}

int EpollDemultiplexer::regist(Handle handle, Event evt)
{
    struct epoll_event ev;
    ev.data.fd = handle;
    if(evt & ReadEvent)
    {
        ev.events |= EPOLLIN;
    }
    if(evt & WriteEvent)
    {
        ev.events |= EPOLLOUT;
    }
    ev.events |= EPOLLET;
    if(epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, handle, &ev) != 0)
    {
        if(errno == ENOENT)
        {
            if(epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, handle, &ev) != 0)
            {
                std::cerr << "epoll_ctl add error " << errno << std::endl;
                return -errno;
            }
            ++_max_fd;
        }
        else
        {
            ++_max_fd;
        }
    }
    return 0;
}

int EpollDemultiplexer::remove(Handle handle)
{
    struct epoll_event ev;
    if(epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, handle, &ev) != 0)
    {
        std::cerr << "epoll_ctl del error" << errno << std::endl;
        return -errno;
    }
    --_max_fd;
    return 0;
}
