/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 25.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_TBYTEARRAY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_TBYTEARRAY_H_

#include "Task.h"
#include "PMessageByteArray.h"

class TByteArray : public Task {

  public:
    TByteArray(CanardPortID port_id,
               int8_t *data,
               size_t size,
               uint64_t interval);

    uint64_t Interval() const override;

    void Execute(Node &node, uint64_t current_time) override;

  private:
    PMessageByteArray message_;
    uint64_t interval_;
    size_t size_;
    int8_t data_[uavcan_primitive_array_Integer8_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_];
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_TBYTEARRAY_H_
