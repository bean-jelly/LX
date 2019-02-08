#include "sys/socket.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "reactor.h"
#include "event_handler.h"
#include "listen_handler.h"
#include "event.h"

using namespace std;

int main()
{
    int socketfd = -1;
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "socket error" << errno << std::endl;
        exit(-1);
    }

    std::cout << "ready to connect" << std::endl;

    struct sockaddr_in seraddr;
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(53031);
    seraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // TODO

    if (connect(socketfd, (struct sockaddr*)&seraddr, sizeof(seraddr)) < 0) {
        std::cerr << "connect error " << errno << std::endl;
        exit(-2);
    }

    std::cout << "ready to write" << std::endl;
 
    char wbuf[64] = {0};
    while(1)
    {
        memset(wbuf, 0, 64);
        fgets(wbuf, 64, stdin);
    	int n = write(socketfd, wbuf, strlen(wbuf));
        std::cout << "write socketfd: " << socketfd << " n: " << n << std::endl;
        char rbuf[64] = {0};
        memset(rbuf, 0, sizeof(rbuf));
        n = read(socketfd, rbuf, sizeof(rbuf));
        std::cout << "send [" << wbuf << "] reply [" << rbuf << "]" << std::endl;
        if (n < 0) {
            std::cerr << "read error " << errno << std::endl;
            exit(-3);
        }
    }
    close(socketfd);
    return 0;

    /*Reactor& actor = Reactor::get_instance();
    EventHandler* handler = new ListenHandler(socketfd);
    actor.regist(handler, ReadEvent);

    while(true) {
        actor.dispatch(-1);
        std::cout << "client one loop" << std::endl;
    }

    return 0;*/
}
