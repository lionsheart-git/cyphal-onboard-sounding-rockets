/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_IPMESSAGE_H_
#define SOCKETCAN_SRC_OPENCYPHAL_IPMESSAGE_H_

#include "canard.h"

/**
 * @brief Interface for all publish messages.
 *
 * All messages that are published should inherit from this base class.
 */
class IPMessage {
  public:
    /**
     * @brief Metadata needed to transfer the payload.
     *
     * @return The metadata for the transfer.
     */
    virtual CanardTransferMetadata Metadata() = 0;

    /**
     * @briefThe serialized message to be send.
     *
     * @return A pointer to the array storing the serialized message.
     */
    virtual uint8_t *SerializedMessage() = 0;

    /**
     * @brief Size of the serialized message.
     *
     * @return The size of the buffer holding the serialized message.
     */
    virtual size_t SerializedSize() const = 0;

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_IPMESSAGE_H_
