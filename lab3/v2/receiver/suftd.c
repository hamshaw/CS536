#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <math.h>
#include "writeToFile.h"
#include "getDroppedPackets.h"
#include <unistd.h>

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
    //get packets to skip:
    int toskip[10];
    for (int j=0; j<10; j++)
        toskip[j] = getDroppedPackets(j);
    socklen_t clen = sizeof(clientaddr);
    char metadata[14];
    if ((recvfrom(sd, metadata, 14, 0, (struct sockaddr*)&clientaddr, &clen))==-1){
        printf("error getting meta data\n");
    }

    char filename[6] = {0};
    int filesize;
    int payloadsize;
    memcpy(filename, metadata, 6);
    memcpy(&filesize, metadata+6, 4);
    memcpy(&payloadsize, metadata+10, 4);
    filename[6] = '\0';
    printf("Got metadata: %s, %u, %u\n", filename, filesize, payloadsize);
	
    if ((sendto(sd, filename, 6*sizeof(char), 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr)))==-1){
            printf("error sending metadata ack\n");
            exit(1);
    }
    //ualarm(250, 0);
    printf("acknowledged metadata\n");
    char underscoren[] = "_n\0";
    memcpy(filename+6, underscoren, 3);
    char *buffer = malloc(filesize);
    char *packet = malloc(payloadsize+4);
    

    int numpackets = ceil(((double)filesize)/((double)payloadsize));
    int *datastructure = calloc((numpackets+1), sizeof(int));
    socklen_t slen = sizeof(clientaddr);
    int nextsegmentexpected = 0;
    int done = 0;
    while (nextsegmentexpected < numpackets){
        //for (int i = 0; i<numpackets; i++){
            if (recvfrom(sd, packet, payloadsize+4, 0, (struct sockaddr*)&clientaddr, &slen) == -1){
	            printf("error recieving\n");
                exit(-1);
            }
	    //ualarm(0, 0);
            int seq;
            memcpy(&seq, packet, 4);
            int skip = 0;
            for (int k = 0; k <10; k++){
                if (seq == toskip[k]){
                    toskip[k] = -1;
                    skip = 1;
                }
            }
            if (skip){
                printf("dropping packet #%d\n", seq);
                //continue;
            }else{
                printf("recieved packet #%d\n", seq);
                memcpy(buffer+(seq*payloadsize), packet+4, payloadsize);
                datastructure[seq] = 1;
            }
            //int nextsegmentexpected = 0;
            for (int spot = 0; spot < numpackets+1; spot++){
                if (datastructure[spot] == 0){
                    nextsegmentexpected = spot;
                    //printf("nse = %d, numpackets = %d\n", spot, numpackets);
                    if (spot >= numpackets){
                        done = 1;
                    }
                    break;
                }
            }
            int reps = 1;
            if (nextsegmentexpected >= numpackets)
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
    //free(datastructure);
    exit(0);
}
