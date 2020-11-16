#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void pre_flush(void* address) {
    flush(address + RECEIVER_READY_OFFSET);
    flush(address + RECEIVER_RECV_OFFSET);
    flush(address + PACKET_OK);
    flush(address + PACKET_CORRUPT);
}

void main_loop(void* address) {
    char message[1024];
    char packet = 0;
    int index  = 0;

    int packet_count = 0;

    char ch, file_name[] = "recv.jpg";
    FILE *fp;

    fp = fopen(file_name, "wb");

    if (fp == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        int is_end = 0;
        for(index = 0; index < 8; index++) {

            // The receiver tells  it is ready to read the bit
            // until it receives a 0 or a 1
            int bit = spam_question(
                address,
                RECEIVER_READY_OFFSET,
                COMM_ONE_OFFSET,
                COMM_ZERO_OFFSET);
            
            // adds the data to the packet
            packet |= bit << index;
            
            // Confirms that the data is read
            is_end = spam_question(address, RECEIVER_RECV_OFFSET, WAS_PACKET_END, WAS_NOT_PACKET_END);

            //printf("Received %d (%d)\n", bit, (++index));
        }

        if (is_end) {
            fclose(fp);
            return;
        }
        
        if(packet_count % 1000 == 0)
            printf("seq = %d, byte = %u\n", packet_count, (uint8_t) packet);
            
        packet_count++;
        fwrite(&packet,1,1,fp);
        packet = 0;
    }

}

int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Need more arguments\n");
        exit(2);
    }
    // Memory sharing
    char* addr = load_lib(argv[1]);

    // Base address for the communication
    size_t offset;
    sscanf(argv[2], "%lx", &offset);

    // Cleans the cache to prevent false positives
    pre_flush(addr);
    main_loop((void*) addr + offset);

    return 0;
}
