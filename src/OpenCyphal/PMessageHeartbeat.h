/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEHEARTBEAT_H_
#define SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEHEARTBEAT_H_

#include "canard.h"

#include "uavcan/node/Heartbeat_1_0.h"

#include "IPMessage.h"
#include "Node.h"

/**
 * @brief Message for publishing a heartbeat.
 *
 * This class should be only instantiated once for each node sending a heartbeat. The internal counter needed to keep
 * track of the heartbeats number is needed.
 */
class PMessageHeartbeat : public IPMessage{
  public:
    /**
     * @brief Constructs a new instance of the heartbeat message.
     *
     * @copydetails PMessageHeartbeat
     *
     * @param started_at Time the node was started. Needed to calculate uptime.
     */
    explicit PMessageHeartbeat(Node const &parent_node);

    /**
     * @brief Metadata of the heartbeat message.
     *
     * @return The metadata of the heartbeat message.
     */
    CanardTransferMetadata Metadata() override;

    /**
     * @brief Serializes the message using the current health and time data.
     *
     * @param current_time Current time.
     * @param health Current health of the node
     * @return Pointer to the serialized message.
     */
    uint8_t *SerializeMessage(uint64_t current_time, uint8_t health);

    /**
     * @brief Returns the serialized message to be published.
     *
     * @return Pointer to the serialized message.
     */
    uint8_t *SerializedMessage() override;

    /**
     * @copydoc IPMessage::SerializedSize
     */
    size_t SerializedSize() const override;

  private:
    Node const *node_;
    uint64_t started_at_; /**< Time the node was started */
    uint8_t serialized_[uavcan_node_Heartbeat_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_]; /** Buffer holding the serialized message */
    uint8_t transfer_id_; /**< Counter keeping track of the current transfer id. Incremented each time a message is send. */
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEHEARTBEAT_H_
