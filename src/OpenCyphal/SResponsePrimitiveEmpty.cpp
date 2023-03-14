/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 13.02.23
*/

#include "SResponsePrimitiveEmpty.h"

#include <uavcan/primitive/Empty_1_0.h>

#include "Macros.h"

SResponsePrimitiveEmpty::SResponsePrimitiveEmpty()
    : SMessage(CanardTransferKindResponse,
               LATENCY_MEASUREMENT_PORT_ID,
               uavcan_primitive_Empty_1_0_EXTENT_BYTES_,
               CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC) {

}
