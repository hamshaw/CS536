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
