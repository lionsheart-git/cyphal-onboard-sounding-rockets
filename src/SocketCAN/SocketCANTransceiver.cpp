/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 01.12.22
*/

#include <string>
#include "SocketCANTransceiver.h"

SocketCANTransceiver::SocketCANTransceiver(std::string ifrName)
: socket_(0) {
    CreateSocket();
    BindSocket(IfrIndex(ifrName));
}
void SocketCANTransceiver::CreateSocket() {
    if ((socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket");
        //@todo Error handling
    }
}

void SocketCANTransceiver::BindSocket(struct sockaddr_can addr) {
    if (bind(socket_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        //@todo Error handling
    }
}

struct sockaddr_can SocketCANTransceiver::IfrIndex(std::string ifrName) {
    struct ifreq ifr;
    strcpy(ifr.ifr_name, ifrName.c_str());
    ioctl(socket_, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    return addr;
}
