#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#define SENDER_SENT_OFFSET     (0)
#define SENDER_RECV_OFFSET     (1024)
#define LISTENER_SENT_OFFSET   (2048)
#define LISTENER_RECV_OFFSET   (3072)
#define COMM_ADDR_OFFSET       (4096)

void main_loop(void* addr, int times) {
    
    const void * sender_sent_addr = addr + SENDER_SENT_OFFSET;
    const void * bit_addr = addr + COMM_ADDR_OFFSET;
    void * recv_recved_addr = addr + LISTENER_RECV_OFFSET;

    for(int i = 0; i < times; i++) {
        write_bit(sender_sent_addr, bit_addr, recv_recved_addr, i%2, i%2);
        printf("Sent %d\n", i % 2);
    }
}

int main(int argc, char** argv) {
    
    if (argc < 4) {
        printf("Need more arguments\n");
        exit(2);
    }

    char* addr = load_lib(argv[1]);

    size_t offset;
    sscanf(argv[2], "%lx", &offset);

    int times;
    sscanf(argv[3], "%d", &times);

    main_loop((void*) addr + offset, times);
    return 0;
}