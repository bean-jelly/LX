#ifndef _SOCKER_H_
#define _SOCKER_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>

#include <vector>
#include <iostream>

using namespace std;

#define MAX_LISTEN_QUEUE 100000
#define MAX_HOSTNAME 20

class CSocker
{
public:
    static vector<int> sessions;
    static int maxnums;
public:
    CSocker();
    ~CSocker();
public:
    static int ServerListen(int fd, int port);

    static int ServerAccept(int fd);

    static int SocketSend(int fd, void* buf, size_t len);

    static int SocketRecv(int fd, void* buf, size_t len);

    static void SocketClose(int fd);

    static int ClientConnect(int fd, const char* ip, int port);

    static int SocketNoBlock(int fd);

    static int SocketReUse(int fd);

    static int SocketInit(void);

    static int ConnNoBlock(int fd, const char* ip, int port);

    static int WaitForConnect(int seconds);
};