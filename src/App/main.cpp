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
static void *memAllocate(CanardInstance *const ins, const size_t amount);
static void memFree(CanardInstance *const ins, void *const pointer);
static void getUniqueID(uint8_t out[uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_]);
static uavcan_node_GetInfo_Response_1_0 processRequestNodeGetInfo();
static CanardMicrosecond getMonotonicMicroseconds();
static void sendResponse(CanardTxQueue *tx_queue,
                         CanardInstance *ins,
                         const CanardRxTransfer *const original_request_transfer,
                         const size_t payload_size,
                         const void *const payload);
static void processReceivedTransfer(CanardTxQueue *tx_queue,
                                    CanardInstance *ins,
                                    SocketCANTransceiver *const transceiver,
                                    const CanardRxTransfer *const transfer);
static void handle1HzLoop(CanardInstance &canard,
                          CanardTxQueue &tx_queue,
                          O1HeapInstance const *heap,
                          const CanardMicrosecond monotonic_time,
                          CanardMicrosecond const started_at);

int main() {

    // Create an o1heap and Canard instance
    O1HeapInstance *my_allocator;
    alignas(O1HEAP_ALIGNMENT) static uint8_t heap_arena[O1HEAP_MEM_SIZE] = {0};
    my_allocator = o1heapInit(heap_arena, sizeof(heap_arena));

    // Initialize canard as CANFD and node no. 96
    CanardInstance ins;
    CanardTxQueue txQueue;
    ins = canardInit(&memAllocate, &memFree);
    ins.user_reference = my_allocator;
    ins.node_id = NODE_ID;

    txQueue = canardTxInit(100, CANARD_MTU_CAN_FD);

    // Init SocketCanTransceiver
    SocketCANTransceiver transceiver("vcan0", true);

    // Subscribe to GetInfo requests
    static CanardRxSubscription rx;
    const int8_t res =  //
        canardRxSubscribe(&ins,
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
//    if (monotonic_time >= next_fast_iter_at) {
//        next_fast_iter_at += fast_loop_period;
//        handleFastLoop(&state, monotonic_time);
//    }
        if (monotonic_time >= next_1_hz_iter_at) {
            next_1_hz_iter_at += MEGA;
            handle1HzLoop(ins, txQueue, my_allocator, monotonic_time, started_at);
        }
//    if (monotonic_time >= next_01_hz_iter_at) {
//        next_01_hz_iter_at += MEGA * 10;
//        handle01HzLoop(&state, monotonic_time);
//    }

        // Manage CAN RX/TX per redundant interface.
        for (uint8_t ifidx = 0; ifidx < CAN_REDUNDANCY_FACTOR; ifidx++) {
            // Transmit pending frames from the prioritized TX queues managed by libcanard.
            CanardTxQueue *const que = &txQueue;
            const CanardTxQueueItem *tqi = canardTxPeek(que);  // Find the highest-priority frame.
            while (tqi != nullptr) {
                // Attempt transmission only if the frame is not yet timed out while waiting in the TX queue.
                // Otherwise just drop it and move on to the next one.
                if ((tqi->tx_deadline_usec == 0) || (tqi->tx_deadline_usec > monotonic_time)) {
                    // Send fame non-blocking.
                    const int16_t result = transceiver.SendCanardFrame(tqi->frame, 0);
                    if (result == 0) {
                        break;  // The queue is full, we will try again on the next iteration.
                    }
                    if (result < 0) {
                        return -result;  // SocketCAN interface failure (link down?)
                    }
                }
                ins.memory_free(&ins, canardTxPop(que, tqi));
                tqi = canardTxPeek(que);
            }

            // Process received frames by feeding them from SocketCAN to libcanard.
            // The order in which we handle the redundant interfaces doesn't matter -- libcanard can accept incoming
            // frames from any of the redundant interface in an arbitrary order.
            // The internal state machine will sort them out and remove duplicates automatically.
            CanardFrame frame = {0};
            uint8_t buf[CANARD_MTU_CAN_FD] = {0};
            uint64_t out_timestamp_usec;
            int16_t socketcan_result = transceiver.ReceiveCanardFrame(0, out_timestamp_usec, frame, buf);
            if (socketcan_result == 0)  // The read operation has timed out with no frames, nothing to do here.
            {
                break;
            }
            if (socketcan_result < 0)  // The read operation has failed. This is not a normal condition.
            {
                return -socketcan_result;
            }
            // The SocketCAN adapter uses the wall clock for timestamping, but we need monotonic.
            // Wall clock can only be used for time synchronization.
            const CanardMicrosecond timestamp_usec = getMonotonicMicroseconds();
            CanardRxTransfer transfer = {static_cast<CanardPriority>(0)};
            const int8_t canard_result = canardRxAccept(&ins, timestamp_usec, &frame, ifidx, &transfer, NULL);
            if (canard_result > 0) {
                processReceivedTransfer(&txQueue, &ins, &transceiver, &transfer);
                ins.memory_free(&ins, (void *) transfer.payload);
            } else if ((canard_result == 0) || (canard_result == -CANARD_ERROR_OUT_OF_MEMORY)) {
                (void) 0;  // The frame did not complete a transfer so there is nothing to do.
                // OOM should never occur if the heap is sized correctly. You can track OOM errors via heap API.
            } else {
                assert(false);  // No other error can possibly occur at runtime.
            }
        }

        // Run every 5ms to prevent using too much CPU.
        usleep(TX_PROC_SLEEP_TIME);

    }
}

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

