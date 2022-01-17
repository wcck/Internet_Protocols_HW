#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<net/if.h>
#include<sys/ioctl.h>//ioctl
#include<arpa/inet.h>
#include<string.h>
#include<netinet/in.h>
#include<unistd.h>
#include<linux/if_ether.h>
#include<linux/tcp.h>
#include<linux/udp.h>
#include<linux/ip.h>

struct ether_header{
	unsigned char ether_dhost[ETH_ALEN];
	unsigned char ether_shost[ETH_ALEN];
	unsigned short ether_type;
};

int main(int argc, char *argv[]){

	struct sockaddr_in addr;
	int fd,r=0,ip_counter=0,arp_counter=0,rarp_counter=0,tcp_counter=0,udp_counter=0,ICMP_counter=0,IGMP_counter=0; 

	struct ethhdr *peth;
	struct iphdr *pip;
	char *ptemp;
	struct tcphdr *ptcp;
	struct udphdr *pudp;
	struct ifreq ethreq;
	fd=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

	strncpy(ethreq.ifr_name, "ens33", IFNAMSIZ);
	
	if(ioctl(fd,SIOCGIFFLAGS,&ethreq) == -1){	
		perror("ioctl");
		exit(1);
	}

	ethreq.ifr_flags|=IFF_PROMISC;
	if(ioctl(fd,SIOCGIFFLAGS,&ethreq) == -1){	
		perror("ioctl");
		exit(3);
	}

	int address_length = sizeof(addr); 

	for(int i=0;i<100;i++){	
		char buf[1024]={};
		r = recvfrom(fd, (char *)buf, sizeof(buf), 0, (struct sockaddr *)&addr, (socklen_t*)&address_length);
		ptemp = buf;
		peth = (struct ethhdr *)ptemp; //ethernet header
		
		switch(ntohs(peth->h_proto)){

			case 0x0800:
				ip_counter++;
				break;
			case 0x0806:
				arp_counter++;
				break;
			case 0x8035:
				rarp_counter++;
				break;
		}

		ptemp += sizeof(struct ether_header);
		pip = (struct ip *)ptemp;

		switch(pip->protocol){

			case IPPROTO_TCP:
				//ptcp = (struct tcphdr *)ptemp;
				tcp_counter++;
				break;
			 case IPPROTO_UDP:
				//pudp = (struct udphdr *)ptemp;
				udp_counter++;
				break;
			case IPPROTO_ICMP:
				ICMP_counter++;
				break;
			case IPPROTO_IGMP:
				IGMP_counter++;
				break;
		}
	}

	printf("--------------statistics---------------\n");
		printf("IP:        %d\nARP:        %d\nRARP:       %d\nTCP:       %d\nUDP:       %d\nICMP:      %d\nIGMP:      %d \n ",ip_counter,arp_counter,rarp_counter,tcp_counter,udp_counter,ICMP_counter,IGMP_counter);
	printf("---------------finish------------\n");

	ethreq.ifr_flags&=~IFF_PROMISC;
	return 0;
}
