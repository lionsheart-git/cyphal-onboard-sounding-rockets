/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 05.03.23
*/

#ifndef SOCKETCAN_SRC_TESTS_T104CHANGEOFCOMMUNICATIONINTERFACETEST_H_
#define SOCKETCAN_SRC_TESTS_T104CHANGEOFCOMMUNICATIONINTERFACETEST_H_

#include <gtest/gtest.h>
#include <glog/logging.h>

#include "LatencyMeasurementNode.h"
#include "Node.h"
#include "NodeFactory.h"
#include "Clock.h"

class T104ChangeOfCommunicationInterfaceTest : public ::testing::Test {

  protected:
    void SetUp() override;
    void TearDown() override;

    void HandleLoop();
    void HandleLoop(uint64_t monotonic_time);

    void WarmUp(float seconds = 10);

    NodeFactory node_factory_;

    std::unique_ptr<LatencyMeasurementNode> latency_measurement_node_1_;
    std::unique_ptr<LatencyMeasurementNode> latency_measurement_node_2_;

};

TEST_F(T104ChangeOfCommunicationInterfaceTest, CommuicationInterfaceChange) {

    WarmUp();

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(61 * MEGA);

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        HandleLoop();
    }

}

#endif //SOCKETCAN_SRC_TESTS_T104CHANGEOFCOMMUNICATIONINTERFACETEST_H_
