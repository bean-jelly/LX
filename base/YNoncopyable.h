#ifndef LX_NONCOPYABLE_H
#define LX_NONCOPYABLE_H

namespace LX
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