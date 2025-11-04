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

    for
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    //socket()
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("creating socket failed");
        exit(-1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(pn);
    struct tunneltab forwardtab[NUMSESSIONS];
    memset(forwardtab, 0, sizeof(forwardtab));//might not work, check later

    //bind()
    if (bind(sd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(-1);
    }
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
    int_bytes = read(sock, hopefully_315, 2);
    
    if (hopefully_315 != 315){//if they dont send 315
        printf("receieved wrong value, closing request\n");
        close(sock);
        break;
    else{
        ssize_t secret_bytes;
        char hopefully_secret[6];
        secret_bytes = read(sock, hopefully_secret, 6);
        if (memcmp(secret, hopefully_secret) != 0){//if wrong secret
            printf("received wrong secret, closing request\n");
            close(sock);
            break;
        }else{
            unsigned int sessionindex = -1;
            for (int i = 0, i < NUMSESSIONS; i++){
                if (forwardtabl[i].sourceaddr == 0){
                    printf("initalizing connection #%d\n", i);
                    sessionindex = i;
                    break;
                }
            }
            if (sessionindex == -1){
                printf("requests exceed NUMSESSIONS, dropping connection\n");
                continue;
            }
            read(sock, forwardtab[sessionindex].destaddr, 4);
            read(sock, forwardtab[sessionindex].despt, 2);
            read(sock, forwardtab[sessionindex].sourceaddr, 4);
            
    }




    return 0;
}
