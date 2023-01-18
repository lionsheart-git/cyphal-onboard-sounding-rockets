add_library(OpenCyphal OpenCyphal.cpp OpenCyphal.h CanardTransceiver.h Node.cpp Node.h CanardTransferReceiver.h)

target_include_directories(OpenCyphal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OpenCyphal Canard Cyphal_DSDL O1Heap)