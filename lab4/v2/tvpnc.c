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
    unsigned short pn_ts = htons(atoi(argv[2]));
    const char * secret = argv[3];
    unsigned long ip_UDPc;
    inet_pton(AF_INET, argv[4], &ip_UDPc);
    unsigned long ip_fd;//const char * ip_fd ip_fd = argv[5];
    inet_pton(AF_INET, argv[5], &ip_fd);
    unsigned short pn_fd = htons(atoi(argv[6]));

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

    char int_secret[sizeof(unsigned int) + 6 + sizeof(unsigned long)+ sizeof(unsigned short) + sizeof(unsigned long)] = {0};
    unsigned int code = 315;
    memcpy(int_secret, &code, sizeof(unsigned int));
    memcpy(int_secret+(sizeof(unsigned int)), secret, 6);
    printf("sending %lu \n", ip_fd);
    memcpy(int_secret+6+(sizeof(unsigned int)), &ip_fd, sizeof(unsigned long));
    printf("sending %hu\n", pn_fd);
    memcpy(int_secret+6+sizeof(unsigned long)+(sizeof(unsigned int)), &pn_fd, sizeof(unsigned short));
    printf("sending %lu \n", ip_UDPc);
    memcpy(int_secret+6 +sizeof(unsigned long) + sizeof(unsigned short)+(sizeof(unsigned int)), &ip_UDPc, sizeof(unsigned long));
    //write()
    write(sd, int_secret, sizeof(unsigned int) + 6 + sizeof(unsigned long)+ sizeof(unsigned short) + sizeof(unsigned long));
    
    //read()
    char hopefully_secret[6];
    read(sd, hopefully_secret, 6);
    if (strncmp(secret, hopefully_secret, 6) != 0){
        printf("wrong secret, terminating\n");
        close(sd);
        exit(-1);
    }
    unsigned short provided_port;
    read(sd, &provided_port, sizeof(unsigned short));
    printf("the provided port number is:\t%hu\n", ntohs(provided_port));
    
    char key[7] = {0};
    key[6] = '\0';
    while (1) {
        printf("Enter secret to terminate: ");
        
        if (fgets(key, sizeof(key), stdin) == NULL) {
            printf("fgets failed");
            return 1;
        }

        key[strcspn(key, "\n")] = '\0';
	write(sd, key, 6);
        if (strncmp(key, secret, 6) == 0) {
            break;
        }
    }
	close(sd);
}



