/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 23.01.23
*/

#include "SMessageGetInfo.h"

#include "uavcan/node/GetInfo_1_0.h"

SMessageGetInfo::SMessageGetInfo()
    : SMessage(CanardTransferKindRequest,
               uavcan_node_GetInfo_1_0_FIXED_PORT_ID_,
               uavcan_node_GetInfo_Request_1_0_EXTENT_BYTES_,
               CANARD_DEFAULT_TRANSFER_ID_TIMEOUT_USEC) {
}
