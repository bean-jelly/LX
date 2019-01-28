#include "listen_handler.h"
#include "reactor.h"
#include "socket_handler.h"
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <new>
#include <assert.h>

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

ListenHandler::ListenHandler(Handle fd) : _listen_fd(fd){}

ListenHandler::~ListenHandler()
{
    close(_listen_fd);
}

void ListenHandler::handle_read()
{
    int fd = accept(_listen_fd, NULL, NULL);
    EventHandler* h = new(std::nothrow)SocketHandler(setnonblocking(fd));
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