/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 11.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_OPENCYPHAL_H_
#define SOCKETCAN_SRC_OPENCYPHAL_OPENCYPHAL_H_

#include <cassert>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <memory>

#include <canard.h>
#include <o1heap.h>

#include "uavcan/node/GetInfo_1_0.h"
#include "uavcan/_register/Value_1_0.h"

#include "Macros.h"
#include "CanardTransceiver.h"
#include "CanardTransferReceiver.h"

class OpenCyphal {
  public:
    /**
     * @brief Instantiates a cyphal instance.
     *
     * @param node_id The id of this node.
     * @param transceiver The transceiver through which to communicate.
     */
    OpenCyphal(uint8_t node_id, std::unique_ptr<CanardTransceiver> transceiver);

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
    void addTransceiver(std::unique_ptr<CanardTransceiver> transceiver);

    /**
     * @brief Adds a new handler for received transfers.
     *
     * @param transfer_receiver Transfer handler to add.
     */
    void addTransferReceiver(CanardTransferReceiver &transfer_receiver);

    /**
     * @brief Handles the transmission and receive queues.
     *
     * This function has to be called continuously to publish data and messages and answer to requests.
     * @return Negative errno on error.
     */
    int32_t HandleTxRxQueues(uint64_t monotonic_time);

  private:
    // Canard variables
    CanardTxQueue tx_queues_[CAN_REDUNDANCY_FACTOR]; /**< Transmission queue for each receiver */
    std::unique_ptr<CanardTransceiver> transceiver_[CAN_REDUNDANCY_FACTOR]; /**< Transceiver of this canard instance */
    CanardInstance instance_; /**< Canard instance */

    // O1Heap variables
    O1HeapInstance *o1heap_allocator_; /**< O1Heap instance for cyphal to allocate memory */
    alignas(O1HEAP_ALIGNMENT) std::array<std::byte, O1HEAP_MEM_SIZE> heap_arena = {}; /**< Memory space for O1Heap to allocate in */

    std::vector<CanardTransferReceiver *> transfer_receiver_; /**< Nodes that handle received transfers */

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
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_OPENCYPHAL_H_
