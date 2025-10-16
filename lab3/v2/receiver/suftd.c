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

    char filename[6] = {0};
    unsigned int filesize;
    unsigned int payloadsize;
    memcpy(filename, metadata, 6);
    memcpy(&filesize, metadata+6, 4);
    memcpy(&payloadsize, metadata+10, 4);
    filename[6] = '\0';
    printf("Got metadata: %s, %u, %u\n", filename, filesize, payloadsize);

    if ((sendto(sd, filename, 6*sizeof(char), 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr)))==-1){
            printf("error sending metadata ack\n");
            exit(1);
    }
    printf("acknowledged metadata\n");
    char underscoren[] = "_n\0";
    memcpy(filename+6, underscoren, 3);
    char *buffer = malloc(filesize);
    char *packet = malloc(payloadsize+4); 
    int *datastructure = calloc((2 + filesize/payloadsize), sizeof(int));
    socklen_t slen = sizeof(clientaddr);
    for (int i = 0; i<(1 + filesize/payloadsize); i++){
        if (recvfrom(sd, packet, payloadsize+4, 0, (struct sockaddr*)&clientaddr, &slen) == -1){
	        printf("error recieving\n");
            exit(-1);
        }
        int seq;
        memcpy(&seq, packet, 4);
        memcpy(buffer+(seq*payloadsize), packet+4, payloadsize);
        //create nextsegmentexpected
        datastructure[seq] = 1;
        int nextsegmentexpected = 0;
        for (int spot = 0; spot < filesize/payloadsize+2; spot++){
            if (datastructure[spot] == 0){
                nextsegmentexpected = spot;
                break;
            }
        }
        int reps = 1;
        if (nextsegmentexpected > filesize/payloadsize)
            reps = 10;//this is so it acks 10 times on the last one
        for (int h = 0; h<reps; h++){
            if (sendto(sd, &nextsegmentexpected, sizeof(int), 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) == -1){
                printf("error sending ack\n");
                exit(1);
            }
            printf("Sent next expected #%d\n", nextsegmentexpected);
        }
    }
    if (writeToFile(filename, buffer, filesize) == -1){
        printf("error writing to file\n");
        exit(1);
    }
    free(datastructure);
    exit(0);
}
