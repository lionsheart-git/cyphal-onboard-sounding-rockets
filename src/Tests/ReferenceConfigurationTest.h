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
#include "Clock.h"

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
    std::unique_ptr<Node> redundant_sensors_;
    std::unique_ptr<Node> payload_;

  private:
    void SetUpFlightComputer();
    void SetUpTelemetry();
    void SetUpSensors();
    void SetUpRedundantSensors();
    void SetUpPayload();

};

#define SENSOR_DATA_PORT_ID 42
#define TELEMETRY_DATA_PORT_ID 99
#define FLIGHT_COMPUTER_DATA_PORT_ID 642
#define PAYLOAD_PORT_ID 5555


TEST_F(ReferenceConfigurationTest, VariablesNotNullpointers) {
    ASSERT_NE(flight_computer_, nullptr);
    ASSERT_NE(telemetry_, nullptr);
    ASSERT_NE(sensors_, nullptr);
    ASSERT_NE(payload_, nullptr);
}

TEST_F(ReferenceConfigurationTest, ReferenceConfigurationTest) {

    WarmUp();

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(60 * MEGA);

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        HandleLoop();
    }
}

#endif //SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_
