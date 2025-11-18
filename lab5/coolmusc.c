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
//client
int main(int argc, char const* argv[]){
    if (argc != 7){
            printf("Expected: >>coolmusc audiofile serverIPserverOirt ubvganna oaran,dat cdatakig,dat\n");
            exit(1);
    }
