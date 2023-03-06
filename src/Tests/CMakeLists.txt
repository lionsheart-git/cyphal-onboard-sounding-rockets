add_executable(UnitTests Tests.cpp
        Tests.h
        ReferenceConfigurationTest.cpp ReferenceConfigurationTest.h
        SocketCanTest.cpp SocketCanTest.h
        ReferenceValuesTest.cpp ReferenceValuesTest.h
        T104ChangeOfCommunicationInterfaceTest.cpp T104ChangeOfCommunicationInterfaceTest.h)

target_link_libraries(UnitTests GTest SocketCAN TestUtilities OpenCyphal glog)