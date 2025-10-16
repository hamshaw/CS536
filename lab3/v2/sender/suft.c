#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "paramsProcessor.h"
#include "alarmHandler.h"
#include <unistd.h>

int main(int argc, char *argv[]){
    
    if (argc != 4){
        printf("please enter <rcvip> <rcvport> <filename>\n");
        exit(1);
    }
    
    struct sigaction alarmAction;
    alarmAction.sa_handler = alarmHandler;
    alarmAction.sa_flags = 0;
    sigemptyset(&alarmAction.sa_mask);
    sigaction(SIGALRM, &alarmAction, NULL);

    int sd;
    const char * rcvip = argv[1];
    int pn = atoi(argv[2]);
    char filename[6];
    memcpy(filename, argv[3], 6);

	struct sockaddr_in serveraddr, clientaddr;
    memset(&serveraddr, 0, sizeof(serveraddr));

	//socket()
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		printf("error creating socket\n");

	serveraddr.sin_addr.s_addr = inet_addr(rcvip);
    serveraddr.sin_port = htons(pn);
    serveraddr.sin_family = AF_INET;
    
    int maxfilesize = paramsProcessor(0);
    int micropace = paramsProcessor(1);
    int payloadsize = paramsProcessor(2);

    FILE *file = fopen(filename, "rb");
    if (!file){
        printf("error opening file");
    }
    fseek(file, 0, SEEK_END);
    long int filesize = ftell(file);
    rewind(file);
    char *buffer = malloc(sizeof(char)*filesize +1);
    size_t bytesRead = fread(buffer, 1, filesize, file);
    buffer[bytesRead] = '\0';
    fclose(file);
    char metadata[14];
    if (filesize>maxfilesize){
        printf("file is too large to transmit...quitting");
        exit(1);
    }
    memcpy(metadata, filename, 6);
    memcpy(metadata+6, &filesize, 4);
    memcpy(metadata+10, &payloadsize, 4);
    //*(metadata+1) = 't';//FOR TESTING ONLY< REMOVE BEFORE SUBMISSION
    char hopefullyfilename[6];
    for (int i = 0; i<5; i++){
        ualarm(250, 0);
        if (sendto(sd, metadata, 14, 0, (const struct sockaddr*) &serveraddr, sizeof(serveraddr))==-1){
            printf("error sending metadata\n");
        }
        socklen_t slen = sizeof(serveraddr);
        if (recvfrom(sd, hopefullyfilename, 6*sizeof(char), 0, (struct sockaddr*)&serveraddr, &slen) == -1){// || strncmp(filename, hopefullyfilename, 5) != 0){
            printf("error recieving metadata ack\n");
            free(buffer);
            exit(1);
        }else{
            ualarm(0, 0);
            i = 5;//break;
        }
    }

    int numpackets = filesize/payloadsize +1;
    int *acked = calloc(numpackets+2, sizeof(int));
    //memset(acked, 0, numpackets+2);
    printf("acked[numpackts]: %d\n", acked[numpackets]);
    while(acked[numpackets-1]==0){
    
    for (int i = 0; i < numpackets; i++){
        if (acked[i] == 1)
            continue;
        char packet[payloadsize+4];
        memcpy(packet, &i, 4);
        memcpy(packet+4, buffer+(i*payloadsize), payloadsize);
        //printf("packet: %.*s\n", 10, packet+4);
        if (sendto(sd, packet, 4+payloadsize, 0, (const struct sockaddr*) &serveraddr, sizeof(serveraddr)) == -1){
            printf("send failure\n");
        }
        printf("sending packet #%d\n", i);
        int ackNum;
        if (recvfrom(sd, &ackNum, sizeof(int), 0, (struct sockaddr*)NULL, NULL) == -1){
            printf("receiving ack failed");
            return -1;
        }
        printf("got ack #%d\n", ackNum);
        for (int spot = 0; spot <ackNum; spot++)
            acked[spot] = 1;
        usleep(micropace);
    }
    }
    free(buffer);
    return 1;
}
