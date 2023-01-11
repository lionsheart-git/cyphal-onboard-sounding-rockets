cmake_minimum_required(VERSION 3.22)
project(SocketCAN
        VERSION 0.1
        DESCRIPTION "Simulation of a Cyphal netowrk with linux-can / SocketCAN"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -pedantic -fstrict-aliasing")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdouble-promotion -Wswitch-enum -Wfloat-equal -Wundef")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion -Wtype-limits")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wsign-conversion -Wcast-align -Wmissing-declarations")

# Forward the revision information to the compiler so that we could expose it at runtime. This is entirely optional.
execute_process(
COMMAND git rev-parse --short=16 HEAD
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
OUTPUT_VARIABLE vcs_revision_id
OUTPUT_STRIP_TRAILING_WHITESPACE
)

# For remote deploys the .git directory is sometimes not transferred.
# To build anyways the id is set to 1 and not an empty string.
IF(vcs_revision_id MATCHES "")
    set(vcs_revision_id 1)
ENDIF()

message(STATUS "vcs_revision_id: ${vcs_revision_id}")
add_definitions(
-DVERSION_MAJOR=0
-DVERSION_MINOR=1
-DVCS_REVISION_ID=0x${vcs_revision_id}ULL
-DNODE_NAME="org.icarus.test"
)

add_definitions(-DNUNAVUT_ASSERT=assert)

add_subdirectory(src)
