/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 18.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_NODE_H_
#define SOCKETCAN_SRC_OPENCYPHAL_NODE_H_

#include <cstdint>
#include <memory>

#include "uavcan/node/GetInfo_1_0.h"

#include "OpenCyphal.h"
#include "CanardTransferReceiver.h"
#include "SMessage.h"
#include "IPMessage.h"
class Task;

/**
 * @class Node
 * @brief Represents a basic cyphal node with its basic functions.
 */
class Node : public CanardTransferReceiver, public OpenCyphal {

    /**
     * @todo Currently a new cyphal class has to be created for every node with a different id and then handed to the
     * Node class. This leaves room for user error by handing over the same class to different nodes. This should be
     * mitigated by moving the cyphal instantiation inside the Node class.
     */
  public:

    /**
     * @brief Creates a new cyphal communication node.
     *
     * @param node_id Id of the node.
     * @param transceiver Transceiver of the node.
     * @param info Info of the node.
     */
    Node(uint8_t node_id, std::unique_ptr<CanardTransceiver> transceiver, uavcan_node_GetInfo_Response_1_0 info);
    ~Node();

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
    void Schedule(std::unique_ptr<Task> task);

    /**
     * @brief Sets the node to active.
     * @param started_at Time the node was started.
     */
    void StartNode(uint64_t started_at);

    /**
     * @brief Subscribe to a topic.
     *
     * @param message Message with topic to subscribe to.
     * @return Information if the subscription was successful.
     * @retval 1 Subscription was created as requested.
     * @retval 0 Subscription already existed. Existing subscription is terminated and then a new one is created in its place.
     * @retval Negative Negated errno like invalid arguments.
     */
    int8_t Subscribe(SMessage & message);

    /**
     * @brief Publishes a new message.
     *
     * @param timeout Timeout until the message has to be published.
     * @param message Message to publish.
     * @return Number of frames enqueued or negated error.
     */
    int32_t Publish(CanardMicrosecond timeout, IPMessage & message);

    /**
     * @brief Health value of the Node.
     *
     * @return uavcan_node_Health_1_0 health value.
     */
    uint8_t Health();

    /**
     * @brief Gets the information of the node.
     *
     * @return Struct containing all the information.
     */
    uavcan_node_GetInfo_Response_1_0 GetInfo() const;

    /**
     * @brief Time the node was started.
     *
     * @return The time the node was started.
     */
    uint64_t StartedAt() const;

  private:

    /**
     * @brief Processes the gotten GetInfo request.
     *
     * @return Populated GetInfo request.
     */
    uavcan_node_GetInfo_Response_1_0 ProcessRequestNodeGetInfo();

    std::vector<std::unique_ptr<SMessage>> subscribers_; /**< List of all subscribers. */
    std::vector<std::unique_ptr<Task>> schedule_; /**< Vector containing all tasks. */
    uavcan_node_GetInfo_Response_1_0 info_; /**< Information about this node */
    uint64_t started_at_; /**< Instant the node was started */
    bool online_; /**< Indicator if the node is active */
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_NODE_H_
