add_library(OpenCyphal
        OpenCyphal.cpp OpenCyphal.h
        CanardTransceiver.h
        Node.cpp Node.h
        CanardTransferReceiver.h
        Clock.cpp Clock.h
        PMessage.h
        PMessageHeartbeat.cpp PMessageHeartbeat.h
        Task.h Task.cpp
        THeartbeat.cpp THeartbeat.h)

target_include_directories(OpenCyphal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OpenCyphal Canard Cyphal_DSDL O1Heap Utility)