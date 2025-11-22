#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <net/if.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <regex.h>
#include "coolmuss.h"
#include <math.h>
//server
struct timespec marker;
void set_mark(void){
        clock_gettime(CLOCK_MONOTONIC, &marker);
}
double msec_since_mark(void){
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        return (now.tv_sec - marker.tv_sec)*1000 + (now.tv_nsec - marker.tv_nsec)/1000000;
}

int main(int argc, char const* argv[]){
    if (argc != 5){
            printf("Expected: >>coolmuss invlambda datalog.dat serberIP serverPort\n");
            exit(1);
    }
    char Es = 'E';
    char As = 'A';
    //organize arguements
    float invlambda = atof(argv[1]);//in miliseconds
    const char * logfile = argv[2];//just hard coding this in bec who cares
                                     //change logfile[12] = client number for each new client

    size_t lenlf = strlen(logfile);
    char dfilename[20] = {'\0'};
    memcpy(dfilename, logfile, lenlf);
    const char * serverIP = argv[3];
    unsigned short pn = atoi(argv[4]);

    //mmap()
    struct client *clients = mmap(NULL, NUMSESSIONS*sizeof(struct client), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    //socket() - TCP
    int sd;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("creating socket failed");
        exit(-1);
    }

    //set up sockaddr_in
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(pn);

    //bind()
    if (bind(sd, (struct sockaddr*)&address, addrlen) < 0) {
        perror("bind failed");
        exit(-1);
    }
	printf("begin listening...\n");
    while(1){
        
        //listen()
        if (listen(sd, 5) < 0) {
            perror("failed listen");
            exit(-1);
        }
        
        //accept()
        int sock;
        if ((sock = accept(sd, (struct sockaddr*)&address, &addrlen)) < 0) {
            perror("accept failed");
            exit(-1);
        }
	printf("Accepting connection...\n");
        
        //read()
        ssize_t bytes1, bytes2;
        char hopefully_secret[7] = {'\0'};
        char filename[10] = {'\0'};
        bytes1 = read(sock, hopefully_secret, 6);
	bytes2 = read(sock, filename, 7);
        
        printf("Received potential secret %s, %ld Bytes and filename %s, %ld Bytes ...\n", hopefully_secret, bytes1, filename, bytes2);
        //Handling REGEX
        if(strncmp(SECRET, hopefully_secret, 6) != 0){// || regexret == 0){//REGEX HERE
            printf("received invalid password or filename from client, ignoring client\n");
            write(sock, &Es, 1);//sending client "E"
            close(sock);
            continue;
        }else printf("Valid client request ( %hu) pertaining to file %s\n", address.sin_port, filename);
        
        //check number of clients
        unsigned int sessionindex = -1;
        for (int i = 0; i < NUMSESSIONS; i++){
            if (clients[i].sessionindex == 0){
                printf("initalizing connection #%d\n", i);
                sessionindex = i;
                clients[i].sessionindex = i;
                clients[i].IPaddr = address.sin_addr.s_addr;
                clients[i].pn = address.sin_port;
                ///memcpy(clients[i].filename, filename, 10);//check memcpy syntax
                break;
            }
        }
        if (sessionindex == -1){
            printf("requests exceed NUMSESSIONS, dropping connection\n");
            write(sock, &Es, 1);//sending E if we cant take a client?? check if need
            continue;
        }
        
        //fork()
        int pid = fork();
        if (pid ==0){//child code
            

            FILE *fp;
            dfilename[lenlf] = (char)sessionindex;
            fp = fopen(dfilename, "w");
            //socket() - UDP
            int sdUDP;
            if ((sdUDP = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                printf("CREATING SOCKET FAILED\n");
                exit(-1);
            }
            //bind() - UDP
            
            //write(A)
            write(sock, &As, 1);

            //read portnumber (already converted), blocksize
            read(sock, &(clients[sessionindex].pn), sizeof(unsigned short));
            read(sock, &(clients[sessionindex].blocksize), sizeof(unsigned short));
	    printf("Got portnum: %hu and blocksize %hu\n", clients[sessionindex].pn, clients[sessionindex].blocksize);
            //create addr to communicate with this client
            struct sockaddr_in childaddr;
            int lenca = sizeof(childaddr);
            childaddr.sin_family = AF_INET;
            childaddr.sin_addr.s_addr = clients[sessionindex].IPaddr;
            childaddr.sin_port = clients[sessionindex].pn;


            //finish sending and reciving congestion feed back
            //maybe use select() - theres only one place you might recieve from but,
            //if we say if select() == 0 then didnt recieve anything, send another packet
            //if select() is not zero, then look at ack and update invlambda 
            //before sending another packet
            
            FILE *file = fopen(filename, "rb");
            if (!file){
                printf("error opening file");
            }
            fseek(file, 0, SEEK_END);
            float filesize = (float)ftell(file);
            rewind(file);
            char *buffer = malloc(sizeof(char)*filesize +1);
            size_t bytesRead = fread(buffer, 1, filesize, file);
            buffer[bytesRead] = '\0';
            fclose(file);
            
            //How many packets we sending?
            int numPackets = (int)ceil(filesize/clients[sessionindex].blocksize);
            printf("sending %d packets...%f\n", numPackets, filesize);
	    int i = 0;
            struct timeval *timestamps = calloc(numPackets, sizeof(struct timeval));
            float *ivls = calloc(numPackets, sizeof(float));

            fd_set readfds;
            int max_fd;

            //need this for select??
            struct timeval notime;
            memset(&notime, 0, sizeof(notime));

            //need this for nanosleep?
            struct timespec sleeptime;
            sleeptime.tv_sec = 0;
            //sleeptime.tv_nsec = invlambda;
            
            set_mark();
            fprintf(fp, "%f\t%f\n", msec_since_mark(), invlambda);

            //while()/select()
            while(i < numPackets){//something
                sleeptime.tv_nsec = invlambda*1000000;
                FD_ZERO(&readfds);
                FD_SET(sdUDP, &readfds);//HMMMM
                max_fd = sdUDP+1;
                int ready = select(max_fd, &readfds, NULL, NULL, &notime);//last arg timeval = 0?
                if (ready < 0){
                    printf("select failed\n");
                    exit(1);
                }
                if (ready == 0){//we dont have an updated sending rate
                    nanosleep(&sleeptime, NULL);
		    sendto(sdUDP, &(buffer[i*clients[sessionindex].blocksize]), clients[sessionindex].blocksize, 0, (struct sockaddr *)&childaddr, lenca);
                    i++;
                }else if (FD_ISSET(sdUDP, &readfds)){//we have an updated sending rate
                    recvfrom(sdUDP, &invlambda, sizeof(float), 0, (struct sockaddr*) &childaddr, &lenca);
                    struct timeval now;
                    gettimeofday(&now, NULL);
                    timestamps[i] = now;
                    ivls[i] = invlambda;
                    fprintf(fp, "%f\t%f\n", msec_since_mark(), invlambda);
                }
            }//end sending and receiving while()
	    printf("done sending to client.\n");
	    sendto(sdUDP, &Es, 1, 0, (struct sockaddr *)&childaddr, lenca);
            //write everything to our info file
            //make sure to chnage the name of the file based on session index!
            //set session index = 0?
        }//end child code
        else{//parent code
        
        }//end parent code

    }//end while(1)
}//end main

