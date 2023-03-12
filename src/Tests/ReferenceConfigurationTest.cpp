/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 10.02.23
*/

#include "ReferenceConfigurationTest.h"

#include "Clock.h"
#include "Macros.h"

void ReferenceConfigurationTest::SetUp() {
    Test::SetUp();

    char argv[] = {"ReferenceConfigurationTest"};

    google::InitGoogleLogging(&argv[0]);
    // FLAGS_alsologtostderr = 1;
    FLAGS_log_dir = LOG_PATH;

    node_factory_.AddSocketCanInterface("vcan0");


    LOG(INFO) << "Sending Packages every " << MEGA / 5 << " microseconds.";

    latency_measurement_node_1_ = node_factory_.CreateLatencyRequestNode(1, MEGA / 5);
    latency_measurement_node_2_ = node_factory_.CreateLatencyResponseNode(2);

    flight_computer_ = node_factory_.CreateNode(42);
    telemetry_ = node_factory_.CreateNode(56);
    sensors_ = node_factory_.CreateNode(12);
    payload_ = node_factory_.CreateNode(3);

    auto started_at = Clock::GetMonotonicMicroseconds();

    latency_measurement_node_1_->StartNode(started_at);
    latency_measurement_node_2_->StartNode(started_at);

    flight_computer_->StartNode(started_at);
    telemetry_->StartNode(started_at);
    sensors_->StartNode(started_at);
    payload_->StartNode(started_at);
}

void ReferenceConfigurationTest::TearDown() {
    Test::TearDown();
}

void ReferenceConfigurationTest::HandleLoop() {
    // Run a trivial scheduler polling the loops that run the business logic.
    CanardMicrosecond monotonic_time = Clock::GetMonotonicMicroseconds();

    latency_measurement_node_1_->CheckScheduler(monotonic_time);
    latency_measurement_node_2_->CheckScheduler(monotonic_time);

    flight_computer_->CheckScheduler(monotonic_time);
    telemetry_->CheckScheduler(monotonic_time);
    sensors_->CheckScheduler(monotonic_time);
    payload_->CheckScheduler(monotonic_time);

    // Manage CAN RX/TX per redundant interface.
    latency_measurement_node_1_->HandleTxRxQueues(monotonic_time);
    latency_measurement_node_2_->HandleTxRxQueues(monotonic_time);

    flight_computer_->HandleTxRxQueues(monotonic_time);
    telemetry_->HandleTxRxQueues(monotonic_time);
    sensors_->HandleTxRxQueues(monotonic_time);
    payload_->HandleTxRxQueues(monotonic_time);
}

void ReferenceConfigurationTest::WarmUp(float seconds) {

    FLAGS_minloglevel = 10;

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(seconds * 1000000);

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        HandleLoop();
    }

    FLAGS_minloglevel = 0;

}
