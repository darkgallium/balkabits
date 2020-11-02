#include <sys/types.h> // size_t
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#define SENDER_SENT_OFFSET   (0)
#define SENDER_RECV_OFFSET   (1024)
#define LISTENER_SENT_OFFSET (2048)
#define LISTENER_RECV_OFFSET (3072)
#define COMM_ADDR_OFFSET     (4096)

void main_loop(const void* addr) {

    int received_value;
    int total_received = 0;
    const void* sender_sent_addr = addr + SENDER_SENT_OFFSET;
    const void* bit_addr = addr + COMM_ADDR_OFFSET;
    void* recv_recved_addr = addr + LISTENER_RECV_OFFSET;

    printf("Starts listening\n");

    while(1) {
        received_value = read_bit(sender_sent_addr, bit_addr, recv_recved_addr, total_received%2);
        printf("Received a %d (total received : %d)\n", received_value, ++total_received);
    }
}

int main(int argc, char** argv) {
    
    if (argc < 3) {
        printf("Need more arguments\n");
        exit(2);
    }

    char* addr = load_lib(argv[1]);

    size_t offset;
    sscanf(argv[2], "%lx", &offset);

    main_loop((void*) addr + offset);
    return 0;
}