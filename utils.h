#ifndef UTILS_H
#define UTILS_H

#include <sys/time.h> // timeval
#include <stdint.h> // uint64_t

#define LIMIT (220) // Max amount of cycles for an access to be counted as a cache hit
#define SENDING_TIMEOUT_US   (1000)
#define RECEIVING_TIMEOUT_US (500)
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

/**
 * On the receiver side : waits until the sender gives the sending confirmation, then reads the bit at the address
 * and finally notify the sender that the data has been read.
 * @param sender_sent_addr : the address where the sender says that the data has been sent.
 * @param bit_addr : the address of the read data.
 * @param recv_recved_addr : the address for the reading confirmation.
 * @return the bit read at bit_addr.
 */
int read_bit(const void* sender_sent_addr, const void* bit_addr, void* recv_recved_addr, int parity);


void write_bit(const void* sender_sent_addr, const void* bit_addr, void* recv_recved_addr, const int bit, const int parity);

#endif
