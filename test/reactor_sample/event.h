#ifndef R_EVENT_H
#define R_EVENT_H

#include <unistd.h>

typedef unsigned int Event;

enum EventMask{
    ReadEvent = 0x01,
    WriteEvent = 0x02,
    ErrorEvent = 0x04,
    EventMask = 0xFF
};

#endif
