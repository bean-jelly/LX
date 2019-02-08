#include "socket_handler.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <new>

#include "reactor.h"

SocketHandler::SocketHandler(Handle fd) : _socket_fd(fd)
{
    _buf = new (std::nothrow)char[MAX_SIZE];
    assert(_buf != NULL);
    memset(_buf, 0, MAX_SIZE);
}

SocketHandler::~SocketHandler()
{
    close(_socket_fd);
    delete[] _buf;
}

void SocketHandler::handle_read()
{
    printf("SocketHandler::handle_read()\n");
    if(read(_socket_fd, _buf, MAX_SIZE) > 0)
    {
	printf("%s\n", _buf);
        write(_socket_fd, _buf, strlen(_buf));
    }
    printf("read error\n");
    handle_error();
}

void SocketHandler::handle_write()
{
    
}

void SocketHandler::handle_error (){
    Reactor &r = Reactor::get_instance();
    r.remove(this);
}
