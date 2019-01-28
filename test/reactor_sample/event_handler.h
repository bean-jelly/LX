#ifndef R_EVENTHANDLER_H
#define R_EVENTHANDLER_H

#include <unistd.h>
#include <assert.h>
#include <fcntl.h>

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

typedef int Handle;

class EventHandler
{
public:
    EventHandler(){}
    virtual ~EventHandler() {}
    virtual Handle get_handle() const = 0;
    virtual void handle_read() = 0;
    virtual void handle_write() = 0;
    virtual void handle_error() = 0;
};

#endif
