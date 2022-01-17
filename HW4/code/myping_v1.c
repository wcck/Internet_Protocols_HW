#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>

unsigned short cal_chksum(unsigned short *addr,int len){   
	int nleft = len;
	int sum = 0;
	unsigned short *w = (unsigned short*)addr;
	unsigned short answer = 0;
	while(nleft > 1){
		sum += *w++;
		nleft -= 2;
	}


	if( nleft == 1){   
		*(unsigned char *)(&answer) = *(unsigned char *)w;
		sum += answer;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return answer;
}


int main(int argc, char *argv[]){
	struct sockaddr_in address;
	unsigned int T1, T2, T3, T4, RTT;
	char buffer[1024],feeback[1024];
	int byte_sent, byte_recv;
	int sd = socket(PF_INET, SOCK_RAW, 1);
	//printf("sd:%d\n",sd);
	if (sd < 0)    printf("Error creating socket\n");

	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(argv[1]);
	unsigned int arr[10];
	for(int i=0;i<10;i++){
		sleep(1);
		struct icmp *icmp_hdr = (struct icmp *) buffer;
		memset(buffer, 0, sizeof(buffer));

		//初始設定 ICMP packet
		icmp_hdr->icmp_type = ICMP_ECHO;
		icmp_hdr->icmp_code = 0;
		icmp_hdr->icmp_id = htons(getpid());
		icmp_hdr->icmp_seq = 0;
		icmp_hdr->icmp_cksum=0; 
		
		struct  timeval start;
		gettimeofday(&start, NULL);
		unsigned int t=start.tv_sec%86400*1000 + start.tv_usec/1000;

		icmp_hdr->icmp_otime = htonl(t);
		icmp_hdr->icmp_rtime = 0;
		icmp_hdr->icmp_ttime = 0;
		icmp_hdr->icmp_cksum = cal_chksum((unsigned short *)icmp_hdr, sizeof(struct icmp));//用 chksum function 算checksum
		
		
		
		int address_length=20;
		//printf("len: %d\n", address_length);

		byte_sent=sendto(sd, buffer, address_length, 0, (struct sockaddr *)&address, sizeof(address));
		//printf("%d\n", byte_sent);
		if (byte_sent < 0)    printf("Error sending packet\n");

		byte_recv = recvfrom(sd, feeback, sizeof(feeback), 0, NULL, NULL);
		//printf("%d\n", byte_recv);
		struct ip *recv_ip = (struct ip*)feeback;
		struct icmp *icmp_hdr_recv = (struct icmp*)(feeback + (recv_ip->ip_hl<<2));  //Skip IP header
		
		struct  timeval end;
		gettimeofday(&end, NULL);
		
		
		T1 = htonl(icmp_hdr_recv->icmp_otime);
		T2 = htonl(icmp_hdr_recv->icmp_rtime);   
		T3 = htonl(icmp_hdr_recv->icmp_ttime);
		T4 = (end.tv_sec%86400)*1000 + end.tv_usec/1000;
		RTT = (T4-T3) + (T2-T1);
		arr[i] = RTT;

		//-------------顯示收到的timestamp reply 和運算結果------
		//printf("%u ,%u ,%u, %u\n", T1, T2, T3, T4);
		printf("replyfrom = %s, icmp_type = %d, icmp_code = %d, icmp_seq = %d, ",argv[1], icmp_hdr_recv->icmp_type, icmp_hdr_recv->icmp_code, i+1);
		printf("RTT: %.2f ms\n", (float)RTT);
		//break;

	}

	//for bubble sort, small to large
	unsigned int sum=0, temp=0;
	for(int j=0; j<10; j++){
		sum+=arr[j];
		for(int k=j+1; k<10; k++){
			if(arr[j]>arr[k]){
				temp = arr[j];
				arr[j] = arr[k];
				arr[k] = temp;
			}
		}
	}

	printf("Max RTT: %.2f ms, Min RTT: %.2f ms, Avg RTT: %.2f ms", (float)arr[9], (float)arr[0], (float)sum/10);
}
