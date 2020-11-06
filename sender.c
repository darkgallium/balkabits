#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "hamming.h"

#define SENDER_SENT_OFFSET     (0)
#define SENDER_RECV_OFFSET     (1024)
#define LISTENER_SENT_OFFSET   (2048)
#define LISTENER_RECV_OFFSET   (3072)
#define COMM_ADDR_OFFSET       (4096)

void send_payload(void *addr, uint16_t payload, uint16_t seq_no) {

    const void * sender_sent_addr = addr + SENDER_SENT_OFFSET;
    const void * bit_addr = addr + COMM_ADDR_OFFSET;
    void * recv_recved_addr = addr + LISTENER_RECV_OFFSET;

    for(int i = 0; i < 16; i++) {
        write_bit(sender_sent_addr, bit_addr, recv_recved_addr, (payload >> (15-i)) & 1, i%2);
        printf("Sent bit = %d, index = %d/15, seq = %d\n", (payload >> (15-i)) & 1, i, seq_no);
    }
}

void main_loop(void* addr) {
    uint8_t data[10];

    scanf("%s", (char *)data);

    for (int i = 0; i < 4; i++) {
        uint16_t payload = data_to_hamming_buf(data[i]); 
        compute_parity_bits(&payload);
        send_payload(addr, payload, i);
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
