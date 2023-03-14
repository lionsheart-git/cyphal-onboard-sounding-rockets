/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 26.01.23
*/

#ifndef SOCKETCAN_SRC_TESTUTILITIES_NODEFACTORY_H_
#define SOCKETCAN_SRC_TESTUTILITIES_NODEFACTORY_H_

#include <cstdint>
#include <vector>
#include <memory>

#include "Node.h"
#include "LatencyMeasurementNode.h"
#include "CanardTransceiver.h"
#include "Macros.h"

/**
 * @class NodeFactory
 * @brief Factory for creating nodes.
 */
class NodeFactory {
  public:

    /**
     * @brief Node factory with all node ids available.
     */
    NodeFactory() = default;

    /**
     * @brief Creates a new node factory.
     *
     * @param used_ids Array of ids already used.
     * @param size Size of the array.
     */
    NodeFactory(uint8_t *used_ids, size_t size);

    /**
     * @brief Creates a new node with a random id.
     *
     * @return Pointer to the new node.
     */
    std::unique_ptr<Node> CreateNode();

    /**
     * @brief Creates a new node with the desired id.
     *
     * @param node_id ID of the new node to create.
     * @return Pointer to the new node.
     */
    std::unique_ptr<Node> CreateNode(uint8_t node_id);

    /**
     * @brief Creates a new latency node without any tasks.
     * @param node_id ID of the node.
     *
     * @return A new latency measurement node.
     */
    std::unique_ptr<LatencyMeasurementNode> CreateLatencyNode(uint8_t node_id);

    /**
     * @brief Creates a new latency measurement node with a sending of latency packets.
     *
     * This node has the task TLatencyMeasurement.
     *
     * @param node_id ID of the node.
     * @param interval Interval in which to send latency measurement packets.
     * @return A new latency measurement node sending latency measurement packets.
     */
    std::unique_ptr<LatencyMeasurementNode> CreateLatencyRequestNode(uint8_t node_id, uint64_t interval);

    /**
     * @brief Creates a new latency measurement node with the ability to answer to received latency packets.
     *
     * This node subscribes to PrimitveEmpty packets on the LATENCY_MEASUREMENT_PORT_ID.
     *
     * @param node_id ID of the node.
     * @return A new latency measurement node answering to latency measurement packets.
     */
    std::unique_ptr<LatencyMeasurementNode> CreateLatencyResponseNode(uint8_t node_id);

    /**
     * @brief Adds a new Canard transceiver to the list.
     *
     * @param transceiver The new transceiver to add.
     */
    void AddTransceiver(std::unique_ptr<CanardTransceiver> transceiver);

    /**
     * @brief Adding of socketcan interface names to the list.
     *
     * @param socket_can_interface Name of the interface to add.
     */
    void AddSocketCanInterface(std::string socket_can_interface);

  private:
    /**
     * @brief Creates a 128bit unique id.
     *
     * @param out Array to populate.
     */
    static void GetUniqueID(uint8_t out[16U]);

    std::vector<uint8_t> used_ids_; /**< List of already used node ids. */
    std::vector<std::unique_ptr<CanardTransceiver>> transceiver_;/**< Array of transceivers. */
    std::vector<std::string> socket_can_interfaces_; /**< List of socketcan interfaces. */
};

#endif //SOCKETCAN_SRC_TESTUTILITIES_NODEFACTORY_H_
