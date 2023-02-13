/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_PUAVCANPRIMITIVEEMPTY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_PUAVCANPRIMITIVEEMPTY_H_

#include "IPMessage.h"

#include "uavcan/primitive/Empty_1_0.h"

class PUavcanPrimitiveEmpty : public IPMessage{
  public:

    PUavcanPrimitiveEmpty(CanardPortID port_id, CanardNodeID target);
    PUavcanPrimitiveEmpty(CanardTransferMetadata metadata);

    CanardTransferMetadata Metadata() override;
    uint8_t *SerializedMessage() override;
    size_t SerializedSize() const override;

  private:
    CanardTransferMetadata metadata_;
    uint8_t serialized_[uavcan_primitive_Empty_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_PUAVCANPRIMITIVEEMPTY_H_
