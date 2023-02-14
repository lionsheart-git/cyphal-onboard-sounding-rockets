add_library(OpenCyphal
        OpenCyphal.cpp OpenCyphal.h
        CanardTransceiver.h
        Node.h Node.cpp
        CanardTransferReceiver.h
        Clock.cpp Clock.h
        IPMessage.h
        PMessageHeartbeat.cpp PMessageHeartbeat.h
        Task.h Task.cpp
        THeartbeat.cpp THeartbeat.h
        SMessage.cpp SMessage.h
        SMessageGetInfo.h SMessageGetInfo.cpp
        PMessageByteArray.cpp PMessageByteArray.h
        TByteArray.cpp TByteArray.h
        PUavcanPrimitiveEmpty.cpp PUavcanPrimitiveEmpty.h
        TPrimitiveEmpty.cpp TPrimitiveEmpty.h
        SRequestPrimitiveEmpty.cpp SRequestPrimitiveEmpty.h
        SResponsePrimitiveEmpty.cpp SResponsePrimitiveEmpty.h)

target_include_directories(OpenCyphal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OpenCyphal Canard Cyphal_DSDL O1Heap Utility)