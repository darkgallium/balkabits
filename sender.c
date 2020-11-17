#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#include "utils.h"

void pre_flush(void* address) {
    flush(address + END);
    flush(address + NOT_END);
    flush(address + COMM_ZERO_OFFSET);
    flush(address + COMM_ONE_OFFSET);
}

void main_loop(void* address, char *filename) {
    
    int packets = 1024, packet_count = 0;
    char number = 0;
    char number_next = 0;

    FILE *fp;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    struct stat stat_buf;
    fstat(fileno(fp), &stat_buf);
    off_t total_size = stat_buf.st_size;

    fread(&number, 1, 1, fp);

    while(fread(&number_next, 1, 1, fp) != 0) {
        
        if(packet_count % 500 == 0) {
            printf("\rProgress: %0.2f%% (%d/%d)\033[K", ((double)packet_count/total_size)*100, packet_count, total_size);
            fflush(stdout);
        }

        packet_count++;
            
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
    
    for(int index = 0; index < 8; index++) {
        int bit = (number >> index) & 1;
        int target_offset = bit ? COMM_ONE_OFFSET : COMM_ZERO_OFFSET;
        //printf("Sent %d (%d)\n", bit, index+1);
        // Waits for the listener to receive
        spam(address, target_offset, RECEIVER_RECV_OFFSET);
        // Acknowledges the reception
        spam(address, END, RECEIVER_READY_OFFSET);
    }

    printf("\rProgress: 100%% (%d/%d)\033[K", ++packet_count, total_size);
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
