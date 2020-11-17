#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

void pre_flush(void* address) {
    flush(address + RECEIVER_READY_OFFSET);
    flush(address + RECEIVER_RECV_OFFSET);
}

int main_loop(void* address, char *filename) {
    char message[1024];
    char packet = 0;
    int index  = 0;

    int packet_count = 0;

    FILE *fp;

    fp = fopen(filename, "wb");

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
            is_end = spam_question(address, RECEIVER_RECV_OFFSET, END, NOT_END);

            //printf("Received %d (%d)\n", bit, (++index));
        }
        
        if(packet_count % 1000 == 0)
            printf("seq = %d, byte = %u\n", packet_count, (uint8_t) packet);
            
        packet_count++;
        fwrite(&packet,1,1,fp);

        if (is_end) {
            fclose(fp);
            break;
        }

        packet = 0;
    }

    return packet_count;

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

    struct timeval before, after;
    gettimeofday(&before, NULL);
    
    int bytes_read = main_loop((void*) addr + offset, argv[3]);

    gettimeofday(&after, NULL);

    int time_delta = after.tv_sec - before.tv_sec;
    double speed = ((double) bytes_read) / ((double) time_delta);

    printf("Received %d bytes in %d seconds, speed is %0.4f B/s\n", bytes_read, time_delta, speed);

    return 0;
}
