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

/**
 * @class T104ChangeOfCommunicationInterfaceTest
 * @brief Tests for test case T104.
 */
class T104ChangeOfCommunicationInterfaceTest : public ::testing::Test {

  protected:
    /**
     * @brief Initializes logging and all variables.
     */
    void SetUp() override;

    /**
     * @brief Handles the scheduler and tx/rx loops for all nodes.
     */
    void HandleLoop();

    /**
     * @brief Handles the scheduler and tx/rx loops for all nodes.
     *
     * @param monotonic_time Current time to check with.
     */
    void HandleLoop(uint64_t monotonic_time);

    /**
     * @brief Warm up phase for all nodes before the tests start.
     *
     * @param seconds Duration of warm up.
     */
    void WarmUp(float seconds = 10);

    NodeFactory node_factory_; /**< Node factory for node creation. */

    std::unique_ptr<LatencyMeasurementNode> latency_measurement_node_1_; /**< First latency measurement node. */
    std::unique_ptr<LatencyMeasurementNode> latency_measurement_node_2_; /**< Second latency measurement node. */

};

/**
 * @test CommunicationInterfaceChange
 * @brief Tests the influence of the loss of a communication interface on nodes.
 *
 * Make sure the CAN_REDUNDANCY_FACTOR is set to 3.
 * The disabling of the interface has to be done manually.
 */
TEST_F(T104ChangeOfCommunicationInterfaceTest, CommunicationInterfaceChange) {

    WarmUp();

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(60 * MEGA);

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        HandleLoop();
    }

}

#endif //SOCKETCAN_SRC_TESTS_T104CHANGEOFCOMMUNICATIONINTERFACETEST_H_
