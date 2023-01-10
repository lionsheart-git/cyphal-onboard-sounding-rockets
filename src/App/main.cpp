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
static void processReceivedTransfer(CanardTxQueue *tx_queue,
                                    CanardInstance *ins,
                                    SocketCANTransceiver *const transceiver,
                                    const CanardRxTransfer *const transfer);

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


//@todo: Figure out why GetInfo response is send only sometimes

int main() {

    // Create an o1heap and Canard instance
    O1HeapInstance *my_allocator;
    alignas(O1HEAP_ALIGNMENT) static uint8_t heap_arena[O1HEAP_MEM_SIZE] = {0};
    my_allocator = o1heapInit(heap_arena, sizeof(heap_arena));

    // Transfer ID
    static uint8_t my_message_transfer_id = 0;

    // Uptime counter for heartbeat message
    uint32_t test_uptimeSec = 0;

    // Buffer for serialization of a heartbeat message
    size_t hbeat_ser_buf_size = uavcan_node_Heartbeat_1_0_EXTENT_BYTES_;
    uint8_t hbeat_ser_buf[uavcan_node_Heartbeat_1_0_EXTENT_BYTES_];

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

    size_t last_time = time(NULL);

    // Main control loop. Run until break condition is found.
    for (;;) {
        if (time(NULL) > last_time + 1) {
            // Sleep for 1 second so our uptime increments once every second.
            last_time = time(NULL);

            // Initialize a Heartbeat message
            uavcan_node_Heartbeat_1_0 test_heartbeat = {
                .uptime = test_uptimeSec,
                .health = {uavcan_node_Health_1_0_NOMINAL},
                .mode = {uavcan_node_Mode_1_0_OPERATIONAL}
            };

            // Print data from Heartbeat message before it's serialized.
            system("clear");
            printf("Preparing to send the following Heartbeat message: \n");
            printf("Uptime: %d\n", test_uptimeSec);
            printf("Health: %d\n", uavcan_node_Health_1_0_NOMINAL);
            printf("Mode: %d\n", uavcan_node_Mode_1_0_OPERATIONAL);

            // Serialize the data using the included serialize function from the Heartbeat C header.
            int8_t result1 = uavcan_node_Heartbeat_1_0_serialize_(&test_heartbeat, hbeat_ser_buf, &hbeat_ser_buf_size);

            // Make sure the serialization was successful.
            if (result1 < 0) {
                printf("Serializing message failed. Aborting...\n");
                break;
            }

            // Create a CanardTransfer and give it the required data.
            CanardTransferMetadata transfer = {
                .priority = CanardPriorityNominal,
                .transfer_kind = CanardTransferKindMessage,
                .port_id = uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_,
                .remote_node_id = CANARD_NODE_ID_UNSET,
                .transfer_id = my_message_transfer_id,
            };

            // Increment our uptime and transfer ID.
            ++test_uptimeSec;
            ++my_message_transfer_id;

            // Stop the loop once we hit 30s of transfer.
            if (test_uptimeSec > UPTIME_SEC_MAX) {
                printf("Reached 30s uptime! Exiting...\n");
                break;
            }

            // Push our CanardTransfer to the Libcanard instance's transfer stack.
            int32_t result2 =
                canardTxPush(&txQueue, &ins, time(NULL) + 1, &transfer, hbeat_ser_buf_size, hbeat_ser_buf);

            // Make sure our push onto the stack was successful.
            if (result2 < 0) {
                printf("Pushing onto TX stack failed. Aborting...\n");
                break;
            }
        }

        // Run every 5ms to prevent using too much CPU.
        usleep(TX_PROC_SLEEP_TIME);

        // Loop through all of the frames in the transfer stack.
        for (const CanardTxQueueItem *txf = nullptr; (txf = canardTxPeek(&txQueue)) != nullptr;) {
            // Make sure we aren't sending a message before the actual time.
            if (txf->tx_deadline_usec < (unsigned long) time(NULL)) {
                // Instantiate a SocketCAN CAN frame.
                const int16_t result = transceiver.SendCanardFrame(txf->frame, 0);
                if (result == 0) {
                    break;  // The queue is full, we will try again on the next iteration.
                }
                if (result < 0) {
                    return -result;  // SocketCAN interface failure (link down?)
                }

                // Pop the sent data off the stack and free its memory.
                ins.memory_free(&ins, canardTxPop(&txQueue, txf));
            }
        }

//        // Process received frames by feeding them from SocketCAN to libcanard.
//        // The order in which we handle the redundant interfaces doesn't matter -- libcanard can accept incoming
//        // frames from any of the redundant interface in an arbitrary order.
//        // The internal state machine will sort them out and remove duplicates automatically.
//        CanardFrame frame = transceiver.ReceiveCanardFrame(0);
//        uint8_t buf[CANARD_MTU_CAN_FD] = {0};
//
//        // The SocketCAN adapter uses the wall clock for timestamping, but we need monotonic.
//        // Wall clock can only be used for time synchronization.
//        const CanardMicrosecond timestamp_usec = getMonotonicMicroseconds();
//        CanardRxTransfer transfer = {static_cast<CanardPriority>(0)};
//        const int8_t canard_result = canardRxAccept(&ins, timestamp_usec, &frame, 0, &transfer, NULL);
//        if (canard_result > 0) {
//            processReceivedTransfer(&txQueue, &ins, &transceiver, &transfer);
//            ins.memory_free(&ins, (void *) transfer.payload);
//        } else if ((canard_result == 0) || (canard_result == -CANARD_ERROR_OUT_OF_MEMORY)) {
//            (void) 0;  // The frame did not complete a transfer so there is nothing to do.
//            // OOM should never occur if the heap is sized correctly. You can track OOM errors via heap API.
//        } else {
//            assert(false);  // No other error can possibly occur at runtime.
//        }

        // Process received frames by feeding them from SocketCAN to libcanard.
        // The order in which we handle the redundant interfaces doesn't matter -- libcanard can accept incoming
        // frames from any of the redundant interface in an arbitrary order.
        // The internal state machine will sort them out and remove duplicates automatically.
        CanardFrame frame = {0};
        uint8_t buf[CANARD_MTU_CAN_FD] = {0};
        const int16_t socketcan_result = socketcanPop(3, &frame, NULL, sizeof(buf), buf, 0, NULL);
        if (socketcan_result == 0)  // The read operation has timed out with no frames, nothing to do here.
        {

        }
        if (socketcan_result < 0)  // The read operation has failed. This is not a normal condition.
        {
            return -socketcan_result;
        }
        // The SocketCAN adapter uses the wall clock for timestamping, but we need monotonic.
        // Wall clock can only be used for time synchronization.
        const CanardMicrosecond timestamp_usec = getMonotonicMicroseconds();
        CanardRxTransfer transfer = {static_cast<CanardPriority>(0)};
        const int8_t canard_result = canardRxAccept(&ins, timestamp_usec, &frame, 0, &transfer, NULL);
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
