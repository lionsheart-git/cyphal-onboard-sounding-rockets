/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 20.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_SMESSAGE_H_
#define SOCKETCAN_SRC_OPENCYPHAL_SMESSAGE_H_

#include "canard.h"

/**
 * @brief Base class for subscribe messages.
 */
class SMessage {

  public:
    /**
     * @brief Constructs a new subscribe message.
     *
     * @param transfer_kind Kind of transfer.
     * @param port_id Port to listen on.
     * @param extent Maximum size of the message.
     * @param transfer_id_timeout_usec Timeout.
     */
    SMessage(CanardTransferKind transfer_kind,
             CanardPortID port_id,
             size_t extent,
             CanardMicrosecond transfer_id_timeout_usec);

    /**
     * @brief Pointer to stored subscription.
     *
     * Is needed by Canard and should be stored for the lifetime of the subscription.
     *
     * @return A pointer to the subscription object.
     */
    CanardRxSubscription* Subscription();

    /**
     * @brief Target port ID.
     *
     * @return The target port ID.
     */
    CanardPortID PortID() const;

    /**
     * @brief Extent of the message.
     * @return The extent of the message.
     */
    size_t Extent() const;

    /**
     * @brief Transfer IDs timeout.
     * @return The transfer ids timeout.
     */
    CanardMicrosecond TransferIDTimeout() const;

    /**
     * @brief The transfer kind.
     * @return The transfer kind.
     */
    CanardTransferKind TransferKind();

  private:
    CanardRxSubscription subscription_; /**< The stored canard subscription. */
    CanardTransferKind const transfer_kind_; /**< The transfer kind */
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_SMESSAGE_H_
