/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 26.01.23
*/

#include "NodeFactory.h"

#include <cstddef>
#include <random>
#include <string>

#include "Node.h"
#include "SocketCANTransceiver.h"

#include "uavcan/node/GetInfo_1_0.h"
#include "SResponsePrimitiveEmpty.h"
#include "SRequestPrimitiveEmpty.h"
#include "TLatencyMeasurement.h"

NodeFactory::NodeFactory(uint8_t *used_ids, size_t size) {
    for (int i = 0; i < size; ++i) {
        used_ids_.push_back(used_ids[i]);
    }
}

std::unique_ptr<Node> NodeFactory::CreateNode() {

    std::default_random_engine generator;
    std::uniform_int_distribution<uint8_t> distribution(0,125); // 127 - 2 for reserved addresses

    uint8_t random_id = distribution(generator);

    while (true) {
        for (auto node_id : used_ids_) {
            if (random_id == node_id) {
                random_id = distribution(generator);
                break;
            }
        }
        break;
    }

    return CreateNode(random_id);
}

std::unique_ptr<Node> NodeFactory::CreateNode(uint8_t node_id) {
    used_ids_.push_back(node_id);

    uavcan_node_GetInfo_Response_1_0 node_info;

    std::string name("org.icarus.nodefactory.");
    name.append(std::to_string(node_id));

    node_info.name.count = name.size();
    memcpy(&node_info.name.elements, name.c_str(), node_info.name.count);

    node_info.software_version.major = VERSION_MAJOR;
    node_info.software_version.minor = VERSION_MINOR;
    node_info.software_vcs_revision_id = VCS_REVISION_ID;

    GetUniqueID(node_info.unique_id);

//    if (transceiver_[0] == nullptr && socket_can_interfaces_.empty()) {
//        //@todo Figure out what happens if no transceiver present.
//        // Could be mitigated if transceiver is handed in via constructor.
//    }

    auto node = std::make_unique<Node>(node_id, std::move(std::make_unique<SocketCANTransceiver>(socket_can_interfaces_[0], true)), node_info);

    return node;
}

// Returns the 128-bit unique-ID of the local node. This value is used in uavcan.node.GetInfo.Response and during the
// plug-and-play node-ID allocation by uavcan.pnp.NodeIDAllocationData. The function is infallible.
void NodeFactory::GetUniqueID(uint8_t *out) {
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

void NodeFactory::AddTransceiver(std::unique_ptr<CanardTransceiver> transceiver) {
    transceiver_.push_back(std::move(transceiver));
}

void NodeFactory::AddSocketCanInterface(std::string socket_can_interface) {
    socket_can_interfaces_.push_back(socket_can_interface);
}

std::unique_ptr<LatencyMeasurementNode> NodeFactory::CreateLatencyRequestNode(uint8_t node_id, uint64_t interval) {

    auto node = CreateLatencyNode(node_id);

    auto latency_response = std::make_unique<SResponsePrimitiveEmpty>();
    node->Subscribe(std::move(latency_response));

    auto primitive_empty = std::make_unique<TLatencyMeasurement>(LATENCY_MEASUREMENT_PORT_ID, 2, interval);
    node->Schedule(std::move(primitive_empty));

    return node;
}

std::unique_ptr<LatencyMeasurementNode> NodeFactory::CreateLatencyNode(uint8_t node_id) {
    used_ids_.push_back(node_id);

    uavcan_node_GetInfo_Response_1_0 node_info;

    std::string name("org.icarus.latency.");
    name.append(std::to_string(node_id));

    node_info.name.count = name.size();
    memcpy(&node_info.name.elements, name.c_str(), node_info.name.count);

    node_info.software_version.major = VERSION_MAJOR;
    node_info.software_version.minor = VERSION_MINOR;
    node_info.software_vcs_revision_id = VCS_REVISION_ID;

    GetUniqueID(node_info.unique_id);

    auto node = std::make_unique<LatencyMeasurementNode>(node_id, std::move(std::make_unique<SocketCANTransceiver>(socket_can_interfaces_[0], true)), node_info);

    return node;
}

std::unique_ptr<LatencyMeasurementNode> NodeFactory::CreateLatencyResponseNode(uint8_t node_id) {

    auto node = CreateLatencyNode(node_id);

    auto latency_request = std::make_unique<SRequestPrimitiveEmpty>();
    node->Subscribe(std::move(latency_request));

    return node;
}


