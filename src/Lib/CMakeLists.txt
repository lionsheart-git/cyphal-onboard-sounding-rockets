# Canard
add_library(Canard
        libcanard/libcanard/canard.h libcanard/libcanard/canard.c
        libcanard/libcanard/cavl.h)
set_target_properties(Canard PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Canard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libcanard/libcanard/)

# O1Heap
add_library(O1Heap
        o1heap/o1heap/o1heap.h o1heap/o1heap/o1heap.c)

target_include_directories(O1Heap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/o1heap/o1heap)

# DSDL
add_library(Cyphal_DSDL cyphal_public_regulated_data_types_compiled)
set_target_properties(Cyphal_DSDL PROPERTIES LINKER_LANGUAGE C)

target_include_directories(Cyphal_DSDL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cyphal_public_regulated_data_types_compiled)

# GoogleTest
add_subdirectory(googletest)
add_library(GTest ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(GTest gtest gtest_main)
set_target_properties(GTest PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(GTest PUBLIC ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# GFlags
# add_subdirectory(gflags)

# Google Benchmark
# add_subdirectory(benchmark)

# Glog
add_subdirectory(glog)
set(ENV{GLOG_logtostderr} 1)
set(ENV{GLOG_log_dir} LOG_DIRECTORY)
