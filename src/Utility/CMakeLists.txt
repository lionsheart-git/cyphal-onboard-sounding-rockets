add_library(Utility
        Macros.h)

target_include_directories(Utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(Utility PROPERTIES LINKER_LANGUAGE CXX)

# target_link_libraries(Utility)