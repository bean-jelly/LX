#include "event_handler.h"
#include "event_demultiplexer.h"
#include <map>

class EpollDemultiplexer : public EventDemultiplexer
{
public:
    EpollDemultiplexer();
    virtual ~EpollDemultiplexer();
    virtual int wait_event(std::map<Handle, EventHandler*>& handlers, int time_out=0);
    virtual int regist(Handle handle, Event evt);
    virtual int remove(Handle handle);
private:
    int _max_fd;
    int _poll_fd;
};