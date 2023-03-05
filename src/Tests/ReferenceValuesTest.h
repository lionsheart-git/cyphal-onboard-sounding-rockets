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

class ReferenceValuesTest
    : public ::testing::Test {

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

TEST_F(ReferenceValuesTest, VariablesNotNullpointers) {
    ASSERT_NE(latency_measurement_node_1_, nullptr);
    ASSERT_NE(latency_measurement_node_2_, nullptr);
}

TEST_F(ReferenceValuesTest, ReferenceLatencyAndJitter) {

    WarmUp();

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(61 * MEGA);

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        HandleLoop();
    }

}

TEST_F(ReferenceValuesTest, ReferenceDatarate) {

    WarmUp(5);

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

TEST_F(ReferenceValuesTest, MaxNumberNodes) {

    WarmUp();

    std::vector<std::unique_ptr<Node>> nodes;

    for (int i = 3; i < 128; ++i) {
        nodes.push_back(node_factory_.CreateNode(i));
    }

    auto started_at = Clock::GetMonotonicMicroseconds();

    for (auto const & node : nodes) {
        node->StartNode(started_at);
    }

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(61 * MEGA);

    uint64_t monotonic_time;

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
