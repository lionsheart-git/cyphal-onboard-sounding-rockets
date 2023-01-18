/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 18.01.23
*/

#ifndef SOCKETCAN_SRC_OPENCYPHAL_CLOCK_H_
#define SOCKETCAN_SRC_OPENCYPHAL_CLOCK_H_

#include <cstdint>

class Clock {
  public:
    static uint64_t GetMonotonicMicroseconds();

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_CLOCK_H_
