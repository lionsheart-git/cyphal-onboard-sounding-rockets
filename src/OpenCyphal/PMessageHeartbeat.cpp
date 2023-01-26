/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#include "PMessageHeartbeat.h"

#include "Macros.h"
PMessageHeartbeat::PMessageHeartbeat(Node const &parent_node) :
    node_(&parent_node), started_at_(), transfer_id_(0), serialized_()
{
}

CanardTransferMetadata PMessageHeartbeat::Metadata() {

    const CanardTransferMetadata transfer = {
        .priority       = CanardPriorityNominal,
        .transfer_kind  = CanardTransferKindMessage,
        .port_id        = uavcan_node_Heartbeat_1_0_FIXED_PORT_ID_,
        .remote_node_id = CANARD_NODE_ID_UNSET,
        .transfer_id    = (CanardTransferID) (transfer_id_++),
    };

    return transfer;
}

uint8_t *PMessageHeartbeat::SerializeMessage(uint64_t current_time, uint8_t health) {
    uavcan_node_Heartbeat_1_0 heartbeat = {0};
    heartbeat.uptime = (uint32_t) ((current_time - node_->StartedAt()) / MEGA);
    heartbeat.mode.value = uavcan_node_Mode_1_0_OPERATIONAL;
    heartbeat.health.value = health;

    size_t serialized_size = SerializedSize();

    const int8_t err = uavcan_node_Heartbeat_1_0_serialize_(&heartbeat, &serialized_[0], &serialized_size);
    assert(err >= 0);

    return serialized_;
}

uint8_t *PMessageHeartbeat::SerializedMessage() {
    return serialized_;
}

inline size_t PMessageHeartbeat::SerializedSize() const {
    return sizeof(serialized_);
}

