/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 11.01.23
*/

#include "OpenCyphal.h"

#include "uavcan/node/Health_1_0.h"

#include "Clock.h"

OpenCyphal::OpenCyphal(uint8_t node_id, std::unique_ptr<CanardTransceiver> transceiver)
    : o1heap_allocator_(),
      instance_(canardInit(&memAllocate, &memFree)), transfer_receiver_() {

    o1heap_allocator_ = o1heapInit(heap_arena.data(), sizeof(heap_arena));

    instance_.user_reference = o1heap_allocator_;
    instance_.node_id = node_id;

    transceiver_[0] = std::move(transceiver);
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
    // Figure out how to deal with the failing of a push to one tx queue.
    int32_t retval = 0;
    for (uint8_t ifidx = 0; ifidx < CAN_REDUNDANCY_FACTOR; ifidx++) {
        retval = Publish(tx_deadline_usec, metadata, payload_size, payload, ifidx);
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

void OpenCyphal::addTransceiver(std::unique_ptr<CanardTransceiver> transceiver) {
    for (uint8_t ifidx = 0; ifidx < CAN_REDUNDANCY_FACTOR; ifidx++) {
        if (transceiver_[ifidx] == nullptr) {
            transceiver_[ifidx] = std::move(transceiver);
            tx_queues_[ifidx] = canardTxInit(100, CANARD_MTU_CAN_FD);
        }
    }
}

int32_t OpenCyphal::HandleTxRxQueues(uint64_t monotonic_time) {

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
        int16_t
            socketcan_result = transceiver_[ifidx]->ReceiveCanardFrame(0, out_timestamp_usec, frame, sizeof(buf), buf);
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
        const CanardMicrosecond timestamp_usec = monotonic_time;
        CanardRxTransfer transfer = {static_cast<CanardPriority>(0)};
        const int8_t canard_result = canardRxAccept(&instance_, timestamp_usec, &frame, ifidx, &transfer, NULL);
        if (canard_result > 0) {
            for (CanardTransferReceiver *receiver : transfer_receiver_) {
                receiver->ProcessReceivedTransfer(ifidx, transfer);
            }
            instance_.memory_free(&instance_, (void *) transfer.payload);
        } else if ((canard_result == 0) || (canard_result == -CANARD_ERROR_OUT_OF_MEMORY)) {
            (void) 0;  // The frame did not complete a transfer so there is nothing to do.
            // OOM should never occur if the heap is sized correctly. You can track OOM errors via heap API.
        } else {
            assert(false);  // No other error can possibly occur at runtime.
        }
    }
    return 0;
}

uint8_t OpenCyphal::Health() {
    const O1HeapDiagnostics heap_diag = o1heapGetDiagnostics(o1heap_allocator_);
    if (heap_diag.oom_count > 0) {
        return uavcan_node_Health_1_0_CAUTION;
    } else {
        return uavcan_node_Health_1_0_NOMINAL;
    }
}

void OpenCyphal::addTransferReceiver(CanardTransferReceiver &transfer_receiver) {
    transfer_receiver_.push_back(&transfer_receiver);
}
