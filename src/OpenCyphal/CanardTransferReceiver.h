/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 18.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_CANARDTRANSFERRECEIVER_H_
#define SOCKETCAN_SRC_OPENCYPHAL_CANARDTRANSFERRECEIVER_H_

#include <cstdint>

#include "canard.h"

/**
 * @brief Defines a function to process received transfers.
 */
class CanardTransferReceiver {

  public:
    /**
     * @brief Has to be implemented to receive a transfer.
     *
     * @param interface_index The interface index the transfer was received on.
     * @param transfer The frame that was received.
     */
    virtual void ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const &transfer) = 0;
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_CANARDTRANSFERRECEIVER_H_
