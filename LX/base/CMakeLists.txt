set(base_SRCS
    YCondition.cpp
    YCountDownLatch.cpp
    YDate.cpp
    YException.cpp
    YFileUtil.cpp
    YLogFile.cpp
    YLogging.cpp
    YLogStream.cpp
    YTimestamp.cpp
    YTimeZone.cpp
    YThread.cpp
    YThreadPool.cpp
    )

add_library(lx_base ${base_SRCS})
target_link_libraries(lx_base ypthread rt)

install(TARGETS lx_base DESTINATION lib)

file(GLOB HEADERS ".h")
install(FILES ${HEADERS} DESTINATION include/lx/base)