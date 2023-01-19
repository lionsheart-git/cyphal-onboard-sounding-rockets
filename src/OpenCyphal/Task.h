/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_TASK_H_
#define SOCKETCAN_SRC_OPENCYPHAL_TASK_H_

#include <cstdint>

#include "OpenCyphal.h"

/**
 * @brief Base class for a task to be run by the nodes scheduler.
 */
class Task {

  public:
    /**
     * @brief Time the task has to be run again.
     *
     * @return The time the task has to be run again.
     */
    uint64_t NextRun() const;

    /**
     * @brief Updates the time when the task has to be run again.
     *
     * @param current_time The current time.
     */
    void UpdateNextRun(uint64_t const &current_time);

    /**
     * @brief The interval of time the task should be run at.
     *
     * @return The tasks interval.
     */
    virtual uint64_t Interval() const = 0;

    /**
     * @brief Executes the task.
     *
     * @param cyphal Cyphal instance used to publish messages of the task.
     * @param current_time Current time the task is executed at.
     */
    virtual void Execute(OpenCyphal & cyphal, uint64_t current_time) = 0;

  private:
    uint64_t next_run_; /**< Time the task has to be run again */
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_TASK_H_
