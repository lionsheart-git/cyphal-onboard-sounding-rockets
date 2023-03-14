/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_PUAVCANPRIMITIVEEMPTY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_PUAVCANPRIMITIVEEMPTY_H_

#include "IPMessage.h"

#include "uavcan/primitive/Empty_1_0.h"

/**
 * @class PUavcanPrimitiveEmpty
 * @brief Publishing of a primitive empty message.
 */
class PUavcanPrimitiveEmpty
    : public IPMessage {
  public:

    /**
     * @brief Creates a new primitive empty message.
     *
     * @param port_id Port id to publish on.
     * @param target Target node id.
     */
    PUavcanPrimitiveEmpty(CanardPortID port_id, CanardNodeID target);

    /**
     * @brief Creates a new message with specific metadata.
     * @param metadata Metadata to use.
     */
    explicit PUavcanPrimitiveEmpty(CanardTransferMetadata metadata);

    /**
     * @brief Get the metadata of the message.
     *
     * @return The metadata of the message.
     */
    CanardTransferMetadata Metadata() override;

    /**
     * @brief Get a pointer to the serialized message.
     *
     * @return A pointer to the serialized message.
     */
    uint8_t *SerializedMessage() override;

    /**
     * @brief Get the size of the serialized message.
     *
     * @return The size of the serialized message.
     */
    size_t SerializedSize() const override;

  private:
    CanardTransferMetadata metadata_; /**< The metadata of the message. */
    uint8_t serialized_[uavcan_primitive_Empty_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_]; /**< Array containing the serialized message. */

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_PUAVCANPRIMITIVEEMPTY_H_
