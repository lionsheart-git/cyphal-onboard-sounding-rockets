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

class SocketCANTransceiver : public CanardTransceiver {
  public:
    explicit SocketCANTransceiver(const std::string& ifrName, bool can_fd);
    SocketCANTransceiver(const SocketCANTransceiver& other_transceiver);
    ~SocketCANTransceiver() override;

    int16_t SendCanardFrame(CanardFrame const &frame, uint64_t const &timeout_usec) const override;
    int16_t ReceiveCanardFrame(uint64_t const &timeout_usec,
                               uint64_t &out_timestamp_usec,
                               CanardFrame &out_frame,
                               size_t buf_size,
                               uint8_t buf[]) const override;
    int16_t CanardFilter(size_t const num_configs, struct CanardFilter const &configs) const override;

  private:
    int socket_;
    std::string ifrName_;
    bool can_fd_;

};

#endif //SOCKETCAN_SOCKETCANTRANSCEIVER_H_
