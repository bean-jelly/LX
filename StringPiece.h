#ifndef YBASE_STRINGPIECE_H
#define YBASE_STRINGPIECE_H

#include <string.h>
#include <iosfwd>

#include "YTypes.h"

namespace YBASE
{
    class StringArg
    {
    public:
        StringArg(const char* str) : str_(str){}
    public:
        StringArg(const string& str) : str_(str.c_str()){}

    const char* c_str() const {return str_;}
    private:
        const char* str_;
    };

    class StringPiece
    {
    private:
        const char* ptr_;
        int         length_;
    };
}

#endif