#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "math_functions.h"
#include "alarmHandler.h"
int main(int argc, char const* argv[]){
        if (argc != 6){
                printf("please enter servip, portnum, secret, portnum2, and pcount");
                exit(1);
        }

	struct sigaction alarmAction;
        alarmAction.sa_handler = alarmHandler;
        alarmAction.sa_flags = 0;
        sigemptyset(&alarmAction.sa_mask);
    	sigaction(SIGALRM, &alarmAction, NULL);


        const char * ip_addr = argv[1];
        int pn = atoi(argv[2]);
        const char * secret = argv[3];
	int pn2 = atoi(argv[4]);
	int pcount = atoi(argv[5])%100;

	int values[pcount];
	memset(values, 0, sizeof(values));
	char buffer[100];
    	int sd;
    	struct sockaddr_in6 clientaddr, serveraddr;
	
	memset(&clientaddr, 0, sizeof(clientaddr));
    	memset(&serveraddr, 0, sizeof(serveraddr));
    	//serveraddr.sin6_addr.s_addr = inet_pton(ip_addr, "fe80::a6bb:6dff:fe44:fc43%eth0");
    	//serveraddr.sin6_port = htons(pn);
    	//serveraddr.sin6_family = AF_INET6;
        //serveraddr.sin6_port_id = 32;
	clientaddr.sin6_port = htons(pn2);
	clientaddr.sin6_family = AF_INET6;
	clientaddr.sin6_addr.s_addr = htonl(INADDR_ANY, "fe80::a6bb:6dff:fe44:fc43%eth0");
    clientaddr.sin6_port_id = 32;
    
    	//scoket()
    	sd = socket(AF_INET, SOCK_DGRAM, 0);
   
	//bind()
	if (bind(sd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) != 0){
		printf("Binding failed, closing");
		close(sd);
		exit(1);
	}

	srand(time(NULL));
	int number = rand()%1000;
        char message[10];
        memcpy(message, secret, 6);
	printf("Begining to send %d ping messages\n", pcount);
	for (int i=0; i<pcount; i++){
		sprintf(message+6, "%d", number+i);
		ualarm(123456, 0);
		
		//sendto()
    		if (sendto(sd, message, 10, 0, (const struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 1){
			printf("sendfailure\n");
		}
	
    		//recvfrom()
    		recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
		if (atoi(buffer+6) == number+i){//not how you check the index
			int val = ualarm(0, 0);
			values[i] = val;
		}
	}
	printf("Completed sending of ping messages\n");
    	
	printf("Minimum:\t\t%d\n", minimum(values, pcount));
	printf("Maximum:\t\t%d\n", maximum(values, pcount));
	printf("Mean:\t\t\t%d\n", mean(values, pcount));
	printf("Standard Deviations:\t%d\n", stdv(values, pcount));
	
	//close()
    	close(sd);
}
