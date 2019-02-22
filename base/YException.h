#ifndef LX_EXCEPTION_H
#define LX_EXCEPTION_H

#include <LX/base/YTypes.h>
#include <exception>

namespace LX
{
    class Exception:public std::exception
    {
    public:
        explicit Exception(const char* what);
        explicit Exception(const string& what);
        virtual ~Exception() throw();
        virtual const char* what() const throw();
        const char* stackTrace() const throw();

    private:
        void fillStackTrace();

        string message_;
        string stack_;    
    };
}

#endif //LX_EXCEPTION_H