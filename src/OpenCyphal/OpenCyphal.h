/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 11.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_OPENCYPHAL_H_
#define SOCKETCAN_SRC_OPENCYPHAL_OPENCYPHAL_H_

#define CAN_REDUNDANCY_FACTOR 1
#define O1HEAP_MEM_SIZE 4096
#define NODE_ID 96

#define KILO 1000L
#define MEGA ((int64_t) KILO * KILO)

#include <cassert>
#include <ctime>
#include <cstdlib>

#include <canard.h>
#include <o1heap.h>

#include "uavcan/node/GetInfo_1_0.h"

#include "CanardTransceiver.h"
#include "uavcan/_register/Value_1_0.h"

class OpenCyphal {
  public:
    explicit OpenCyphal(CanardTransceiver &transceiver);

    int8_t Subscribe(const CanardTransferKind transfer_kind,
                     CanardPortID const port_id,
                     size_t const extent,
                     CanardMicrosecond const transfer_id_timeout_usec,
                     CanardRxSubscription *out_subscription);

    int32_t Publish(const CanardMicrosecond tx_deadline_usec,
                    const CanardTransferMetadata *const metadata,
                    const size_t payload_size,
                    const void *const payload);

    int32_t Publish(const CanardMicrosecond tx_deadline_usec,
                    const CanardTransferMetadata *const metadata,
                    const size_t payload_size,
                    const void *const payload,
                    uint8_t interface_index);

    /**
     * @brief Returns the health of the current OpenCyphal instance.
     *
     * Currently given by the healt of the O1Heap stack.
     *
     * @return uavcan_node_Health_1_0 health value.
     */
    uint8_t Health();

    void addTransceiver(CanardTransceiver &transceiver);

    int32_t HandleTxRxQueues();

  private:
    // Canard variables
    CanardTxQueue tx_queues_[CAN_REDUNDANCY_FACTOR];
    CanardTransceiver *transceiver_[CAN_REDUNDANCY_FACTOR];
    CanardInstance instance_;

    // O1Heap variables
    O1HeapInstance *o1heap_allocator_;
    alignas(O1HEAP_ALIGNMENT) uint8_t heap_arena[O1HEAP_MEM_SIZE] = {0};

    void ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const & transfer);
    static uavcan_node_GetInfo_Response_1_0 ProcessRequestNodeGetInfo();

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

    static CanardMicrosecond getMonotonicMicroseconds() {
        struct timespec ts{};
        if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
            abort();
        }
        return (uint64_t) (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
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
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_OPENCYPHAL_H_
