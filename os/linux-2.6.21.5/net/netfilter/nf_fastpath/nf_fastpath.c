#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/ip.h>
#include <linux/if_vlan.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include "nf_fastpath.h"

#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Atheros"); 
MODULE_DESCRIPTION("IPv4 NF Fastpath");

extern unsigned long nfp_ct_mark;

extern void netdev_register_netif_receive_fastpath (int (*fp)(struct sk_buff *skb));
extern void netdev_unregister_netif_receive_fastpath(void);

extern unsigned int nf_ct_udp_timeout_stream;
extern unsigned int nf_ct_tcp_timeout_max_retrans;
extern unsigned int nf_ct_tcp_timeout_established;
extern int nf_ct_tcp_max_retrans;

extern struct nf_conntrack_l3proto nf_conntrack_l3proto_ipv4;
extern struct nf_conntrack_l4proto nf_conntrack_l4proto_tcp4;
extern struct nf_conntrack_l4proto nf_conntrack_l4proto_udp4;


#define IPV4_IHL_MIN 5  // Minimum IPV4 header length. should be defined by Linux.


extern int ip_finish_output(struct sk_buff *skb);

/* What TCP flags are set from RST/SYN/FIN/ACK, copy from nf_conntrack_proto_tcp.c */
enum tcp_bit_set {
	TCP_SYN_SET,
	TCP_SYNACK_SET,
	TCP_FIN_SET,
	TCP_ACK_SET,
	TCP_RST_SET,
	TCP_NONE_SET,
};


#ifdef CONFIG_NF_FASTPATH_STATS
DEFINE_PER_CPU(struct nfp_stats_struct, nfp_stats);
#endif


/*
 * nfp_chk_packet - basic sanity checks of the packet
 * @skb: pointer to packet
 *
 * Runs some basic checks on this packet.  Note these are copied from
 * netif_receive_skb() and ip_rcv() and need to be maintained in future kernel
 * versions.
 *
 */
inline int nfp_chk_packet(struct sk_buff *skb)
{
        struct iphdr *iph;
        int len;
        

        /*************************************************************/
        /* Some basic checks and setup COPIED from netif_receive_skb */
        if (unlikely(!skb->iif))
                skb->iif = skb->dev->ifindex;

        skb->h.raw = skb->nh.raw = skb->data;
        skb->mac_len = skb->nh.raw - skb->mac.raw;

        /* end of netif_receive_skb copied section */
        /*****************************************************/


        /*************************************************************/
        /* some basic checks and setup COPIED from ip_rcv */

        /*
         *	RFC1122: 3.1.2.2 MUST silently discard any IP frame that fails
         *	the checksum.
         *
         *	Is the datagram acceptable?
         *
         *	1. Length at least the size of an ip header
         *	2. Version of 4
         *	3. Checksums correctly. [Speed optimisation for later,
         *	   skip loopback checksums]
         *	4. Doesn't have a bogus length
         */
        if (unlikely((skb = skb_share_check(skb, GFP_ATOMIC)) == NULL)) {
                IP_INC_STATS_BH(IPSTATS_MIB_INDISCARDS);
                NFP_STAT_INC(unhandled_other);
                return 1;
        }
        if (unlikely(!pskb_may_pull(skb, IPV4_IHL_MIN*4))) {
                NFP_STAT_INC(unhandled_other);
                return 1;
        }

        iph =  skb->nh.iph;

        /* We only handle 'normal' ip headers, i.e. no options.  This check also
         * covers invalid (too small) values of iph
         */
        if (unlikely(iph->ihl != IPV4_IHL_MIN))
        {
                NFP_STAT_INC(unhandled_other);
                return 1;
        }
                
        if (unlikely(ip_fast_csum((u8 *)iph, iph->ihl))) {
                NFP_STAT_INC(unhandled_other);
                return 1;
        }
        
        len = ntohs(iph->tot_len);
        if (unlikely((skb->len < len) || (len < (iph->ihl*4)))) {
                NFP_STAT_INC(unhandled_other);
                return 1;
        }
        /* end of ip_rcv copied section */
        /*****************************************************/

        if (unlikely(iph->ttl <= 1)) {
                NFP_STAT_INC(unhandled_other);
                return 1;
        }
    
    
        /* We do NOT handle fragments!! Pass them back for Linux to deal with */
        if (unlikely(iph->frag_off & htons(IP_MF|IP_OFFSET))) {
                NFP_STAT_INC(unhandled_other);
                return 1;
        }


        return 0;
}


