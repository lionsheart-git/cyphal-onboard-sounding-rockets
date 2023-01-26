add_library(TestUtilities
        NodeFactory.h NodeFactory.cpp)

target_include_directories(TestUtilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(TestUtilities OpenCyphal)