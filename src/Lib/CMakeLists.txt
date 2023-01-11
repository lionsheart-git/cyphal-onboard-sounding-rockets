# Canard
add_library(Canard
        libcanard/libcanard/canard.h libcanard/libcanard/canard.c
        libcanard/libcanard/cavl.h)

target_include_directories(Canard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libcanard/libcanard/)

# O1Heap
add_library(O1Heap
        o1heap/o1heap/o1heap.h o1heap/o1heap/o1heap.c)

target_include_directories(O1Heap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/o1heap/o1heap)

# DSDL
add_library(Cyphal_DSDL cyphal_public_regulated_data_types_compiled)
set_target_properties(Cyphal_DSDL PROPERTIES LINKER_LANGUAGE C)

target_include_directories(Cyphal_DSDL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cyphal_public_regulated_data_types_compiled)