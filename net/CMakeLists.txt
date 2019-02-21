set(net_SRCS
    YBuffer.cpp
    YChannel.cpp
    YEventLoop.cpp
    YInetAddress.cpp
    YSocket.cpp
    YPoller.cpp
    YSocketsOps.cpp
    YTcpConnection.cpp
    YTimer.cpp
    YTimerQueue.cpp
    poller/DefaultPoller.cc
    poller/YEPollPoller.cpp
)

add_library(lx_net ${net_SRCS})
target_link_libraries(lx_net lx_base)

install(TARGETS lx_net DESTINATION lib)

set(HEADERS
    YBuffer.h
    YCallbacks.h
    YChannel.h
    YEndian.h
    YEventLoop.h
    YInetAddress.h
    YTcpConnection.h
    YTimerId.h
  )

  install(FILES ${HEADERS} DESTINATION include/lx/net)