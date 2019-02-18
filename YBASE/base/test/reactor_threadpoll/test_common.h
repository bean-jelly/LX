#ifndef _TEST_COMMON_H_
#define _TEST_COMMON_H_

#include "reactor.h"

extern bool IsValidHandle(reactor::handle_t handle)
{
    return handle >= 0;
}

extern void ReportSocketError(const char * msg)
{
    fprintf(stderr, "%s error: %s\n", msg, strerror(errno));
}

#endif