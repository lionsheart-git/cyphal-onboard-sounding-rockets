/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 04.03.23
*/

#ifndef SOCKETCAN_SRC_TESTS_REFERENCEVALUESTEST_H_
#define SOCKETCAN_SRC_TESTS_REFERENCEVALUESTEST_H_

#include <gtest/gtest.h>
#include <glog/logging.h>

#include "LatencyMeasurementNode.h"
#include "Node.h"
#include "NodeFactory.h"
#include "TByteArray.h"

/**
 * @class ReferenceValuesTest
 * @brief Test for determining reference values.
 */
class ReferenceValuesTest
    : public ::testing::Test {

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
 * @test VariablesNotNullpointers
 * @brief Checks that no variable is a null pointer.
 */
TEST_F(ReferenceValuesTest, VariablesNotNullpointers) {
    ASSERT_NE(latency_measurement_node_1_, nullptr);
    ASSERT_NE(latency_measurement_node_2_, nullptr);
}

/**
 * @test ReferenceLatencyAndJitter
 * @brief Runs only the latency nodes for 60 seconds while logging the latency measurements.
 */
TEST_F(ReferenceValuesTest, ReferenceLatencyAndJitter) {

    WarmUp();

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(60 * MEGA);

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        HandleLoop();
    }

}

/**
 * @test ReferenceDatarate
 * @brief Determines a maximum data rate through publishing byte arrays.
 *
 * Runs the latency measurement nodes and a node publishing byte arrays as fast as it can. The latency is logged.
 */
TEST_F(ReferenceValuesTest, ReferenceDatarate) {

    WarmUp();

    size_t data_size = 256;
    int8_t random_data[256];

    for (int8_t i = 0; i < data_size; ++i) {
        random_data[i] = i;
    }
    auto node = node_factory_.CreateNode(0);

    auto byte_array = std::make_unique<TByteArray>(32, random_data, data_size, MEGA / 1000);
    node->Schedule(std::move(byte_array));

    auto started_at = Clock::GetMonotonicMicroseconds();

    node->StartNode(started_at);

    auto end_at = started_at + static_cast<uint64_t>(60 * MEGA);

    uint64_t monotonic_time;

    do {
        monotonic_time = Clock::GetMonotonicMicroseconds();

        HandleLoop(monotonic_time);

        node->CheckScheduler(monotonic_time);
        node->HandleTxRxQueues(monotonic_time);
    } while (monotonic_time < end_at);

}

/**
 * @test MaxNumberNodes
 * @brief Tries to find the maximum number of nodes.
 *
 * Creates as many nodes as it can without any on misbehaving.
 */
TEST_F(ReferenceValuesTest, MaxNumberNodes) {

    std::vector<std::unique_ptr<Node>> nodes;

    for (int i = 3; i < 128; ++i) {
        nodes.push_back(node_factory_.CreateNode(i));
    }

    auto started_at = Clock::GetMonotonicMicroseconds();

    for (auto const & node : nodes) {
        node->StartNode(started_at);
    }

    // Custom WarmUp
    FLAGS_minloglevel = 10;

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(10 * 1000000);

    uint64_t monotonic_time;

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        monotonic_time = Clock::GetMonotonicMicroseconds();

        HandleLoop(monotonic_time);

        for (auto const & node : nodes) {
            node->CheckScheduler(monotonic_time);
            node->HandleTxRxQueues(monotonic_time);
        }
    }

    FLAGS_minloglevel = 0;

    end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(240 * MEGA);

    do {
        monotonic_time = Clock::GetMonotonicMicroseconds();

        HandleLoop(monotonic_time);

        for (auto const & node : nodes) {
            node->CheckScheduler(monotonic_time);
            node->HandleTxRxQueues(monotonic_time);
        }
    } while (monotonic_time < end_at);

}

#endif //SOCKETCAN_SRC_TESTS_REFERENCEVALUESTEST_H_
