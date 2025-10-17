#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getDroppedAcks.h"

int getDroppedAcks(int index) {
    char *filename = "sender.lossmodel";
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return -1; // Indicate error
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    // Allocate buffer
    char *buffer = malloc(filesize);
    
    // Read entire file into buffer
    size_t bytesRead = fread(buffer, 1, filesize, file);
    buffer[bytesRead] = '\0';
    fclose(file);

    // Tokenize string and find ith value
    int count = 0;
    char *token = strtok(buffer, ",");
    while (token != NULL) {
        if (count == index) {
            int value = atoi(token);
            free(buffer);
            if (value <0)
                printf("negative value in lossmodel, ignoring");
            return value;
        }
        token = strtok(NULL, ",");
        count++;
    }

    // If we get here, index was out of bounds
    free(buffer);
    //fprintf(stderr, "Index %d out of range.\n", index);
    return -1;
}
