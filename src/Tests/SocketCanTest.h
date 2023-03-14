/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 10.02.23
*/

#ifndef SOCKETCAN_SRC_TESTS_SOCKETCANTEST_H_
#define SOCKETCAN_SRC_TESTS_SOCKETCANTEST_H_

#include <gtest/gtest.h>

#include "SocketCANTransceiver.h"

/**
 * @test SendingReceivingCanardFrames
 * @brief Checks if sending and receiving of canard frames works.
 */
TEST(SocketCANTest, SendingReceivingCanardFrames) {
    SocketCANTransceiver sender("vcan0", true);
    SocketCANTransceiver receiver("vcan0", true);

    std::string payload("Hello World");

    CanardFrame send_frame = {
        .extended_can_id = 274735200,
        .payload_size = payload.size(),
        .payload = payload.c_str()
    };

    sender.SendCanardFrame(send_frame, 0);

    CanardFrame frame = {0};
    uint8_t buf[CANARD_MTU_CAN_FD] = {0};
    uint64_t out_timestamp_usec;

    receiver.ReceiveCanardFrame(0, out_timestamp_usec, frame, sizeof(buf), buf);

    EXPECT_EQ(frame.extended_can_id, send_frame.extended_can_id);
    EXPECT_EQ(frame.payload_size, payload.size());

    EXPECT_EQ(std::string((char*) buf), payload);
}


#endif //SOCKETCAN_SRC_TESTS_SOCKETCANTEST_H_
