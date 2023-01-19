/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_THEARTBEAT_H_
#define SOCKETCAN_SRC_OPENCYPHAL_THEARTBEAT_H_

#include "Task.h"
#include "PMessageHeartbeat.h"

class THeartbeat : public Task {
  public:
    THeartbeat(uint64_t started_at, int64_t interval);

    uint64_t NextRun() override;
    void Execute(OpenCyphal & cyphal, uint64_t current_time) override;

  private:
    PMessageHeartbeat heartbeat_;
    int64_t interval_;

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_THEARTBEAT_H_
