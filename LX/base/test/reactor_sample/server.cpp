#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <errno.h>
#include "reactor.h"
#include "event_handler.h"
#include "listen_handler.h"

int main() {
    int listenfd = -1;
    if ((listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
        std::cerr << "socket error " << errno << std::endl;
        return -1;
    }
    
    int on = 1;
    ::setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof on);
    ::setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof on);
    
    struct sockaddr_in seraddr;
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(53031);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&seraddr, sizeof(seraddr)) < 0) {
        std::cerr << "bind error " << errno << std::endl;
        return -2;
    }

    if (listen(listenfd, 5) < 0) {
        std::cerr << "listen error " << errno << std::endl;
        return -3;
    }

    Reactor& actor = Reactor::get_instance();
    EventHandler* handler = new ListenHandler(listenfd);
    actor.regist(handler, ReadEvent);

    actor.dispatch(-1);
    std::cout << "actor.dispatch" << std::endl;
    while(true){
	;
    }
    
    return 0;
}
