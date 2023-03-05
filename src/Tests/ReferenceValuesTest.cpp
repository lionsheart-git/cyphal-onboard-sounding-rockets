/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 04.03.23
*/

#include "Clock.h"
#include "Macros.h"

#include "ReferenceValuesTest.h"
void ReferenceValuesTest::SetUp() {
    Test::SetUp();

    char argv[] = {"ReferenceValuesTest"};

    google::InitGoogleLogging(&argv[0]);
    // FLAGS_alsologtostderr = 1;
    FLAGS_log_dir = LOG_PATH;

    node_factory_.AddSocketCanInterface("vcan0");

    LOG(INFO) << "Sending Packages every " << MEGA / 5 << " microseconds.";

    latency_measurement_node_1_ = node_factory_.CreateLatencyRequestNode(1, MEGA / 5);
    latency_measurement_node_2_ = node_factory_.CreateLatencyResponseNode(2);

    auto started_at = Clock::GetMonotonicMicroseconds();

    latency_measurement_node_1_->StartNode(started_at);
    latency_measurement_node_2_->StartNode(started_at);

}

void ReferenceValuesTest::TearDown() {
    Test::TearDown();
}

void ReferenceValuesTest::HandleLoop() {
    auto monotonic_time = Clock::GetMonotonicMicroseconds();

    // Run a trivial scheduler polling the loops that run the business logic.
    latency_measurement_node_1_->CheckScheduler(monotonic_time);
    latency_measurement_node_2_->CheckScheduler(monotonic_time);

    // Manage CAN RX/TX per redundant interface.
    latency_measurement_node_1_->HandleTxRxQueues(monotonic_time);
    latency_measurement_node_2_->HandleTxRxQueues(monotonic_time);

    // Run every 5ms to prevent using too much CPU.
    usleep(TX_PROC_SLEEP_TIME);
}

void ReferenceValuesTest::WarmUp(float seconds) {

    FLAGS_minloglevel = 10;

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(seconds * 1000000);

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        HandleLoop();
    }

    FLAGS_minloglevel = 0;

}