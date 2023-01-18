#include <iostream>

#include <cstring>
#include <unistd.h>

#include "SocketCANTransceiver.h"
#include "OpenCyphal.h"
#include "Node.h"
#include "Clock.h"

#include "uavcan/node/Heartbeat_1_0.h"
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

    task_t task;
    task.intervall = MEGA;
    task.task_function = &handle1HzLoop;
    node.Schedule(task);

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

int node_heartbeat = 0;
int node_pnp = 0;

/// Invoked every second.
static void handle1HzLoop(OpenCyphal &cyphal, const CanardMicrosecond monotonic_time,
                          CanardMicrosecond const started_at) {
    const bool anonymous = NODE_ID > CANARD_NODE_ID_MAX;
    // Publish heartbeat every second unless the local node is anonymous. Anonymous nodes shall not publish heartbeat.
    if (!anonymous) {
        uavcan_node_Heartbeat_1_0 heartbeat = {0};
        heartbeat.uptime = (uint32_t) ((monotonic_time - started_at) / MEGA);
        heartbeat.mode.value = uavcan_node_Mode_1_0_OPERATIONAL;
        heartbeat.health.value = cyphal.Health();

        uint8_t serialized[uavcan_node_Heartbeat_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_] = {0};
        size_t serialized_size = sizeof(serialized);
        const int8_t err = uavcan_node_Heartbeat_1_0_serialize_(&heartbeat, &serialized[0], &serialized_size);
        assert(err >= 0);
        if (err >= 0) {
            const CanardTransferMetadata transfer = {
                .priority       = CanardPriorityNominal,
                .transfer_kind  = CanardTransferKindMessage,
                .port_id        = uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_,
                .remote_node_id = CANARD_NODE_ID_UNSET,
                .transfer_id    = (CanardTransferID) (node_heartbeat++),
            };
            cyphal.Publish(
                 monotonic_time + MEGA,  // Set transmission deadline 1 second, optimal for heartbeat.
                 &transfer,
                 serialized_size,
                 &serialized[0]);
        }
    } else  // If we don't have a node-ID, obtain one by publishing allocation request messages until we get a response.
    {
        // The Specification says that the allocation request publication interval shall be randomized.
        // We implement randomization by calling rand() at fixed intervals and comparing it against some threshold.
        // There are other ways to do it, of course. See the docs in the Specification or in the DSDL definition here:
        // https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/pnp/8165.NodeIDAllocationData.2.0.dsdl
        // Note that a high-integrity/safety-certified application is unlikely to be able to rely on this feature.
        if (rand() > RAND_MAX / 2)  // NOLINT
        {
            // Note that this will only work over CAN FD. If you need to run PnP over Classic CAN, use message v1.0.
            uavcan_pnp_NodeIDAllocationData_2_0 msg = {0};
            msg.node_id.value = UINT16_MAX;
            getUniqueID(msg.unique_id);
            uint8_t serialized[uavcan_pnp_NodeIDAllocationData_2_0_SERIALIZATION_BUFFER_SIZE_BYTES_] = {0};
            size_t serialized_size = sizeof(serialized);
            const int8_t err = uavcan_pnp_NodeIDAllocationData_2_0_serialize_(&msg, &serialized[0], &serialized_size);
            assert(err >= 0);
            if (err >= 0) {
                const CanardTransferMetadata transfer = {
                    .priority       = CanardPrioritySlow,
                    .transfer_kind  = CanardTransferKindMessage,
                    .port_id        = uavcan_pnp_NodeIDAllocationData_2_0_FIXED_PORT_ID_,
                    .remote_node_id = CANARD_NODE_ID_UNSET,
                    .transfer_id    = (CanardTransferID) (node_pnp++),
                };
                cyphal.Publish( // The response will arrive asynchronously eventually.
                     monotonic_time + MEGA,
                     &transfer,
                     serialized_size,
                     &serialized[0]);
            }
        }
    }
}