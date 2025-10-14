#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
int main(int argc, char *argv[]){
    
    if (argc != 2){
                printf("please enter <rcvport>\n");
                exit(1);
        }
    
    int pn = atoi(argv[1]);

	struct sockaddr_in serveraddr, clientaddr;
    memset(&serveraddr, 0, sizeof(serveraddr));


	//socket()
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("error creating socket\n");

	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serveraddr.sin_port = htons(pn);
        serveraddr.sin_family = AF_INET;

	//bind()
	if ((bind = bind(sd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) == -1)
		printf("error binding\n");


	return 1;
}
