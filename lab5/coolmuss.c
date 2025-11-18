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
//server
int main(int argc, char const* argv[]){
    if (argc != 5){
            printf("Expected: >>coolmuss invlambda datalog.dat serberIP serverPort\n");
            exit(1);
    }
    
    //organize arguements
    float invlambda = atoi(argv[1]);//NOT CORRECT CONVERSION
    char logfile[] = "datalog.dat.1";//just hard coding this in bec who cares
                                     //change logfile[12] = client number for each new client
    //choosing to ignore the server IP address bec not needed i think
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
        char hopefully_password[6];
        char filename[10];
        bytes = read(sock, hopefully_password, 6);
        bytes = read(sock, filename, 10);
        
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(address.sin_addr), ip_str, INET_ADDRSTRLEN);

        if(strncmp(password, hopefully_password, 6) != 0 || REGEX THING){//REGEX HERE
            printf("received invalid password or filename from client, ignoring client\n");
            write(sock, E, 1);//sending client "E"
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
                clients[i].IPaddr = address.sin_addr;
                clients[i].pn = address.sin_port;
                break;
            }
        }
        if (sessionindex == -1){
            printf("requests exceed NUMSESSIONS, dropping connection\n");
            write(sock, E, 1);//sending E if we cant take a client?? check if need
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
            write(sock, A, 1);

            //read portnumber, blocksize
            read(sock, clients[sessionindex].pn, 2);
            read(sock, clients[sessionindex].blocksize, 2);

            //create addr to communicate with this client
            struct sockaddr_in childaddr;
            childaddr.sin_faily = AF_INET;
            childaddr.sin_addr.saddr = clients[sessionindex].IPaddr;
            childaddr.sin_port = clients[sessionindex].pn;


            //finish sending and reciving congestion feed back
            //maybe use select() - theres only one place you might recieve from but,
            //if we say if select() == 0 then didnt recieve anything, send another packet
            //if select() is not zero, then look at ack and update invlambda 
            //before sending another packet
        
        }//end child code
        else{//parent code
        
        }//end parent code

    }//end while(1)
}//end main

