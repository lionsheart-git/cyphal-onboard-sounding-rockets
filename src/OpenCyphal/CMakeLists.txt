add_library(OpenCyphal Canard.cpp Canard.h)

target_include_directories(OpenCyphal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OpenCyphal Canard Cyphal_DSDL)