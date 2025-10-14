#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char const* argv[]){
	if (argc != 4){
                printf("please enter socket number and secret\n");
                exit(1);
        }
        const char * ip_addr = argv[1];
	int pn = atoi(argv[2]);
        const char * secret = argv[3];

	int sd;
	struct sockaddr_in serv_addr;// = htons(atoi(argv[1]));
	char buffer[25];

	//socket()
    	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        	printf("socket error\n");
        	exit(-1);
    	}

    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons(pn);

	if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0) {
        	printf("bad IP address \n");
        	return -1;
    	}

	//connect()
    	if ((connect(sd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        	printf("connect failed\n");
       		exit(-1);
    	}
  
	//write()
	write(sd, secret, strlen(secret));
    	char command[25];
	while (1){
                fgets(command, 24, stdin);

                command[strlen(command)-1] = '\0';
                write(sd, command, strlen(command));
                //close(fd);
	}

	close(sd);
    	exit(-1);
}
