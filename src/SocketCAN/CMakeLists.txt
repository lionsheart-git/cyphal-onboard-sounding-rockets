add_library(SocketCAN
        SocketCANTransceiver.h SocketCANTransceiver.cpp)

target_include_directories(SocketCAN PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})