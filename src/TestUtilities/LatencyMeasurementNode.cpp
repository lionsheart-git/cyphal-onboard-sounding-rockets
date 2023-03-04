/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#include "LatencyMeasurementNode.h"

#include <iostream>

#include <glog/logging.h>

#include "PUavcanPrimitiveEmpty.h"
#include "Clock.h"

void LatencyMeasurementNode::ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const &transfer) {
    Node::ProcessReceivedTransfer(interface_index, transfer);

    if (transfer.metadata.transfer_kind == CanardTransferKindRequest) {
        if (transfer.metadata.port_id == LATENCY_MEASUREMENT_PORT_ID) {
            CanardTransferMetadata meta = transfer.metadata;
            meta.transfer_kind = CanardTransferKindResponse;

            PUavcanPrimitiveEmpty answer(meta);
            OpenCyphal::Publish(transfer.timestamp_usec + MEGA, &meta, answer.SerializedSize(), answer.SerializedMessage(), interface_index);
            // Publish(interface_index, transfer.timestamp_usec + MEGA, answer);
        } else if (transfer.metadata.port_id == LATENCY_MEASUREMENT_PORT_ID + 1) {
            LOG(INFO) << "Got latency response from " << transfer.metadata.remote_node_id <<
                      " with transmission id" << transfer.metadata.transfer_id;
        }
    }
    if (transfer.metadata.transfer_kind == CanardTransferKindResponse) {
        if (transfer.metadata.port_id == LATENCY_MEASUREMENT_PORT_ID) {
            auto delta = Clock::GetMonotonicMicroseconds() - latency_[transfer.metadata.transfer_id];

            LOG(INFO) << ";" << static_cast<int>(transfer.metadata.remote_node_id) <<
                      ";" << static_cast<int>(transfer.metadata.transfer_id) << ";" << delta;
        }
    }
}

LatencyMeasurementNode::LatencyMeasurementNode(uint8_t node_id,
                                               std::unique_ptr<CanardTransceiver> transceiver,
                                               uavcan_node_GetInfo_Response_1_0 info)
                                               : Node(node_id, std::move(transceiver), info) {

}
