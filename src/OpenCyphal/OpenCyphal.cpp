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
                             extent,
                             transfer_id_timeout_usec,
                             out_subscription);
}

int32_t OpenCyphal::Publish(CanardMicrosecond const tx_deadline_usec,
                            CanardTransferMetadata const *const metadata,
                            size_t const payload_size,
                            void const *const payload) {
    //@todo Figure out how to deal with the failing of a push to one tx queue.
    int32_t retval = 0;
    for (uint8_t ifidx = 0; ifidx < CAN_REDUNDANCY_FACTOR; ifidx++) {
        retval = Publish(tx_deadline_usec, metadata, payload_size, payload);
    }

    return retval;
}

int32_t OpenCyphal::Publish(CanardMicrosecond const tx_deadline_usec,
                            CanardTransferMetadata const *const metadata,
                            size_t const payload_size,
                            void const *const payload,
                            uint8_t interface_index) {
    return canardTxPush(&tx_queues_[interface_index],
                        &instance_,
                        tx_deadline_usec,
                        metadata,
                        payload_size,
                        payload);;
}

void OpenCyphal::addTransceiver(CanardTransceiver &transceiver) {
    for (uint8_t ifidx = 0; ifidx < CAN_REDUNDANCY_FACTOR; ifidx++) {
        if (transceiver_[ifidx] == nullptr) {
            transceiver_[ifidx] = &transceiver;
            tx_queues_[ifidx] = canardTxInit(100, CANARD_MTU_CAN_FD);
        }
    }
}

int32_t OpenCyphal::HandleTxRxQueues() {

    CanardMicrosecond monotonic_time = getMonotonicMicroseconds();
    for (uint8_t ifidx = 0; ifidx < CAN_REDUNDANCY_FACTOR; ifidx++) {
        // Transmit pending frames from the prioritized TX queues managed by libcanard.
        CanardTxQueue *const que = &tx_queues_[ifidx];
        const CanardTxQueueItem *tqi = canardTxPeek(que);  // Find the highest-priority frame.
        while (tqi != nullptr) {
            // Attempt transmission only if the frame is not yet timed out while waiting in the TX queue.
            // Otherwise just drop it and move on to the next one.
            if ((tqi->tx_deadline_usec == 0) || (tqi->tx_deadline_usec > monotonic_time)) {
                // Send fame non-blocking.
                const int16_t result = transceiver_[ifidx]->SendCanardFrame(tqi->frame, 0);
                if (result == 0) {
                    break;  // The queue is full, we will try again on the next iteration.
                }
                if (result < 0) {
                    return -result;  // SocketCAN interface failure (link down?)
                }
            }
            instance_.memory_free(&instance_, canardTxPop(que, tqi));
            tqi = canardTxPeek(que);
        }

        // Process received frames by feeding them from SocketCAN to libcanard.
        // The order in which we handle the redundant interfaces doesn't matter -- libcanard can accept incoming
        // frames from any of the redundant interface in an arbitrary order.
        // The internal state machine will sort them out and remove duplicates automatically.
        CanardFrame frame = {0};
        uint8_t buf[CANARD_MTU_CAN_FD] = {0};
        uint64_t out_timestamp_usec;
        int16_t socketcan_result = transceiver_[ifidx]->ReceiveCanardFrame(0, out_timestamp_usec, frame, buf);
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
        const int8_t canard_result = canardRxAccept(&instance_, timestamp_usec, &frame, ifidx, &transfer, NULL);
        if (canard_result > 0) {
            ProcessReceivedTransfer(ifidx, transfer);
            instance_.memory_free(&instance_, (void *) transfer.payload);
        } else if ((canard_result == 0) || (canard_result == -CANARD_ERROR_OUT_OF_MEMORY)) {
            (void) 0;  // The frame did not complete a transfer so there is nothing to do.
            // OOM should never occur if the heap is sized correctly. You can track OOM errors via heap API.
        } else {
            assert(false);  // No other error can possibly occur at runtime.
        }
    }
}

void OpenCyphal::ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const &transfer) {
    if (transfer.metadata.transfer_kind == CanardTransferKindRequest) {
        if (transfer.metadata.port_id == uavcan_node_GetInfo_1_0_FIXED_PORT_ID_) {
            // The request object is empty so we don't bother deserializing it. Just send the response.
            const uavcan_node_GetInfo_Response_1_0 resp = ProcessRequestNodeGetInfo();
            uint8_t serialized[uavcan_node_GetInfo_Response_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_] = {0};
            size_t serialized_size = sizeof(serialized);
            const int8_t res = uavcan_node_GetInfo_Response_1_0_serialize_(&resp, &serialized[0], &serialized_size);
            if (res >= 0) {
                CanardTransferMetadata meta = transfer.metadata;
                meta.transfer_kind = CanardTransferKindResponse;
                Publish(transfer.timestamp_usec + MEGA, &meta, serialized_size, serialized, interface_index);
            } else {
                assert(false);
            }
        }
    }
}

uavcan_node_GetInfo_Response_1_0 OpenCyphal::ProcessRequestNodeGetInfo() {
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
