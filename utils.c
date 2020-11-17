#include <stdlib.h> 
#include <sys/mman.h> // PROT_READ, MAP_SHARED
#include <fcntl.h> // O_RDONLY
#include <sched.h> // sched_yield
#include <stdio.h> // printf
#include <time.h>

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
    for(int i = 0; i < 20; i++) sched_yield();
}

void spam(void* base_address, int offset, int confirm_offset) {
    for(int i = 0; i < REPEATS_FOR_SECURE; i++) {
        do {
            maccess(base_address + offset);
            repeated_sched_yield();
        } while(!cache_hit(base_address + confirm_offset));
    }
    flush(base_address + offset);
    flush(base_address + confirm_offset);
}


int spam_question(void* base_address, int target, int offset_true, int offset_false) {
    int good = 0;
    int bad = 0;
    int ret = 0;
    while(1) {
        //printf("%d %d\n", good, bad);
        maccess(base_address + target);
        repeated_sched_yield();
        if (cache_hit(base_address + offset_false)) bad ++;
        if (cache_hit(base_address + offset_true)) good ++;
        if (bad  >= REPEATS_FOR_SECURE) break;
        if (good >= REPEATS_FOR_SECURE) {
            ret = 1;
            break;
        }
    }
    flush(base_address + target);
    flush(base_address + offset_false);
    flush(base_address + offset_true);
    return ret;

}
