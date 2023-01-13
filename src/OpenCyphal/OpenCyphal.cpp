/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 11.01.23
*/

#include "OpenCyphal.h"

OpenCyphal::OpenCyphal(CanardTransceiver &transceiver)
    : o1heap_allocator_(o1heapInit(heap_arena, sizeof(heap_arena))),
    instance_(canardInit(&memAllocate, &memFree)) {
    instance_.user_reference = o1heap_allocator_;
    instance_.node_id = NODE_ID;

    transceiver_[0] = &transceiver;
    tx_queues_[0] = canardTxInit(100, CANARD_MTU_CAN_FD);
}

int8_t OpenCyphal::Subscribe(CanardTransferKind const transfer_kind,
                             CanardPortID const port_id,
                             size_t const extent,
                             CanardMicrosecond const transfer_id_timeout_usec,
                             CanardRxSubscription *out_subscription) {

    return canardRxSubscribe(&instance_,
                             transfer_kind,
                             port_id,
                             transfer_id_timeout_usec,
                             CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC,
                             out_subscription);
}

int32_t OpenCyphal::Publish(CanardMicrosecond const tx_deadline_usec,
                            CanardTransferMetadata const *const metadata,
                            size_t const payload_size,
                            void const *const payload) {
    //@todo Figure out how to deal with the failing of a push to one tx queue.
    int32_t retval = 0;
    for (uint8_t ifidx = 0; ifidx < CAN_REDUNDANCY_FACTOR; ifidx++) {
        retval = canardTxPush(&tx_queues_[ifidx],
                            &instance_,
                            tx_deadline_usec,
                            metadata,
                            payload_size,
                            payload);
    }

    return retval;
}

void OpenCyphal::addTransceiver(CanardTransceiver &transceiver) {
    for (uint8_t ifidx = 0; ifidx < CAN_REDUNDANCY_FACTOR; ifidx++) {
        if (transceiver_[ifidx] == nullptr) {
            transceiver_[ifidx] = &transceiver;
            tx_queues_[ifidx] = canardTxInit(100, CANARD_MTU_CAN_FD);
        }
    }
}
