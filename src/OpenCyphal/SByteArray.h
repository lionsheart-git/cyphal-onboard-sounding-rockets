/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 12.03.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_SBYTEARRAY_H_
#define SOCKETCAN_SRC_OPENCYPHAL_SBYTEARRAY_H_

#include "canard.h"

#include "SMessage.h"

class SByteArray : public SMessage {

  public:
    explicit SByteArray(CanardPortID port_id);

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_SBYTEARRAY_H_
