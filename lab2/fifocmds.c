#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include "helperFunctions.h"
#include <sys/socket.h>

int main(int argc, char * argv[]){
	if (argc != 3){
		printf("please enter socket number and secret\n");
		exit(1);
	}
	struct sockadder_in saddr;
	socklen_t c_len;
	char buf[1024];//change this
	
	int pn = atoi(argv[1]);
	char * secret = argv[2];
	int fd, c_fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("socket failed\n");//error check
	}
	//memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	//saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = pn;
	if (bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1){
		printf("Error with bind\n");
		close(fd);
		exit(1);
	}
	if (listen(fd, 1) == -1){
		printf("error with listen\n");
		close(fd);
		exit(1);
	}
	c_len = sizeof(caddr);
	c_fd = accept(fd, (struct sockadd *)&caddr, &c_len);
	//check error ^
	memset(buf, 0, 1024);//change 1024
	ssize_t bytes_read = read(c_fd, buf, 1023);//change value in this and line before
	//if (bytes_read<0) do error
	printf("received: %s\n", buf);

}	
