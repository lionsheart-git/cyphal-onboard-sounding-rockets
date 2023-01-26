/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#include "THeartbeat.h"
THeartbeat::THeartbeat(Node const & node, int64_t interval)
: heartbeat_(node), interval_(interval) {

}

void THeartbeat::Execute(Node &node, uint64_t current_time) {
    heartbeat_.SerializeMessage(current_time, node.Health());

    node.Publish(
        current_time + MEGA,  // Set transmission deadline 1 second, optimal for heartbeat.
        heartbeat_
        );
}

uint64_t THeartbeat::Interval() const {
    return interval_;
}
