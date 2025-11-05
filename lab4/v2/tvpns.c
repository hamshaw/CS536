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
#include <sys/select.h>
#include "tvpns.h"
//server
int main(int argc, char const* argv[]){
    if (argc != 4){
            printf("please enter IPv4 address of server, the socket number, and secret\n");
            exit(1);
    }

    unsigned short pn = atoi(argv[2]);
    const char * secret = argv[3];
    size_t secret_len = 6;


    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    //socket()
    int sd;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("creating socket failed");
        exit(-1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(pn);
    struct tunneltab *forwardtab = mmap(NULL, NUMSESSIONS*sizeof(struct tunneltab), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    memset(forwardtab, 0, sizeof(forwardtab));//might not work, check later

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
        ssize_t int_bytes;
        unsigned int hopefully_315;
        int_bytes = read(sock, &hopefully_315, sizeof(unsigned int));

        if (hopefully_315 != 315){//if they dont send 315
            printf("receieved wrong value %d, closing request\n", hopefully_315);
            close(sock);
            continue;
        }
        ssize_t secret_bytes;
        char hopefully_secret[7];
        secret_bytes = read(sock, hopefully_secret, 6);
	hopefully_secret[6] = '\0';
        if (memcmp(secret, hopefully_secret, 6) != 0){//if wrong secret
            printf("received wrong secret %s, closing request\n", hopefully_secret);
            close(sock);
            continue;
        }
        unsigned int sessionindex = -1;
        for (int i = 0; i < NUMSESSIONS; i++){
            if (forwardtab[i].sourceaddr == 0){
                printf("initalizing connection #%d\n", i);
                sessionindex = i;
                break;
            }
        }
        if (sessionindex == -1){
            printf("requests exceed NUMSESSIONS, dropping connection\n");
            continue;
        }

        read(sock, &(forwardtab[sessionindex].destaddr), sizeof(unsigned long));
	read(sock, &(forwardtab[sessionindex].destpt), sizeof(unsigned short));
	read(sock, &(forwardtab[sessionindex].sourceaddr), sizeof(unsigned long));
        int pid = fork();
        if (pid == 0){//child code
            //do stuffi
            int new_sd;
            if ((new_sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                printf("CREATING SOCKET FAILED\n");
                exit(-1);
            }
            struct sockaddr_in childaddr, fdaddr;

	    memset(&fdaddr, 0, sizeof(fdaddr));
            memset(&childaddr, 0, sizeof(childaddr));
	    
	    fdaddr.sin_family = AF_INET;
	    fdaddr.sin_addr.s_addr = forwardtab[sessionindex].destaddr;//CONTAINS FINAL DEST INFO 
	    fdaddr.sin_port = forwardtab[sessionindex].destpt;
            
            childaddr.sin_addr.s_addr = htonl(INADDR_ANY);//CONTAINS servers port with which it will reach ping client
	    childaddr.sin_family = AF_INET;
            
	    unsigned short port2 = 55500;
            for (int j = 0; j< 100; j++){//CHANGE TO WHILE(1)
                childaddr.sin_port = htons(port2);
                if (bind(new_sd, (struct sockaddr*)&childaddr, sizeof(childaddr)) == 0){//success
                    printf("Successfully bound to port #%d, informing client\n", port2);
                    forwardtab[sessionindex].sourcept = port2;
                    char secret_port[6+sizeof(unsigned short)];
                    memcpy(secret_port, secret, 6);
                    unsigned short nport = htons(port2);
                    memcpy(secret_port + 6, &nport, sizeof(unsigned short));
                    write(sock, secret_port, 6+sizeof(unsigned short));//sending client port number
                    break;
                }
                printf("failed to bind to %d\n", port2);
                port2++;
            }
	    //exit(1);//takeout
            int new_sd_out;
            if ((new_sd_out = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                printf("CREATING SOCKET FAILED\n");
                exit(-1);
            }
            struct sockaddr_in childaddr_out;//CONTAINS SERVERS INFO, TO CONTACT FD
            memset(&childaddr_out, 0, sizeof(childaddr_out));
            childaddr_out.sin_addr.s_addr = htonl(INADDR_ANY);//forwardtab[sessionindex].destaddr;//sin_addr.s_addr?
            childaddr_out.sin_family = AF_INET;
            
	    int port = 57500;
	
            for (int p = 0; p<200; p++){
                childaddr_out.sin_port = htons(port);
                if (bind(new_sd_out, (struct sockaddr*)&childaddr_out, sizeof(childaddr_out)) == 0){//success
                    printf("Successfully bound to port #%d for sending\n", port);
                    forwardtab[sessionindex].tunnelpt = port;
                    break;
                }
                printf("out - failed to bind to %d\n", port);
                port++;
            }
	    struct sockaddr_in clientaddr;
            memset(&clientaddr, 0, sizeof(clientaddr));
	    clientaddr.sin_family = AF_INET;
	    
	    //sock
            //new_sd
            //new_sd_out
            fd_set readfds;
            int max_fd;
            while(1){
                FD_ZERO(&readfds);
                FD_SET(sock, &readfds);
                FD_SET(new_sd, &readfds);
                FD_SET(new_sd_out, &readfds);
                max_fd = sock;
                if (new_sd > max_fd) max_fd = new_sd;
                if (new_sd_out > max_fd) max_fd = new_sd_out;
                max_fd += 1;

                int ready = select(max_fd, &readfds, NULL, NULL, NULL);
                if (ready < 0) {
                    printf("select failed\n");
                    break;
                }
		printf("select success\n");
                if (FD_ISSET(sock, &readfds)){
                    //sd is ready
		    printf("recieved something from tcp\n");
		    char hopefully_secret2[6];
                    read(sock, hopefully_secret2, 6);
                    if (strncmp(secret, hopefully_secret2, 6) != 0){
                        printf("received bad secret %s, not terminating\n", hopefully_secret2);
                    }else{
                        printf("recieved secret, terminating\n");
                        forwardtab[sessionindex].sourceaddr = 0;//make shared mem
                        close(new_sd);
                        close(new_sd_out);
                        close(sd);
			close(sock);
                        munmap(forwardtab, NUMSESSIONS*sizeof(struct tunneltab));
                        break;
                    }
                }
                if (FD_ISSET(new_sd, &readfds)){
                    //new_sd is ready
		    printf("got a message from client\n");
                    char buffer[100] = {0};
                    int len = sizeof(clientaddr);
                    recvfrom(new_sd, buffer, sizeof(buffer), 0, (struct sockaddr*) &clientaddr, &len);

		    sendto(new_sd_out, buffer, sizeof(buffer), 0, (struct sockaddr*) &fdaddr, sizeof(fdaddr));
                }
                if (FD_ISSET(new_sd_out, &readfds)){
                    //new_sd_out is ready
		    printf("got a message from server\n");
                    char buffer[100] = {0};
                    int len = sizeof(fdaddr);
                    recvfrom(new_sd_out, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);//&fdaddr, &len);
                    sendto(new_sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
                }
            }//ends the childs big while for select
            printf("child done\n");
	    exit(0);
        }//else{//parent code
            //int status;
          //  waitpid(pid, &status, 0);
        //}
    }//end of huge while
    return 0;
}
