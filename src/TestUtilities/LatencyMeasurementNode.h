/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#ifndef SOCKETCAN_SRC_TESTUTILITIES_LATENCYMEASUREMENTNODE_H_
#define SOCKETCAN_SRC_TESTUTILITIES_LATENCYMEASUREMENTNODE_H_

#include <array>

#include "Node.h"

class LatencyMeasurementNode : public Node {

  public:

    LatencyMeasurementNode(uint8_t node_id, std::unique_ptr<CanardTransceiver> transceiver, uavcan_node_GetInfo_Response_1_0 info);

    void ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const &transfer) override;

    std::array<uint64_t, 32> latency_;

};

#endif //SOCKETCAN_SRC_TESTUTILITIES_LATENCYMEASUREMENTNODE_H_
