#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/notifier.h>
#include <net/ip.h>
#include <linux/tcp.h>
#include <net/route.h>
#include <linux/inetdevice.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include "nf_fastpath.h"

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

//defined in net/netfilter/nf_conntrack_proto_tcp.c, copied here.
extern unsigned int nf_ct_tcp_timeout_syn_sent;
extern unsigned int nf_ct_tcp_timeout_syn_recv;
extern unsigned int nf_ct_tcp_timeout_established;
extern unsigned int nf_ct_tcp_timeout_fin_wait;
extern unsigned int nf_ct_tcp_timeout_close_wait;
extern unsigned int nf_ct_tcp_timeout_last_ack;
extern unsigned int nf_ct_tcp_timeout_time_wait;
extern unsigned int nf_ct_tcp_timeout_close;
extern unsigned int nf_ct_tcp_timeout_max_retrans;

extern unsigned int nf_ct_udp_timeout;
extern unsigned int nf_ct_udp_timeout_stream;

extern unsigned long nfp_ct_mark; /* the mask of bits in CONNMARK value we will
                                   * match against */

/* Notifier chains that we need to register against */
extern struct atomic_notifier_head nf_conntrack_chain;
extern struct atomic_notifier_head ip_tables_modify_chain;
extern struct atomic_notifier_head rt_cache_flush_chain;

static rwlock_t nf_fastpath_lock;

static int (*orig_tcp_print_conntrack)(struct seq_file *,
                                       const struct nf_conn *) = NULL;
static int (*orig_udp_print_conntrack)(struct seq_file *,
                                       const struct nf_conn *) = NULL;

inline void  nfp_read_lock(void)
{
        read_lock(nf_fastpath_lock);
}

inline void nfp_read_unlock(void)
{
        read_unlock(nf_fastpath_lock);
}


static int __nfpcall  nfp_tracker(struct sk_buff **pskb,
                       struct nf_conn *ct,
                       enum ip_conntrack_info ctinfo)
{
        struct sk_buff *skb      = *pskb;
        struct iphdr *iph        = skb->nh.iph;
        struct tcphdr *l4h       = (void *) iph + iph->ihl*4;

        struct nfp_bidir_struct *nfp_bidir;
#ifdef CONFIG_IP_NF_CT_ACCT
        struct nfp_stats_ct_struct *stats;
#endif
        struct nfp_struct *nfp;
        u32 dir = CTINFO2DIR(ctinfo);
        struct rtable *rt = (struct rtable *)skb->dst;
        __u8   protocol = iph->protocol;

        nfp_read_lock();
        /* We only handle TCP and UDP packets */
        if ((protocol != IPPROTO_TCP) && (protocol != IPPROTO_UDP))
                goto no_track;

        /* Determine whether fast-path applies to the packet or not */
        if (!(ct->mark & nfp_ct_mark) && (ct->mark)) {
                goto no_track;
        }
    
        if (rt->rt_flags & RTCF_LOCAL)  /* locally routed traffic */
                goto no_track;

        if (skb->iif == 0)  /* locally sourced */
                goto no_track;
    
        if (!ct)  /* conntrack NULL (shouldn't happen?) */
                goto no_track;

        if ((protocol == IPPROTO_TCP) &&
            ((ct->proto.tcp.state != TCP_CONNTRACK_ESTABLISHED) ||
             (!test_bit(IPS_ASSURED_BIT, &ct->status))) ) {
                goto no_track;
        }
        if (!ct->misc) {
                /* allocate the fast-path structure */
                nfp_bidir = kzalloc(sizeof(struct nfp_bidir_struct), GFP_ATOMIC);
                if (!nfp_bidir) {
                        printk(KERN_ERR "Error allocating fast-path\n");
                        goto no_track;
                }

#ifdef CONFIG_IP_NF_CT_ACCT
                stats = alloc_percpu(struct nfp_stats_ct_struct);
                if (!stats) {
                        kfree(nfp_bidir);
                        printk(KERN_ERR "Error allocating fast-path\n");
                        goto no_track;
                }
                nfp_bidir->stats = stats;
#endif

                ct->misc = (void*)nfp_bidir;

                if (protocol == IPPROTO_TCP)
                        NFP_STAT_INC(active_tcp);
                else if (protocol == IPPROTO_UDP)
                        NFP_STAT_INC(active_udp);
        } else {
                nfp_bidir = (struct nfp_bidir_struct *)ct->misc;
        }

        nfp = &nfp_bidir->nfp[dir];

