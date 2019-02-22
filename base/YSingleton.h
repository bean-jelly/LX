#ifndef LX_SINGLETON_H
#define LX_SINGLETON_H

/*
生产代码中的线程的等待可分为两种，一种是等待资源可用；一种是等待进入临界区（mutex上）以便读写数据，
通常后一种等待时间极短。
*/

#include <boost/noncopyable.hpp>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

namespace LX
{
    namespace detail
    {
        template<typename T>
        struct has_no_destroy
        {
        #ifdef __GXX_EXPERIMENTAL_CXX0X__
            template<typename C> static char test(decltype(&C::no_destroy));
        #else
            template<typename C> static char test(typeof(&C::no_destroy));
        #endif
            template<typename C> static int32_t test(...);
            const static bool value = sizeof(test<T>(0)) == 1;
        };
    }

    template<typename T>
    class Singleton:boost::noncopyable
    {
    public:
        static T& instance()
        {
            //只能调用一次init()
            pthread_once(&ponce_, &Singleton::init);
            assert(value != NULL);
            return *value_;
        }
    private:
        //将构造函数和析构函数设为private，则只能用instance()进行初始化
        Singleton();
        ~Singleton();

        static void init()
        {
            vlaue_ = new T();
            if(!detail::has_no_destroy<T>::value)
            {
                ::atexit(destroy);
            }
        }

        //无参数无返回的函数
        static void destroy()
        {
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type dummy;
            (void)dummy;
            
            delete value_;
            value_ = NULL;
        }

    private:
        static pthread_once_t   ponce_;
        static T*               value_;
    };

    template<typename T>
    pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

    template<typename T>
    T* Singleton<T>::value_ = NULL;
}

#endif