#include <LX/base/YException.h>

#include <execinfo.h>
#include <stdlib.h>

using namespace LX;

Exception::Exception(const char* msg) : message_(msg)
{
    fillStackTrace();
}

Exception::Exception(const string& msg) : message_(msg)
{
    fillStackTrace();
}

Exception::~Exception() throw()
{

}

const char* Exception::what() const throw()
{
    return message_.c_str();
}

const char* Exception::stackTrace() const throw()
{
    return stack_.c_str();
}

void Exception::fillStackTrace()
{
    const int len = 200;
    void* buffer[len];
    int nptrs = ::backtrace(buffer, len);                // backtrace 栈回溯，保存各个栈帧的地址
    char ** strings = ::backtrace_symbols(buffer, nptrs);// backtrace_symbols 根据地址，转成相应的函数符号
    if(strings)
    {
        for(int i = 0; i < nptrs; i++)
        {
            stack_.append(strings[i]);
            stack_.push_back('\n');
        }
        free(strings);
    }
}