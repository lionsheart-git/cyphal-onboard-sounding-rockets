#include <iostream>

#include <cstring>
#include <unistd.h>

#include "SocketCANTransceiver.h"
#include "OpenCyphal.h"
#include "Node.h"
#include "Clock.h"
#include "THeartbeat.h"
#include "Macros.h"
#include "SMessageGetInfo.h"
#include "TByteArray.h"

#include "uavcan/node/GetInfo_1_0.h"

// Defines
#define TX_PROC_SLEEP_TIME 5000

// Returns the 128-bit unique-ID of the local node. This value is used in uavcan.node.GetInfo.Response and during the
// plug-and-play node-ID allocation by uavcan.pnp.NodeIDAllocationData. The function is infallible.
static void getUniqueID(uint8_t out[uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_]) {
    // A real hardware node would read its unique-ID from some hardware-specific source (typically stored in ROM).
    // This example is a software-only node, so we store the unique-ID in a (read-only) register instead.
    uavcan_register_Value_1_0 value = {0};
    uavcan_register_Value_1_0_select_unstructured_(&value);
    // Populate the default; it is only used at the first run if there is no such register.
    for (uint8_t i = 0; i < uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_; i++) {
        value.unstructured.value.elements[value.unstructured.value.count++] = (uint8_t) rand();  // NOLINT
    }
    // registerRead("uavcan.node.unique_id", &value);
    assert(uavcan_register_Value_1_0_is_unstructured_(&value) &&
        value.unstructured.value.count == uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_);
    memcpy(&out[0], &value.unstructured.value, uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_);
}

int main() {
    // Init SocketCanTransceiver
    SocketCANTransceiver transceiver("vcan0", true);

    uavcan_node_GetInfo_Response_1_0 node_info;
    node_info.name.count = strlen(NODE_NAME);
    memcpy(&node_info.name.elements, NODE_NAME, node_info.name.count);

    node_info.software_version.major = VERSION_MAJOR;
    node_info.software_version.minor = VERSION_MINOR;
    node_info.software_vcs_revision_id = VCS_REVISION_ID;

    getUniqueID(node_info.unique_id);

    Node node(NODE_ID, transceiver, node_info);

    // Subscribe to GetInfo requests
    SMessageGetInfo getInfo;

    const int8_t res = node.Subscribe(getInfo);

    if (res < 0) {
        return -res;
    }

    size_t data_size = 256;
    int8_t random_data[256];

    for (int8_t i = 0; i < data_size; ++i) {
        random_data[i] = i;
    }

    // Now the node is initialized and we're ready to roll.
    auto started_at = Clock::GetMonotonicMicroseconds();

    THeartbeat heartbeat(node, MEGA);
    node.Schedule(heartbeat);

    TByteArray byte_array(32, random_data, data_size, MEGA);
    node.Schedule(byte_array);

    node.StartNode(started_at);

    while (true) {

        // Run a trivial scheduler polling the loops that run the business logic.
        CanardMicrosecond monotonic_time = Clock::GetMonotonicMicroseconds();

        node.CheckScheduler(monotonic_time);

        // Manage CAN RX/TX per redundant interface.
        node.HandleTxRxQueues();

        // Run every 5ms to prevent using too much CPU.
        usleep(TX_PROC_SLEEP_TIME);

    }
}
