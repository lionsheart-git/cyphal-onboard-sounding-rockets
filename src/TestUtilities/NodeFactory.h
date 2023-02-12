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
     * @brief Adds a new Canard transceiver to the list.
     *
     * @param transceiver The new transceiver to add.
     */
    void AddTransceiver(std::unique_ptr<CanardTransceiver> transceiver);

    void AddSocketCanInterface(std::string socket_can_interface);

  private:
    /**
     * @brief Creates a 128bit unique id.
     *
     * @param out Array to populate.
     */
    static void GetUniqueID(uint8_t out[16U]);

    std::vector<uint8_t> used_ids_; /**< List of already used node ids. */
    std::vector<std::unique_ptr<CanardTransceiver>> transceiver_;/**< Array of transceivers */
    std::vector<std::string> socket_can_interfaces_;
};

#endif //SOCKETCAN_SRC_TESTUTILITIES_NODEFACTORY_H_
