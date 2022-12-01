cmake_minimum_required(VERSION 3.22)
project(SocketCAN VERSION 0.1
        DESCRIPTION "Simulation of a Cyphal netowrk with linux-can / SocketCAN"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
