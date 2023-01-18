/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 18.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_CANARDTRANSFERRECEIVER_H_
#define SOCKETCAN_SRC_OPENCYPHAL_CANARDTRANSFERRECEIVER_H_

#include <cstdint>

#include "canard.h"

class CanardTransferReceiver {
  public:
    virtual void ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const &transfer) = 0;
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_CANARDTRANSFERRECEIVER_H_
