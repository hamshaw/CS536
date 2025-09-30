#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char const* argv[]){
	//printf("len of argv: %d", argc);
	if (argc != 4){
                printf("please enter socket number and secret\n");
                exit(1);
        }
        char * ip_addr = argv[1];
	int pn = atoi(argv[2]);
        char * secret = argv[3];

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
  
    	// subtract 1 for the null
    	// terminator at the end
    	//dont use send!!!!!send(client_fd, hello, strlen(hello), 0);
	write(sd, secret, strlen(secret));
    	char command[25];
	while (1){
                //fd = open(file, O_WRONLY);
                fgets(command, 24, stdin);
                //if (hasArguements(command) || strlen(command)>25){
                 //       printf("Invalid entry, enter a command with no arguements, and less than 25 characters.\n");
                 //       close(fd);
                  //      continue;
                //}

                command[strlen(command)-1] = '\0';
                write(sd, command, strlen(command));
                //close(fd);
        
	}

	close(sd);
    	exit(-1);
	}
