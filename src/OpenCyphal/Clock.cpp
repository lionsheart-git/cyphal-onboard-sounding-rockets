/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 18.01.23
*/

#include "Clock.h"

#include <cstdlib>
#include <ctime>
#include <chrono>

uint64_t Clock::GetMonotonicMicroseconds() {
    uint64_t us = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    return us;
}
