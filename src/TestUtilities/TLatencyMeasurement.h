/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.02.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_TPRIMITIVEEMPTY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_TPRIMITIVEEMPTY_H_

#include "Task.h"

#include "PUavcanPrimitiveEmpty.h"

/**
 * @class TLatencyMeasurement
 * @brief Task for sending out latency measurement packets.
 *
 * A PrimitiveEmpty packet is send from one node to another. The other node replies with a PrimitiveEmpty packet as well.
 * A timestamp of the current monotonic time is save together with the number of the packet.
 * Since the number of the packet received is the same as of the send one, the delta between the time the packet was send
 * and the time it was received can be computed.
 */
class TLatencyMeasurement : public Task {

  public:

    /**
     * @brief Creates a new latency measurement task.
     *
     * @param port_id Port to send packets to.
     * @param target Target node id.
     * @param[us] interval Interval in which to send packets.
     */
    TLatencyMeasurement(CanardPortID port_id, CanardNodeID target, uint64_t interval);

    /**
     * @brief Get the publication interval.
     *
     * @return The interval in which packets are send in microseconds.
     */
    uint64_t Interval() const override;

    /**
     * @brief Executes the task.
     *
     * @param node Reference to node to use for sending.
     * @param current_time Current monotonic time for timestamping.
     */
    void Execute(Node &node, uint64_t current_time) override;

  private:
    uint64_t interval_; /**< The sending interval. */
    PUavcanPrimitiveEmpty primitive_empty_; /**< The primitive empty object that is send as latency measurement packet */

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_TPRIMITIVEEMPTY_H_
