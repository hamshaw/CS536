#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void alarmHandler(int sig){
        printf("Client time out\n");
        alarm(0);
}

