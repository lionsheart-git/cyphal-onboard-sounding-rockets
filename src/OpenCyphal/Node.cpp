/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 18.01.23
*/

#include "Node.h"

#include "Clock.h"

Node::Node(OpenCyphal &cyphal, uavcan_node_GetInfo_Response_1_0 info)
    : cyphal_(cyphal), info_(info), started_at_() {
}

void Node::ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const &transfer) {
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
                cyphal_.Publish(transfer.timestamp_usec + MEGA, &meta, serialized_size, serialized, interface_index);
            } else {
                assert(false);
            }
        }
    }
}

uavcan_node_GetInfo_Response_1_0 Node::ProcessRequestNodeGetInfo() {
    uavcan_node_GetInfo_Response_1_0 resp = {0};
    resp.protocol_version.major = CANARD_CYPHAL_SPECIFICATION_VERSION_MAJOR;
    resp.protocol_version.minor = CANARD_CYPHAL_SPECIFICATION_VERSION_MINOR;

    // The hardware version is not populated in this demo because it runs on no specific hardware.
    // An embedded node like a servo would usually determine the version by querying the hardware.
    resp.software_version.major = info_.software_version.major;
    resp.software_version.minor = info_.software_version.minor;
    resp.software_vcs_revision_id = info_.software_vcs_revision_id;

    memcpy(&resp.unique_id, &info_.unique_id, uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_);

    // The node name is the name of the product like a reversed Internet domain name (or like a Java package).
    resp.name.count = info_.name.count;
    memcpy(&resp.name.elements, &info_.name.elements, info_.name.count);

    // The software image CRC and the Certificate of Authenticity are optional so not populated in this demo.
    return resp;
}

void Node::CheckScheduler(CanardMicrosecond monotonic_time) {
    if (online_) {
        for (Task *task : schedule_) {
            if (monotonic_time >= task->NextRun()) {
                task->UpdateNextRun(monotonic_time);
                task->Execute(cyphal_, monotonic_time);
            }
        }
    }
}

void Node::Schedule(Task &task) {
    schedule_.push_back(&task);
}

void Node::StartNode(uint64_t started_at) {
    started_at_ = started_at;
    online_ = true;
}

uint8_t Node::Health() {
    return cyphal_.Health();
}