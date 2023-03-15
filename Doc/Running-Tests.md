# Running the tests

## Setting up the environment

Make sure you're system has the vcan (SocketCAN) kernel module, otherwise it can not be loaded.

1. Make sure alle prerequisites are installed and working.
2. Run the `start-vcans.sh`. This enables the vcan kernel module and creates `vcan0`, `vcan1` and `vcan2`. Otherwise
   run:
    1. sudo modprobe vcan
    2. For all virtual can busses that should be created run.
        1. sudo ip link add dev <name-of-can-bus> type vcan fd on
        2. sudo ip link set up <name-of-can-bus>
3. Configure the yakut monitoring tool for the current configuration. This can be done by
   running `. export-yakut-variables.sh`
   or manually with the following steps:
    1. Select all the vcans that should be monitored by setting the environment variable
       `export UAVCAN__CAN__IFACE='socketcan:vcan0 socketcan:vcan1 socketcan:vcan2`
    2. Choose use of CAN-FD packets with `export UAVCAN__CAN__MTU=64`
    3. Set the node id of the terminal. The node id 0 can be chosen safely, otherwise conflicts with tests might occure.
       `export UAVCAN__NODE__ID=0`
4. Run the yakut monitor with the command `yakut monitor`.
5. In a different terminal, run `candump -decaxta any` for monitoring the can traffic.

The output of the monitoring tools can be captured like so:

```text
y --tsvh mon > <name-of-file>.log
candump -decaxta any > <name-of-file>.log
```

## Executing specific tests

Loading or building the CMakeList.txt in the root directory gives access to all test targets. They can be selected
individually or
run all at once by choosing the UnitTests target. Please be aware that test T104ChangeOfCommunicationInterfaceTest needs
manual
intervention and results gathered by running the UnitTest target are not correct. This test should be run individually.

### Setting up all tests except T104ChangeOfCommunicationInterfaceTest

1. Make sure the `CAN_REDUNDANCY_FACTOR` macro in `src/Utility/Macros.h`is set to 1.

Execute the desired test.

### Setting up test T104ChangeOfCommunicationInterfaceTest

1. Make sure the `CAN_REDUNDANCY_FACTOR` macro in `src/Utility/Macros.h`is set to 3.

Execute the test T104ChangeOfCommunicationInterfaceTest. After 15 seconds disable one of the communication interfaces by
running:

1. `sudo ip link set vcan2 down`

Verify with:

1. `ip link show`

The interface should have gone from

```text
vcan2: <NOARP,UP,LOWER_UP>
```

to

```text
vcan2: <NOARP>
```

### Running specific test

After building the project using cmake go to the build directory. Here it is called `build`. In there, all executables
can be found.
A specific test can be chosen by filtering for it using the `--gtest_filter=<test-case>` command on the UnitTest target,
for example for the `ReferenceConfigurationTest` like so:

```text
build/src/Tests/UnitTests --gtest_filter=ReferenceConfigurationTest.ReferenceConfigurationTest:ReferenceConfigurationTest/*.ReferenceConfigurationTest:ReferenceConfigurationTest.ReferenceConfigurationTest/*:*/ReferenceConfigurationTest.ReferenceConfigurationTest/*:*/ReferenceConfigurationTest/*.ReferenceConfigurationTest --gtest_color=no
```
