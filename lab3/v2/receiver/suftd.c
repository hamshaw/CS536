#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "writeToFile.h"
int main(int argc, char *argv[]){
    
    if (argc != 2){
                printf("please enter <rcvport>\n");
                exit(1);
        }
    
    int pn = atoi(argv[1]);
    int sd;

	struct sockaddr_in serveraddr, clientaddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    socklen_t len = sizeof(serveraddr);
    

	//socket()
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("error creating socket\n");

	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(pn);
    serveraddr.sin_family = AF_INET;

	//bind()
	if (bind(sd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
        printf("error binding\n");
        exit(-1);
    }
    socklen_t clen = sizeof(clientaddr);
    char metadata[14];
    if ((recvfrom(sd, metadata, 14, 0, (struct sockaddr*)&clientaddr, &clen))==-1){
        printf("error getting meta data\n");
    }
    int zero = 0;
    if ((sendto(sd, &zero, sizeof(int), 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr)))==-1){
            printf("error sending meta data ack\n");
            exit(1);
    }
    char filename[6] = {0};
    unsigned int filesize;
    unsigned int payloadsize;

    memcpy(filename, metadata, 6);
    memcpy(&filesize, metadata+6, 4);
    memcpy(&payloadsize, metadata+10, 4);
    filename[6] = '\0';
    char buffer[filesize];
    
    for (int i = 0; i<(1 + filesize/payloadsize); i++){
        if (recvfrom(sd, buffer+(i*payloadsize), payloadsize, 0, (struct sockaddr*)NULL, NULL) == -1){
	    printf("error recieving\n");
        exit(-1);
        }
        printf("%s\n", buffer);
        if (sendto(sd, &i, sizeof(int), 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) == -1){
            printf("error sending ack\n");
            return -1;
        }
    }
    if (writeToFile(filename, buffer, filesize) == -1){
        printf("error writing to file\n");
        return -1;
    }
    return 1;
}
