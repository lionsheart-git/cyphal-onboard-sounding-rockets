#!/bin/bash
# Common Cyphal register configuration for testing & debugging.
# Source this file into your sh/bash/zsh session before using Yakut and other Cyphal tools.
# You can also insert automatic iface initialization here, e.g., by checking if /sys/class/net/slcan0 exists.
export UAVCAN__CAN__IFACE='socketcan:vcan0 socketcan:vcan1 socketcan:vcan2'
export UAVCAN__CAN__MTU=64
export UAVCAN__NODE__ID=0  # Pick an unoccupied node-ID automatically for this shell session.
echo "Node-ID for this session: $UAVCAN__NODE__ID"