        /* collect all the information that we need to process subsequent
           packets belonging to the same connection */
        nfp->iif = skb->iif;
        nfp->output_dev = skb->dev;
        nfp->dst = dst_clone(skb->dst);
        nfp->tos = iph->tos;
        nfp->priority = skb->priority;
        nfp->mark = skb->mark ;

        /* check whether NAT is in effect for this flow */
        if ((iph->saddr != ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip) &&
            (iph->saddr != ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip)) {
                /* source NAT is needed */
                nfp->nat = IPS_SRC_NAT;

                /* Save the post routed information to a fast-path structure */
                nfp->saddr = iph->saddr;
                nfp->sport = l4h->source;
                nfp->csum_diff[0] = ~ct->tuplehash[dir].tuple.src.u3.ip;
                nfp->csum_diff[1] = nfp->saddr;
                /* This is a bit hacky, we're going to rely on the fact that TCP
                 * and UDP ports are at the same offset in the tuple struct, so
                 * the following line doesn't require a test of protocol.
                 */
                nfp->csum_diff[2] =
                        ct->tuplehash[dir].tuple.src.u.tcp.port ^ 0xFFFF;
                nfp->csum_diff[3] = nfp->sport;
#if NFP_DEBUG
                printk(KERN_DEBUG "SNAT (%u.%u.%u.%u:%d) --> (%u.%u.%u.%u:%d)\n",
                       NIPQUAD(ct->tuplehash[dir].tuple.src.u3.ip),
                       ct->tuplehash[dir].tuple.src.u.tcp.port,
                       NIPQUAD(nfp->saddr),
                       nfp->sport);
#endif
        } else if ((iph->daddr !=
                    ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip) &&
                   (iph->daddr != ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip)) {
                /* dest NAT is needed */
                nfp->nat = IPS_DST_NAT;

                /* Save the post routed information to a fast-path structure */
                nfp->daddr = iph->daddr;
                nfp->dport = l4h->dest;
                nfp->csum_diff[0] = ~ct->tuplehash[dir].tuple.dst.u3.ip;
                nfp->csum_diff[1] = nfp->daddr;
                nfp->csum_diff[2] =
                        ct->tuplehash[dir].tuple.dst.u.tcp.port ^ 0xFFFF;
                nfp->csum_diff[3] = nfp->dport;
#if NFP_DEBUG
                printk(KERN_DEBUG "DNAT (%u.%u.%u.%u:%d) --> (%u.%u.%u.%u:%d)\n",
                       NIPQUAD(ct->tuplehash[dir].tuple.dst.u3.ip),
                       ct->tuplehash[dir].tuple.dst.u.tcp.port,
                       NIPQUAD(nfp->daddr),
                       nfp->dport);
#endif
        }

        if (nfp_log_enabled()) {
                printk(KERN_INFO "\n%s (ifindex %d):\n"
                       "%d (%u.%u.%u.%u:%d) -> %s (%u.%u.%u.%u:%d)\n"
                       "Original: (%u.%u.%u.%u:%d) -> (%u.%u.%u.%u:%d)\n"
                       "Reply   : (%u.%u.%u.%u:%d) -> (%u.%u.%u.%u:%d)\n",
                       __FUNCTION__, nfp->iif,
                       skb->iif,
                       NIPQUAD(iph->saddr), l4h->source,
                       skb->dev->name,
                       NIPQUAD(iph->daddr), l4h->dest,
                       NIPQUAD(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip),
                       ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.tcp.port,
                       NIPQUAD(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip),
                       ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.tcp.port,
                       NIPQUAD(ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip),
                       ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u.tcp.port,
                       NIPQUAD(ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip),
                       ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.tcp.port);
        }
    
no_track:
        nfp_read_unlock();
        return NF_ACCEPT;

}

static int nfp_print_conntrack(struct seq_file *s,
                               const struct nf_conn *conntrack)
{
        u_int8_t proto
                = conntrack->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum;


        if (conntrack->misc) {
#ifdef CONFIG_IP_NF_CT_ACCT
                struct nfp_bidir_struct *nfp_bidir
                        = (struct nfp_bidir_struct *)conntrack->misc;
                if (seq_printf(s, "[FAST_PATH (handled=%d unhandled=%d)] ",
                               NFP_STAT_CT_GET(nfp_bidir, handled),
                               NFP_STAT_CT_GET(nfp_bidir, unhandled)))
                        return -ENOSPC;
#else
                if (seq_printf(s, "[FAST_PATH] "))
                        return -ENOSPC;
#endif
        }
        /* execute the original print_conntrack function */
        if (proto == IPPROTO_TCP)
                return (*orig_tcp_print_conntrack)(s, conntrack);
        else if (proto == IPPROTO_UDP)
                return (*orig_udp_print_conntrack)(s, conntrack);
        else
                return 0;
}

