#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "paramsProcessor.h"
int paramsProcessor(int i){
    FILE *file = fopen("sender/sender.param", "r");
    
    int params[3];
    char line[64];

    while (fgets(line, sizeof(line), file)) {
        char *equals = strchr(line, '=');

        *equals = '\0'; // split the string in-place
        char *key = line;
        char *value_str = equals + 1;
        int value = atoi(value_str); // convert to integer


        if (strcmp(key, "maxfilesize") == 0) {
            params[0] = value;
        } else if (strcmp(key, "micropace") == 0) {
            params[1] = value;
        } else if (strcmp(key, "payloadsize") == 0) {
            params[2] = value;
        }
    }

    fclose(file);
    if (params[i] < 0){
        printf("parameter value is negative, flipping sign\n");
        params[i] *=-1;
    }
    return params[i];
}
