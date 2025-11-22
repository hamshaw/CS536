#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coolmusc.h"
//chatGPT assisted with making this funtion to save time

int load_params(const char *filename, struct client_params *params) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening parameter file");
        return 0;
    }

    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;

        char *eq = strchr(line, '=');
        if (!eq) continue;  // skip malformed lines

        *eq = '\0';
        char *key = line;
        char *value = eq + 1;

        if (strcmp(key, "BLOCKSIZE") == 0) {
            params->BLOCKSIZE = (unsigned short)atoi(value);
        } 
        else if (strcmp(key, "BUFFERSIZE") == 0) {
            params->BUFFERSIZE = strtol(value, NULL, 10);
        } 
        else if (strcmp(key, "TARGETBUF") == 0) {
            params->TARGETBUF = strtof(value, NULL);
        } 
        else if (strcmp(key, "INVLAMBDA") == 0) {
            params->INVLAMBDA = strtof(value, NULL);
        } 
        else if (strcmp(key, "EPSILON") == 0) {
            params->EPSILON = strtof(value, NULL);
        } 
        else if (strcmp(key, "BETA") == 0) {
            params->BETA = strtof(value, NULL);
        }
    }

    fclose(fp);
    return 1;
}
