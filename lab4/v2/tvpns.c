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

    //socket()
    int sd;
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
            //read(sock, destaddr, 4);
            //read(sock, despt, 2);
            //read(sock, sourceaddr, 4);

            read(sock, &(forwardtab[sessionindex].destaddr), 4);
            read(sock, &(forwardtab[sessionindex].destpt), 2);
            read(sock, &(forwardtab[sessionindex].sourceaddr), 4);
            int pid = fork();
            if (pid == 0){//child code
                //do stuff
                int new_sd;
                if ((new_sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
                    printf("CREATING SOCKET FAILED\n");
                    exit(-1);
                }
                struct sockaddr_in childaddr;
                memset(&childaddr, 0, sizeof(childaddr));
                childaddr.sin_addr.s_addr = forwardtab[sessionindex].sourceaddr;//sin_addr.s_addr?
                childaddr.sin_family = AF_INET;
                int port = 55500;
                while(1){
                    childaddr.sin_port = gtons(port);
                    if (bind(new_sd, (struct sockaddr*)&childaddr, sizeof(childaddr)) == 0){//success
                        printf("Successfully bound to port #%d, informing client\n", port);
                        forwardtab[sessionindex].sourcept = port;
                        char secret_port[8];
                        memcpy(secret_port, secret, 6);
                        unsigned int nport = htons(nport);
                        memcpy(secret_port + 6, &nport, 2);
                        write(new_sd, secret_port, 8);//sending client port number
                        break;
                    }
                    printf("failed to bind to %d\n", port);
                    port++;
                }
                int new_sd_out;
                if ((new_sd_out = socket(AF_NET, SOCK_DGRAM, 0)) < 0){
                    printf("CREATING SOCKET FAILED\n");
                    exit(-1);
                }
                struct sockaddr_in childaddr_out;
                memset(&childaddr_out, 0, sizeof(childaddr_out));
                childaddr_out.sin_addr.s_addr = forwardtab[sessionindex].destaddr;//sin_addr.s_addr?
                childaddr_out.sin_family = AF_INET;
                int port = 57500;

                while(1){
                    childaddr_out.sin_port = htons(port);
                    if (bind(new_sd_out, (struct sockaddr*)&childaddr_out, sizeof(childaddr_out)) == 0){//success
                        printf("Successfully bound to port #%d for sending\n", port);
                        forwardtab[sessionindex].tunnelpt = port;
                        break;
                    }
                    printf("out - failed to bind to %d\n", port);
                    port++;
                }
                //sd
                //new_sd
                //new_sd_out
                fd_set readfds;
                int max_fd;
                while(1){
                    FD_ZERO(&readfds);
                    FD_SET(sd, &readfds);
                    FD_SET(new_sd, &readfds);
                    FD_SET(new_sd_out, &readfds);
                    max_fd = sd;
                    if (new_sd > max_fd) max_fd = new_sd;
                    if (new_sd_out > max_fd) max_fd = new_sd_out;
                    max_fd += 1;

                    int ready = select(max_fd, &readfds, NULL, NULL, NULL);
                    if (ready < 0) {
                        printf("select failed\n");
                        break;
                    }
                    if (FD_ISSET(sd, &readfds)){
                        //sd is ready
                        char hopefully_secret[6];
                        read(sd, hopefully_secret, 6);
                        if (strncmp(secret, hopefully_secret, 6) != 0){
                            printf("received bad secret, not terminating\n");
                        }else{
                            printf("recieved secret, terminating\n");
                            forwardtab[sessionindex].sourceaddr = 0;//make shared mem
                            break;
                        }
                    }
                    if (FD_ISSET(new_sd, &readfds)){
                        //new_sd is ready
                        char buffer[100] = {0};
                        int len = sizeof(childaddr);
                        recvfrom(new_sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&childaddr, &len);
                        sendto(new_sd_out, buffer, sizeof(buffer), 0, (struct sockaddr*)&childaddr_out, sizeof(childadder_out));
                    }
                    if (FD_ISSET(new_sd_out, &readfds)){
                        //new_sd_out is ready
                        char buffer[100] = {0};
                        int len = sizeof(childaddr);
                        recvfrom(new_sd_out, buffer, sizeof(buffer), 0, (struct sockaddr*)&childaddr_out, &len);
                        sendto(new_sd, buffer, sizeof(buffer), 10, 0, (struct sockaddr*)&childaddr, sizeof(childaddr));
                    }
                }
                exit(0);
            }else{//parent code
                int status;
                waitpid(pid, &status, 0);
            }
    }




    return 0;
}
