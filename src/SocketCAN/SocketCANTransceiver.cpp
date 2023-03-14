/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 01.12.22
*/

#include <string>

#include "SocketCANTransceiver.h"

SocketCANTransceiver::SocketCANTransceiver(const std::string &ifrName, bool can_fd)
    : socket_(0) {
    socket_ = socketcanOpen(ifrName.c_str(), can_fd);
    ifrName_ = ifrName;
    can_fd_ = can_fd;
}

SocketCANTransceiver::SocketCANTransceiver(SocketCANTransceiver const &other_transceiver)
    : socket_(0), ifrName_(other_transceiver.ifrName_), can_fd_(other_transceiver.can_fd_) {

    socket_ = socketcanOpen(ifrName_.c_str(), can_fd_);
}

SocketCANTransceiver::~SocketCANTransceiver() {
    if (close(socket_) < 0) {
        perror("Close");
    }
}

int16_t SocketCANTransceiver::SendCanardFrame(CanardFrame const &frame, uint64_t const &timeout_usec) const {
    return socketcanPush(socket_, &frame, timeout_usec);
}

// Figure out how to encapsulate payload so the frame can be returned without need for all the pointers.
int16_t SocketCANTransceiver::ReceiveCanardFrame(uint64_t const &timeout_usec,
                                                 uint64_t &out_timestamp_usec,
                                                 CanardFrame &out_frame,
                                                 size_t buf_size,
                                                 uint8_t buf[]) const {
    return socketcanPop(socket_, &out_frame, &out_timestamp_usec, buf_size, buf, timeout_usec,
                        nullptr);

}

int16_t SocketCANTransceiver::CanardFilter(size_t const num_configs, struct CanardFilter const &configs) const {
    return socketcanFilter(socket_, num_configs, &configs);
}
