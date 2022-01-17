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

int main(int argc, char *argv[]){
	struct sockaddr_in address;
	struct timeval start;
	struct timeval end;
	float diff;
	int sock, byte_sent;
	char buffer[1024];
	char arr[1024];

	//create client socket
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock<0) printf("Error creating socket\n");

	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(Server_PortNumber);
	address.sin_addr.s_addr = inet_addr(Server_Address);
	socklen_t address_length = sizeof(address);

	//write a string to server and write in buffer
	printf("Packet content: ");
	scanf("%[^\n]", buffer);

	//write to server
	gettimeofday(&start, NULL);//start
	byte_sent = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&address, address_length);
	if(byte_sent<0) printf("Error sending packet!\n");
	

	//recive from server
	int byte_recv;
	byte_recv = recvfrom(sock, arr, sizeof(arr), 0, (struct sockaddr *)&address, &address_length);
	gettimeofday(&end, NULL);//end
	if(byte_recv<0) printf("Error recving packet\n");
	else printf("%s\n", arr);
		
	diff = 1000000 * (end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;//calculate the latency
	printf("The latency of UDP is %.2f(ms).\n", diff);//show latency
	
	float len;
	len = strlen(buffer);
	printf("%.2f\n", len/(diff/1000));
	
	//close socket
	close(sock);
	return 0;
}