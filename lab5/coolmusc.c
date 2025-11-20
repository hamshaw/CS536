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
#include <time.h>

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
    float invgamma = atof(argv[4])*1000000;//in nanoseconds
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
    int lenUa = sizeof(UDPaddr);
    memset(&UDPaddr, 0, lenUa);
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
    pthread_t pt;
    pipe(pipefd);
    pthread_create(&pt, NULL, player_thread, invgamma);//add file nameeeeeee
    
    float invlambdat = invlambda;//prolly not int, check
    char buffer[cp.BLOCKSIZE];
    while (1){
        if (recvfrom(UDPsock, buffer, sizeof(buffer), 0, (struct sockaddr *)&UDPaddr, &lenUa)==0){
            printf("got entire file\n");
            break;
        }
        write(pipefd[1], buffer, cp.BLOCKSIZE);
        //get stats, send ack!!
        //check unread data in pipe (aka Q(t))
        int Qt = 0;
        ioctl(pipefd[0], FIONREAD, &Qt);
        invlambdat = invlambdat + 1/(cp.EPSILON*(cp.TARGETBUF-Qt)) + (1/cp.BETA)*(invgamma-invlambdat);
        /*
         i hope:
         invlambdat = float (1/lam) in nano seconds
         EPSILON = float (epsilon)
         TARGETBUF= any (Q* in Bytes)
         Qt =       amy (Q(t) in bytes) ARE THESE SUPPOSED TO BE PERCENTAGES????
         BETA =     float (beta)
         invlambda = float (1/gamma) - outflux in nanoseconds
         */

        sendto(UDPsock, invlambdat, sizeof(invlambdat), 0, (struct sockaddr *)&UDPaddr, lenUa);
        printf("sending server new lamda: %f\n", 1/invlambdat);

    }//end while(1)
    while(1){
        int end=-1;
        ioctl(pipefd[0], FIONREAD, &Qt);
        if (end==0) break;
        sleep(1);
        printf("playing...\n");
    }
    pthread_cancel(pt);
    printf("done\n")
}//end main





