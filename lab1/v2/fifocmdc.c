#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "helperFunctions.h"
#include <errno.h>


int main(void){
	int fd;
	
	char * file = "./.myreqfifo";
	signal(SIGPIPE, pipeCloseHandler);
	

	char command[100];
	while (1){
		fd = open(file, O_WRONLY);
		fgets(command, 100, stdin);
		if (hasArguements(command) || strlen(command)>25){
			printf("Invalid entry, enter a command with no arguements, and less than 25 characters.\n");
			close(fd);
			continue;
		}
		command[strlen(command)-1] = '\n';
		write(fd, command, strlen(command)+1);
		close(fd);
	}		
}

