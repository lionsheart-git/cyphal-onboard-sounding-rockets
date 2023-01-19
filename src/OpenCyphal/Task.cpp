/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#include "Task.h"

uint64_t Task::NextRun() const {
    return next_run_;
}

void Task::UpdateNextRun(uint64_t const &current_time) {
    next_run_ = current_time + Interval();
}
