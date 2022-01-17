#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for bzero()
#include <unistd.h>  //for close()
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <netinet/in.h>
#include <string.h>

#include <arpa/inet.h>
#include <linux/ip.h>

int main(){

	struct sockaddr_in addr;
	int fd,r,ip_counter=0,arp_counter=0,rarp_counter=0,tcp_counter=0,udp_counter=0,ICMP_counter=0,IGMP_counter=0; 
	struct in_addr adrsrc,adrdest;
	struct ethhdr *peth;
	struct iphdr *pip;
	char *ptemp;
	struct tcphdr *ptcp;
	struct udphdr *pudp;
	struct ifreq ethreq;
	unsigned char addr1[4]  = {0,0,0,0};
	unsigned char port1[2]  = {0,0};
	char adr2[]="192.168.245.131";
	char *adr3;

	fd=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
	
	strncpy(ethreq.ifr_name,"ens33",IFNAMSIZ);
	if(ioctl(fd,SIOCGIFFLAGS,&ethreq) == -1){	
		perror("ioctl");exit(1);
	}
	ethreq.ifr_flags|=IFF_PROMISC;
	if(ioctl(fd,SIOCGIFFLAGS,&ethreq) == -1){	
		perror("ioctl");exit(3);
	}

	int address_length = sizeof(addr); 

	while(udp_counter<10){	
		char buf[50]={""};
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
	ptemp += sizeof(struct ethhdr);
	pip = (struct ip *)ptemp;
	switch(pip->protocol){
		
		case IPPROTO_UDP:
		ptcp = (struct tcphdr *)ptemp;
		pudp = (struct udphdr *)ptemp;
		memcpy(&adrdest, &pip->daddr, 4);
		//printf("destIP: %s\n",inet_ntoa(adrdest));

		if (!strcmp(adr2, inet_ntoa(adrdest))){
			udp_counter++;
			printf("Source MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", (unsigned)peth->h_source[0], (unsigned)peth->h_source[1], (unsigned)peth->h_source[2], (unsigned)peth->h_source[3], (unsigned)peth->h_source[4], (unsigned)peth->h_source[5]);
			printf("Destination MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", (unsigned)peth->h_dest[0], (unsigned)peth->h_dest[1], (unsigned)peth->h_dest[2], (unsigned)peth->h_dest[3], (unsigned)peth->h_dest[4], (unsigned)peth->h_dest[5]);
			printf("IP->protocol = UDP\n");
			memcpy(&adrsrc, &pip->saddr, 4);
			printf("Src_port = %d\n",ntohs(ptcp->source));	
			printf("Dst_port = %d\n",ntohs(ptcp->dest));
		}
		break;
		
	}
		 

	}
	ethreq.ifr_flags&=~IFF_PROMISC;
	return 0;
}