inline struct nf_conn *nfp_find_conntrack(struct sk_buff *skb,
                                             struct iphdr *iph,
                                             struct nf_conntrack_l4proto *proto,
                                             u32 *dir)
{
        struct nf_conntrack_tuple tuple;
        struct nf_conntrack_tuple_hash *tuple_hash_p;
        if (unlikely(!nf_ct_get_tuple(skb, 
                                      (unsigned int)(skb->nh.raw - skb->data),
                                       skb->nh.raw - skb->data + skb->nh.iph->ihl*4,
                                       PF_INET,
                                       skb->nh.iph->protocol,
                                       &tuple,
                                       &nf_conntrack_l3proto_ipv4,
                                       proto)))
        {
                NFP_STAT_INC(unhandled_other);
                return NULL;
        }

        tuple_hash_p = nf_conntrack_find_get(&tuple, NULL);
        if(unlikely(NULL == tuple_hash_p))
        {
                NFP_STAT_INC(unhandled_no_fp);
                return NULL;
        }

        *dir = NF_CT_DIRECTION(tuple_hash_p);

        return (nf_ct_tuplehash_to_ctrack(tuple_hash_p));
}

/**
 * nfp_update_protocol_state - update TCP/UDP conntrack state.
 * @skb: pointer to packet.
 * @ct: pointer to conntrack for this connection
 * @iph: pointer to header in packet
 * @tcph: pointer to tcp header in packet
 * @dir: direction of the packet in the connection
 * 
 */
inline int nfp_update_protocol_state(struct sk_buff *skb,
                                     struct nf_conn *ct,
                                     struct iphdr *iph,
                                     struct tcphdr *tcph, u32 dir)
{
        unsigned long timeout;
        unsigned int index;
    
        if (likely((iph->protocol == IPPROTO_TCP))) 
        {
                if (unlikely(ct->proto.tcp.state !=
                             TCP_CONNTRACK_ESTABLISHED)) {
                        return 1;
                }

                {
                        if (tcph->rst) index = TCP_RST_SET;
	                else if (tcph->syn) index = (tcph->ack ? TCP_SYNACK_SET : TCP_SYN_SET);
	                else if (tcph->fin) index = TCP_FIN_SET;
	                else if (tcph->ack) index = TCP_ACK_SET;
	                else index = TCP_NONE_SET;
                }				//copy from get_conntrack_index(), which is defined as static function.

                if (unlikely(index != TCP_ACK_SET))
                        return 1;
        
                ct->proto.tcp.last_index = index;               
                timeout = ((ct->proto.tcp.retrans >= nf_ct_tcp_timeout_max_retrans) ?
                           nf_ct_tcp_timeout_max_retrans :
                           nf_ct_tcp_timeout_established);

                nf_conntrack_event_cache(IPCT_PROTOINFO_VOLATILE, skb);
        } 
        else
        {
                timeout = nf_ct_udp_timeout_stream;
        }

        if (unlikely(dir == IP_CT_DIR_ORIGINAL))
                nf_ct_refresh_acct(ct, IP_CT_ESTABLISHED, skb, timeout);
        else
                nf_ct_refresh_acct(ct, IP_CT_IS_REPLY, skb, timeout);
    
        return 0;        
}

