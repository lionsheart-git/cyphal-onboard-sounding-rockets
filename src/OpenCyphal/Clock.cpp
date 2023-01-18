/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 18.01.23
*/

#include "Clock.h"

#include <cstdlib>
#include <ctime>

uint64_t Clock::GetMonotonicMicroseconds() {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        abort();
    }
    return (uint64_t) (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}
