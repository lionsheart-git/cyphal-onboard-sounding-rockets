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

#ifndef O1HEAP_MEM_SiZE
#define O1HEAP_MEM_SIZE 2048U
#endif

#ifndef NODE_ID
#define NODE_ID 96
#endif

#ifndef TX_PROC_SLEEP_TIME
#define TX_PROC_SLEEP_TIME 5000
#endif

#endif //SOCKETCAN_SRC_UTILITY_MACROS_H_
