#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>
#include <asm/io.h>
#include <configs/opipmux.h>
#define ETH_ALEN	6		/* Octets in one ethernet addr	 */

void proc_arp(unsigned char *pck,int len);
void debug_pck_Rcv_force(unsigned char *pck,int len,unsigned short bd_id);
void debug_pck_Rcv(unsigned char *pck,int len,unsigned short bd_id);
void debug_pck_send(unsigned char *pck,int len,unsigned short bd_id);
void debug_pck_Rcv(unsigned char *pck,int len,unsigned short bd_id);
void debug_pck_display_statistic(void);
void debug_pck_clear_statistic(void);
void debug_pck(int rx_or_tx,unsigned char *pck,int len,unsigned short bd_id);
void debug_pck_send(unsigned char *pck,int len,unsigned short bd_id);
extern void print_mem(char *p,UINT32 len);
u16 debug_rx =0 ; /*0:no debug; 1:print tcp,icmp 2:print simple info; 3:print mem of all packet header; 4:tftpb */
u16 debug_tx =0 ;
struct icmphdr_WX {
 	 unsigned char	type;
	 unsigned char 	code;
	 unsigned short	checksum;
	 unsigned short	id;
	 unsigned short	sequence;
};

struct iphdr {
	char verhdrlen;
	char service;
	unsigned short len;
	unsigned short ident;
	unsigned short frags;
	char ttl;
	char protocol;
	unsigned short chksum;
	int src;
	int dest;
};

struct arphdr
{
	unsigned short	ar_hrd;		/* format of hardware address	*/
	unsigned short	ar_pro;		/* format of protocol address	*/
	unsigned char	ar_hln;		/* length of hardware address	*/
	unsigned char	ar_pln;		/* length of protocol address	*/
	unsigned short	ar_op;		/* ARP opcode (command)		*/
	 /*
	  *	 Ethernet looks like this : This bit is variable sized however...
	  */
#if 0
	unsigned char		ar_sha[ETH_ALEN];	/* sender hardware address	*/
	unsigned char		ar_sip[4];		/* sender IP address		*/
	unsigned char		ar_tha[ETH_ALEN];	/* target hardware address	*/
	unsigned char		ar_tip[4];		/* target IP address		*/
#endif
};

struct ethhdr
{
	unsigned char	h_dest[ETH_ALEN];	/* destination eth addr	*/
	unsigned char	h_source[ETH_ALEN];	/* source ether addr	*/
	unsigned short	h_proto;		/* packet type ID field	*/
} ;

void proc_arp(unsigned char *pck,int len)
{

	struct ethhdr *eh;
	//struct ethhdr *ah;
	eh=(struct ethhdr*)pck;

	if ((eh->h_proto==0x0806) || (eh->h_proto==0x0608))
	{
		printf("get arp p=%x",eh->h_proto);	
	}
}

void debug_pck_Rcv_force(unsigned char *pck,int len,unsigned short bd_id)
{
	int itmp=debug_rx;

	debug_rx=3;
	debug_pck_Rcv(pck,len,bd_id);
	//print_mem(pck,len);
	debug_rx=itmp;
}

void debug_pck_send(unsigned char *pck,int len,unsigned short bd_id)
{

	debug_pck(0,pck,len,bd_id);
}

void debug_pck_Rcv(unsigned char *pck,int len,unsigned short bd_id)
{

	debug_pck(1,pck,len,bd_id);
}

void debug_pck_display_statistic(void)
{

	debug_pck(2,NULL,0,0);
}

void debug_pck_clear_statistic(void)
{
	debug_pck(3,NULL,0,0);
}


void debug_pck(int rx_or_tx,unsigned char *pck,int len,unsigned short bd_id)
{
	static char strsect[10]="";
	static u32 ip_count[2]={0,0};
	static u32 icmp_count[2]={0,0};
	static u32 arp_count[2]={0,0};
	static u32 boardcast_count[2]={0,0};
	static u32 total_count[2]={0,0};
	int i;
	u16 debug_level=0; 
	struct ethhdr *eh;
	//struct arphdr *ah;
	struct icmphdr_WX *icmph;
	struct iphdr *ih;

	switch (rx_or_tx)	{
		case 3:
			for (i=0;i<2;i++)
			{
				ip_count[0]=0;
				arp_count[0]=0;
				boardcast_count[0]=0;
				icmp_count[0]=0;
				total_count[0]=0;
			}
			return;
			break;
		case 2:

			for (i=0;i<2;i++)
			{
				if (i==1) printf("RX statistic:\n ");
				if (i==0) printf("TX statistic:\n ");
				
				printf("	total = %6u (broadcast =%6u)\n", total_count[i],boardcast_count[i]);
				printf("	   ip = %6u (icmp      =%6u)\n", ip_count[i], icmp_count[i]);
				printf("	   arp= %6u\n", arp_count[i]);
			}
			return;
			break;
		case 1:
			debug_level=debug_rx;
			break;
		case 0:
			debug_level=debug_tx;
			break;			
	}

	if (debug_level ==0) return;
	sprintf(strsect,"%s%4u ", rx_or_tx?"< ":" >",total_count[rx_or_tx]);

	total_count[rx_or_tx]++;
	eh=(struct ethhdr*)pck;
	
	if (3<=debug_level) printf("%s@ : eh->h_proto=%4x, bd_id=%x, pckaddr=0x%08x, len=%d\n",strsect,eh->h_proto,bd_id,pck,len);

	if ((0xff ==pck[0]) && (0xff ==pck[1]) && (0xff ==pck[2]) \
		&& (0xff ==pck[3]) && (0xff ==pck[4]) && (0xff ==pck[5]))
	{
		boardcast_count[rx_or_tx]++;
	}

	switch  (eh->h_proto)
		{
		case 0x08:
		ip_count[rx_or_tx]=ip_count[rx_or_tx]++;
		ih=(struct iphdr*)(pck+14);
		
		if (ih->protocol==0x1)
			{
				icmp_count[rx_or_tx]++;
				icmph=(struct icmphdr*)(pck+14+20);

				if (2<=debug_level) printf("%sip len %4d,icmp tp 0x%x seq=%5u bdi=0x%x\n",strsect,htons(ih->len),*(char *)icmph,icmph->sequence,bd_id);

			}
			else
			{
				//if (4==debug_level) print_mem(pck,len);

				if (5==debug_level) 
				{
					// udp 
					if(ih->protocol==17)
					{
						//print_mem((char *)ih+20+8,4);
						//udelay(1000000);
					}
					
				}

			}

		break;
	case 0x0806:
	case 0x0608:
		if (2<=debug_level)  printf("%sarp %x \n",strsect,arp_count[rx_or_tx]);	
		++arp_count[rx_or_tx];
		break;
	default:
	}

	if (4==debug_level) 
	{
		print_mem((char *)pck,14+20+8);
	}
}