void nfp_free_fastpath(struct nf_conn *ct)
{
        struct nfp_bidir_struct *nfp_bidir
                = (struct nfp_bidir_struct *)ct->misc;
        u_int8_t proto = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum;

        if (nfp_bidir) {
#ifdef CONFIG_IP_NF_CT_ACCT
                free_percpu(nfp_bidir->stats);
#endif
		if(nfp_bidir->dst)
			dst_release(nfp_bidir->dst);
                kfree(nfp_bidir);
                ct->misc = NULL;
                ct->mark = 0;

                if (proto == IPPROTO_TCP)
                        NFP_STAT_DEC(active_tcp);
                else if (proto == IPPROTO_UDP)
                        NFP_STAT_DEC(active_udp);
        }
}

static int nfp_conntrack_notifier_call(struct notifier_block *this,
                                     unsigned long events, void *ptr)
{
        struct nf_conn *ct = (struct nf_conn *)ptr;
        if (events & IPCT_DESTROY) {
                nfp_free_fastpath(ct);
        }

        return NOTIFY_DONE;
}

/* iterate through each conntrack entry */
#define FOREACH_TUPLE_HASH(bucket, h)                                   \
        for ((bucket) = 0; (bucket) < nf_conntrack_htable_size; (bucket)++) \
                list_for_each_entry((h), &nf_conntrack_hash[bucket], list)

static int nfp_clear_fastpaths(struct notifier_block *this,
                               unsigned long events, void *ptr)
{
        int bucket;
        struct nf_conntrack_tuple_hash *h;
        struct nf_conn *ct;
        
        /* iterate through each conntrack entry and free the fast-path */
        /* grab writelock lock of ip_conntrack, which is protecting our fastpath
         * structures. Note we write_lock_irqsave, because kfree will be called.
         * If we don't disable interrupts, we can get a context switch with the
         * lock held, which is BAD.
         */
        write_lock_bh(&nf_fastpath_lock);
        FOREACH_TUPLE_HASH(bucket, h) {
                ct = nf_ct_tuplehash_to_ctrack(h);
                nfp_free_fastpath(ct);
        }

        write_unlock_bh(&nf_fastpath_lock);
        return NOTIFY_DONE;
}

static void nfp_clear_fastpaths_dev(struct net_device *dev)
{
        int bucket;
        struct nf_conntrack_tuple_hash *h;
        struct nf_conn *ct;
        struct nfp_bidir_struct *nfp_bidir;
        
        /* iterate through each conntrack entry and free fast-paths with 'dev' as
         * the input or output device.  Same locking requirements as above in
         * nfp_clear_fastpaths_dev().
         */
        write_lock_bh(&nf_fastpath_lock);
        FOREACH_TUPLE_HASH(bucket, h) { 
                ct = nf_ct_tuplehash_to_ctrack(h);
                nfp_bidir = (struct nfp_bidir_struct *)ct->misc;
                
                if (nfp_bidir &&
                    ((dev->ifindex == nfp_bidir->nfp[IP_CT_DIR_ORIGINAL].iif) ||
                     (dev == nfp_bidir->nfp[IP_CT_DIR_ORIGINAL].output_dev) ||
                     (dev->ifindex == nfp_bidir->nfp[IP_CT_DIR_REPLY].iif) ||
                     (dev == nfp_bidir->nfp[IP_CT_DIR_REPLY].output_dev))) {
                        nfp_free_fastpath(ct);
                }
        }
        write_unlock_bh(&nf_fastpath_lock);
}

/* "Device down" notifier callback. callback needs to free all fast-paths using
 * the device.
 */
static int nfp_device_event(struct notifier_block *this,
                            unsigned long event, void *ptr)
{
        struct net_device *dev = ptr;

        if (event == NETDEV_DOWN) {
                /* Device was downed. Free any fast-paths using this device. */
                nfp_clear_fastpaths_dev(dev);
        }

        return NOTIFY_DONE;
}

/* "IP address changed" notifier callback. callback needs to free all fast-paths
 * using the device.
 */
