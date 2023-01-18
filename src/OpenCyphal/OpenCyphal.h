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
#include <vector>

#include <canard.h>
#include <o1heap.h>

#include "uavcan/node/GetInfo_1_0.h"
#include "uavcan/_register/Value_1_0.h"

#include "CanardTransceiver.h"
#include "CanardTransferReceiver.h"

class OpenCyphal {
  public:
    explicit OpenCyphal(CanardTransceiver &transceiver);

    /**
     * @brief Subscribes to a specific topic.
     *
     * @param transfer_kind Type of transfer.
     * @param port_id Port ID of topic or service.
     * @param extent Maximum possible size of received objects of this type.
     * @param transfer_id_timeout_usec Transfer-ID timeout value.
     * @param out_subscription Pointer to subscription.
     *
     * @return
     */
    int8_t Subscribe(const CanardTransferKind transfer_kind,
                     CanardPortID const port_id,
                     size_t const extent,
                     CanardMicrosecond const transfer_id_timeout_usec,
                     CanardRxSubscription *out_subscription);

    /**
     * @brief Publishes the message to all interfaces.
     *
     * @param tx_deadline_usec Deadline to which the message has to be send.
     * @param metadata Metadata of the message
     * @param payload_size Payload size.
     * @param[in] payload Pointer to payload.
     * @param interface_index Index of interface in array.
     *
     * @return
     */
    int32_t Publish(const CanardMicrosecond tx_deadline_usec,
                    const CanardTransferMetadata *const metadata,
                    const size_t payload_size,
                    const void *const payload);

    /**
     * @brief Publishes the message to the specified interface.
     *
     * @param tx_deadline_usec Deadline to which the message has to be send.
     * @param metadata Metadata of the message
     * @param payload_size Payload size.
     * @param[in] payload Pointer to payload.
     * @param interface_index Index of interface in array.
     *
     * @return
     */
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

    /**
     * @brief Adds a transceiver to the OpenCyphal instance. A maximum of three transceivers are supported.
     *
     * @param transceiver The transceiver to add.
     */
    void addTransceiver(CanardTransceiver &transceiver);

    void addTransferReceiver(CanardTransferReceiver &transfer_receiver);

    /**
     * @brief Handles the transmission and receive queues.
     *
     * This function has to be called continuously to publish data and messages and answer to requests.
     * @return Negative errno on error.
     */
    int32_t HandleTxRxQueues();

  private:
    // Canard variables
    CanardTxQueue tx_queues_[CAN_REDUNDANCY_FACTOR]; /**< Transmission queue for each receiver */
    CanardTransceiver *transceiver_[CAN_REDUNDANCY_FACTOR]; /**< Transceiver of this canard instance */
    CanardInstance instance_; /**< Canard instance */

    // O1Heap variables
    O1HeapInstance *o1heap_allocator_; /**< O1Heap instance for cyphal to allocate memory */
    alignas(O1HEAP_ALIGNMENT) uint8_t heap_arena[O1HEAP_MEM_SIZE] = {0}; /**< Memory space for O1Heap toa allocate in */

    std::vector<CanardTransferReceiver *> transfer_receiver_;

    /**
     * @brief Standard memAllocate from O1Heap examples.
     *
     * Needed by canard.
     *
     * @param ins Canard instance.
     * @param amount Amount to allocate.
     * @return Pointer to the allocated memory or NULL.
     */
    static void *memAllocate(CanardInstance *const ins, const size_t amount) {
        O1HeapInstance *const heap = (O1HeapInstance *) ins->user_reference;
        assert(o1heapDoInvariantsHold(heap));
        return o1heapAllocate(heap, amount);
    }

    /**
     * @brief Standard memFree from O1Heap examples.
     *
     * Needed by canard.
     *
     * @param ins Canard instance.
     * @param pointer Pointer to memory to free.
     */
    static void memFree(CanardInstance *const ins, void *const pointer) {
        O1HeapInstance *const heap = (O1HeapInstance *) ins->user_reference;
        o1heapFree(heap, pointer);
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
