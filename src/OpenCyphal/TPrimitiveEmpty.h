/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_TPRIMITIVEEMPTY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_TPRIMITIVEEMPTY_H_

#include "Task.h"

#include "PUavcanPrimitiveEmpty.h"

class TPrimitiveEmpty : public Task {

  public:

    TPrimitiveEmpty(CanardPortID port_id, CanardNodeID target, uint64_t interval);

    uint64_t Interval() const override;
    void Execute(Node &node, uint64_t current_time) override;

  private:
    uint64_t interval_;
    PUavcanPrimitiveEmpty primitive_empty_;

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_TPRIMITIVEEMPTY_H_
