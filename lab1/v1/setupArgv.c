#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include "helperFunctions.h"
int setupArgv(char **argv, char *buf){				//set up argv in provided argv pointer
    int len = strlen(buf);
    if(len == 1)                            			// case: only return key pressed
        return(-1);
    buf[len-1] = '\0';                    			// case: command entered
    int i = 0;
    char *token = strtok(buf, " ");
    argv[i++] = token;
    while ((token = strtok(NULL, " ")) != NULL && i<10){    	//arbitrary stopping point at 10 args
        argv[i++] = token;
    }
    argv[i] = NULL;
    return 1;
}
