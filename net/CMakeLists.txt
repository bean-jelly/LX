set(net_SRCS
    YBuffer.cpp
    YChannel.cpp
    YEventLoop.cpp
    YInetAddress.cpp
    YSocket.cpp
    YSocketsOps.cpp
    YTcpConnection.cpp
    YTimer.cpp
    YTimerQueue.cpp
)

add_library(lx_net &{net_SRCS})
target_link_libraries(lx_net lx_base)

install(TARGETS lx_net DESTINATION lib)

set(HEADERS
    Buffer.h
    Callbacks.h
    Channel.h
    Endian.h
    EventLoop.h
    InetAddress.h
    TcpConnection.h
    TimerId.h
  )

  install(FILES ${HEADERS} DESTINATION include/lx/net)