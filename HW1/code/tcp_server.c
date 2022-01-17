#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>// for bzero()
#include<unistd.h>// for close()
#include<signal.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

#define PortNumber 5555

int main(int argc, char **argv){
	struct sockaddr_in addr_svr;
	int sockfd, byte_sent;
	char buffer[50];
	//create server address
	memset(&addr_svr, 0, sizeof(addr_svr));
	if(argc == 2) addr_svr.sin_addr.s_addr = inet_addr(argv[1]);
	else addr_svr.sin_addr.s_addr = inet_addr("127.0.0.1");

	addr_svr.sin_family = AF_INET;
	addr_svr.sin_port = htons(PortNumber);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//bind socket
	if(bind(sockfd, (struct sockaddr *)&addr_svr, sizeof(addr_svr)) == -1){
		printf("Error: bind()\n");
		exit(1);
	}

	//make listening socket
	if(listen(sockfd, 10) == -1){
		printf("Error: listen()\n");
		exit(1);
	}

	struct sockaddr_in addr_cln;
	socklen_t sLen=sizeof(addr_cln);

	int recfd, byte_recv;
	if((recfd=accept(sockfd, (struct sockaddr *)&addr_cln, &sLen))==-1){
		printf("Accept failed!\n");
		close(sockfd);
	}

	//recive from client
	byte_recv = recv(recfd, buffer, sizeof(buffer), 0);
	if(byte_recv<0) printf("Error recving packet\n");
	else{
		//read(sockfd, buffer, sizeof(buffer));
		printf("Recived packet: %s\n", buffer);
	}

	//responds client
	char arr[50] ="Welcome!\0";
	byte_sent = send(recfd, arr, sizeof(arr), 0);
	if(byte_sent<0) printf("Error sending packet!\n");
	close(sockfd);
	
	return 0;
}
