#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>// for bzero()
#include<unistd.h>// for close()
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#define PortNumber 5555

int main(int argc, char *argv[]){
	struct sockaddr_in address, client_address;
	int sock, byte_recv;
	socklen_t client_address_length;
	char buffer[50];

	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock<0) printf("Error creating socket\n");
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PortNumber);
	address.sin_addr.s_addr = INADDR_ANY;

	//bind socket
	if(bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1){
		printf("Error binding\n");
		close(sock);
	}

	//recive from client
	byte_recv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, &client_address_length);
	if(byte_recv<0) printf("Error recving packet!\n");
	else printf("Recived packet: %s\n", buffer);
	
	//response to client
	char arr[1024] = "Welcome!\0";
	int byte_sent;
	byte_sent = sendto(sock, arr, sizeof(arr), 0, (struct sockaddr *)&client_address, client_address_length);
	if(byte_sent<0) printf("Error sending packet!\n");
	close(sock);
	
	return 0;
}