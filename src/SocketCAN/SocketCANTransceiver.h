/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 01.12.22
*/

#ifndef SOCKETCAN_SOCKETCANTRANSCEIVER_H_
#define SOCKETCAN_SOCKETCANTRANSCEIVER_H_

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <vector>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "socketcan.h"
#include "CanardTransceiver.h"

/**
 * @class SocketCANTransceiver
 * @brief Transceiver for sending and receiving frames through SocketCAN.
 */
class SocketCANTransceiver : public CanardTransceiver {

  public:
    /**
     * @brief Creates a new socketcan transceiver.
     *
     * @param ifrName Name of the interface.
     * @param can_fd True, if should use can-fd.
     */
    explicit SocketCANTransceiver(const std::string& ifrName, bool can_fd);

    /**
     * @brief Creates a copy of a SocketCANTransceiver.
     *
     * @param other_transceiver Transceiver to copy.
     */
    SocketCANTransceiver(const SocketCANTransceiver& other_transceiver);

    /**
     * @brief Makes sure the inteface is close.
     */
    ~SocketCANTransceiver() override;

    /**
     * @brief Sends a canard frame.
     *
     * @copydetails socketcanPush()
     *
     * @param frame Frame to send.
     * @param timeout_usec Time to wait for sending.
     *
     * @return Returns 1 on success, 0 on timeout, negated errno on error.
     */
    int16_t SendCanardFrame(CanardFrame const &frame, uint64_t const &timeout_usec) const override;

    /**
     * @brief Receives a new canard frame.
     *
     * @copydetails socketcanPop()
     *
     * @param[in] timeout_usec How long to wait for a frame.
     * @param[out, us] out_timestamp_usec Time the frame was received in microseconds.
     * @param[out] out_frame Frame to save in.
     * @param[in] buf_size Size of the buffer for the payload.
     * @param[out] buf Payload buffer.
     *
     * @return Returns 1 on success, 0 on timeout, negated errno on error.
    */
    int16_t ReceiveCanardFrame(uint64_t const &timeout_usec,
                               uint64_t &out_timestamp_usec,
                               CanardFrame &out_frame,
                               size_t buf_size,
                               uint8_t buf[]) const override;

    /**
     * @brief Sets a new canard filter.
     *
     * @copydetails socketcanFilter()
     *
     * @param num_configs Number of configs.
     * @param configs The filter to apply.
     *
     * @return Returns 0 on success, negated errno on error.
     */
    int16_t CanardFilter(size_t const num_configs, struct CanardFilter const &configs) const override;

  private:
    int socket_; /**< File descriptor of socket. */
    std::string ifrName_; /**< Interface name. */
    bool can_fd_; /**< Uses can-fd. */

};

#endif //SOCKETCAN_SOCKETCANTRANSCEIVER_H_
