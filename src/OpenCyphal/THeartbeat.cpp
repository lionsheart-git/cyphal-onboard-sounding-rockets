/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#include "THeartbeat.h"
THeartbeat::THeartbeat(uint64_t started_at, int64_t interval)
: heartbeat_(started_at), interval_(interval) {

}
uint64_t THeartbeat::NextRun() {
    return next_run_;
}
void THeartbeat::Execute(OpenCyphal &cyphal, uint64_t current_time) {
    heartbeat_.SerializeMessage(current_time, cyphal.Health());
    CanardTransferMetadata metadata = heartbeat_.Metadata();

    next_run_ = current_time + interval_;

    cyphal.Publish(
        current_time + MEGA,  // Set transmission deadline 1 second, optimal for heartbeat.
        &metadata,
        heartbeat_.SerializedSize(),
        heartbeat_.SerializedMessage()
        );
}
