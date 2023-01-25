#include <iostream>

#include <cstring>
#include <unistd.h>

#include "SocketCANTransceiver.h"
#include "OpenCyphal.h"
#include "Node.h"
#include "Clock.h"
#include "PMessageHeartbeat.h"
#include "THeartbeat.h"
#include "Macros.h"
#include "SMessageGetInfo.h"

#include "uavcan/node/GetInfo_1_0.h"
#include "uavcan/_register/Value_1_0.h"
#include "uavcan/pnp/NodeIDAllocationData_2_0.h"
#include "uavcan/si/unit/angle/Scalar_1_0.h"

// Defines
#define TX_PROC_SLEEP_TIME 5000

// Function prototypes
static void getUniqueID(uint8_t out[uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_]);

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

    OpenCyphal cyphal(NODE_ID, transceiver);

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
    SMessageGetInfo getInfo;

    const int8_t res = node.Subscribe(getInfo);

    if (res < 0) {
        return -res;
    }

    // Now the node is initialized and we're ready to roll.
    auto started_at = Clock::GetMonotonicMicroseconds();

    THeartbeat heartbeat(started_at, MEGA);

    node.Schedule(heartbeat);

    node.StartNode(started_at);

    CanardTransferID id = 0;

    CanardTransferMetadata metadata = {
        .priority = CanardPriorityNominal,
        .transfer_kind = CanardTransferKindMessage,
        .port_id = 33,
        .remote_node_id = CANARD_NODE_ID_UNSET,
        .transfer_id = id++
    };

    uavcan_si_unit_angle_Scalar_1_0 angle;
    angle.radian = 2;

    size_t serialized_size = uavcan_si_unit_angle_Scalar_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_;
    uint8_t serialized[serialized_size];

    uavcan_si_unit_angle_Scalar_1_0_serialize_(&angle, &serialized[0], &serialized_size);

    while (true) {

        // Run a trivial scheduler polling the loops that run the business logic.
        CanardMicrosecond monotonic_time = Clock::GetMonotonicMicroseconds();

        node.CheckScheduler(monotonic_time);

        cyphal.Publish(monotonic_time + MEGA, &metadata, serialized_size, serialized);

        // Manage CAN RX/TX per redundant interface.
        cyphal.HandleTxRxQueues();

        // Run every 5ms to prevent using too much CPU.
        usleep(TX_PROC_SLEEP_TIME);

    }
}
