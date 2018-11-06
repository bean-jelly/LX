#ifndef YBASE_LOGGING_H
#define YBASE_LOGGING_H

#include "LogStream.h"
#include "Timestamp.h"

namespace YBASE
{
    class TimeZone;

    class Logger
    {
    public:
        enum LogLevel
        {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_LOG_LEVELS,
        };
        class SourceFile
        {
        public:
            //将N当作常数来传进使用
            template<int N>
            SourceFile(const char (&arr)[N]):data_(arr), size_(N-1)
            {
                const char* slash = strrchr(data_, '/');
                if(slash)
                {
                    data_ = slash + 1;
                    size_ -= static_cast<int>(data_ - arr);
                }
            }

            explicit SourceFile(const char* filename):data_(filename)
            {
                const char* slash = strrchr(filename, '/');
                if(slash)
                {
                    data_ = slash + 1;
                }
                size_ = static_cast<int>(strlen(data_));
            }

            const char* data_;
            int size_;
        };

        
    }
}

#endif