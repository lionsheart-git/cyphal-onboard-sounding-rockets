/**
 * @file ${FILE}
 * @author Patrick Glöckner
 * @date 19.01.23
*/

#ifndef SOCKETCAN_SRC_UTILITY_MACROS_H_
#define SOCKETCAN_SRC_UTILITY_MACROS_H_

/**
 * @def CAN_REDUNDANCY_FACTOR
 * @brief Sets the number of can interfaces to use.
 */
#ifndef CAN_REDUNDANCY_FACTOR
#define CAN_REDUNDANCY_FACTOR 1
#endif

#ifndef KILO
#define KILO 1000L
#endif

#ifndef MEGA
#define MEGA ((int64_t) KILO * KILO)
#endif

/**
 * @def O1HEAP_MEM_SIZE
 * @brief Max memory size of an O1Heap instance.
 */
#ifndef O1HEAP_MEM_SIZE
#define O1HEAP_MEM_SIZE 2048U
#endif

/**
 * @def TX_PROC_SLEEP_TIME
 * @brief Sleep time of the thread in main.
 *
 * Value given in microseconds.
 */
#ifndef TX_PROC_SLEEP_TIME
#define TX_PROC_SLEEP_TIME 5000
#endif

/**
 * @def LATENCY_MEASUREMENT_PORT_ID
 * @brief Port ID used to send the latency measurement packets.
 */
#ifndef LATENCY_MEASUREMENT_PORT_ID
#define LATENCY_MEASUREMENT_PORT_ID 256
#endif

#endif //SOCKETCAN_SRC_UTILITY_MACROS_H_
