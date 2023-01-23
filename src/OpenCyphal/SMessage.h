/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 20.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_SMESSAGE_H_
#define SOCKETCAN_SRC_OPENCYPHAL_SMESSAGE_H_

#include "canard.h"

class SMessage {

  public:
    SMessage(CanardTransferKind transfer_kind,
             CanardPortID port_id,
             size_t extent,
             CanardMicrosecond transfer_id_timeout_usec);

    CanardRxSubscription* Subscription();

    CanardPortID PortID() const;

    size_t Extent() const;

    CanardMicrosecond TransferIDTimeout() const;

    CanardTransferKind TransferKind();

  private:
    CanardRxSubscription subscription_;
    CanardTransferKind const transfer_kind_;
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_SMESSAGE_H_
