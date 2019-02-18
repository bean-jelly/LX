#ifndef _REACTOR_LX_H
#define _REACTOR_LX_H

#include <list>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#define WORKER_THREAD_NUM 5

class MyReactor
{
public:
    MyReactor();
    ~MyReactor();

    bool init(const char* ip, short nport);
    bool uninit();

    bool close_client(int clientfd);

    static void* main_loop(void* p);
private:
    MyReactor(const MyReactor& rhs);
    MyReactor& operator=(const MyReactor& rhs);
    bool create_server_listener(const char* ip, short port);

    static void accept_thread_proc(MyReactor* pReactor);
    static void worker_thread_proc(MyReactor* pReactor);
private:
    int m_listenfd = 0;
    int m_epollfd = 0;
    bool m_bStop = false;
    std::shared_ptr<std::thread> m_acceptthread;
    std::shared_ptr<std::thread> m_workerthreads[WORKER_THREAD_NUM];
    std::condition_variable m_acceptcond;
    std::mutex m_acceptmutex;
    std::condition_variable m_workercond;
    std::mutex m_workermutex;
    std::list<int> m_listClients;
};

#endif