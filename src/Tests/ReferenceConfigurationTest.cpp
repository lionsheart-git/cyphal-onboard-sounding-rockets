/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 10.02.23
*/

#include "ReferenceConfigurationTest.h"

#include "Clock.h"
#include "Macros.h"

#include "SByteArray.h"
#include "TByteArray.h"

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

    SetUpFlightComputer();
    SetUpTelemetry();
    SetUpSensors();
    SetUpRedundantSensors();
    SetUpPayload();

    auto started_at = Clock::GetMonotonicMicroseconds();

    latency_measurement_node_1_->StartNode(started_at);
    latency_measurement_node_2_->StartNode(started_at);

    flight_computer_->StartNode(started_at);
    telemetry_->StartNode(started_at);
    sensors_->StartNode(started_at);
    redundant_sensors_->StartNode(started_at);
    payload_->StartNode(started_at);
}

void ReferenceConfigurationTest::HandleLoop() {
    // Run a trivial scheduler polling the loops that run the business logic.
    CanardMicrosecond monotonic_time = Clock::GetMonotonicMicroseconds();

    latency_measurement_node_1_->CheckScheduler(monotonic_time);
    latency_measurement_node_2_->CheckScheduler(monotonic_time);

    flight_computer_->CheckScheduler(monotonic_time);
    telemetry_->CheckScheduler(monotonic_time);
    sensors_->CheckScheduler(monotonic_time);
    redundant_sensors_->CheckScheduler(monotonic_time);
    payload_->CheckScheduler(monotonic_time);

    // Manage CAN RX/TX per redundant interface.
    latency_measurement_node_1_->HandleTxRxQueues(monotonic_time);
    latency_measurement_node_2_->HandleTxRxQueues(monotonic_time);

    flight_computer_->HandleTxRxQueues(monotonic_time);
    telemetry_->HandleTxRxQueues(monotonic_time);
    sensors_->HandleTxRxQueues(monotonic_time);
    redundant_sensors_->HandleTxRxQueues(monotonic_time);
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

void ReferenceConfigurationTest::SetUpFlightComputer() {

    flight_computer_ = node_factory_.CreateNode(42);

    auto subscribe_sensor_data = std::make_unique<SByteArray>(SENSOR_DATA_PORT_ID);

    flight_computer_->Subscribe(std::move(subscribe_sensor_data));

    size_t data_size = 57;
    int8_t random_data[57];

    for (int8_t i = 0; i < data_size; ++i) {
        random_data[i] = i;
    }

    auto byte_array = std::make_unique<TByteArray>(FLIGHT_COMPUTER_DATA_PORT_ID, random_data, data_size, MEGA / 10);
    flight_computer_->Schedule(std::move(byte_array));
}

void inline ReferenceConfigurationTest::SetUpTelemetry() {
    telemetry_ = node_factory_.CreateNode(56);

    size_t data_size = 256;
    int8_t random_data[256];

    for (int8_t i = 0; i < data_size; ++i) {
        random_data[i] = i;
    }

    auto byte_array = std::make_unique<TByteArray>(TELEMETRY_DATA_PORT_ID, random_data, data_size, MEGA / 245);
    telemetry_->Schedule(std::move(byte_array));
}

void inline ReferenceConfigurationTest::SetUpSensors() {
    sensors_ = node_factory_.CreateNode(12);

    size_t data_size = 57;
    int8_t random_data[57];

    for (int8_t i = 0; i < data_size; ++i) {
        random_data[i] = i;
    }

    auto byte_array = std::make_unique<TByteArray>(SENSOR_DATA_PORT_ID, random_data, data_size, MEGA / 100);
    sensors_->Schedule(std::move(byte_array));

}
void ReferenceConfigurationTest::SetUpRedundantSensors() {
    redundant_sensors_ = node_factory_.CreateNode(64);

    size_t data_size = 57;
    int8_t random_data[57];

    for (int8_t i = 0; i < data_size; ++i) {
        random_data[i] = i;
    }

    auto byte_array = std::make_unique<TByteArray>(SENSOR_DATA_PORT_ID, random_data, data_size, MEGA / 100);
    redundant_sensors_->Schedule(std::move(byte_array));
}

void ReferenceConfigurationTest::SetUpPayload() {
    payload_ = node_factory_.CreateNode(3);

    size_t data_size = 256;
    int8_t random_data[256];

    for (int8_t i = 0; i < data_size; ++i) {
        random_data[i] = i;
    }

    auto byte_array = std::make_unique<TByteArray>(PAYLOAD_PORT_ID, random_data, data_size, MEGA / 117);
    payload_->Schedule(std::move(byte_array));
}