int _nfp_forward_packet(struct sk_buff *skb)
{
        struct iphdr *iph;
        struct nfp_bidir_struct *nfp_bidir;
        struct nfp_struct *nfp;
        u32 dir;
        struct tcphdr *tcph;
        static struct nf_conntrack_l4proto *proto;
        struct nf_conn *ct;
        __u16  *cksum_ptr;

        if(nfp_chk_packet(skb))
                return 1;
    
        iph = skb->nh.iph;
        tcph = (void *)iph + IPV4_IHL_MIN*4;

        switch(iph->protocol)
        {
        case IPPROTO_TCP:
                proto = &nf_conntrack_l4proto_tcp4;
                cksum_ptr = &(tcph->check);
                break;
        case IPPROTO_UDP:
                proto = &nf_conntrack_l4proto_udp4;
                cksum_ptr = &(((struct udphdr *)tcph)->check);
                break;
        default:
                NFP_STAT_INC(unhandled_l4proto);
                return 1;
        }

        if(unlikely((ct = nfp_find_conntrack(skb, iph, proto, &dir)) == NULL))
                return 1;

        nfp_read_lock();

        if(unlikely(!(ct->mark & nfp_ct_mark) && (ct->mark)))			//force to go through Linux stack
        {
                NFP_STAT_INC(unhandled_wrongmark);
                goto nohandle;
        }

        nfp_bidir = (struct nfp_bidir_struct *)ct->misc;

        if(likely(nfp_bidir))
        {
                nfp = &nfp_bidir->nfp[dir];
                if (unlikely(nfp->iif == 0)) 			//local generated
                {
                        NFP_STAT_INC(unhandled_no_fp);
                        goto nohandle;
                }
        }
        else							//fast path does not constructed yet.
        {
                NFP_STAT_INC(unhandled_no_fp);
                goto nohandle;
        }

        if (unlikely (nfp_update_protocol_state(skb, ct, iph, tcph, dir) != 0))
        {
                NFP_STAT_CT_INC(nfp_bidir, unhandled);
                goto nohandle;
        }

        /* update the IP header and TCP/UDPports */
        if (likely(nfp->nat)) {
                switch (nfp->nat)
                {
                case (IPS_SRC_NAT):
                        iph->saddr  = nfp->saddr;
                        tcph->source = nfp->sport;
                        break;
                case (IPS_DST_NAT):
                        iph->daddr = nfp->daddr;
                        tcph->dest  = nfp->dport;
                        break;
                default:
                        break;
                }
                /* adjust the TCP/UDP checksum */
                *cksum_ptr = csum_fold(csum_partial((char *) nfp->csum_diff,
                                                    sizeof(nfp->csum_diff),
                                                    *cksum_ptr ^ 0xFFFF));
        }

        skb->nfct = &ct->ct_general;
        ip_decrease_ttl(iph);
        skb->dev   = nfp->output_dev;
        skb->dst   = dst_clone(nfp->dst);
        iph->tos   = nfp->tos;
        skb->priority = nfp->priority;
        skb->mark = nfp->mark;

        nfp_read_unlock();

        ip_send_check(iph);

        IP_INC_STATS_BH(IPSTATS_MIB_OUTFORWDATAGRAMS);
        IP_INC_STATS_BH(IPSTATS_MIB_OUTREQUESTS);

        NFP_STAT_CT_INC(nfp_bidir, handled);
        NFP_STAT_INC(handled);

        ip_finish_output(skb);

        return 0;

nohandle:
        nfp_read_unlock();
        atomic_dec(&ct->ct_general.use);
        return 1;
}

int nfp_forward_packet(struct sk_buff *skb)
{
        if(likely(ETH_P_IP == skb->protocol))
        {
               return  _nfp_forward_packet(skb);
        }
        else if(likely(ETH_P_IPV6 == skb->protocol))
        {
                return 1;			//ipv6 handling here!
        }
        return 1;				//unhandled by fastpath
}

void nfp_register_netdev_receive_fastpath(void)
{
        netdev_register_netif_receive_fastpath(nfp_forward_packet);
}

void nfp_unregister_netdev_receive_fastpath(void)
{
        netdev_unregister_netif_receive_fastpath();
}

int nfp_register_all(void)
{
        nfp_register_netdev_receive_fastpath();
    
        return nfp_ct_init();
    
}

void nfp_unregister_all(void)
{
        nfp_unregister_netdev_receive_fastpath();
        nfp_ct_fini();

}

static void __exit nfp_fini(void)
{
        nfp_unregister_all();
        nfp_proc_fini();
}

static int __init nfp_init(void)
{
        if (nfp_register_all() != 0) {
                printk(KERN_ERR "Error registering fastpath\n");
                /* Try and unregister */
                nfp_unregister_all();
                return -1;
        }
        nfp_proc_init();
        return 0;
}

module_init(nfp_init);
module_exit(nfp_fini);

