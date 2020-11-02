#include <stdlib.h> 
#include <sys/mman.h> // PROT_READ, MAP_SHARED
#include <fcntl.h> // O_RDONLY
#include <sched.h> // sched_yield
#include <stdio.h> // printf

#include "utils.h"

uint64_t rdtsc() {
    uint64_t a, d;
    asm volatile ("mfence");
    asm volatile ("rdtsc" : "=a" (a), "=d" (d));
    a = (d<<32) | a;
    asm volatile ("mfence");
    return a;
}

void maccess(const void* p)
{
  asm volatile ("movq (%0), %%rax\n"
    :
    : "c" (p)
    : "rax");
}

void flush(const void* p) {
    asm volatile ("clflush 0(%0)\n"
      :
      : "c" (p)
      : "rax");
}

int cache_hit(const void* addr) {
    size_t time = rdtsc();
    maccess(addr);
    size_t delay = rdtsc() - time;
    flush(addr);
    return delay < LIMIT;
}

unsigned char* load_lib(const char* filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 3) {
        printf("Could not open library file\n");
        exit(2);
    }

    unsigned char* addr = (unsigned char*) mmap(NULL, 64 * 1024 * 1024, PROT_READ, MAP_SHARED, fd, 0);
    return addr;
}

void repeated_sched_yield() {
    for(int i = 0; i < 10; i++) sched_yield();
}

struct timeval add_us(const struct timeval origin_time, const int microseconds) {
    struct timeval ret;
    ret.tv_sec = origin_time.tv_sec;
    ret.tv_usec = origin_time.tv_usec + microseconds;
    if (ret.tv_usec >= 1000000) {
      ret.tv_usec -= 1000000;
      ret.tv_sec ++;
    }
    return ret;
}

int read_bit(const void* sender_sent_addr, const void* bit_addr, void* recv_recved_addr, int parity) {
    // Switches between the two reception addresses depending on the parity
    void * recv_receved_addr_timeout = recv_recved_addr;
    if (parity) {
        recv_recved_addr += 512;
    } else {
        recv_receved_addr_timeout += 512;
    }

    // Sets up time values for timeout detection
    struct timeval current_time, time_limit;
    gettimeofday(&current_time, NULL);
    time_limit = add_us(current_time, RECEIVING_TIMEOUT_US);
    // Waits the sending confirmation
    while(!cache_hit(sender_sent_addr)) {
        gettimeofday(&current_time, NULL);
        // Timeout behavior
        if (current_time.tv_sec > time_limit.tv_sec || current_time.tv_usec > time_limit.tv_usec) {
            maccess(recv_receved_addr_timeout);
            time_limit = add_us(current_time, RECEIVING_TIMEOUT_US);
            //printf("Reading timed out\n");
        }

        // Gives time to other processes
        repeated_sched_yield();
    }
    // Reads the bit
    int bit = cache_hit(bit_addr);
    // Writes the reading confirmation
    maccess(recv_recved_addr);
    // Returns the measured value
    return bit;
}


void write_bit(const void* sender_sent_addr, const void* bit_addr, void* recv_recved_addr, const int bit, const int parity) {
    // Switches between the two reception addresses depending on the parity
    void * recv_receved_addr_timeout = recv_recved_addr;
    if (parity) {
        recv_recved_addr += 512;
    } else {
        recv_receved_addr_timeout += 512;
    }
    
    // Writes the bit to the target address
    if (bit) maccess(bit_addr);
    // Writes the sending confirmation
    maccess(sender_sent_addr);

    // Sets up time values for timeout detection
    struct timeval current_time, time_limit;
    gettimeofday(&current_time, NULL);
    time_limit = add_us(current_time, SENDING_TIMEOUT_US);
    //printf("Current time : %ld %ld\n", current_time.tv_sec, current_time.tv_usec);
    //printf("Time limit : %ld %ld\n", time_limit.tv_sec, time_limit.tv_usec);
    // Waits for the reading confirmation
    do {
        gettimeofday(&current_time, NULL);
        // Timeout behavior
        //printf("In da loop %ld %ld %ld %ld\n", current_time.tv_sec, current_time.tv_usec, time_limit.tv_sec, time_limit.tv_usec);
        if (current_time.tv_sec > time_limit.tv_sec || (! current_time.tv_sec > time_limit.tv_sec && current_time.tv_usec > time_limit.tv_usec)) {
            printf("Writing timed out : ");
            
            if (cache_hit(recv_receved_addr_timeout)) {
                // Receiver still waiting for the value so sending it back
                printf("Sending back the value\n");
                if (bit) maccess(bit_addr);
                maccess(sender_sent_addr);
                repeated_sched_yield();
            } else {
                // Receiver seems to have received the value so leaving the function
                printf("Gess it has been received\n");
            }

            time_limit = add_us(current_time, SENDING_TIMEOUT_US);
        }

        // Gives time to other processes
        repeated_sched_yield();
    } while(!cache_hit(recv_recved_addr));
}