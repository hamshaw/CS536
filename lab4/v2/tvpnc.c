#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
//client
int main(int argc, char const* argv[]){
    if (argc != 7){
            printf("please enter IPv4 and port of tunneling server, the secret, IPv4 of UDP client, and IPv4 and port of final destination (6)\n");
            exit(1);
    }
    const char * ip_ts = argv[1];
    int pn_ts = htons(atoi(argv[2]));
    const char * secret = argv[3];
    unsigned int ip_UDPc;
    inet_pton(AF_INET, argv[4], &ip_UDPc);
    unsigned int ip_fd;//const char * ip_fd ip_fd = argv[5];
    inet_pton(AF_INET, argv[5], &ip_fd);
    int pn_fd = htons(atoi(argv[6]));

    //socket()
    int sd;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("creating socket failed");
        exit(-1);
    }
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_port = pn_ts;
    inet_pton(AF_INET, ip_ts, &(address.sin_addr));

    //connect()
    int status;
    if ((status = connect(sd, (struct sockaddr*)&address, sizeof(address))) < 0) {
        printf("connect failed\n");
        exit(-1);
    }

    char int_secret[18];
    unsigned int code = 315;
    memcpy(int_secret, &code, sizeof(unsigned int));
    memcpy(int_secret+2, secret, 6);
    memcpy(int_secret+8, &ip_fd, 4);
    memcpy(int_secret+12, &pn_fd, 2);
    memcpy(int_secret+14, &ip_UDPc, 4);
    //write()
    write(sd, int_secret, 18);
    
    //read()
    char hopefully_secret[6];
    read(sd, hopefully_secret, 6);
    if (strcmp(secret, hopefully_secret) != 0){
        printf("wrong secret, terminating\n");
        close(sd);
        exit(-1);
    }
    int provided_port;
    read(sd, &provided_port, 2);
    printf("the provided port number is:\t%d\n", provided_port);
}



