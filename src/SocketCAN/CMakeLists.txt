add_library(SocketCAN
        SocketCANTransceiver.h SocketCANTransceiver.cpp
        socketcan.h socketcan.c)

target_include_directories(SocketCAN PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SocketCAN Canard)