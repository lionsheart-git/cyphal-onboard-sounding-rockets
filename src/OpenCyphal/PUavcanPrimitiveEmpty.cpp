/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#include "PUavcanPrimitiveEmpty.h"

PUavcanPrimitiveEmpty::PUavcanPrimitiveEmpty(CanardPortID port_id, CanardNodeID target)
    : metadata_(CanardTransferMetadata{
    .priority = CanardPriorityImmediate,
    .transfer_kind = CanardTransferKindRequest,
    .port_id = port_id,
    .remote_node_id = target,
    .transfer_id = 0
}) {
}

PUavcanPrimitiveEmpty::PUavcanPrimitiveEmpty(CanardTransferMetadata metadata)
    : metadata_(metadata) {
}

CanardTransferMetadata PUavcanPrimitiveEmpty::Metadata() {
    metadata_.transfer_id++;

    return metadata_;
}

uint8_t *PUavcanPrimitiveEmpty::SerializedMessage() {

    uavcan_primitive_Empty_1_0 primitive_empty;

    uavcan_primitive_Empty_1_0_initialize_(&primitive_empty);

    size_t serialized_size = SerializedSize();
    const int8_t err = uavcan_primitive_Empty_1_0_serialize_(&primitive_empty, &serialized_[0], &serialized_size);

    assert(err >= 0);

    return serialized_;
}

size_t PUavcanPrimitiveEmpty::SerializedSize() const {
    return sizeof(serialized_);
}