static int nfp_inet_event(struct notifier_block *this,
                          unsigned long event, void *ptr)
{
        struct net_device *dev = ((struct in_ifaddr *)ptr)->ifa_dev->dev;

        if (dev && (event == NETDEV_DOWN)) {
                /* IP address was changed. Free any fast-paths using this device. */
                nfp_clear_fastpaths_dev(dev);
        }

        return NOTIFY_DONE;
}

static struct notifier_block nfp_conntrack_notifier = {
        .notifier_call = nfp_conntrack_notifier_call
};
static struct notifier_block nfp_ip_tables_modify_notifier = {
        .notifier_call = nfp_clear_fastpaths
};
static struct notifier_block nfp_dev_notifier = {
        .notifier_call  = nfp_device_event
};
static struct notifier_block nfp_inet_notifier = {
        .notifier_call  = nfp_inet_event
};
static struct notifier_block nfp_rt_cache_flush_notifier = {
        .notifier_call  = nfp_clear_fastpaths
};


static unsigned int nfp_nf_hook(unsigned int hook, struct sk_buff **pskb,
                                         const struct net_device *indev,
                                         const struct net_device *outdev,
                                         int (*okfn)(struct sk_buff *))
{
        struct nf_conn *ct;
        enum ip_conntrack_info ctinfo;

        ct = nf_ct_get(*pskb, &ctinfo);
        if (ct && (ctinfo != (IP_CT_RELATED + IP_CT_IS_REPLY)))
                return nfp_tracker(pskb, ct, ctinfo);

        return NF_ACCEPT;
}

static struct nf_hook_ops nfp_ops[] = {
        {
                .hook     = nfp_nf_hook,
                .owner    = THIS_MODULE,
                .pf       = PF_INET,
                .hooknum  = NF_IP_POST_ROUTING,
                .priority = NF_IP_PRI_LAST
        }
};


/* Cleanup function on module exit */
void nfp_ct_fini(void)
{
        nf_conntrack_l4proto_tcp4.print_conntrack = orig_tcp_print_conntrack;
        nf_conntrack_l4proto_udp4.print_conntrack = orig_udp_print_conntrack;

        nf_unregister_hooks(nfp_ops, ARRAY_SIZE(nfp_ops));

        atomic_notifier_chain_unregister(&nf_conntrack_chain,
                                         &nfp_conntrack_notifier);
        atomic_notifier_chain_unregister(&ip_tables_modify_chain,
                                         &nfp_ip_tables_modify_notifier);
        unregister_netdevice_notifier(&nfp_dev_notifier);
        unregister_inetaddr_notifier(&nfp_inet_notifier);
        atomic_notifier_chain_unregister(&rt_cache_flush_chain,
                                         &nfp_rt_cache_flush_notifier);


        /* Now that our helper is unregistered, we can clear all existing fast
         * paths.
         */
        nfp_clear_fastpaths(NULL, 0, NULL);
}

int nfp_ct_init(void)
{
        /* Initialize our fastpath lock */
	rwlock_init(&nf_fastpath_lock);
        
        /* Hook tracker into NetFilter POSTROUTING */
        if (nf_register_hooks(nfp_ops, ARRAY_SIZE(nfp_ops)) < 0)
                return -EINVAL;

        orig_tcp_print_conntrack = nf_conntrack_l4proto_tcp4.print_conntrack;
        nf_conntrack_l4proto_tcp4.print_conntrack = nfp_print_conntrack;

        orig_udp_print_conntrack = nf_conntrack_l4proto_udp4.print_conntrack;
        nf_conntrack_l4proto_udp4.print_conntrack = nfp_print_conntrack;

        /* Register a notifier to the connection tracking module */
        if (atomic_notifier_chain_register(&nf_conntrack_chain,
                                           &nfp_conntrack_notifier) < 0)
                return -EINVAL;

        /* Register for iptables modification reports */
        if (atomic_notifier_chain_register(&ip_tables_modify_chain,
                                           &nfp_ip_tables_modify_notifier) < 0)
                return -EINVAL;

        /* Register for device down reports */
        if (register_netdevice_notifier(&nfp_dev_notifier) < 0)
                return -EINVAL;

        /* Register IP address change reports */
        if (register_inetaddr_notifier(&nfp_inet_notifier) < 0)
                return -EINVAL;

        /* Register for FIB modification reports */
        if (atomic_notifier_chain_register(&rt_cache_flush_chain,
                                           &nfp_rt_cache_flush_notifier) < 0)
                return -EINVAL;
    
        return 0;
}
