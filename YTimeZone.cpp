#include "YTimeZone.h"
#include "YNoncopyable.h"
#include "YDate.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <assert.h>
#include <endian.h>
#include <stdint.h>
#include <stdio.h>

namespace YBASE
{
    namespace detail
    {
        struct Transition
        {
            time_t gmttime;
            time_t localtime;
            int localtimeIdx;

            Transition(time_t t, time_t l, int localIdx) : gmttime(t), localtime(l), localtimeIdx(localIdx){} 
        };

        struct Comp
        {
            bool compareGmt;
            Comp(bool gmt) : compareGmt(gmt){}

            bool operator()(const Transition& lhs, const Transition& rhs)
            {
                if(compareGmt)
                    return lhs.gmttime < rhs.gmttime;
                else
                    return lhs.localtime < rhs.localtime;
            }

            bool equal(const Transition& lhs, const Transition& rhs) const
            {
                if(compareGmt)
                    return lhs.gmttime == rhs.gmttime;
                else
                    return lhs.localtime == rhs.localtime;
            }
        };

        
    }
}