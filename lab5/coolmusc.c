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
#include <sys/ioctl.h>
#include <pthread.h>
int CONTROLLAW = 0;
//client
int pipefd[2];
struct timespec marker;
void *player_thread(void *arg) {
     // Unpack arguments
    struct thread_args {
        float * slptime;
        shared_t *shared;
    } *args = arg;

    printf("in player_thread!\n");
    //int slptime = 313;//*(int*)arg;
    play(pipefd[0], *(args->slptime), args->shared);
    return NULL;
}
void set_mark(void){
	clock_gettime(CLOCK_MONOTONIC, &marker);
}
double msec_since_mark(void){
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (now.tv_sec - marker.tv_sec)*1000 + (now.tv_nsec - marker.tv_nsec)/1000000;
}
int main(int argc, char const* argv[]){
    if (argc != 7){
            printf("Expected: >>coolmusc audiofile serverIP serverPort invgammaa param.dat cdatalog.dat\n");
            exit(1);
    }

    //organize arguements
    const char * audioFile = argv[1];//string? or will this work
    int fileNameLen;
    //audioFile[fileNameLen] = '\0';
    if ((fileNameLen = strlen(audioFile))>10){
        printf("Audio file name too long, exiting\n");
        exit(-1);
    }
    //audioFile[fileNameLen] = '\0';
    printf("%s\n", audioFile);
    const char * serverIP = argv[2];
    unsigned short serverPort = htons(atoi(argv[3]));
    float invgamma = atof(argv[4]);//in MSEC
    const char * paramsFile = argv[5];//same string problem
    const char * datalogFile = argv[6];//same string problem
    

    FILE *fp;
    fp = fopen(datalogFile, "w");

    //read params file
    struct client_params cp;
    memset(&cp, 0, sizeof(cp));
    int ret = load_params(paramsFile, &cp);

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

    printf("Connnecting...\n");

    //connect()
    int status;
    if ((status = connect(sd, (struct sockaddr*)&address, sizeof(address))) < 0) {
        printf("connect failed\n");
        exit(-1);
    }


    char * sanda = calloc(7+fileNameLen, sizeof(char));//]={'\0'};
    memcpy(sanda, SECRET, 6);
    memcpy(sanda+6, audioFile, fileNameLen);
    printf("%s\n", sanda);
    if (write(sd, sanda, 6+fileNameLen) < 1) printf("failing write\n");

    //read() for A or E
    char AorE;
    read(sd, &AorE, 1);
    if (AorE != A){
        printf("Server refused connecting (recieved: %c), exiting\n", AorE);
        exit(-1);
    }
    printf("Just received %c from server\n", AorE);
    
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
    int UDPport;
    while(1){
        UDPaddr.sin_port = htons(PORTNUM + offset);
        if (bind(UDPsock, (struct sockaddr*)&UDPaddr, sizeof(UDPaddr)) == 0){
            printf("Successfully bound to port %d for UDP socket\n", PORTNUM + offset);
            UDPport = htons(PORTNUM + offset);
            break;
        }
        offset++;
    }

    //write() UDPport number and the blocksize
    unsigned short portblock[2];
    portblock[0] = UDPport;
    portblock[1] = cp.BLOCKSIZE;
    write(sd, portblock, 2*sizeof(unsigned short));
    //write(sd, &(cp.BLOCKSIZE), sizeof(unsigned short));//CHECK TYPE OF BLOCKSIZE IN INSTRUCTIONS

    //START RECIEVING INFO FROM SERVER
    //SEND BACK ACKS PERTAINING TO CONGESTION CONTROL
    pthread_t pt;

    shared_t shared = {
        .counter = 0,
        .mutex = PTHREAD_MUTEX_INITIALIZER
    };

    struct thread_args {
        float * slptime;
        shared_t *shared;
    } args = {&invgamma, &shared };

    pipe(pipefd);
    pthread_create(&pt, NULL, player_thread, &args);//add file nameeeeeee
    //set_mark();
    float invlambdat = invgamma;//prolly not int, check
    char buffer[cp.BLOCKSIZE];
    float old = invgamma;
    set_mark();
    fprintf(fp, "%f\t%f\n", msec_since_mark(), invlambdat);
    int Qt;
    while (1){
	int amt;
        if ((amt = recvfrom(UDPsock, buffer, sizeof(buffer), 0, (struct sockaddr *)&UDPaddr, &lenUa))==0) break;
        
        pthread_mutex_lock(&shared.mutex);
        shared.counter += cp.BLOCKSIZE;
        Qt = shared.counter;
        pthread_mutex_unlock(&shared.mutex);
        
        write(pipefd[1], buffer, cp.BLOCKSIZE);
        
	printf("Qt: %d \n", Qt);
        if (CONTROLLAW == 0) invlambdat = invlambdat + cp.EPSILON*(Qt-cp.TARGETBUF)+cp.BETA*(invgamma -invlambdat);	
        if (old != invlambdat) {
		printf("%f vs. %f\n", old, invlambdat);
		sendto(UDPsock, &invlambdat, sizeof(float), 0, (struct sockaddr *)&UDPaddr, lenUa);
        	fprintf(fp, "%f\t%f\n", msec_since_mark(), invlambdat);
		old = invlambdat;
	}
	if (amt <cp.BLOCKSIZE) break;

    }//end while(1)
    while(1){
        int end=-1;
        ioctl(pipefd[0], FIONREAD, &end);
        if (end==0) break;
        sleep(1);
        printf("playing...\n");
    }
    pthread_cancel(pt);
    fclose(fp);
    printf("done\n");
}//end main





