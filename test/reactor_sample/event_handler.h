#ifndef R_EVENTHANDLER_H
#define R_EVENTHANDLER_H

#include <unistd.h>
#include <assert.h>

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
