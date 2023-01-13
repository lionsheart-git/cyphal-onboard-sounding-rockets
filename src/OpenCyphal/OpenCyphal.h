/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 11.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_OPENCYPHAL_H_
#define SOCKETCAN_SRC_OPENCYPHAL_OPENCYPHAL_H_

#define CAN_REDUNDANCY_FACTOR 1
#define O1HEAP_MEM_SIZE 4096

#include <canard.h>
#include <o1heap.h>

#include "CanardTransceiver.h"

class OpenCyphal {
  public:
    OpenCyphal(CanardTransceiver const &transceiver);

    int8_t Subscribe(const CanardTransferKind transfer_kind,
                     CanardPortID const port_id,
                     size_t const extent,
                     CanardMicrosecond const transfer_id_timeout_usec,
                     CanardRxSubscription *out_subscription);

    int32_t Publish(const CanardMicrosecond tx_deadline_usec,
                    const CanardTransferMetadata *const metadata,
                    const size_t payload_size,
                    const void *const payload);

    void addTransceiver(CanardTransceiver const &transceiver);

  private:
    // Canard variables
    CanardTxQueue tx_queues_[CAN_REDUNDANCY_FACTOR];
    CanardTransceiver *transceiver_[CAN_REDUNDANCY_FACTOR];
    CanardInstance instance_;

    // O1Heap variables
    O1HeapInstance *o1heap_allocator_;
    alignas(O1HEAP_ALIGNMENT) uint8_t heap_arena[O1HEAP_MEM_SIZE] = {0};

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_OPENCYPHAL_H_
