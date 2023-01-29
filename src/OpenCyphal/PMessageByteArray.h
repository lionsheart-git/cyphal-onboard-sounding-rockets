/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 25.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEBYTEARRAY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEBYTEARRAY_H_

#include "canard.h"
#include "uavcan/primitive/array/Integer8_1_0.h"

#include "IPMessage.h"

/**
 * @class PMessageByteArray
 * @brief Publish message sending a byte array.
 */
class PMessageByteArray : public IPMessage {

  public:

    /**
     * @brief New byte array with target port id.
     *
     * @param port_id Port id for the topic of the byte array.
     */
    explicit PMessageByteArray(CanardPortID port_id);

    /**
     * @brief New byte array with the transfers metadata.
     *
     * @param metadata Metadata of the transfer.
     */
    explicit PMessageByteArray(CanardTransferMetadata metadata);

    /**
     * @brief Metadata of the transfer.
     *
     * @return The populated CanardTransferMetadata struct.
     */
    CanardTransferMetadata Metadata() override;

    /**
     * @brief The serialized message.
     *
     * @return A pointer to the array containing the serialized message.
     */
    uint8_t* SerializedMessage() override;

    /**
     * @brief Size of the serialized message.
     *
     * @return The size of the serialized message.
     */
    size_t SerializedSize() const override;

    /**
     * @brief Serializes a new message.
     *
     * @param byte_array The byte array to serialize.
     * @param size Size of the input array.
     * @return Error of the serialization.
     */
    uint8_t SerializeMessage(int8_t const *byte_array, size_t size);

  private:
    uint8_t serialized_[uavcan_primitive_array_Integer8_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_]{}; /**< Array containing the serialized message. */
    CanardTransferMetadata metadata_; /**< Metadata of the transfer. */
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEBYTEARRAY_H_
