add_library(Canard
        libcanard/libcanard/canard.h libcanard/libcanard/canard.c
        libcanard/libcanard/cavl.h)

target_include_directories(Canard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libcanard/libcanard/)

add_library(O1Heap
        o1heap/o1heap/o1heap.h o1heap/o1heap/o1heap.c)

target_include_directories(Canard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/o1heap/o1heap)