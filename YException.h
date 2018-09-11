#ifndef YBASE_EXCEPTION_H
#define YBASE_EXCEPTION_H

#include "YTypes.h"
#include <exception>

namespace YBASE
{
    class Exception:public std::exception
    {
    public:
        explicit Exception(const char* what);
        explicit Exception(const string& what);
        virtual const char* what() const throw();
        const char* stackTrace() const throw();

    private:
        void fillStackTrace();

        string message_;
        string stack_;    
    };
}

#endif //YBASE_EXCEPTION_H