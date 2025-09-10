// Concurrent server example: simple shell using fork() and execlp().

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
int k;
char buf[100];
char *ptrs[20];
int status;
int len;

  while(1) {

	// print prompt
  	fprintf(stdout,"%% ");

	// read command from stdin
	fgets(buf, 50, stdin);
	len = strlen(buf);
	if(len == 1) 				// case: only return key pressed
	  continue;
	char *argv[] = {buf, NULL};	// case: command entered
	//int j = 0;
	//for (int i=0; i<len; i++){
//		if (buf[i]==' ' || buf[i]=='\0'){
//			ptrs[j] = buf+i;
//			j++;
//		}
//	}
  	k = fork();
  	if (k==0) {
  	// child code
    	  if(execvp(buf, argv) == -1)	// if execution failed, terminate child
	  	exit(1);
  	}
  	else {
  	// parent code 
	  waitpid(k, &status, 0);		// block until child process terminates
  	}
  }
}

