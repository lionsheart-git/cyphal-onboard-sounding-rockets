add_library(Canard
        libcanard/libcanard/canard.h libcanard/libcanard/canard.c
        libcanard/libcanard/cavl.h)

target_include_directories(Canard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libcanard/libcanard/)