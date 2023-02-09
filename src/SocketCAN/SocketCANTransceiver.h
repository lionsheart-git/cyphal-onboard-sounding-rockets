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
    ~SocketCANTransceiver();

    int16_t SendCanardFrame(CanardFrame const &frame, uint64_t const &timeout_usec) const override;
    int16_t ReceiveCanardFrame(uint64_t const &timeout_usec,
                               uint64_t &out_timestamp_usec,
                               CanardFrame &out_frame,
                               size_t buf_size,
                               uint8_t buf[]) const override;
    int16_t CanardFilter(size_t const num_configs, struct CanardFilter const &configs) const override;

    bool Send(uint32_t canid, std::vector<uint8_t> data);
    uint8_t Receive();

  private:
    void SendCANFrame(struct can_frame frame) const;
    void SendCANFrame(struct canfd_frame frame);

    int socket_;
    std::string ifrName_;
    bool can_fd_;

};

#endif //SOCKETCAN_SOCKETCANTRANSCEIVER_H_