/// Constructs a response to uavcan.node.GetInfo which contains the basic information about this node.
static uavcan_node_GetInfo_Response_1_0 processRequestNodeGetInfo() {
    uavcan_node_GetInfo_Response_1_0 resp = {0};
    resp.protocol_version.major = CANARD_CYPHAL_SPECIFICATION_VERSION_MAJOR;
    resp.protocol_version.minor = CANARD_CYPHAL_SPECIFICATION_VERSION_MINOR;

    // The hardware version is not populated in this demo because it runs on no specific hardware.
    // An embedded node like a servo would usually determine the version by querying the hardware.

    resp.software_version.major = VERSION_MAJOR;
    resp.software_version.minor = VERSION_MINOR;
    resp.software_vcs_revision_id = VCS_REVISION_ID;

    getUniqueID(resp.unique_id);

    // The node name is the name of the product like a reversed Internet domain name (or like a Java package).
    resp.name.count = strlen(NODE_NAME);
    memcpy(&resp.name.elements, NODE_NAME, resp.name.count);

    // The software image CRC and the Certificate of Authenticity are optional so not populated in this demo.
    return resp;
}

static CanardMicrosecond getMonotonicMicroseconds() {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        abort();
    }
    return (uint64_t) (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

static void send(CanardTxQueue *tx_queue,
                 CanardInstance *ins,
                 const CanardMicrosecond tx_deadline_usec,
                 const CanardTransferMetadata *const metadata,
                 const size_t payload_size,
                 const void *const payload) {
    for (uint8_t ifidx = 0; ifidx < CAN_REDUNDANCY_FACTOR; ifidx++) {
        (void) canardTxPush(tx_queue,
                            ins,
                            tx_deadline_usec,
                            metadata,
                            payload_size,
                            payload);
    }
}

static void sendResponse(CanardTxQueue *tx_queue,
                         CanardInstance *ins,
                         const CanardRxTransfer *const original_request_transfer,
                         const size_t payload_size,
                         const void *const payload) {
    CanardTransferMetadata meta = original_request_transfer->metadata;
    meta.transfer_kind = CanardTransferKindResponse;
    send(tx_queue, ins, original_request_transfer->timestamp_usec + MEGA, &meta, payload_size, payload);
}

int node_heartbeat = 0;
int node_pnp = 0;

/// Invoked every second.
static void handle1HzLoop(CanardInstance &canard,
                          CanardTxQueue &tx_queue,
                          O1HeapInstance const *heap,
                          const CanardMicrosecond monotonic_time,
                          CanardMicrosecond const started_at) {
    const bool anonymous = canard.node_id > CANARD_NODE_ID_MAX;
    // Publish heartbeat every second unless the local node is anonymous. Anonymous nodes shall not publish heartbeat.
    if (!anonymous) {
        uavcan_node_Heartbeat_1_0 heartbeat = {0};
        heartbeat.uptime = (uint32_t) ((monotonic_time - started_at) / MEGA);
        heartbeat.mode.value = uavcan_node_Mode_1_0_OPERATIONAL;
        const O1HeapDiagnostics heap_diag = o1heapGetDiagnostics(heap);
        if (heap_diag.oom_count > 0) {
            heartbeat.health.value = uavcan_node_Health_1_0_CAUTION;
        } else {
            heartbeat.health.value = uavcan_node_Health_1_0_NOMINAL;
        }

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
            send(&tx_queue,
                 &canard,
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
                send(&tx_queue,
                     &canard, // The response will arrive asynchronously eventually.
                     monotonic_time + MEGA,
                     &transfer,
                     serialized_size,
                     &serialized[0]);
            }
        }
    }
}

static void processReceivedTransfer(CanardTxQueue *tx_queue,
                                    CanardInstance *ins,
                                    SocketCANTransceiver *const transceiver,
                                    const CanardRxTransfer *const transfer) {
    if (transfer->metadata.transfer_kind == CanardTransferKindRequest) {
        if (transfer->metadata.port_id == uavcan_node_GetInfo_1_0_FIXED_PORT_ID_) {
            // The request object is empty so we don't bother deserializing it. Just send the response.
            const uavcan_node_GetInfo_Response_1_0 resp = processRequestNodeGetInfo();
            uint8_t serialized[uavcan_node_GetInfo_Response_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_] = {0};
            size_t serialized_size = sizeof(serialized);
            const int8_t res = uavcan_node_GetInfo_Response_1_0_serialize_(&resp, &serialized[0], &serialized_size);
            if (res >= 0) {
                sendResponse(tx_queue, ins, transfer, serialized_size, &serialized[0]);
            } else {
                assert(false);
            }
        }
    }
}

/* Standard memAllocate and memFree from o1heap examples. */
static void *memAllocate(CanardInstance *const ins, const size_t amount) {
    O1HeapInstance *const heap = (O1HeapInstance *) ins->user_reference;
    assert(o1heapDoInvariantsHold(heap));
    return o1heapAllocate(heap, amount);
}

static void memFree(CanardInstance *const ins, void *const pointer) {
    O1HeapInstance *const heap = (O1HeapInstance *) ins->user_reference;
    o1heapFree(heap, pointer);
}
