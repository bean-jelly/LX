all = server client

source = epoll_demultiplexer.cpp listen_handler.cpp reactor_impl.cpp reactor.cpp socket_handler.cpp

head = epoll_demultiplexer.h event_demultiplexer.h event_handler.h event.h listen_handler.h reactor_impl.h reactor.h socket_handler.h

server : $(source) server.cpp
	g++ -o server &(source) server.cpp

client : &(source) client.cpp
	g++ -o client $(source) client.cpp

.PHONY: all clean
