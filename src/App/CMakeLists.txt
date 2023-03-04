# Executables

## Main executable
add_executable(main main.cpp)
target_link_libraries(main OpenCyphal TestUtilities SocketCAN O1Heap Cyphal_DSDL glog)