#include <iostream>

#include <cstring>
#include <unistd.h>

#include "SocketCANTransceiver.h"
#include "OpenCyphal.h"
#include "Node.h"
#include "Clock.h"
#include "PMessageHeartbeat.h"
#include "THeartbeat.h"

#include "uavcan/node/GetInfo_1_0.h"
#include "uavcan/_register/Value_1_0.h"
#include "uavcan/pnp/NodeIDAllocationData_2_0.h"

// Defines
#define O1HEAP_MEM_SIZE 4096
#define NODE_ID 96
#define UPTIME_SEC_MAX 31
#define TX_PROC_SLEEP_TIME 5000

#define CAN_REDUNDANCY_FACTOR 1
#define KILO 1000L
#define MEGA ((int64_t) KILO * KILO)

// Function prototypes
static void getUniqueID(uint8_t out[uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_]);
static void handle1HzLoop(OpenCyphal &cyphal, const CanardMicrosecond monotonic_time,
                          CanardMicrosecond const started_at);

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

    OpenCyphal cyphal(transceiver);

    uavcan_node_GetInfo_Response_1_0 node_info;
    node_info.name.count = strlen(NODE_NAME);
    memcpy(&node_info.name.elements, NODE_NAME, node_info.name.count);

    node_info.software_version.major = VERSION_MAJOR;
    node_info.software_version.minor = VERSION_MINOR;
    node_info.software_vcs_revision_id = VCS_REVISION_ID;

    getUniqueID(node_info.unique_id);

    Node node(cyphal, node_info);

    cyphal.addTransferReceiver(node);

    // Subscribe to GetInfo requests
    static CanardRxSubscription rx;
    const int8_t res =  //
        cyphal.Subscribe(
                          CanardTransferKindRequest,
                          uavcan_node_GetInfo_1_0_FIXED_PORT_ID_,
                          uavcan_node_GetInfo_Request_1_0_EXTENT_BYTES_,
                          CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC,
                          &rx);
    if (res < 0) {
        return -res;
    }

    // Now the node is initialized and we're ready to roll.
    auto started_at = Clock::GetMonotonicMicroseconds();

    THeartbeat heartbeat(started_at, MEGA);

    node.Schedule(heartbeat);

    node.StartNode(started_at);

    while (true) {

        // Run a trivial scheduler polling the loops that run the business logic.
        CanardMicrosecond monotonic_time = Clock::GetMonotonicMicroseconds();

        node.CheckScheduler(monotonic_time);

        // Manage CAN RX/TX per redundant interface.
        cyphal.HandleTxRxQueues();

        // Run every 5ms to prevent using too much CPU.
        usleep(TX_PROC_SLEEP_TIME);

    }
}
