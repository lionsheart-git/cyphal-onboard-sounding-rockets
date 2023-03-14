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

/**
 * @class ReferenceConfigurationTest
 * @brief Tests of the reference configuration.
 */
class ReferenceConfigurationTest : public ::testing::Test {

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
     * @brief Warm up phase for all nodes before the tests start.
     *
     * @param seconds Duration of warm up.
     */
    void WarmUp(float seconds = 10);

    NodeFactory node_factory_; /**< Node factory for node creation. */

    std::unique_ptr<LatencyMeasurementNode> latency_measurement_node_1_; /**< First latency measurement node. */
    std::unique_ptr<LatencyMeasurementNode> latency_measurement_node_2_; /**< Second latency measurement node. */

    std::unique_ptr<Node> flight_computer_; /**< Node resembling a flight computer. */
    std::unique_ptr<Node> telemetry_; /**< Node resembling telemetry. */
    std::unique_ptr<Node> sensors_; /**< Node resembling sensors. */
    std::unique_ptr<Node> redundant_sensors_; /**< Node resembling set of redundant sensors. */
    std::unique_ptr<Node> payload_; /**< Node resembling payloads. */

  private:
    /**
     * @brief Sets up the flight computer node.
     *
     * The flight computer publishes ByteArrays 57 bytes large at 10 Hz and subscribes to the sensor data.
     */
    void SetUpFlightComputer();

    /**
     * @brief Sets up the telemetry.
     *
     * The telemetry publishes ByteArrays 256 bytes large at 245 Hz to get to roughly 500 kbit/s.
     */
    void SetUpTelemetry();

    /**
     * @brief Sets up the sensors.
     *
     * The sensors publish ByteArrays 57 bytes large at 100 Hz.
     */
    void SetUpSensors();

    /**
     * @brief Sets up the redundant sensors.
     *
     * The redundant sensors publish ByteArrays 57 bytes large at 100 Hz.
     */
    void SetUpRedundantSensors();

    /**
     * @brief Sets up the payload
     *
     * The payload publishes ByteArrays 256 bytes large at 117 Hz to get to roughly 30 kbit/s.
     */
    void SetUpPayload();

};

#define SENSOR_DATA_PORT_ID 42
#define TELEMETRY_DATA_PORT_ID 99
#define FLIGHT_COMPUTER_DATA_PORT_ID 642
#define PAYLOAD_PORT_ID 5555

/**
 * @test VariablesNotNullpointers
 * @brief Checks that no variable is a null pointer.
 */
TEST_F(ReferenceConfigurationTest, VariablesNotNullpointers) {
    ASSERT_NE(latency_measurement_node_1_, nullptr);
    ASSERT_NE(latency_measurement_node_2_, nullptr);
    ASSERT_NE(flight_computer_, nullptr);
    ASSERT_NE(telemetry_, nullptr);
    ASSERT_NE(sensors_, nullptr);
    ASSERT_NE(redundant_sensors_, nullptr);
    ASSERT_NE(payload_, nullptr);
}

/**
 * @test ReferenceConfigurationTest
 * @brief Runs the reference configuration for 60 seconds while logging the latency measurements.
 */
TEST_F(ReferenceConfigurationTest, ReferenceConfigurationTest) {

    WarmUp();

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(60 * MEGA);

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        HandleLoop();
    }
}

#endif //SOCKETCAN_SRC_TESTS_REFERENCECONFIGURATIONTEST_H_
