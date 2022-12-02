/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 01.12.22
*/

#include <string>
#include "SocketCANTransceiver.h"

SocketCANTransceiver::SocketCANTransceiver(std::string ifrName)
: socket_(0) {
    socket_ = socketcanOpen(ifrName.c_str(), 1);
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
    return false;
}
