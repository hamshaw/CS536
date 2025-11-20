#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <net/if.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include "coolmusc.h"

//client
int main(int argc, char const* argv[]){
    if (argc != 7){
            printf("Expected: >>coolmusc audiofile serverIP serverPort invgammaa param.dat cdatalog.dat\n");
            exit(1);
    }

    //organize arguements
    const char * audioFile = argv[1];//string? or will this work
    if (int fileNameLen = strlen(audioFile)>10){
        printf("Audio file name too long, exiting\n");
        exit(-1);
    }
    const char * serverIP = argv[2];
    unsigned short serverPort = htons(atoi(argv[3]));
    float invgamma = argv[4];
    char paramsFile[] = argv[5];//same string problem
    char datalogFile[] = argv[6];//same string problem
   
    //read params file
    struct client_params cp;
    memset(cp, 0, sizeof(cp));
    int ret = load_params(paramsFile, cp);

    //socket() - TCP
    int sd;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("creating socket failed");
        exit(-1);
    }

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_port = serverPort;
    inet_pton(AF_INET, serverIP, &(address.sin_addr));

    //connect()
    int status;
    if ((status = connect(sd, (struct sockaddr*)&address, sizeof(address))) < 0) {
        printf("connect failed\n");
        exit(-1);
    }
    write(sd, SECRET, 6);
    write(sd, audioFile, fileNameLen);

    //read() for A or E
    char AorE;
    read(sd, &AorE, 1);
    if (AorE != A){
        printf("Server refused connecting (recieved: %c), exiting\n", AorE);
        exit(-1);
    }
    
    //Starting up UDP connection

    //socket()
    int UDPsock;
    if ((UDPsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        printf("CREATING SOCKET FAILED\n");
        exit(-1);
    }

    struct sockaddr_in UDPaddr;
    memset(&UDPaddr, 0, sizeof(UDPaddr));
    UDPaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    UDPaddr.sin_family = AF_INET;

    //find a valid port number
    unsigned short offset = 0;
    while(1){
        UDPaddr.sin_port = htons(PORTNUM + offset);
        if (bind(UDPsock, (struct sockaddr*)&UDPaddr, sizeof(UDPaddr)) == 0){
            printf("Successfully bound to port %d for UDP socket\n", PORTNUM + offset);
            int UDPport = htons(PORTNUM + offset);
            break;
        }
    }

    //write() UDPport number and the blocksize
    write(sd, UDPport, sizeof(unsigned short));
    write(sd, cp.BLOCKSIZE, sizeof(unsigned short));//CHECK TYPE OF BLOCKSIZE IN INSTRUCTIONS

    //START RECIEVING INFO FROM SERVER
    //SEND BACK ACKS PERTAINING TO CONGESTION CONTROL

}//end main





