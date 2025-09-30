#include "helperFunctions.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
void endEverything(int pid, int ppid){
    if (kill(ppid, SIGTSTP)==-1){	//kill parent process
        printf("error killing parent");
    }
    if (kill(pid, SIGTSTP)==-1){	//kill child process
        printf("error killing child");
    }
}
