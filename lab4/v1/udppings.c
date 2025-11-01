#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <net/if.h>
int main(int argc, char const* argv[]){
        if (argc != 3){
                printf("please enter socket number and secret\n");
                exit(1);
        }
        int pn = atoi(argv[1]);
        const char * secret = argv[2];   
    	char buffer[100];
    	int len, ssd;
	size_t secret_len = 6;
	struct sockaddr_in6 serveraddr, clientaddr;
    	memset(&serveraddr, 0, sizeof(serveraddr));

    	//socket()
    	ssd = socket(AF_INET6, SOCK_DGRAM, 0);        
    
    	//inet_pton(AF_INET6, "fe80::a6bb:6dff:fe44:ddb8", &(serveraddr.sin6_addr));
    	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(pn);
    	serveraddr.sin6_family = AF_INET6; 
        serveraddr.sin6_scope_id = if_nametoindex("eth0");
	//bind()
    	if(0 != bind(ssd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))){
		printf("binding failed, closing\n");
		//close(ssd);
		exit(1);
	}
     	while(1){
    	
		//recvfrom()
    		len = sizeof(clientaddr);
    		int n = recvfrom(ssd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientaddr, &len);
    	
		printf("got message \n");
        	if (memcmp(buffer, secret, secret_len) == 0){
			printf("Secret matched, returning message: %s\n", buffer);

    			//sendto()
    			if (sendto(ssd, buffer, 10, 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 1){
				printf("sendfailure");
			}
		}
	}
}
