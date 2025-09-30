#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include "helperFunctions.h"
void myinthandler(int){
    printf("\nTo quit please enter 'byebye' at the prompt.\n");
}
