#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <sys/time.h> // timeval
#include <stdint.h> // uint64_t

#define REPEATS_FOR_SECURE (4)

#define LIMIT (190) // Max amount of cycles for an access to be counted as a cache hit

#define END                   (0)
#define NOT_END               (2048)

#define RECEIVER_READY_OFFSET (4096)
#define RECEIVER_RECV_OFFSET  (6144)

#define COMM_ZERO_OFFSET      (8192)
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
 * Checks whether the address pointed by p is a cache hit. 
 * It is likely to produce false negatives, but the trade is that it is almost
 * guaranteed that a hit is a true positive.
 * @param p : the pointer to the target address.
 * @return 1 if it was a hit, 0 if it was a miss.
 */
int cache_hit(const void* p);

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
 * Accesses a specific address until it is sure that the value has been received,
 * by measuring the cache accesses on the confirmation offset.
 * @param base_address : the address from where the program is counting
 * @param offset : the address accessed
 * @param confirm_offset : the address observed to check the confirmation.
 */
void spam(void* base_address, int offset, int confirm_offset);

/**
 * Accesses a specific address until one of the two observed addresses is set to true (several cache hits observed).
 * Returns wether the first or the second address has been detected as set to true.
 * @param base_address : the address from where the program is counting
 * @param offset : the address accessed
 * @param offset_true : the address for the true value observed
 * @param offset_false : the address for the false value observed
 * @return 1 if the true value has enough cache hits, 0 if the false value has enough cache hits
 */
int spam_question(void* base_address, int offset, int offset_true, int offset_false);

#endif
