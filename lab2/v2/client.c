#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
void alarmHandler(int sig){
	printf("Client time out\n");
	alarm(0);
}


int main(int argc, char const* argv[]){
	//printf("len of argv: %d", argc);
	if (argc != 4){
                printf("please enter socket number and secret\n");
                exit(1);
        }
	// signal(SIGALRM, alarmHandler);
	struct sigaction alarmAction;
	alarmAction.sa_handler = alarmHandler;
	alarmAction.sa_flags = 0;
	sigemptyset(&alarmAction.sa_mask);

	sigaction(SIGALRM, &alarmAction, NULL);


        const char * ip_addr = argv[1];
	int pn = atoi(argv[2]);
        const char * secret = argv[3];

    	int status;
	int sd;
	struct sockaddr_in serv_addr;// = htons(atoi(argv[1]));
	char buffer[25];
    	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        	printf("socket error\n");
        	exit(-1);
    	}

    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(pn);

	if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
        	printf("\nInvalid address/ Address not supported \n");
        	return -1;
    	}

    	if ((status = connect(sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        	printf("connect failed\n");
       		exit(-1);
    	}
  
	write(sd, secret, strlen(secret));
    	char command[25];
	char output[1024];
	while (1){
                fgets(command, 24, stdin);

                command[strlen(command)-1] = '\0';
                write(sd, command, strlen(command));
		
		//char output[1024];
        	ssize_t received;
		memset(output, 0, sizeof(output));
		//set alarm 3 sec
		alarm(3);
		if ((received = read(sd, output, 1024)) == -1){
			memset(output, 0, sizeof(output));
			continue;
		}
		//set alarm to 0 to cancel it
		alarm(0);
		fputs(output, stdout);
		memset(output, 0, sizeof(output));
	}

	close(sd);
    	exit(-1);
	}
