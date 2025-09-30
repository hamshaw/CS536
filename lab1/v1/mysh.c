// Concurrent server example: simple shell using fork() and execlp().

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "helperFunctions.h"
int main(void){
    int k, status;
    char buf[100];
    char *prompt;
    char startingprompt[] = "%";
    prompt = mmap(NULL, sizeof(char)*10, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    memcpy(prompt, startingprompt, sizeof(startingprompt));
    char newprompt[] = "newprompt";
    char byebye[] = "byebye";

    signal(SIGINT, myinthandler);

    while(1) {

        // print prompt
        fprintf(stdout,"%s ", prompt);

        // read command from stdin
        fgets(buf, 50, stdin);

	//set up argv to contain the command and arguements as separate strings
        char *argv[50];
        if (setupArgv(argv, buf) == -1){
            continue;					//continue if nothing is entered
        }

	//send a child process to execute the command
        k = fork();
        if (k==0) {					//child process
		//"newprompt" is entered
           	if (strcmp(argv[0], newprompt)==0){
        		memcpy(prompt, argv[1], sizeof(argv[1]));
            	}
		//"byebye" is entered
            	else if (strcmp(argv[0], byebye)==0){
                	endEverything(getpid(), getppid());
            	}
		//execute shell command
            	else if(execvp(argv[0],argv) == -1)        // if execution failed, terminate child
                	exit(1);
            exit(1);
        }
        else {
        // parent code
          waitpid(k, &status, 0);               // block until child process terminates
        }
  }
}
