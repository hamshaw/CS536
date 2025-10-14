#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
int main(int argc, char *argv[]){
    
    if (argc != 4){
        printf("please enter <rcvip> <rcvport> <filename>\n");
        exit(1);
    }
    
    int sd;
    const char * rcvip = argv[1];
    int pn = atoi(argv[2]);
    const char * filename = argv[3];

	struct sockaddr_in serveraddr, clientaddr;
    memset(&serveraddr, 0, sizeof(serveraddr));


	//socket()
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("error creating socket\n");

	serveraddr.sin_addr.s_addr = inet_addr(ip_addr);;
    serveraddr.sin_port = htons(pn);
    serveraddr.sin_family = AF_INET;

	//bind()
	//if ((bind = bind(sd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) == -1)
	//	printf("error binding\n");
    if (sendto(sd, message, 10, 0, (const struct sockaddr*) &serveraddr, sizeof(serveraddr)) == -1){
        printf("send failure\n");
    }

	return 1;
}
