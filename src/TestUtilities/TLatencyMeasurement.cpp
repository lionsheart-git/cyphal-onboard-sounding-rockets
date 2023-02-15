/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#include "TLatencyMeasurement.h"

#include "LatencyMeasurementNode.h"
#include "Clock.h"

uint64_t TLatencyMeasurement::Interval() const {
    return interval_;
}

void TLatencyMeasurement::Execute(Node &node, uint64_t current_time) {

    //@todo Remove cast need for latency measurement
    auto & latency_node =  dynamic_cast<LatencyMeasurementNode&>(node);

    latency_node.latency_[primitive_empty_.Metadata().transfer_id % 32] = Clock::GetMonotonicMicroseconds();

    node.Publish(current_time + Interval() - 1, primitive_empty_);
}

TLatencyMeasurement::TLatencyMeasurement(CanardPortID port_id, CanardNodeID target, uint64_t interval)
: primitive_empty_(port_id, target), interval_(interval) {

}
