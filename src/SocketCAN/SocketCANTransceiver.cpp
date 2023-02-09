/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 01.12.22
*/

#include <string>

#include "SocketCANTransceiver.h"

SocketCANTransceiver::SocketCANTransceiver(const std::string& ifrName, bool can_fd)
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
    if (close(socket_ < 0)) {
        perror("Close");
    }
}

void SocketCANTransceiver::SendCANFrame(struct can_frame frame) const {
    ssize_t nbytes = write(socket_, &frame, sizeof(struct can_frame));
}

void SocketCANTransceiver::SendCANFrame(struct canfd_frame frame) {
    ssize_t nbytes = write(socket_, &frame, sizeof(struct canfd_frame));
}

bool SocketCANTransceiver::Send(uint32_t canid, std::vector<uint8_t> data) {
    struct canfd_frame frame{};

    frame.can_id = canid;
    if (data.size() > 8) {
        return false;
    }

    frame.len = data.size();
    std::copy(data.begin(), data.end(), frame.data);

    SendCANFrame(frame);

    return true;
}

uint8_t SocketCANTransceiver::Receive() {
    struct can_frame frame{};

    int nbytes = read(socket_, &frame, sizeof(struct can_frame));

    if (nbytes < 0) {
        return 0;
    }

    return nbytes;
}

int16_t SocketCANTransceiver::SendCanardFrame(CanardFrame const &frame, uint64_t const &timeout_usec) const {
    return socketcanPush(socket_, &frame, timeout_usec);
}

//@todo Figure out how to encapsulate payload so the frame can be returned without need for all the pointers.
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
