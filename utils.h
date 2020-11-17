#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <sys/time.h> // timeval
#include <stdint.h> // uint64_t

#define PACKET_SIZE (32) // bits : 24 for content, 2 for sequence number, 6 for hamming

#define REPEATS_FOR_SECURE (4)

#define LIMIT_SECURE (190) // Max amount of cycles for an access to be counted as a cache hit
#define LIMIT (230)
#define SENDING_TIMEOUT_US    (2000)
#define RECEIVING_TIMEOUT_US  (1500)

#define SENDER_SENT_OFFSET    (0)
#define WAS_PACKET_END        (1024)
#define WAS_NOT_PACKET_END    (2048)
#define PACKET_ACK            (3072)

#define RECEIVER_READY_OFFSET (4096)
#define RECEIVER_RECV_OFFSET  (5120)
#define PACKET_OK             (6144)
#define PACKET_CORRUPT        (8192)

#define COMM_ZERO_OFFSET      (9216)
#define COMM_ONE_OFFSET       (10240)
/**
 * Credit : Clementine M (https://github.com/clementine-m/cache_template_attacks). 
 * Gets the current value of the cycle counter..
 */
uint64_t rdtsc();

/**
 * Credit : Clementine M (https://github.com/clementine-m/cache_template_attacks). 
 * Accesses the address pointed by p so the next access is likely to be a cache hit.
 * @param p : the pointer to the target address.
 */
void maccess(const void* p);


/**
 * Credit : Clementine M (https://github.com/clementine-m/cache_template_attacks). 
 * Flushed the address pointed by p so the next access is likely to be a cache miss.
 * @param p : the pointer to the target address.
 */
void flush(const void* p);

/**
 * Checks whether the address pointed by p is a cache hit or a cache miss. 
 * @param p : the pointer to the target address.
 * @return 1 if it was a hit, 0 if it was a miss.
 */
int cache_hit(const void* p);

int cache_hit_secure(const void* p);

/**
 * Loads a shared library using mmap then returns a pointer to the corresponding address. 
 * @param filepath : the path of the library.
 * @return the first address of the shared memory.
 */
unsigned char* load_lib(const char* filepath);

/**
 * Let another process be called by the scheduler multiple times. 
 * This is done in order to have the other communicating program executed.
 */
void repeated_sched_yield();

/**
 * Adds a certain amount of microseconds to a timeval and returns the resulting timeval.
 * @param origin_time : the starting time
 * @param microseconds : the amount of microseconds to add. Must not exceed 1 000 000.
 * @return the time advanced by the specified amount of microseconds.
 */
struct timeval add_us(const struct timeval origin_time, const int microseconds);

void spam(void* base_address, int offset, int confirm_offset);

int spam_question(void* base_address, int offset, int offset_true, int offset_false);

#endif
