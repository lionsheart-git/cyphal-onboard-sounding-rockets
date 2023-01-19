/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_TASK_H_
#define SOCKETCAN_SRC_OPENCYPHAL_TASK_H_

#include <cstdint>

#include "OpenCyphal.h"

class Task {
  public:
    virtual uint64_t NextRun() = 0;
    virtual void Execute(OpenCyphal & cyphal, uint64_t current_time) = 0;

  protected:
    uint64_t next_run_;
};

#endif //SOCKETCAN_SRC_OPENCYPHAL_TASK_H_
