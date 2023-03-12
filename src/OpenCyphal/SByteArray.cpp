/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.03.23
*/

#include "SByteArray.h"

#include "uavcan/primitive/array/Integer8_1_0.h"

SByteArray::SByteArray(CanardPortID port_id) :
    SMessage(CanardTransferKindMessage,
             port_id,
             uavcan_primitive_array_Integer8_1_0_EXTENT_BYTES_,
             CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC){

}
