#include "listen_handler.h"
#include "event_handler.h"
#include "reactor.h"
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <new>
#include <assert.h>


ListenHandler::ListenHandler(Handle fd) : _listen_fd(fd){}

ListenHandler::~ListenHandler()
{
    close(_listen_fd);
}

void ListenHandler::handle_read()
{
    int fd = accept(_listen_fd, NULL, NULL);
    EventHandler* h = new(std::nothrow)SocketHandler(fd);
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