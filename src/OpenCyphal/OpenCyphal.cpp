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
