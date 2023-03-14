/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 13.02.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_SRESPONSEPRIMITIVEEMPTY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_SRESPONSEPRIMITIVEEMPTY_H_

#include "SMessage.h"

/**
 * @class SResponsePrimitiveEmpty
 * @brief Subscribes to response messages of primitive empty's.
 */
class SResponsePrimitiveEmpty
    : public SMessage {

  public:

    /**
     * @brief Creates new primitive empty subscription.
     */
    SResponsePrimitiveEmpty();

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_SRESPONSEPRIMITIVEEMPTY_H_
