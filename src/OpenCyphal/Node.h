/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 18.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_NODE_H_
#define SOCKETCAN_SRC_OPENCYPHAL_NODE_H_

#include <cstdint>

#include "uavcan/node/GetInfo_1_0.h"

#include "OpenCyphal.h"
#include "CanardTransferReceiver.h"

class Node : public CanardTransferReceiver {

  public:
    Node(OpenCyphal & cyphal, uavcan_node_GetInfo_Response_1_0 info);

    /**
     * @brief Processes the received transfers.
     *
     * @param interface_index Interface index the transfer came from.
     * @param transfer Transfer to process.
     */
    void ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const & transfer) override;

  private:

    /**
     * @brief Processes the gotten GetInfo request.
     *
     * @return Populated GetInfo request.
     */
    uavcan_node_GetInfo_Response_1_0 ProcessRequestNodeGetInfo();

    OpenCyphal &cyphal_;
    uavcan_node_GetInfo_Response_1_0 info_;
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_NODE_H_
