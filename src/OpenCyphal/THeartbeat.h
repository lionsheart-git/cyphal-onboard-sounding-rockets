/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_THEARTBEAT_H_
#define SOCKETCAN_SRC_OPENCYPHAL_THEARTBEAT_H_

#include "Task.h"
#include "PMessageHeartbeat.h"

/**
 * @brief Task sending a heartbeat.
 *
 * The cyphal specification expects every node to send a heartbeat at least once per second. So the minimal interval
 * should be MEGA. However this is not checked.
 */
class THeartbeat : public Task {

  public:
    /**
     * @brief Creates a new heartbeat task.
     *
     * @param started_at The instant the task is created.
     * @param interval The interval the task should be run at.
     */
    THeartbeat(Node const & node, int64_t interval);

    /**
     * @copydoc Task::Interval()
     */
    uint64_t Interval() const override;

    /**
     * @copydoc Task::Execute()
     * Publishes the heartbeat.
     */
    void Execute(Node &node, uint64_t current_time) override;

  private:
    PMessageHeartbeat heartbeat_; /**< Heartbeat message to be published. */
    int64_t interval_; /**< Interval to publish at. */

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_THEARTBEAT_H_
