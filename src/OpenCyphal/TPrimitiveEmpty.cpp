/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#include "TPrimitiveEmpty.h"


uint64_t TPrimitiveEmpty::Interval() const {
    return interval_;
}

void TPrimitiveEmpty::Execute(Node &node, uint64_t current_time) {
    node.Publish(current_time + Interval() - 1, primitive_empty_);
}

TPrimitiveEmpty::TPrimitiveEmpty(CanardPortID port_id, CanardNodeID target, uint64_t interval)
: primitive_empty_(port_id, target), interval_(interval) {

}
