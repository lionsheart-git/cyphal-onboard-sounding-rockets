#include <iostream>

#include <cstring>
#include <unistd.h>

#include "SocketCANTransceiver.h"
#include "OpenCyphal.h"
#include "Node.h"
#include "Clock.h"
#include "THeartbeat.h"
#include "Macros.h"
#include "SMessageGetInfo.h"
#include "TByteArray.h"
#include "NodeFactory.h"
#include "TPrimitiveEmpty.h"

#include "uavcan/node/GetInfo_1_0.h"

// Returns the 128-bit unique-ID of the local node. This value is used in uavcan.node.GetInfo.Response and during the
// plug-and-play node-ID allocation by uavcan.pnp.NodeIDAllocationData. The function is infallible.
static void getUniqueID(uint8_t out[uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_]) {
    // A real hardware node would read its unique-ID from some hardware-specific source (typically stored in ROM).
    // This example is a software-only node, so we store the unique-ID in a (read-only) register instead.
    uavcan_register_Value_1_0 value = {0};
    uavcan_register_Value_1_0_select_unstructured_(&value);
    // Populate the default; it is only used at the first run if there is no such register.
    for (uint8_t i = 0; i < uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_; i++) {
        value.unstructured.value.elements[value.unstructured.value.count++] = (uint8_t) rand();  // NOLINT
    }
    // registerRead("uavcan.node.unique_id", &value);
    assert(uavcan_register_Value_1_0_is_unstructured_(&value) &&
        value.unstructured.value.count == uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_);
    memcpy(&out[0], &value.unstructured.value, uavcan_node_GetInfo_Response_1_0_unique_id_ARRAY_CAPACITY_);
}

int main() {
    // Init SocketCanTransceiver
    auto transceiver = std::make_unique<SocketCANTransceiver>("vcan0", true);
    auto second_transceiver = std::make_unique<SocketCANTransceiver>("vcan0", true);
    auto third_transceiver = std::make_unique<SocketCANTransceiver>("vcan0", true);

    uint8_t used_ids[] = {96};
    size_t used_ids_size = sizeof (used_ids);

    NodeFactory factory(used_ids, used_ids_size);
    factory.AddSocketCanInterface("vcan0");
    factory.AddTransceiver(std::move(third_transceiver));

    uavcan_node_GetInfo_Response_1_0 node_info;
    node_info.name.count = strlen(NODE_NAME);
    memcpy(&node_info.name.elements, NODE_NAME, node_info.name.count);

    node_info.software_version.major = VERSION_MAJOR;
    node_info.software_version.minor = VERSION_MINOR;
    node_info.software_vcs_revision_id = VCS_REVISION_ID;

    getUniqueID(node_info.unique_id);

    auto node = std::make_unique<Node>(NODE_ID, std::move(transceiver), node_info);

    size_t data_size = 256;
    int8_t random_data[256];

    for (int8_t i = 0; i < data_size; ++i) {
        random_data[i] = i;
    }

    uavcan_node_GetInfo_Response_1_0 node_info2;

    std::string name("org.icarus.nodefactory.67");

    node_info2.name.count = name.size();
    memcpy(&node_info2.name.elements, name.c_str(), node_info2.name.count);

    node_info2.software_version.major = VERSION_MAJOR;
    node_info2.software_version.minor = VERSION_MINOR;
    node_info2.software_vcs_revision_id = VCS_REVISION_ID;

    getUniqueID(node_info2.unique_id);
    auto node2 = std::make_unique<Node>(67, std::move(second_transceiver), node_info2);

    auto node3 = factory.CreateNode(42);
    auto node4 = factory.CreateNode(58);

    auto byte_array = std::make_unique<TByteArray>(32, random_data, data_size, MEGA);
    node->Schedule(std::move(byte_array));

    auto primitive_empty = std::make_unique<TPrimitiveEmpty>(LATENCY_MEASUREMENT_PORT_ID, CANARD_NODE_ID_UNSET, MEGA / 2);
    node4->Schedule(std::move(primitive_empty));

    // Now the node is initialized and we're ready to roll.
    auto started_at = Clock::GetMonotonicMicroseconds();

    node->StartNode(started_at);
    node2->StartNode(started_at);
    node3->StartNode(started_at);
    node4->StartNode(started_at);

    while (true) {

        // Run a trivial scheduler polling the loops that run the business logic.
        CanardMicrosecond monotonic_time = Clock::GetMonotonicMicroseconds();

        node->CheckScheduler(monotonic_time);
        node2->CheckScheduler(monotonic_time);
        node3->CheckScheduler(monotonic_time);
        node4->CheckScheduler(monotonic_time);

        // Manage CAN RX/TX per redundant interface.
        node->HandleTxRxQueues();
        node2->HandleTxRxQueues();
        node3->HandleTxRxQueues();
        node4->HandleTxRxQueues();

        // Run every 5ms to prevent using too much CPU.
        usleep(TX_PROC_SLEEP_TIME);

    }
}
