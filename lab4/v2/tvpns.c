#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <net/if.h>

//server
int main(int argc, char const* argv[]){
    if (argc != 4){
            printf("please enter IPv4 address of server, the socket number, and secret\n");
            exit(1);
    }

    int pn = atoi(argv[2]);
    const char * secret = argv[3]
    size_t secret_len = 6;

        
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[25];

    //socket()
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("creating socket failed");
        exit(-1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(pn);

    //bind()
    if (bind(sd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(-1);
    }
    return 0;
}
