#include <stdio.h>
#include "alarmHandler.h"
#include <unistd.h>
void alarmHandler(int sig){
        printf("time out\n");
        ualarm(0, 0);
}
