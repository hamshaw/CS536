#include "helperFunctions.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void pipeCloseHandler(int)
{
        fprintf(stdout, "The FIFO has closed.\n");
        exit(1);
}
