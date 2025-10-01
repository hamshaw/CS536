#include <stdio.h>
#include <strings.h>
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
    	int listenfd, len;
    	struct sockaddr_in servaddr, cliaddr;
    	memsest(&servaddr, 0, sizeof(servaddr));

    	//socket()
    	sd = socket(AF_INET, SOCK_DGRAM, 0);        
    
    	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    	servaddr.sin_port = htons(pn);
    	servaddr.sin_family = AF_INET; 
 
	//bind()
    	bind(sd, (struct sockaddr*)&servaddr, sizeof(servaddr));
     
    	//recvfrom()
    	len = sizeof(cliaddr);
    	int n = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len);
    	buffer[n] = '\0';
         
    	//sendto()
    	sendto(sd, send, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
}
