/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 25.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_TBYTEARRAY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_TBYTEARRAY_H_

#include "Task.h"
#include "PMessageByteArray.h"

/**
 * @class TByteArray
 * @brief Creates a new task sending a byte array.
 */
class TByteArray : public Task {

  public:
    /**
     * @brief A new task sending a byte array periodically.
     *
     * @param port_id ID of the topic of the byte array.
     * @param data Array containing data to send.
     * @param size Size of the array.
     * @param interval[us] Interval of how often to send the array.
     */
    TByteArray(CanardPortID port_id,
               int8_t *data,
               size_t size,
               uint64_t interval);

    /**
     * @copydoc Task::Interval
     */
    uint64_t Interval() const override;

    /**
     * @copydoc Task::Execute
     */
    void Execute(Node &node, uint64_t current_time) override;

  private:
    PMessageByteArray message_; /**< Byte array message to send. */
    uint64_t interval_; /**< Sending interval. */
    size_t size_; /**< Size of the data. */
    int8_t data_[uavcan_primitive_array_Integer8_1_0_SERIALIZATION_BUFFER_SIZE_BYTES_]; /**< Array containing data to send. */
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_TBYTEARRAY_H_
