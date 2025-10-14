#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "paramsProcessor.h"
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
    long filesize = ftell(file);
    rewind(file);
    char *buffer = malloc(sizeof(char)*filesize +1);
    size_t bytesRead = fread(buffer, 1, filesize, file);
    buffer[bytesRead] = '\0';
    fclose(file);
    //char message[10] = {"Boob\0"};
	//bind()
	//if ((bind = bind(sd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) == -1)
	//	printf("error binding\n");
    int maxfilesize = paramsProcessor(0);
    int micropace = paramsProcessor(1);
    int payloadsize = paramsProcessor(2);
    
    if (sendto(sd, buffer, bytesRead, 0, (const struct sockaddr*) &serveraddr, sizeof(serveraddr)) == -1){
        printf("send failure\n");
    }
    free(buffer);
	
    return 1;
}
