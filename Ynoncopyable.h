#ifndef YBASE_NONCOPYABLE_H
#define YBASE_NONCOPYABLE_H

namespace YBASE
{
    class noncopyable
    {
    private:
        noncopyable(const noncopyable&)=delete;
        void operator=(const noncopyable&)=delete;

    protected:
        noncopyable()=default;
        ~noncopyable()=default;
    };
}

#endif