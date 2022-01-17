#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>// for bzero()
#include<unistd.h>// for close()
#include<sys/types.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#define Server_PortNumber 5555
#define Server_Address "127.0.0.1"

int main(int argc, char **argv){
	struct sockaddr_in addr_svr;
	struct timeval start;
	struct timeval end;
	unsigned long diff;
	int sockfd;
	char buffer[1024];
	char arr[1024];

	//create server addr
	memset(&addr_svr, 0, sizeof(addr_svr));
	addr_svr.sin_family = AF_INET;
	addr_svr.sin_port = htons(Server_PortNumber);

	//get the server address
	if(argc == 2) addr_svr.sin_addr.s_addr = inet_addr(argv[1]);
	else addr_svr.sin_addr.s_addr = inet_addr(Server_Address);

	//create client socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		//cerr<<"Error: socket()"<<endl;
		printf("Error creating socket!\n");
		exit(1);
	}

	//connect
	if(connect(sockfd, (struct sockaddr*)&addr_svr, sizeof(addr_svr)) == -1){
		//cerr<<"Error: connect()"<<endl;
		printf("Connect failed!\n");
		exit(1);
	}

	//write a string to server
	printf("Packet content: ");
	scanf("%[^\n]", buffer);//input you write string to buffer

	gettimeofday(&start, NULL);//start
	write(sockfd, buffer, strlen(buffer)+1);
	
	//recive from server
	read(sockfd, arr, sizeof(arr));
	gettimeofday(&end, NULL);//end
	printf("%s\n", arr);//print response from server
	diff = 1000000 * (end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;//calculate the latency
	printf("The latency of TCP is %ld(ms).\n", diff);//show latency

	//close socket
	close(sockfd);

	return 0;
}
