/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 13.02.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_SREQUESTPRIMITIVEEMPTY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_SREQUESTPRIMITIVEEMPTY_H_

#include "SMessage.h"

/**
 * @class SRequestPrimitiveEmpty
 * @brief Subscribes to request messages for primitive empty's.
 */
class SRequestPrimitiveEmpty
    : public SMessage {

  public:
    /**
     * @brief Creates new subscription.
     */
    SRequestPrimitiveEmpty();

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_SREQUESTPRIMITIVEEMPTY_H_
