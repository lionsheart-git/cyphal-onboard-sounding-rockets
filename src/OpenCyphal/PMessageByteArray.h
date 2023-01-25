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


class PMessageByteArray : public IPMessage {

  public:

    explicit PMessageByteArray(CanardPortID port_id);

    explicit PMessageByteArray(CanardTransferMetadata metadata);

    CanardTransferMetadata Metadata() override;

    uint8_t* SerializedMessage() override;

    size_t SerializedSize() const override;

    uint8_t SerializeMessage(int8_t const *byte_array, size_t size);

  private:
    uint8_t serialized_[uavcan_primitive_array_Integer8_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_]{};
    CanardTransferMetadata metadata_;
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_PMESSAGEBYTEARRAY_H_
