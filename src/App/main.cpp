#include <iostream>

#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "SocketCANTransceiver.h"
#include "OpenCyphal.h"
#include <vector>

#include "o1heap.h"
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
static uavcan_node_GetInfo_Response_1_0 processRequestNodeGetInfo();
static CanardMicrosecond getMonotonicMicroseconds();
static void handle1HzLoop(OpenCyphal &cyphal, const CanardMicrosecond monotonic_time,
                          CanardMicrosecond const started_at);

int main() {
    // Init SocketCanTransceiver
    SocketCANTransceiver transceiver("vcan0", true);

    OpenCyphal cyphal(transceiver);

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
    auto started_at = getMonotonicMicroseconds();
    const CanardMicrosecond fast_loop_period = MEGA / 50;
    CanardMicrosecond next_fast_iter_at = started_at + fast_loop_period;
    CanardMicrosecond next_1_hz_iter_at = started_at + MEGA;
    CanardMicrosecond next_01_hz_iter_at = started_at + MEGA * 10;

    while (true) {

        // Run a trivial scheduler polling the loops that run the business logic.
        CanardMicrosecond monotonic_time = getMonotonicMicroseconds();

        if (monotonic_time >= next_1_hz_iter_at) {
            next_1_hz_iter_at += MEGA;
            handle1HzLoop(cyphal, monotonic_time, started_at);
        }

        // Manage CAN RX/TX per redundant interface.
        cyphal.HandleTxRxQueues();

        // Run every 5ms to prevent using too much CPU.
        usleep(TX_PROC_SLEEP_TIME);

    }
}

static CanardMicrosecond getMonotonicMicroseconds() {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        abort();
    }
    return (uint64_t) (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
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