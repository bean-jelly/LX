#ifndef YBASE_TYPES_H
#define YBASE_TYPES_H

#include <stdint.h>
#include <string>

#ifndef NDEBUG
#include <assert.t>
#endif

namespace YBASE
{
    using std::string;

    template<typename To, typename From>
    inline To implicit_cast(From const &f)
    {
        return f;
    }

    template<typename To, typename From>
    inline To down_cast(From *f)
    {
        if(false)
        {
            implicit_cast<From*, To>(0);
        }

        #if !defined(NDEBUG) && !define(GOOGLE_PROTOBUF_NO_RTTI)
        assert(f == NULL || dynamic_cast<To>(f) != NULL);
        #endif
        return static_cast<To>(f);
    }
}

#endif