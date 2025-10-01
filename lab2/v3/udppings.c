#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include<netinet/in.h>

int main(int argc, char const* argv[]){
        if (argc != 3){
                printf("please enter socket number and secret\n");
                exit(1);
        }
        int pn = atoi(argv[1]);
        const char * secret = argv[2];   
    	char buffer[100];
    	char *message = "Hello Client";
    	int sd, len;
    	size_t secret_len = 6;
	struct sockaddr_in serveraddr, clientaddr;
    	memset(&serveraddr, 0, sizeof(serveraddr));

    	//socket()
    	sd = socket(AF_INET, SOCK_DGRAM, 0);        
    
    	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    	serveraddr.sin_port = htons(pn);
    	serveraddr.sin_family = AF_INET; 
 
	//bind()
    	bind(sd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
     	while(1){
    	//recvfrom()
    	len = sizeof(clientaddr);
    	int n = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientaddr, &len);
    	//buffer[n] = '\0';
	printf("%s\n", buffer);
        if (memcmp(buffer, secret, secret_len) == 0){
		printf("Secret matched, returning message");

    		//sendto()
    		if (sendto(sd, buffer, 10, 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 1){
			printf("sendfailure");
		}
	}}
}
