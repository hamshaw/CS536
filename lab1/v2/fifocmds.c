#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "helperFunctions.h"

char * file = "./.myreqfifo";
int fd;
void myinthandler(int i){
	close(fd);
	unlink(file);
	exit(1);
}
int main(void){
	int status, len, k;
	//char * file = "./.myreqfifo";
	mkfifo(file, 0777);
	signal(SIGINT, myinthandler);	
	char command[20];
	while (1){
		fd = open(file, O_RDONLY);
		read(fd, command, 20);
		close(fd);
		len = strlen(command);
        	if(len == 1)                            // case: only return key pressed
          		continue;
        	command[len-1] = '\0';
		k = fork();
        	if (k==0) {
       		// child code
			if(execlp(command,command,NULL) == -1)        // if execution failed, terminate child
                		exit(1);
			exit(1);
        	}	
        	else {
        		// parent code
          		waitpid(k, &status, 0);               // block until child process terminates
        	}
	}
}
