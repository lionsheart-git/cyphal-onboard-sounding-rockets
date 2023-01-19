/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_UTILITY_MACROS_H_
#define SOCKETCAN_SRC_UTILITY_MACROS_H_

#ifndef CAN_REDUNDANCY_FACTOR
#define CAN_REDUNDANCY_FACTOR 1
#endif

#ifndef KILO
#define KILO 1000L
#endif

#ifndef MEGA
#define MEGA ((int64_t) KILO * KILO)
#endif

#endif //SOCKETCAN_SRC_UTILITY_MACROS_H_
