/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 01.12.22
*/

#ifndef SOCKETCAN__SOCKETCANTRANSCEIVER_H_
#define SOCKETCAN__SOCKETCANTRANSCEIVER_H_

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

class SocketCANTransceiver {
  public:
    explicit SocketCANTransceiver(std::string ifrName, bool can_fd);
    ~SocketCANTransceiver();

    void SendCanardFrame(CanardFrame const & frame, uint64_t const & timeout_usec) const;
    CanardFrame ReceiveCanardFrame(uint64_t const & timeout_usec) const;
    void CanardFilter(size_t const num_configs, CanardFilter const & configs) const;

    bool Send(uint32_t canid, std::vector<uint8_t> data);
    uint8_t Receive();

  private:
    void SendCANFrame(struct can_frame frame) const;
    void SendCANFrame(struct canfd_frame frame);


    int socket_;

};

#endif //SOCKETCAN__SOCKETCANTRANSCEIVER_H_
