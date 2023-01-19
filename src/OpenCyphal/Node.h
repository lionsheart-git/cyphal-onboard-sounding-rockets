/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 18.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_NODE_H_
#define SOCKETCAN_SRC_OPENCYPHAL_NODE_H_

#include <cstdint>

#include "uavcan/node/GetInfo_1_0.h"

#include "OpenCyphal.h"
#include "CanardTransferReceiver.h"
#include "Task.h"

/**
 * @class Node
 * @brief Represents a basic cyphal node with its basic functions.
 */
class Node : public CanardTransferReceiver {

  public:
    Node(OpenCyphal & cyphal, uavcan_node_GetInfo_Response_1_0 info);

    /**
     * @brief Processes the received transfers.
     *
     * @param interface_index Interface index the transfer came from.
     * @param transfer Transfer to process.
     */
    void ProcessReceivedTransfer(uint8_t interface_index, CanardRxTransfer const & transfer) override;

    /**
     * @brief Checks if there are any tasks to run.
     *
     * Compares if the current time is above the time the task is scheduled. If so, the task is run.
     *
     * @param monotonic_time Current monotonic time.
     */
    void CheckScheduler(CanardMicrosecond monotonic_time);

    /**
     * @brief Adds a task to the schedule.
     *
     * Task will be run the next time CheckSchedule is invoked. If this is not the desired behavior, the task can have
     * a custom next_run value set.
     * @param task Task to run.
     */
    void Schedule(Task &task);

    /**
     * @brief Sets the node to active.
     * @param started_at Time the node was started.
     */
    void StartNode(uint64_t started_at);

    uint8_t Health();

  private:

    /**
     * @brief Processes the gotten GetInfo request.
     *
     * @return Populated GetInfo request.
     */
    uavcan_node_GetInfo_Response_1_0 ProcessRequestNodeGetInfo();

    std::vector<Task*> schedule_; /**< Vector containing all tasks. */
    OpenCyphal &cyphal_; /**< Cyphal instance through which to publish and receive. */
    uavcan_node_GetInfo_Response_1_0 info_; /**< Information about this node */
    uint64_t started_at_; /**< Instant the node was started */
    bool online_; /**< Indicator if the node is active */
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_NODE_H_
