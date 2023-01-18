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

    /// A deeply embedded system should sample a microsecond-resolution non-overflowing 64-bit timer.
    /// Here is a simple non-blocking implementation as an example:
    /// https://github.com/PX4/sapog/blob/601f4580b71c3c4da65cc52237e62a/firmware/src/motor/realtime/motor_timer.c#L233-L274
    /// Mind the difference between monotonic time and wall time. Monotonic time never changes rate or makes leaps,
    /// it is therefore impossible to synchronize with an external reference. Wall time can be synchronized and therefore
    /// it may change rate or make leap adjustments. The two kinds of time serve completely different purposes.
    static uint64_t GetMonotonicMicroseconds();

};

#endif //SOCKETCAN_SRC_OPENCYPHAL_CLOCK_H_
