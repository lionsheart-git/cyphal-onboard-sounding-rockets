/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 26.01.23
*/

#ifndef SOCKETCAN_SRC_TESTUTILITIES_NODEFACTORY_H_
#define SOCKETCAN_SRC_TESTUTILITIES_NODEFACTORY_H_

#include <cstdint>
#include <vector>
#include <memory>

#include "Node.h"
#include "CanardTransceiver.h"
#include "Macros.h"

class NodeFactory {
  public:
    NodeFactory(uint8_t *used_ids, size_t size);

    Node * CreateNode();
    Node * CreateNode(uint8_t node_id);

    void AddTransceiver(CanardTransceiver &transceiver);

  private:
    static void GetUniqueID(uint8_t out[16U]);

    std::vector<uint8_t> used_ids_;
    CanardTransceiver *transceiver_[CAN_REDUNDANCY_FACTOR]{};
};

#endif //SOCKETCAN_SRC_TESTUTILITIES_NODEFACTORY_H_
