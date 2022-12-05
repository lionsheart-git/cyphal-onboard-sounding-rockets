/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 01.12.22
*/

#include <string>
#include "SocketCANTransceiver.h"

SocketCANTransceiver::SocketCANTransceiver(std::string ifrName, bool can_fd)
: socket_(0) {
    socket_ = socketcanOpen(ifrName.c_str(), can_fd);
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
}
