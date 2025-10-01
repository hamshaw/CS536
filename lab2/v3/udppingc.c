#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char const* argv[]){
        if (argc != 6){
                printf("please enter servip, portnum, secret, portnum2, and pcount");
                exit(1);
        }
    	char buffer[100];
    	int sd, n;
    	struct sockaddr_in serveraddr;

	int number = rand()%1000
	char message[10];
	memcpy(message, secret, 6);
	memcpy(message+6, number);

    	memset(&serveraddr, 0, sizeof(serveraddr));
    	serveraddr.sin_addr.s_addr = inet_addr("sIP");
    	serveraddr.sin_port = htons(pn);
    	serveraddr.sin_family = AF_INET;
    
    	//scoket()
    	sd = socket(AF_INET, SOCK_DGRAM, 0);
    
    	//connect()
    	if(connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        	printf("\n Error : Connect Failed \n");
        	exit(0);
    	}
	
	//sendto()CHANGE 1024????????
    	sendto(sd, message, 1024, 0, (struct sockaddr*)NULL, sizeof(serveraddr));
    
    	//recvfrom()
    	recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
    
    	//close()
    	close(sd);
}
