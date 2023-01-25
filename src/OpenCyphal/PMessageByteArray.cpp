/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 25.01.23
*/

#include "PMessageByteArray.h"
PMessageByteArray::PMessageByteArray(CanardPortID port_id)
    : metadata_(CanardTransferMetadata{
    .priority = CanardPriorityNominal,
    .transfer_kind = CanardTransferKindMessage,
    .port_id = port_id,
    .remote_node_id = CANARD_NODE_ID_UNSET,
    .transfer_id = 0
}) {
}

PMessageByteArray::PMessageByteArray(CanardTransferMetadata metadata)
    : metadata_(metadata) {

}

CanardTransferMetadata PMessageByteArray::Metadata() {
    metadata_.transfer_id++;

    return metadata_;
}

uint8_t PMessageByteArray::SerializeMessage(int8_t const *byte_array, size_t size) {

    if (size > uavcan_primitive_array_Integer8_1_0_value_ARRAY_CAPACITY_) {
        return 5;
    }

    uavcan_primitive_array_Integer8_1_0 array;
    uavcan_primitive_array_Integer8_1_0_initialize_(&array);

    memcpy(array.value.elements, &byte_array, size);
    array.value.count = size;

    size_t serialized_size = SerializedSize();
    const int8_t err = uavcan_primitive_array_Integer8_1_0_serialize_(&array, &serialized_[0], &serialized_size);

    assert(err >= 0);

    return err;
}

uint8_t *PMessageByteArray::SerializedMessage() {
    return serialized_;
}

size_t PMessageByteArray::SerializedSize() const {
    return sizeof(serialized_);
}
