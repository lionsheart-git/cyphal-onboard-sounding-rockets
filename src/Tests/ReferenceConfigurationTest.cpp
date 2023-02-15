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

    node_factory_.AddSocketCanInterface("vcan0");

    flight_computer_ = node_factory_.CreateNode(42);
    telemetry_ = node_factory_.CreateNode(56);
    sensors_ = node_factory_.CreateNode(12);
    payload_ = node_factory_.CreateNode(3);

    auto started_at = Clock::GetMonotonicMicroseconds();

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

    flight_computer_->CheckScheduler(monotonic_time);
    telemetry_->CheckScheduler(monotonic_time);
    sensors_->CheckScheduler(monotonic_time);
    payload_->CheckScheduler(monotonic_time);

    // Manage CAN RX/TX per redundant interface.
    flight_computer_->HandleTxRxQueues(monotonic_time);
    telemetry_->HandleTxRxQueues(monotonic_time);
    sensors_->HandleTxRxQueues(monotonic_time);
    payload_->HandleTxRxQueues(monotonic_time);

    // Run every 5ms to prevent using too much CPU.
    usleep(TX_PROC_SLEEP_TIME);
}

void ReferenceConfigurationTest::WarmUp(float seconds) {

    auto end_at = Clock::GetMonotonicMicroseconds() + static_cast<uint64_t>(seconds * 1000000);

    while (Clock::GetMonotonicMicroseconds() < end_at) {
        HandleLoop();
    }

}

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
