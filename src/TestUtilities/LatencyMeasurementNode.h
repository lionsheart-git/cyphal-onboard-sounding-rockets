/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#ifndef SOCKETCAN_SRC_TESTUTILITIES_LATENCYMEASUREMENTNODE_H_
#define SOCKETCAN_SRC_TESTUTILITIES_LATENCYMEASUREMENTNODE_H_

#include <array>

#include "Node.h"

/**
 * @class LatencyMeasurementNode
 * @brief A Node capable of measuring latency.
 *
 * The node implements a processing of received PrimitiveEmpty messages to measure latency.
 */
class LatencyMeasurementNode : public Node {

  public:

    /**
     * @brief Creates a new latency measurement node.
     *
     * @param node_id ID of the node.
     * @param transceiver Mandatory transceiver to send and receive packets.
     * @param info Basic information about the node.
     */
    LatencyMeasurementNode(uint8_t node_id, std::unique_ptr<CanardTransceiver> transceiver, uavcan_node_GetInfo_Response_1_0 info);

    /**
     * @brief Processes a received transfer.
     *
     * @param interface_index Interface the transfer was gotten on. Used for sending back.
     * @param transfer The received transfer.
     */
    void ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const &transfer) override;

    std::array<uint64_t, 32> latency_; /**< Array containing the sending timestamps of the messages. */

};

#endif //SOCKETCAN_SRC_TESTUTILITIES_LATENCYMEASUREMENTNODE_H_
