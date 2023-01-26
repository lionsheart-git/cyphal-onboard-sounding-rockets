/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 25.01.23
*/

#include "TByteArray.h"

TByteArray::TByteArray(CanardPortID port_id,
                       int8_t *data,
                       size_t size,
                       uint64_t interval)
    : message_(port_id), size_(size), interval_(interval), data_() {
    memcpy(data_, data, size);
}

uint64_t TByteArray::Interval() const {
    return interval_;
}

void TByteArray::Execute(Node &node, uint64_t current_time) {
    message_.SerializeMessage(data_, size_);

    node.Publish(current_time + Interval() - 1, message_);
}
