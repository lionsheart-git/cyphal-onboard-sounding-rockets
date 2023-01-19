/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEHEARTBEAT_H_
#define SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEHEARTBEAT_H_

#include "canard.h"

#include "uavcan/node/Heartbeat_1_0.h"

#include "PMessage.h"

class PMessageHeartbeat : public PMessage{
  public:
    explicit PMessageHeartbeat(uint64_t started_at);

    CanardTransferMetadata Metadata() override;
    uint8_t *SerializeMessage(uint64_t current_time, uint8_t health);
    uint8_t *SerializedMessage() override;
    size_t SerializedSize() const override;

  private:
    uint64_t started_at_;
    uint8_t serialized_[uavcan_node_Heartbeat_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
    uint8_t transfer_id_;
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEHEARTBEAT_H_
