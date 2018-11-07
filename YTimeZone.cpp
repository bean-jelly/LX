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
            time_t gmtime;
            time_t localtime;
            int localtimeIdx;

            
        }
    }
}