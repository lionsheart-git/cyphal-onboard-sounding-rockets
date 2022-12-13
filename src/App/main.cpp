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

// Defines
#define O1HEAP_MEM_SIZE 4096
#define NODE_ID 96
#define UPTIME_SEC_MAX 31
#define TX_PROC_SLEEP_TIME 5000

// Function prototypes
static void* memAllocate(CanardInstance* const ins, const size_t amount);
static void memFree(CanardInstance* const ins, void* const pointer);
int main()
{

    // Allocate 4KB of memory for o1heap.
//    void *mem_space = malloc(O1HEAP_MEM_SIZE);

// Create an o1heap and Canard instance
    O1HeapInstance* my_allocator;
    alignas(O1HEAP_ALIGNMENT) static uint8_t heap_arena[1024 * 20] = {0};
    my_allocator = o1heapInit(heap_arena, sizeof(heap_arena));

    CanardInstance ins;
    CanardTxQueue txQueue;

// Transfer ID
    static uint8_t my_message_transfer_id = 0;

// Uptime counter for heartbeat message
    uint32_t test_uptimeSec = 0;

// Buffer for serialization of a heartbeat message
    size_t hbeat_ser_buf_size = uavcan_node_Heartbeat_1_0_EXTENT_BYTES_;
    uint8_t hbeat_ser_buf[uavcan_node_Heartbeat_1_0_EXTENT_BYTES_];

    // Initialize o1heap


    // Initialize canard as CANFD and node no. 96
    ins = canardInit(&memAllocate, &memFree);
    ins.user_reference = my_allocator;
    ins.node_id = NODE_ID;

    txQueue = canardTxInit(100, CANARD_MTU_CAN_FD);

    // Init SocketCanTransceiver
    SocketCANTransceiver transceiver("vcan0", true);

    size_t last_time = time(NULL);

    // Main control loop. Run until break condition is found.
    for(;;) {
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
        for(const CanardTxQueueItem* txf = nullptr; (txf = canardTxPeek(&txQueue)) != nullptr;)
        {
            // Make sure we aren't sending a message before the actual time.
            if(txf->tx_deadline_usec < (unsigned long)time(NULL))
            {
                // Instantiate a SocketCAN CAN frame.
                const int16_t result = transceiver.SendCanardFrame(txf->frame, 0);
                if (result == 0)
                {
                    break;  // The queue is full, we will try again on the next iteration.
                }
                if (result < 0)
                {
                    return -result;  // SocketCAN interface failure (link down?)
                }

                // Pop the sent data off the stack and free its memory.
                ins.memory_free(&ins, canardTxPop(&txQueue, txf));
            }
        }
    }
}

/* Standard memAllocate and memFree from o1heap examples. */
static void* memAllocate(CanardInstance* const ins, const size_t amount)
{
    O1HeapInstance* const heap = (O1HeapInstance*) ins->user_reference;
    assert(o1heapDoInvariantsHold(heap));
    return o1heapAllocate(heap, amount);
}

static void memFree(CanardInstance* const ins, void* const pointer)
{
    O1HeapInstance* const heap = (O1HeapInstance*) ins->user_reference;
    o1heapFree(heap, pointer);
}
