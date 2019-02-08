#include "listen_handler.h"
#include "reactor.h"
#include "socket_handler.h"
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <new>
#include <assert.h>
#include <iostream>

ListenHandler::ListenHandler(Handle fd) : _listen_fd(fd){}

ListenHandler::~ListenHandler()
{
    close(_listen_fd);
}

void ListenHandler::handle_read()
{
    std::cout << "listenhandler handle_read()" << std::endl;
    int acceptfd = accept(_listen_fd, NULL, NULL);
    int oldflag = ::fcntl(acceptfd, F_GETFL, 0);
    int newflag = oldflag | O_NONBLOCK;
    if(::fcntl(acceptfd, F_SETFL, newflag) == -1)
    {
        std::cout << "fcntl error, oldflag =" << oldflag <<", newflag = " << newflag << std::endl;
    }
    EventHandler* h = new(std::nothrow)SocketHandler(acceptfd);
    assert(h != NULL);
    Reactor& r = Reactor::get_instance();
    r.regist(h, ReadEvent);
}

void ListenHandler::handle_write()
{

}

void ListenHandler::handle_error()
{
    
}
