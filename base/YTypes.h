#ifndef LX_TYPES_H
#define LX_TYPES_H

#include <stdint.h>
#include <string.h>
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

namespace LX
{
    using std::string;

    inline void memZero(void* p, size_t n)
    {
        memset(p, 0, n);
    }

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
        return static_cast<To>(f);
    }
}

#endif