#include <sys/types.h> // size_t
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "hamming.h"

#define SENDER_SENT_OFFSET   (0)
#define SENDER_RECV_OFFSET   (1024)
#define LISTENER_SENT_OFFSET (2048)
#define LISTENER_RECV_OFFSET (3072)
#define COMM_ADDR_OFFSET     (4096)

void recv_payload(void *addr, uint16_t *payload, int seq) {

    int total_received = 0;
    const void* sender_sent_addr = addr + SENDER_SENT_OFFSET;
    const void* bit_addr = addr + COMM_ADDR_OFFSET;
    void* recv_recved_addr = addr + LISTENER_RECV_OFFSET;

    for (int i = 15; i >= 0; i--) {
        int bit = read_bit(sender_sent_addr, bit_addr, recv_recved_addr, (15-i)%2);
	printf("Recv bit = %d, index = %d/15, seq = %d\n", bit, (15-i), seq);

	// payload is received and stored in big endian order
	*payload += bit << i;
        total_received++;
    } 

}

void main_loop(const void* addr) {
    printf("Starts listening\n");

    int seq = 0;
    uint16_t payload, data = 0;

    while(1) {
        recv_payload(addr, &payload, seq);
        fix_hamming_buf(&payload);
	data = hamming_buf_to_data(payload);
	
	printf("[PACK] seq=%d, data received = %d\n", seq++, data);
	
	payload=0;
	data = 0;
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
