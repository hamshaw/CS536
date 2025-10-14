#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
int main(int argc, char const* argv[]){
   	if (argc != 3){
                printf("please enter socket number and secret\n");
                exit(1);
        }
	int pn = atoi(argv[1]);
        const char * secret = argv[2];

	int sd;
	int sock;
	ssize_t received;
    	struct sockaddr_in address;
    	socklen_t addrlen = sizeof(address);
    	char buffer[25];

    	//socket()
    	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        	perror("creating socket failed");
        	exit(-1);
    	}
    	address.sin_family = AF_INET;
    	address.sin_addr.s_addr = INADDR_ANY;
    	address.sin_port = htons(pn);

    	//bind()
    	if (bind(sd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        	perror("bind failed");
        	exit(-1);
    	}
	//listen()
    	if (listen(sd, 5) < 0) {
        	perror("failed listen");
        	exit(-1);
    	}
	//accept()
    	if ((sock = accept(sd, (struct sockaddr*)&address, &addrlen)) < 0) {
        	perror("accept failed");
        	exit(-1);
    	}
    	//read()
	received = read(sock, buffer, 24); 
    	char cIP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &address, cIP, sizeof(cIP));
	if (strcmp(secret, buffer) != 0){
		printf("wrong secret from %s:%d, closing connection.\n", cIP, ntohs(address.sin_port));
		close(sock);
	}else{
		printf("Correct secret: %s\n", buffer);
		while(strcmp(buffer, "") != 0){
		memset(buffer, 0, sizeof(buffer));
		received = read(sock, buffer, 24);
		
		//	memset(buffer, 0, sizeof(buffer));
		int k = fork();
                	if (k==0) {
                	// child code
                        	if(execlp(buffer, buffer, NULL) == -1){
                                	printf("failed execution: %s\n", buffer);
					exit(1);
				}
                        	exit(1);
                	}
                	else {
                        	// parent code
                        	int status;
				waitpid(k, &status, 0);
                	}
		}
	}
    	close(sock);
    	close(sd);
    	exit(-1);
}
