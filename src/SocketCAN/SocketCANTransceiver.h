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

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

class SocketCANTransceiver {
  public:
    explicit SocketCANTransceiver(std::string ifrName);

  private:
    void CreateSocket();
    void BindSocket(struct sockaddr_can addr);
    struct sockaddr_can IfrIndex(std::string ifrName);


    int socket_;

};

#endif //SOCKETCAN__SOCKETCANTRANSCEIVER_H_
