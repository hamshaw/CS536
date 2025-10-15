#include <stdio.h>
#include <stdlib.h>
#include "writeToFile.h"
int writeToFile(char * filename, char *buffer, int filesize){
    FILE *file = fopen(filename, "wb");
    if (file == NULL){
        printf("failed to open file\n");
        return -1;
    }
    if (fwrite(buffer, 1, filesize, file) != filesize){
        printf("failed to write all data\n");
        fclose(file);
        return -1;
    }
    fclose(file);
    return 1;
}
    
