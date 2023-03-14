add_executable(UnitTests
        ReferenceConfigurationTest.cpp ReferenceConfigurationTest.h
        SocketCanTest.h
        ReferenceValuesTest.cpp ReferenceValuesTest.h
        T104ChangeOfCommunicationInterfaceTest.cpp T104ChangeOfCommunicationInterfaceTest.h)

target_link_libraries(UnitTests gtest gtest_main SocketCAN TestUtilities OpenCyphal glog)