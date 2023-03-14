#!/bin/bash
# Make sure the script runs with super user priviliges.
[ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"
# Load the kernel module.
modprobe vcan
# Create the virtual CAN interface.
ip link add dev vcan0 type vcan fd on
ip link add dev vcan1 type vcan fd on
ip link add dev vcan2 type vcan fd on
# Bring the virutal CAN interface online.
ip link set up vcan0
ip link set up vcan1
ip link set up vcan2
