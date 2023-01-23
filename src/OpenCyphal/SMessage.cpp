/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 20.01.23
*/

#include "SMessage.h"
SMessage::SMessage(CanardTransferKind transfer_kind,
                   CanardPortID port_id,
                   size_t extent,
                   CanardMicrosecond transfer_id_timeout_usec) :
    subscription_(), transfer_kind_(transfer_kind) {
    subscription_.transfer_id_timeout_usec = transfer_id_timeout_usec;
    subscription_.extent = extent;
    subscription_.port_id = port_id;
}

CanardRxSubscription *SMessage::Subscription() {
    return &subscription_;
}

CanardTransferKind SMessage::TransferKind() {
    return transfer_kind_;
}
CanardPortID SMessage::PortID() const {
    return subscription_.port_id;
}
size_t SMessage::Extent() const {
    return subscription_.extent;
}
CanardMicrosecond SMessage::TransferIDTimeout() const {
    return subscription_.transfer_id_timeout_usec;
}
