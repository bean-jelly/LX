#ifndef YBASE_NONCOPYABLE_H
#define YBASE_NONCOPYABLE_H

namespace YBASE
{
    class noncopyable
    {
    protected:
        noncopyable()=default;
        ~noncopyable()=default;
    
    private:
        noncopyable(const noncopyable&)=delete;
        void operator=(const noncopyable&)=delete;
    };
}

#endif