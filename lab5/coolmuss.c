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
int main(int argc, char const* argv[]){
    if (argc != 5){
            printf("Expected: >>coolmuss invlambda datalog.dat serberIP serverPort\n");
            exit(1);
    }
    
    //organize arguements
    float invlambda = atof(argv[1])*1000000;//converted to NANO
    char logfile[] = "datalog.dat.1";//just hard coding this in bec who cares
                                     //change logfile[12] = client number for each new client
    const char * serverIP = argv[3];
    unsigned short pn = atoi(argv[4]);

    //mmap()
    struct client *clients = mmap(NULL, NUMSESSIONS*sizeof(struct client), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    //socket() - TCP
    int sd;
    if ((sd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("creating socket failed");
        exit(-1);
    }

    //set up sockaddr_in
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(pn);

    //bind()
    if (bind(sd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(-1);
    }

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
        
        //read()
        ssize_t bytes;
        char hopefully_secret[6];
        char filename[10];
        bytes = read(sock, hopefully_secret, 6);
        bytes = read(sock, filename, 10);
        
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(address.sin_addr), ip_str, INET_ADDRSTRLEN);

        //Handling REGEX
        regex_t regex;
        int reti = regcomp(&regex, FILENAME_PATTERN, REG_EXTENDED);
        regmatch_t match;
        int regexret = regexec(&regex, filename, 1, &match, 0);

        if(strncmp(SECRET, hopefully_secret, 6) != 0 || regexret == 0){//REGEX HERE
            printf("received invalid password or filename from client, ignoring client\n");
            write(sock, &E, 1);//sending client "E"
            close(sock);
            continue;
        }else printf("Valid client request (%s, %hu) pertaining to file %s\n", ip_str, address.sin_port, filename);
        
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
            write(sock, &E, 1);//sending E if we cant take a client?? check if need
            continue;
        }
        
        //fork()
        int pid = fork();
        if (pid ==0){//child code
            


            //socket() - UDP
            int sdUDP;
            if ((sdUDP = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                printf("CREATING SOCKET FAILED\n");
                exit(-1);
            }
            //bind() - UDP
            
            //write(A)
            write(sock, &A, 1);

            //read portnumber (already converted), blocksize
            read(sock, &(clients[sessionindex].pn), sizeof(unsigned short));
            read(sock, &(clients[sessionindex].blocksize), sizeof(unsigned short));

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

            //while()/select()
            while(i < numPackets){//something
                sleeptime.tv_nsec = invlambda;
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
                    printf("got new sending rate from client, lambda = %f\n@ time: %ld", 1/invlambda, now.tv_usec);
                }
            }//end sending and receiving while()
            //write everything to our info file
            //make sure to chnage the name of the file based on session index!
            //set session index = 0?
        }//end child code
        else{//parent code
        
        }//end parent code

    }//end while(1)
}//end main

