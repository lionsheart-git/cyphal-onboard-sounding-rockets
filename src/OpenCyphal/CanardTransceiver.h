/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 11.01.23
*/

#include <canard.h>

#ifndef SOCKETCAN_SRC_OPENCYPHAL_CANARDTRANSCEIVER_H_
#define SOCKETCAN_SRC_OPENCYPHAL_CANARDTRANSCEIVER_H_

/**
 * @class CanardTransceiver
 * @brief Abstract definition of an transceiver needed by cyphal.
 *
 * All platform specific transceiver classes need to inherit this class and implement its methods. An example of this is
 * the implementation for socketcan.
 */
class CanardTransceiver {

  public:
    /**
     * @brief Sends a canard frame.
     *
     * @param frame Frame to send.
     * @param[us] timeout_usec Microseconds until the send request should time out.
     *            Block until the frame is enqueued or until the timeout is expired.
     *            0 should lead to non blocking behavior.
     * @return Returns 1 on success, 0 on timeout, negated errno on error.
     * @retval 1 Success
     * @retval 0 Timeout
     */
    virtual int16_t SendCanardFrame(CanardFrame const &frame, uint64_t const &timeout_usec) const = 0;

    /**
     * @brief Receives a canard frame.
     *
     * @param[us] timeout_usec Microseconds until the send request should time out.
     *            Block until the frame is enqueued or until the timeout is expired.
     *            0 should lead to non blocking behavior.
     * @param[us] out_timestamp_usec Timestamp of receiveal in microseconds.
     * @param out_frame Received frame.
     * @param buf Payload buffer of received frame.
     * @return Returns 1 on success, 0 on timeout, negated errno on error.
     * @retval 1 Success
     * @retval 0 Timeout
     */
    virtual int16_t ReceiveCanardFrame(uint64_t const &timeout_usec,
                                       uint64_t &out_timestamp_usec,
                                       CanardFrame &out_frame,
                                       size_t buf_size,
                                       uint8_t buf[]) const = 0;

    /**
     * @brief Apply the specified acceptance filter configuration.
     *
     * Note that it is only possible to accept extended-format data frames.
     * The default configuration is to accept everything.
     *
     * @param num_configs Number of configs.
     * @param configs CanardFilter config to set.
     * @return Returns 0 on success, negated errno on error.
     */
    virtual int16_t CanardFilter(size_t const num_configs, CanardFilter const &configs) const = 0;

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_CANARDTRANSCEIVER_H_
