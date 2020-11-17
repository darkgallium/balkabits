#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"

void pre_flush(void* address) {
    flush(address + END);
    flush(address + NOT_END);
    flush(address + COMM_ZERO_OFFSET);
    flush(address + COMM_ONE_OFFSET);
}

void main_loop(void* address, char *filename) {
    
    int packets = 1024, packet_id = 0;
    char number = 0;
    char number_next = 0;

    FILE *fp;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fread(&number, 1, 1, fp);

    while(fread(&number_next, 1, 1, fp) != 0) {
        if(packet_id % 1000 == 0)
            printf("seq = %d, byte = %u\n", packet_id,  (uint8_t) number);
        packet_id++;
            
        for(int index = 0; index < 8; index++) {
            int bit = (number >> index) & 1;
            int target_offset = bit ? COMM_ONE_OFFSET : COMM_ZERO_OFFSET;
            //printf("Sent %d (%d)\n", bit, index+1);
            // Waits for the listener to receive
            spam(address, target_offset, RECEIVER_RECV_OFFSET);
            // Acknowledges the reception
            spam(address, NOT_END, RECEIVER_READY_OFFSET);
        }
        number = number_next;
    }
    
    printf("%u\n", number);

    for(int index = 0; index < 8; index++) {
        int bit = (number >> index) & 1;
        int target_offset = bit ? COMM_ONE_OFFSET : COMM_ZERO_OFFSET;
        //printf("Sent %d (%d)\n", bit, index+1);
        // Waits for the listener to receive
        spam(address, target_offset, RECEIVER_RECV_OFFSET);
        // Acknowledges the reception
        spam(address, END, RECEIVER_READY_OFFSET);
    }


    fclose(fp);
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
  
    main_loop((void*) addr + offset, argv[3]);
    
    return 0;
}
