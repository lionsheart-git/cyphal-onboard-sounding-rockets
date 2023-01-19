/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_PMESSAGE_H_
#define SOCKETCAN_SRC_OPENCYPHAL_PMESSAGE_H_

#include "canard.h"

class PMessage {
  public:
    virtual CanardTransferMetadata Metadata() = 0;
    virtual uint8_t *SerializedMessage() = 0;
    virtual size_t SerializedSize() const = 0;

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_PMESSAGE_H_
