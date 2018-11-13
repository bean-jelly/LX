#ifndef R_REACTORIMPL_H
#define R_REACTORIMPL_H

#include "event.h"
#include "event_handler.h"
#include "event_demultiplexer.h"
#include <map>

class ReactorImpl
{
public:
    ReactorImpl();
    ~ReactorImpl();
    int regist(EventHandler* handler, Event evt);
    void remove(EventHandler* handler);
    void dispatch(int timeout=0);
private:
    EventDemultiplexer* _demultiplexer;
    std::map<Handle, EventHandler*> _handlers;
};

#endif
