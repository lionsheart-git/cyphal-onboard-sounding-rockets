/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 23.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_SMESSAGEGETINFO_H_
#define SOCKETCAN_SRC_OPENCYPHAL_SMESSAGEGETINFO_H_

#include "SMessage.h"

/**
 * @class SMessageGetInfo
 * @brief Subscribe to get info request messages.
 */
class SMessageGetInfo : public SMessage {

  public:
    /**
     * @brief Constructs a new GetInfo subscribe message.
     */
    SMessageGetInfo();

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_SMESSAGEGETINFO_H_
