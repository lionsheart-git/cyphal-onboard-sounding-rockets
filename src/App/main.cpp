#include <iostream>

#include <cstring>
#include <unistd.h>

#include <glog/logging.h>

#include "SocketCANTransceiver.h"
#include "OpenCyphal.h"
#include "Node.h"
#include "Clock.h"
#include "THeartbeat.h"
#include "Macros.h"
#include "SMessageGetInfo.h"
#include "TByteArray.h"
#include "NodeFactory.h"
#include "TLatencyMeasurement.h"
#include "LatencyMeasurementNode.h"
#include "SRequestPrimitiveEmpty.h"
#include "SResponsePrimitiveEmpty.h"

#include "uavcan/node/GetInfo_1_0.h"

int main(int argc, char *argv[]) {
//    google::InitGoogleLogging(argv[0]);
//    FLAGS_alsologtostderr = 1;
//    FLAGS_log_dir = LOG_PATH;

    NodeFactory factory;
    factory.AddSocketCanInterface("vcan0");
    factory.AddSocketCanInterface("vcan1");
    factory.AddSocketCanInterface("vcan2");


    auto node = factory.CreateLatencyRequestNode(1, MEGA / 2);

    auto node2 = factory.CreateLatencyResponseNode(2);

//     auto node3 = factory.CreateNode(42);
//    auto node4 = factory.CreateNode(58);


//    size_t data_size = 256;
//    int8_t random_data[256];
//
//    for (int8_t i = 0; i < data_size; ++i) {
//        random_data[i] = i;
//    }
//
//    auto byte_array = std::make_unique<TByteArray>(32, random_data, data_size, MEGA);
//    node->Schedule(std::move(byte_array));

    // Now the node is initialized and we're ready to roll.
    auto started_at = Clock::GetMonotonicMicroseconds();

    node->StartNode(started_at);
    node2->StartNode(started_at);
//    node3->StartNode(started_at);
//    node4->StartNode(started_at);

    while (true) {

        // Run a trivial scheduler polling the loops that run the business logic.
        CanardMicrosecond monotonic_time = Clock::GetMonotonicMicroseconds();

        node->CheckScheduler(monotonic_time);
        node2->CheckScheduler(monotonic_time);
//        node3->CheckScheduler(monotonic_time);
//        node4->CheckScheduler(monotonic_time);

        // Manage CAN RX/TX per redundant interface.
        node->HandleTxRxQueues(monotonic_time);
        node2->HandleTxRxQueues(monotonic_time);
//        node3->HandleTxRxQueues();
//        node4->HandleTxRxQueues();

        // Run every 5ms to prevent using too much CPU.
        // usleep(TX_PROC_SLEEP_TIME);

    }
}
