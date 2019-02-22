#ifndef LX_LOGGING_H
#define LX_LOGGING_H

#include <LX/base/YLogStream.h>
#include <LX/base/YTimestamp.h>

namespace LX
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

        Logger(SourceFile file, int line);
        Logger(SourceFile file, int line, LogLevel level);
        Logger(SourceFile file, int line, LogLevel level, const char* func);
        Logger(SourceFile file, int line, bool toAbort);
        ~Logger();

        LogStream& stream() {return impl_.stream_;}

        static LogLevel logLevel();
        static void setLogLevel(LogLevel level);
        
        typedef void (*OutputFunc)(const char* msg, int len);
        typedef void (*FlushFunc)();
        static void setOutput(OutputFunc);
        static void setFlush(FlushFunc);
        static void setTimeZone(const TimeZone& tz);

    private:

        class Impl
        {
        public:
            typedef Logger::LogLevel LogLevel;
            Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
            void formatTime();
            void finish();

            Timestamp time_;
            LogStream stream_;
            LogLevel level_;
            int line_;
            SourceFile basename_;
        };

        Impl impl_;
    };

    extern Logger::LogLevel g_logLevel;
    inline Logger::LogLevel Logger::logLevel()
    {
        return g_logLevel;
    }

    //非常的关键，这里的宏用Logger(__FILE__,__LINE__,XX)创建一个临时对象来输出，输出完了
    //可以立即销毁，不然的话，后打印的log会先输出，因为退出生命周期后，先声明的对象最后销毁
    #define LOG_TRACE if(LX::Logger::logLevel() <= LX::Logger::TRACE) \
        LX::Logger(__FILE__, __LINE__, LX::Logger::TRACE, __func__).stream()
    #define LOG_DEBUG if(LX::Logger::logLevel() <= LX::Logger::DEBUG) \
        LX::Logger(__FILE__, __LINE__, LX::Logger::DEBUG, __func__).stream()
    #define LOG_INFO if(LX::Logger::logLevel() <= LX::Logger::INFO) \
        LX::Logger(__FILE__, __LINE__).stream()
    #define LOG_WARN LX::Logger(__FILE__, __LINE__, LX::Logger::WARN).stream()
    #define LOG_ERROR LX::Logger(__FILE__, __LINE__, LX::Logger::ERROR).stream()
    #define LOG_FATAL LX::Logger(__FILE__, __LINE__, LX::Logger::FATAL).stream()
    #define LOG_SYSERR LX::Logger(__FILE__, __LINE__, false).stream()
    #define LOG_SYSFATAL LX::Logger(__FILE__, __LINE__, true).stream()

    const char* strerror_tl(int savedErrno);


    //check that the input is non null.
    #define CHECK_NOTNULL(val) \
        ::LX::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

    template<typename T>
    T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr)
    {
        if(ptr == NULL)
        {
            Logger(file, line, Logger::FATAL).stream() << names;
        }
        return ptr;
    }
}

#endif