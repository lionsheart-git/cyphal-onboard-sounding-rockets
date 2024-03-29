# Cyphal Onboard Sounding Rockets
## General Information

**Topic:**

(German): Implementierung und Evaluation des Cyphal Protokolls für Experimentalraketen

(English): Implementation and Evaluation of the Cyphal Protocol for Sounding Rockets

## Abstract

Sounding rockets enable scientific and technical experiments at altitudes ranging from
50 km to 1550 km. They are among the most robust, versatile, and cost-efficient experimental
platforms used by space agencies. Currently, these rockets use mainly directed
communication between a few components. Their monolithic structure makes it difficult
to develop subsystems and integrate new experiments. Future rockets are expected to consist
of a distributed network of individual components. This study evaluates the Publisher
Subscriber Model (PSM) used by the Cyphal protocol for its usability within a distributed
network onboard sounding rockets. The design and architecture of the PSM are examined
based on information and standards from NASA, ESA, DLR, and ECSS. In particular, the
Cyphal/CAN specification and implementation for the CAN bus are examined in detail. A
reference configuration of a CAN network is evaluated within a simulation environment.
The information obtained was assessed according to programmability, portability, analyzability,
efficiency, scalability, reliability, protection, and enforcement of a distributed
real-time system.

## Publications
* DOI: 10.5281/zenodo.7725799

## Contents of this repository

This repository contains the written code, tests and scripts that were used in the white paper.

```
├── Doc <- Documentation is generated here
│  └── html
├── Log <- Glog loggs are safed here
├── Scripts  <- Contains the scripts for setting up the can interfaces and selecting a node
│  ├── select-node-id.sh
│  └── start-vcans.sh
├── src <- The sourcecode
│  ├── App <- The main.cpp (not needed for tests)
│  ├── Lib <- All libraries
│  ├── OpenCyphal <- libcanard wrapper and abstraction layers for easy testing
│  ├── SocketCAN <- SocketCAN c++ wrapper
│  ├── Tests <- Tests
│  ├── TestUtilities <- Utilites needed for the tests
│  └── Utility <- File containg macros for configuration
├── CMakeLists.txt <- Root CMakeList.txt
├── Doxyfile <- Doxyfile for creating documentation with doxygen
└── README.md <- This Readme
```

## Running the code

### Prerequisites
Make sure that the repository was pulled with all submodules.

#### Building
* CMake (tested with cmake 3.22.1)
* C++-Compiler (tested with gcc 11.3.0)
* Linux-CAN / SocketCAN (tested with Ubuntu 22.04.1 LTS and linux kernel 5.15.0-67-generic)

#### Monitoring
* Yakut
* candump

### Running the tests

Loading or building the CMakeList.txt in the root directory gives access to all test targets. They can be selected individually or
run all at once by choosing the UnitTests target. Please be aware that test T104ChangeOfCommunicationInterfaceTest needs manual
intervention and results gathered by running the UnitTest target are not correct. This test should be run individually.

The exact steps to run each test can be found in the [Running-Tests.md](Doc/Running-Tests.md) and the white paper.

### Known problems

Some times an error like this occurs.
```
/usr/bin/ld: ../Lib/libCanard.a(canard.c.o): warning: relocation against `CanardCANLengthToDLC' in read-only section `.text'
/usr/bin/ld: ../Lib/libCanard.a(canard.c.o): relocation R_X86_64_PC32 against symbol `CanardCANLengthToDLC' can not be used when making a shared object; recompile with -fPIC
/usr/bin/ld: final link failed: bad value
collect2: error: ld returned 1 exit status
gmake[3]: *** [src/OpenCyphal/CMakeFiles/OpenCyphal.dir/build.make:293: src/OpenCyphal/libOpenCyphal.so] Error 1
gmake[2]: *** [CMakeFiles/Makefile2:1772: src/OpenCyphal/CMakeFiles/OpenCyphal.dir/all] Error 2
gmake[2]: *** Waiting for unfinished jobs....
```
The cause for this error could not be found. The solution is to change something in a CMakeList.txt,
completely irrelevant what, and re-run cmake.
