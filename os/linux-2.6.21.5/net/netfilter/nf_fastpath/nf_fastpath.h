#ifndef _NF_FASTPATH__H
#define _NF_FSSTPATH_H

#define NFP_DEBUG          0     /* This constant turns on/off the debug info */

#include <linux/netfilter/nf_conntrack_tuple_common.h>

int nfp_log_enabled(void);

int  nfp_ct_init(void);
void nfp_ct_fini(void);
int  nfp_register_all(void);
void nfp_unregister_all(void);

void nfp_read_lock(void);
void nfp_read_unlock(void);

int __init nfp_proc_init(void);
void __exit nfp_proc_fini(void);


#ifdef CONFIG_NF_FASTPATH_STATS

/* general fast-path statistics */
struct nfp_stats_struct
{
        u32     active_tcp;
        u32     active_udp;
        u32     handled;
        u32     unhandled_l4proto;
        u32     unhandled_no_fp;
        u32     unhandled_wrongmark;
        u32     unhandled_other;
};
DECLARE_PER_CPU(struct nfp_stats_struct, nfp_stats);

#define NFP_STAT_INC(stat)                                      \
        (per_cpu(nfp_stats, raw_smp_processor_id()).stat++)

#define NFP_STAT_DEC(stat)                                      \
        (per_cpu(nfp_stats, raw_smp_processor_id()).stat--)

#define NFP_STAT_GET(stat)                                      \
        ({                                                      \
                int cpu, tot=0;                                 \
                for (cpu=0; cpu<NR_CPUS; cpu++)                 \
                        tot += per_cpu(nfp_stats, cpu).stat;    \
                tot;                                            \
        })

#define NFP_STAT_CLEAR(stat)                                    \
        ({                                                      \
                int cpu;                                        \
                for (cpu=0; cpu<NR_CPUS; cpu++)                 \
                        per_cpu(nfp_stats, cpu).stat = 0;       \
        })

#else /* CONFIG_NF_FASTPATH_STATS */

#define NFP_STAT_INC(stat)
#define NFP_STAT_DEC(stat)
#define NFP_STAT_GET(stat)
#define NFP_STAT_CLEAR(stat)

#endif

#ifdef CONFIG_IP_NF_CT_ACCT

/* per-connection statistics */
struct nfp_stats_ct_struct
{
        u32     handled;
        u32     unhandled;
};

#define NFP_STAT_CT_INC(nfp_bi, stat)                                   \
        (per_cpu_ptr(nfp_bi->stats, raw_smp_processor_id())->stat++)

#define NFP_STAT_CT_GET(nfp_bi, stat)                                   \
        ({                                                              \
                int cpu, tot=0;                                         \
                for (cpu=0; cpu<NR_CPUS; cpu++)                         \
                        tot += per_cpu_ptr(nfp_bi->stats, cpu)->stat;   \
                tot;                                                    \
        })

#define NFP_STAT_CT_CLEAR(nfp_bi, stat)                                 \
        ({                                                              \
                int cpu;                                                \
                for (cpu=0; cpu<NR_CPUS; cpu++)                         \
                        per_cpu_ptr(nfp_bi->stats, cpu)->stat = 0;      \
        })

#else /* CONFIG_IP_NF_CT_ACCT */

#define NFP_STAT_CT_INC(nfp_bi, stat)
#define NFP_STAT_CT_GET(nfp_bi, stat)
#define NFP_STAT_CT_CLEAR(nfp_bi, stat)

#endif


/**
 * struct nfp_struct - fastpath structure 
 * @iif: index to input device structure
 * @output_dev: pointer to output device
 * @saddr: source ip address (to do NAPT) - copied onto all fastpath-handled
 *         packets
 * @dest: dest ip address (to do NAPT) - copied onto all fastpath-handled
 *        packets
 * @sport: source port (to do NAPT) - copied onto all fastpath-handled packets
 * @dport: dest port (to do NAPT) - copied onto all fastpath-handled packets
 * @priority: sk_buff priority field - copied onto all fastpath-handled packets
 * @tos: sk_buff tos field - copied onto all fastpath-handled packets
 * @dst: route entry pointer - copied to all fastpath handled sk_buffs
 * @nat: flag indicating if nat is needed for this packet.
 * @csum_diff: storage of TCP/UDP checksum difference after mangling, to speed
 *             up recalculation of checksum on all handled packets.
 *
 */

struct nfp_struct
{
        int                 iif;
        struct net_device   *output_dev;
        struct dst_entry    *dst;
        __be32              saddr;
        __be32              daddr;
        __u16               sport;
        __u16               dport;
        __u32               mark;
        __u32               priority;
        __u8                tos;
        u32                 nat;
        u32                 csum_diff[4];
};

/**
 * struct nfp_bidir_struct
 * @nfp: array of nfp_structs (2, one for each direction of a connection)
 * @stats: fastpath connection stats.  Usually only enabled for debug.
 *
 */

struct nfp_bidir_struct
{
        struct nfp_struct nfp[IP_CT_DIR_MAX];
#ifdef CONFIG_IP_NF_CT_ACCT
        struct nfp_stats_ct_struct *stats;
#endif
};

#endif /* _NFP_COMMON_H */

