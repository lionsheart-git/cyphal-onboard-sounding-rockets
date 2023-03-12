/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 10.02.23
*/

#ifndef SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_
#define SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_

#include <gtest/gtest.h>
#include <glog/logging.h>

#include "Node.h"
#include "NodeFactory.h"


class ReferenceConfigurationTest : public ::testing::Test {

  protected:
    void SetUp() override;
    void TearDown() override;

    void HandleLoop();
    void WarmUp(float seconds = 10);

    NodeFactory node_factory_;

    std::unique_ptr<LatencyMeasurementNode> latency_measurement_node_1_;
    std::unique_ptr<LatencyMeasurementNode> latency_measurement_node_2_;

    std::unique_ptr<Node> flight_computer_;
    std::unique_ptr<Node> telemetry_;
    std::unique_ptr<Node> sensors_;
    std::unique_ptr<Node> payload_;

};


TEST_F(ReferenceConfigurationTest, VariablesNotNullpointers) {
    ASSERT_NE(flight_computer_, nullptr);
    ASSERT_NE(telemetry_, nullptr);
    ASSERT_NE(sensors_, nullptr);
    ASSERT_NE(payload_, nullptr);
}

TEST_F(ReferenceConfigurationTest, GetInfoRequests) {

    WarmUp();

    ASSERT_NE(flight_computer_, nullptr);
    ASSERT_NE(telemetry_, nullptr);
    ASSERT_NE(sensors_, nullptr);
    ASSERT_NE(payload_, nullptr);
}

#endif //SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_
