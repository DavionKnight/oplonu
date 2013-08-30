#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/fcntl.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/ethtool.h>
#include <linux/vmalloc.h>/* ioremap, iounremap */
#include <linux/delay.h>
#include <linux/dma-mapping.h>

#include <asm/uaccess.h>
#include <asm/bitops.h>
#include <asm/io.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/inetdevice.h>
#include <linux/platform_device.h>

#ifdef CONFIG_ARCH_ONU
#include <asm/mach-onu/onu_reg.h>
#include <asm/mach-onu/onu_irq.h>
#endif

#ifdef CONFIG_ARCH_ONU2
#include <asm/mach-onu2/onu2_reg.h>
#include <asm/mach-onu2/onu2_irq.h>
#endif

#define WORKQ_MOD   (1)
#ifdef WORKQ_MOD
#include <linux/workqueue.h> 
#endif

#define MODULENAME	"onu_fe_ether"
#define MODULEVERSION	"1.00"

static unsigned int core_debug = 0x0;
module_param(core_debug, int, 0644);
MODULE_PARM_DESC(core_debug,"enable debug messages");

#define DEBUG_COMMON 0x80
#define DEBUG_MEM_TX 0x20
#define DEBUG_MEM_RX 0x10
#define DEBUG_USER_TX 0x8
#define DEBUG_USER_RX 0x4
#define DEBUG_TX 0x2
#define DEBUG_RX 0x1

#define dprintk(fmt...)    if (DEBUG_COMMON & core_debug) \
    {printk("[%-15s,%4d]", __FUNCTION__, __LINE__);printk(fmt);}
#define dprintk_rx(fmt...)    if (DEBUG_RX & core_debug) \
    {printk("[%-15s,%4d]", __FUNCTION__, __LINE__);printk(fmt);}
#define dprintk_tx(fmt...)    if (DEBUG_TX & core_debug) \
    {printk("[%-15s,%4d]", __FUNCTION__, __LINE__);printk(fmt);}
#define dprintk_urx(fmt...)    if (DEBUG_USER_RX & core_debug) \
    {printk("[%-15s,%4d]", __FUNCTION__, __LINE__);printk(fmt);}
#define dprintk_utx(fmt...)    if (DEBUG_USER_TX & core_debug) \
    {printk("[%-15s,%4d]", __FUNCTION__, __LINE__);printk(fmt);}

//#define INTERFACE_ONE_WAN_ONE_LAN
//#define INTERFACE_TWO_WAN_TWO_LAN
#define INTERFACE_THREE_WAN_THREE_LAN

#if defined(INTERFACE_THREE_WAN_THREE_LAN)
#define MAX_INTETFACE_NUM  6
struct net_device *onundevs[MAX_INTETFACE_NUM] = {NULL,NULL,NULL,NULL,NULL,NULL};
#elif defined(INTERFACE_TWO_WAN_TWO_LAN)
#define MAX_INTETFACE_NUM  4
struct net_device *onundevs[MAX_INTETFACE_NUM] = {NULL,NULL,NULL,NULL};
#elif defined(INTERFACE_ONE_WAN_ONE_LAN)
#define MAX_INTETFACE_NUM  2
struct net_device *onundevs[MAX_INTETFACE_NUM] = {NULL,NULL};
#endif

#define MAX_RX_DESC 64
#define MAX_TX_DESC 64
#define TX_START_SHIFT 64
#define MAX_RXBUF_SIZE 0x800
#define MAX_TXBUF_SIZE 0x800 
#define MAX_WAIT_CNT 0x2000
#define ETH0_INTERFACE_ID 0
#define MACEQUAL(src,dst)  ((((u32 *)(src))[0] == ((u32 *)(dst))[0]) && \
                           (((u16 *)(src))[2] == ((u16 *)(dst))[2]))
#define MEMCPY2B(dst,src)  \
{  \
  *((u16 *)(dst)) = *((u16 *)(src)); \
}

#define MEMCPY4B(dst,src)  \
{  \
  *((u32 *)(dst)) = *((u32 *)(src)); \
}

#define MEMCPY6B(dst,src)  \
{  \
  MEMCPY4B(dst,src); \
  ((u16 *)(dst))[2] = ((u16 *)(src))[2];  \
}

#define MAX_DMA1_INTERFACE (MAX_INTETFACE_NUM/2)
#define H_MAXDMABUF  1536 
#define H_MAXDMABUFNUM 64

typedef struct el_appdma_s{
  char data[H_MAXDMABUF];
  int len;
  struct el_appdma_s *next;
}el_appdma_t;

typedef struct VoiceVlanInfo_s
{
  u16 VoiceVlanId; /* CVLAN tag */
  u8  VoiceVlanCos; /* CVLAN COS */
  u8 VoipVlanDSCP; /* DSCP, unused */
  u8 VoipTaggedValid; /* 0 - untag mode, 1 - tag mode */
  u8 VoicePort; /* eth index */
  u16 usVlanTpid;	/* CVLAN Tpid */
  u8 VoipQinQEn;    /* 0 - disable QinQ, 1 - enable QinQ */
  u8  VoiceSVlanCos; /* SVLAN COS */
  u16 VoiceSVlanId; /* SVLAN */
  u16 usSVlanTpid;	/* SVLAN Tpid */
}VoiceVlanInfo_t;

typedef struct VoiceVlanTag_s
{
  u32 bfVoiceTpid: 16;
  u32 bfVoiceCos:  3;
  u32 bfVoiceCFI:  1;
  u32 bfVoiceVlan: 12;
}VoiceVlanTag_t;

struct ipmux_skbuff
{
  struct sk_buff  *skb;
  void *vaddr;
  dma_addr_t dma_ptr;
  u32 length;
  u32 bd_info;
};

struct ipmux_fe_priv
{
  struct net_device *ndev;
  struct resource *dmac_resource, *msg_resource;
  u32   dmac_regs;
  u32   msg_regs;
  int   irq;
  spinlock_t lock;
  void *dma_vaddr;
  dma_addr_t dma_base;
  u32 order;

#ifdef WORKQ_MOD
  struct work_struct  rx_wq;
  struct work_struct  tx_wq;
#endif
  
  struct ipmux_skbuff rx_buffers[MAX_RX_DESC];
  spinlock_t rx_lock;
  u32 rx_ring_read;
  
  int tx_head;
  int tx_tail;
  struct ipmux_skbuff tx_buffers[MAX_TX_DESC];
  spinlock_t tx_lock;

  struct net_device_stats stats[MAX_INTETFACE_NUM];
};

struct atheros_head_t
{
  u16 usVersionHi:2;
  u16 usPriority:2;
  u16 usPackType:4;
  u16 usFromCpu:1;
  u16 usPortNum:7;
};

/* Format of DHCP message */
struct tagDHCP_PKT
{
	u8 ucOp;/* 1 = BOOTREQUEST, 2 = BOOTREPLY */
	u8 ucHwType;/* hardware address type. 1 = 10mb ethernet */
	u8 ucHwLen;
	u8 ucHops;/* used by relay agents only */
	u32 ulXid; /* unique id */
	u16 usSecs;/* elapsed since client begin acquisition/renewal */
	u16 usFlags;/* only one flag so far: */
	u32 ulCiAddr;/* client IP (if client is in BOUND, RENEW or REBINDING state) */
	u32 ulYiAddr;/* 'your' (client) IP address */
	u32 ulSiAddr;
	u32 ulGiAddr;/* relay agent IP address */
	u8 aucChaAddr[16];/* link-layer client hardware address (MAC) */
	u8 aucSname[64];/* server host name (ASCIZ) */
	u8 aucFile[128]; /* boot file name (ASCIZ) */
	u8 aucMagicCookie[4];
}__attribute__((packed));
typedef struct tagDHCP_PKT DHCP_PKT_S;

struct tagDHCP_Msg_Type
{
	u8 ucCode;
	u8 ucLen;
	u8 ucVal;
}__attribute__((packed));
typedef struct tagDHCP_Msg_Type DHCP_Msg_Type_S;

static VoiceVlanTag_t gVoicetag;
static VoiceVlanInfo_t gSysVoiceVlan[MAX_INTETFACE_NUM];
static int HW_DMA0_INITED = 0;
static int HW_DMA1_INITED = 0;
struct net_device *lanndev = NULL;
struct net_device *wanndev = NULL;

static struct atheros_head_t atheros_head;
static u8 g_ucportid = 0x1e;
static u8 auc_broadcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static unsigned char fe_addr[6]={00,0xaa,0xbb,0xcc,0xcc,0xdd};
static unsigned char ge_addr[6]={00,0xaa,0xbb,0xcc,0xcc,0xde};

el_appdma_t s_dmadata[2][H_MAXDMABUFNUM];
int dma_startnum[2];
int dma_endnum[2];
wait_queue_head_t opl_host_dma0_waitq;
wait_queue_head_t opl_host_dma1_waitq;
u32 opl_host_dma0_irq_event = 0;
u32 opl_host_dma1_irq_event = 0;
volatile u8 TxBufdma[2048];

#define DRV_NAME    "onu_fe"
#define DRV_VERSION "2010/08/25"
static const char version[] = DRV_NAME ".c: " DRV_VERSION " Figo.zhang (figo1802@gmail.com).\n";

#define fe_writel(reg, value)  ((*(volatile uint32_t *)(reg)) = value)
#define fe_readl(reg)     (*(volatile uint32_t *)( reg))
#define KSEG0_TO_KSEG1(x) ((unsigned long)(x)|0xa0000000)

static inline void read_tx_bd(struct ipmux_fe_priv *fep, u32 index);
static inline void read_rx_bd(struct ipmux_fe_priv *fep, u32 index);

static inline void print_mem(unsigned char *p, u32 len, u8 flg)
{
  u32 pflg, i;
  
  if ((DEBUG_MEM_TX&core_debug) && (flg==0))
  {
    pflg = 1;
    printk("\n--------TX DATA--------");
  }
  else if ((DEBUG_MEM_RX&core_debug) && (flg==1))
  {
    pflg = 1;
    printk("\n--------RX DATA--------");
  }
  else
  {
    pflg = 0;
  }
  
  if (1 == pflg)
  {
    for (i=0; i<len; i++)
    {
      if (0 == (i%16))
      {
        printk("\n  0x%02x : ",(p+i));
      }
      printk("%02x", *(u8 *)(p+i));
      printk(" ");
    }
    printk("\n\n");
  }
}

static inline void enable_rx_interrupt(struct ipmux_fe_priv *fep)
{
  u32 val = 0;
  
  val = fe_readl(fep->dmac_regs + FE_INT_ENABLE);
  val |= ( RXBD_FULL_INT | FEDMA_RX_INT );
  fe_writel(fep->dmac_regs + FE_INT_ENABLE, val);
}

static inline void disable_rx_interrupt(struct ipmux_fe_priv *fep)
{
  u32 val = 0;
  
  val = fe_readl(fep->dmac_regs + FE_INT_ENABLE);
  val &=~ (RXBD_FULL_INT | FEDMA_RX_INT);
  fe_writel(fep->dmac_regs + FE_INT_ENABLE, val);
}

static inline void disable_fe_dmac(struct ipmux_fe_priv *fep)
{
  fe_writel(fep->dmac_regs + FE_CONTRL, 0);
}

static inline void enable_fe_dmac(struct ipmux_fe_priv *fep)
{
  fe_writel(fep->dmac_regs + FE_CONTRL, 0x3);
}

static inline void read_tx_bd(struct ipmux_fe_priv *fep, u32 index)
{
  u32 i = 0;
  u32 ctrl = 0;
  
  ctrl |= (TXBD_OP_BUSY | TXBD_OP_READ | (index));
  fe_writel(fep->dmac_regs + FE_TXBD_CTL, ctrl);
  
  while (fe_readl(fep->dmac_regs + FE_TXBD_CTL) & TXBD_OP_BUSY)
  {
    if (++i > MAX_WAIT_CNT)
    {
      printk(KERN_DEBUG "write bd index=%d time out\n", index);
      return;
    }
  }
  
  fep->tx_buffers[index].bd_info = fe_readl(fep->dmac_regs + FE_TXBD_SHADOW);
}

static inline void write_tx_bd(struct ipmux_fe_priv *fep, u32 index)
{
  u32 i = 0;
  u32 ctrl = 0;
  
  fep->tx_buffers[index].bd_info |= (index + TX_START_SHIFT);
  
  if (index == MAX_TX_DESC - 1)
  {
    fep->tx_buffers[index].bd_info |= (BD_END_BIT);
  }
  
  fe_writel(fep->dmac_regs + FE_TXBD_SHADOW, fep->tx_buffers[index].bd_info);
  
  ctrl |= (TXBD_OP_BUSY | TXBD_OP_WRITE | BD_TABLE_ID | (index));
  fe_writel(fep->dmac_regs + FE_TXBD_CTL, ctrl);
  
  while (fe_readl(fep->dmac_regs + FE_TXBD_CTL) & TXBD_OP_BUSY)
  {
    if (++i > MAX_WAIT_CNT)
    {
      printk(KERN_DEBUG "write tx bd index=%d time out\n", index);
      return;
    }
  }
}

static inline void read_rx_bd(struct ipmux_fe_priv *fep, u32 index)
{
  u32 i = 0;
  u32 ctrl = 0;
  
  ctrl |= (RXBD_OP_BUSY | RXBD_OP_READ | index);
  fe_writel(fep->dmac_regs + FE_RXBD_CTL, ctrl);
  
  while (fe_readl(fep->dmac_regs + FE_RXBD_CTL) & RXBD_OP_BUSY)
  {
    if (++i > MAX_WAIT_CNT)
    {
      printk(KERN_DEBUG "write bd index=%d time out\n", index);
      return;
    }
  }
  
  fep->rx_buffers[index].bd_info = fe_readl(fep->dmac_regs + FE_RXBD_SHADOW );
}

static inline void write_rx_bd(struct ipmux_fe_priv *fep, u32 index)
{
  u32 i = 0;
  u32 ctrl = 0;
  
  fep->rx_buffers[index].bd_info |= (BD_LENGTH_MASK << BD_LENGTH_SHIFT) | (index);
  
  if (index == (MAX_RX_DESC-1))
  {
    fep->rx_buffers[index].bd_info |= (BD_END_BIT);
  }
  
  fe_writel(fep->dmac_regs + FE_RXBD_SHADOW, fep->rx_buffers[index].bd_info);
  ctrl |= (RXBD_OP_BUSY | RXBD_OP_WRITE | BD_TABLE_ID | index);
  fe_writel(fep->dmac_regs + FE_RXBD_CTL, ctrl);
  
  while (fe_readl(fep->dmac_regs + FE_RXBD_CTL) & RXBD_OP_BUSY)
  {
    if (++i > MAX_WAIT_CNT)
    {
      printk(KERN_DEBUG "write bd index=%d time out\n", index);
      return;
    }
  }
}

static inline int check_rxbd_valid(struct ipmux_fe_priv *fep, u32 index)
{
  read_rx_bd(fep, index);
  
  return fep->rx_buffers[index].bd_info & BD_VALID_BIT;
}


static inline int check_txbd_valid(struct ipmux_fe_priv *fep, u32 index)
{
  read_tx_bd(fep, index);
  
  return fep->tx_buffers[index].bd_info & BD_VALID_BIT;
}


static int onu_fe_init_dma(struct ipmux_fe_priv *fep)
{
  int i;
  u32 order, size, count;
  
  disable_fe_dmac(fep);
  udelay(10);
  
  fe_readl(fep->dmac_regs + FE_INT_STATUS);
  
  size = MAX_RXBUF_SIZE * (MAX_RX_DESC + MAX_TX_DESC);
  fep->dma_vaddr = dma_alloc_coherent(NULL, size, &fep->dma_base, GFP_KERNEL);
  
  if (!fep->dma_vaddr)
  {
    printk("canot alloc coherent dma for rx\n");
    return -ENOMEM;
  }
  
  fep->dma_base = KSEG0_TO_KSEG1(fep->dma_base);
  fep->dma_vaddr = KSEG0_TO_KSEG1(fep->dma_vaddr);
  memset(fep->dma_vaddr, 0, size);
  
  printk("alloc dma done:vaddr=0x%x,dma=0x%x,size=%d\n", fep->dma_vaddr, fep->dma_base,
          MAX_RXBUF_SIZE*(MAX_RX_DESC +MAX_TX_DESC));
  fe_writel(fep->dmac_regs+FE_DMA_BASE_ADDR, fep->dma_base);
  
  for (i=0; i<MAX_RX_DESC; i++)
  {
    fep->rx_buffers[i].dma_ptr = fep->dma_base + MAX_RXBUF_SIZE * i;
    fep->rx_buffers[i].vaddr = fep->dma_vaddr + MAX_RXBUF_SIZE * i;
    
    write_rx_bd(fep, i);
  }
  
  for (i=0; i<MAX_TX_DESC; i++)
  {
    fep->tx_buffers[i].dma_ptr = fep->dma_base + MAX_RXBUF_SIZE*MAX_RX_DESC + MAX_TXBUF_SIZE * i;
    fep->tx_buffers[i].vaddr = fep->dma_vaddr + MAX_RXBUF_SIZE*MAX_RX_DESC + MAX_TXBUF_SIZE * i;
    
    write_tx_bd(fep, i);
	fep->tx_buffers[i].skb = NULL;
  }
}

static void onu_timeout(struct net_device *ndev)
{
  netif_wake_queue(ndev);
}

static void onu_multicast_list(struct net_device *dev)
{
  ;
}

static struct net_device_stats *onu_get_stats(struct net_device *ndev)
{
  struct net_device *devtmp;
  struct ipmux_fe_priv *fep;
  unsigned short stats_pos = 0;
  
#if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )|| defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  if (ndev->dev_id < MAX_DMA1_INTERFACE)
  {
    devtmp = lanndev;
  }
  else
  {
    devtmp = wanndev;
  }
#else
  devtmp = lanndev;
#endif
  
  stats_pos = ndev->dev_id;
  fep = netdev_priv(devtmp);
  
  return &fep->stats[stats_pos];
}

static int netdev_ethtool_ioctl (struct net_device *ndev, void *useraddr)
{
  u32 ethcmd;
  
  if (get_user(ethcmd, (u32 *)useraddr))
  {
    return -EFAULT;
  }
  
  switch (ethcmd)
  {
    case ETHTOOL_GDRVINFO:
    {
      struct ethtool_drvinfo info = {ETHTOOL_GDRVINFO};
      strcpy(info.driver, DRV_NAME);
      strcpy (info.version, DRV_VERSION);
      sprintf(info.bus_info, "ISA 0x%lx", ndev->base_addr);
      if (copy_to_user (useraddr, &info, sizeof (info)))
      {
        return -EFAULT;
      }
      return 0;
    }
    case ETHTOOL_GMSGLVL:
    {
      struct ethtool_value edata = {ETHTOOL_GMSGLVL};
      if (copy_to_user(useraddr, &edata, sizeof(edata)))
      {
        return -EFAULT;
      }
      return 0;
    }
    case ETHTOOL_SMSGLVL: 
    {
      struct ethtool_value edata;
      if (copy_from_user(&edata, useraddr, sizeof(edata)))
      {
        return -EFAULT;
      }
      return 0;
    }
    default:
      break;
  }
  return -EOPNOTSUPP;
}

static int netdev_ioctl (struct net_device *dev, struct ifreq *rq, int cmd)
{
  int rc = 0;
  
  switch (cmd)
  {
    case SIOCETHTOOL:
      rc = netdev_ethtool_ioctl(dev, (void *)rq->ifr_data);
      break;
    default:
      rc = -EOPNOTSUPP;
      break;
  }
  
  return rc;
}

int onu_start_xmit_buf(struct net_device *ndev,u8 *buf, u32 len)
{
	struct ipmux_fe_priv *fep;
	u32 index, status= 0, i;
	unsigned char *buffer;
	unsigned long flags;
	
	fep = netdev_priv(ndev);

	spin_lock_irqsave(&fep->tx_lock, flags);

	index = fep->tx_head;
	/*update for avail pointer*/
	fep->tx_head = (index+1) % MAX_TX_DESC;

	buffer = (unsigned char *)fep->tx_buffers[index].vaddr;

	dprintk_utx("%s head=%d, tail=%d, skb->len=%d\n", __FUNCTION__, index, fep->tx_tail, len);

	/*wait for DMA ready for send*/
	read_tx_bd(fep, index);
	i = 0;
	while(fep->tx_buffers[index].bd_info & BD_VALID_BIT){
		read_tx_bd(fep, index);
		if(++i > MAX_WAIT_CNT){
			if(printk_ratelimit())
			{
				printk("dma not ready for send, head=%d, tail=%d, len=%d\n", 
					index, fep->tx_tail, len);
			}
			goto fail;
		}
	}

	memcpy((unsigned char *)buffer, (unsigned char *)buf, len);

	int wait = 1, timeout = 0x100000;
	while(wait)
	{
		wait = *(buf + len - 8)!=*(buffer + len -8);
		wait += *(buf + len - 7)!=*(buffer + len -7);
		wait += *(buf + len - 6)!=*(buffer + len -6);
		wait += *(buf + len - 5)!=*(buffer + len -5);
		wait += *(buf + len - 4)!=*(buffer + len -4);
		wait += *(buf + len - 3)!=*(buffer + len -3);
		wait += *(buf + len - 2)!=*(buffer + len -2);
		wait += *(buf + len - 1)!=*(buffer + len -1);
		if (--timeout < 0)
			break;
	}

	print_mem((unsigned char *)buffer, len, 0);
	/*write bd to reg, and start DMA*/
#if defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )|| defined(CONFIG_ONU_FE_VOIP_F23_ETH)
	if (ndev->dev_id >= MAX_DMA1_INTERFACE)
	{
		len = len - 1;
	}
#else
	len = len - 1;
#endif
	status = 0;
	status |= ( (len<< BD_LENGTH_SHIFT ) | BD_VALID_BIT);
	fep->tx_buffers[index].bd_info = status;
	write_tx_bd(fep, index);

	dprintk_utx("write BD, status=0x%x,start dma send\n" ,fep->tx_buffers[index].bd_info);

	fep->tx_buffers[index].skb = NULL;
	fep->tx_buffers[index].length = len;

#ifdef WORKQ_MOD
	if (((fep->tx_head-fep->tx_tail)+MAX_TX_DESC) % MAX_TX_DESC >= 8)
	{
	  schedule_work(&fep->tx_wq);
	}
#endif

	ndev->trans_start = jiffies;

	spin_unlock_irqrestore(&fep->tx_lock, flags);
	return NETDEV_TX_OK;

fail:
	spin_unlock_irqrestore(&fep->tx_lock, flags);
	return NETDEV_TX_BUSY;	
}

static int onu_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
  unsigned short dev_id;
  u32 index, i = 0, status= 0;
  struct ipmux_fe_priv *fep;
  unsigned char *buf;
  unsigned char *printbuf;
  unsigned long length;
  unsigned short stats_pos = 0;
  u8 *sbuf;
  struct net_device *devtmp;
  int flags;
  
  dev_id = ndev->dev_id;
  stats_pos = dev_id;
#if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  if (dev_id < MAX_DMA1_INTERFACE)
  {
    devtmp = lanndev;
  }
  else
  {
    devtmp = wanndev;
  }
#else
  devtmp = lanndev;
#endif
  
  fep = netdev_priv(devtmp);

  spin_lock_irqsave(&fep->tx_lock, flags);

  index = fep->tx_head;
  fep->tx_head = (index+1) % MAX_TX_DESC;
  
  sbuf =((struct sk_buff *)skb)->data;
  fep->tx_buffers[index].skb = ((struct sk_buff *)skb);
  
  buf = (unsigned char *)fep->tx_buffers[index].vaddr;
  printbuf = buf;
  length = ((struct sk_buff *)skb)->len;
  dprintk_tx("dev_id=%d, index=%d, buf=%x, length=%d\n", dev_id, index, buf, length);
  
  read_tx_bd(fep, index);
  i = 0;
  while (fep->tx_buffers[index].bd_info & BD_VALID_BIT)
  {
    read_tx_bd(fep, index);
    if (++i > MAX_WAIT_CNT)
    {
		if(printk_ratelimit())
		{
			printk("dma not ready for send, head=%d, tail=%d, dev_id=%d, len=%d\n", 
				index, fep->tx_tail, dev_id, length);
		}
		goto fail;
    }
  }
  
#if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  if (dev_id < MAX_DMA1_INTERFACE)
  {    
    MEMCPY6B(buf, sbuf);
    buf += 6;
    MEMCPY6B(buf, ((u8 *)sbuf)+6);
    buf += 6;
#if defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
    MEMCPY2B(buf, &atheros_head);
    buf += 2;
    length += 2;
#endif	
  }
  else
  {
    ((unsigned char *)buf)[0] = 0x15;
    buf += 1;
    MEMCPY6B(buf, sbuf);
    buf += 6;
    MEMCPY6B(buf, ((u8 *)sbuf)+6);
    buf += 6;
  }
  
  /* add for qinq support */
  if (gSysVoiceVlan[dev_id].VoipQinQEn == 1)
#else

  if (dev_id < (MAX_INTETFACE_NUM/2))
  {
    ((unsigned char *)buf)[0] = 0;
  }
  else
  {
    ((unsigned char *)buf)[0] = 0x15;
  }
  buf += 1;
  MEMCPY6B(buf, sbuf);
  buf += 6;
  MEMCPY6B(buf, ((u8 *)sbuf)+6);
  buf += 6;
  
  /* add for qinq support */
  if ((gSysVoiceVlan[dev_id].VoipQinQEn == 1) && (dev_id != ETH0_INTERFACE_ID))
#endif
  {
    gVoicetag.bfVoiceCos=gSysVoiceVlan[dev_id].VoiceSVlanCos;
    //gVoicetag.bfVoiceTpid=gSysVoiceVlan[dev_id].usSVlanTpid;;
    gVoicetag.bfVoiceTpid=0x8100;
    gVoicetag.bfVoiceVlan=gSysVoiceVlan[dev_id].VoiceSVlanId;
    
    dprintk_tx("add stag =%x\n", gVoicetag.bfVoiceVlan);
    MEMCPY4B(buf, &gVoicetag);
    buf += 4;
    length += 4;
  }

#if defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
	if ((gSysVoiceVlan[dev_id].VoipTaggedValid == 1))
#else
	if ((gSysVoiceVlan[dev_id].VoipTaggedValid == 1) && (dev_id != ETH0_INTERFACE_ID))
#endif
  {
    gVoicetag.bfVoiceCos=gSysVoiceVlan[dev_id].VoiceVlanCos;
	  //gVoicetag.bfVoiceTpid=gSysVoiceVlan[dev_id].usVlanTpid;
    gVoicetag.bfVoiceTpid=0x8100;
    gVoicetag.bfVoiceVlan=gSysVoiceVlan[dev_id].VoiceVlanId;

    
	  dprintk_tx("add ctag =%x\n", gVoicetag.bfVoiceVlan);
    MEMCPY4B(buf, &gVoicetag);
    buf += 4;
    length += 4;
  }

  
  memcpy(buf, ((u8 *)sbuf)+12, length-12);

  int wait = 1, timeout = 0x100000;
	while(wait)
	{
		wait = *(buf + length-12 - 8)!=*(sbuf + length -8);
		wait += *(buf + length-12 - 7)!=*(sbuf + length -7);
		wait += *(buf + length-12 - 6)!=*(sbuf + length -6);
		wait += *(buf + length-12 - 5)!=*(sbuf + length -5);
		wait += *(buf + length-12 - 4)!=*(sbuf + length -4);
		wait += *(buf + length-12 - 3)!=*(sbuf + length -3);
		wait += *(buf + length-12 - 2)!=*(sbuf + length -2);
		wait += *(buf + length-12 - 1)!=*(sbuf + length -1);
		if (--timeout < 0)
			break;
	}

  print_mem(printbuf, length, 0);
  status = 0;
  status |= ( (length<< BD_LENGTH_SHIFT ) | BD_VALID_BIT);
  fep->tx_buffers[index].bd_info = status;
  write_tx_bd(fep, index);

  fep->stats[stats_pos].tx_packets++;
  fep->stats[stats_pos].tx_bytes += length;
  fep->tx_buffers[index].length = 0;

#ifdef WORKQ_MOD
  if (((fep->tx_head-fep->tx_tail)+MAX_TX_DESC) % MAX_TX_DESC >= 8)
  {
    schedule_work(&fep->tx_wq);
  }
#endif

  devtmp->trans_start = jiffies;

  spin_unlock_irqrestore(&fep->tx_lock, flags);
  return NETDEV_TX_OK;

fail:
  spin_unlock_irqrestore(&fep->tx_lock, flags);
  return NETDEV_TX_BUSY;
}

static void onu_tx_interrupt(struct ipmux_fe_priv *fep)
{
  int flags;
   
  //spin_lock_irqsave(&fep->tx_lock, flags);
 
  while(fep->tx_tail != fep->tx_head)
  {
	if(fep->tx_buffers[fep->tx_tail].skb != NULL)
    {
      dev_kfree_skb_any(fep->tx_buffers[fep->tx_tail].skb);
      fep->tx_buffers[fep->tx_tail].skb = NULL;
    }

    /* to next Tx descriptor */
    fep->tx_tail = (fep->tx_tail + 1) % MAX_TX_DESC;
  }

  //spin_unlock_irqrestore(&fep->tx_lock, flags);

  /*if netif queue stop ,wake it up!!!*/
  if (netif_queue_stopped(fep->ndev))
  {
    netif_wake_queue(fep->ndev);
  }
}

static int find_valid_rx(int idx, struct ipmux_fe_priv *fep, u32 index, u32 *valid_index)
{
  int i;
  
  if (idx==0)
  {
    for (i=index; i<MAX_RX_DESC+index; i++)
    {
      if (check_rxbd_valid(fep, i%MAX_RX_DESC))
      {
        *valid_index = i % MAX_RX_DESC;
        
        return 1;
      }
    }
  }
  else
  {
    *valid_index = index;
    
    return check_rxbd_valid(fep, index);
  }
  
  return 0;
}

void dma_rx_receive_kernel(int dmaflg, void *dma_buf, int pkt_len, struct net_device *devtmp, int with_vlan, struct ipmux_fe_priv *fep)
{
  struct sk_buff *skb;
  unsigned short stats_pos = 0;
  int with_qinq, offset, offset2;
  
  stats_pos = devtmp->dev_id;
  dprintk_rx("stats_pos=%d\n", stats_pos);
#ifdef WORKQ_MOD
  skb = __dev_alloc_skb(pkt_len+2,GFP_DMA|GFP_ATOMIC);
  if (skb == NULL)
  {
    printk(KERN_ERR "skb not available...\n");
  }
#else
  skb = dev_alloc_skb(pkt_len + 2);
#endif
  skb_reserve(skb,2);
  skb->dev = devtmp;
  
  offset = 0;
  with_qinq = 0;
  if (with_vlan)
  {
	  dprintk_rx("###############  with vlan ###############\n");
    if (0 == dmaflg)
    {

	  if ((((u8 *)dma_buf)[20] == 0x81) && (((u8 *)dma_buf)[21] == 0x00))
	  {
		  offset = 12;
		  with_qinq = 1;
	  }
	  else
	  {
		  offset = 8;
		  with_qinq = 0;
	  }
      offset2 = 4;
    }
    else
    {
	  if ((((u8 *)dma_buf)[18] == 0x81) && (((u8 *)dma_buf)[19] == 0x00))
	  {
		  offset = 10;
		  with_qinq = 1;
	  }
	  else
	  {
		  offset = 6;
		  with_qinq = 0;
	  }
      offset2 = 0;
    }
	((char *)dma_buf)[offset+11] = ((char *)dma_buf)[offset2+11];
    ((char *)dma_buf)[offset+10] = ((char *)dma_buf)[offset2+10];
    ((char *)dma_buf)[offset+9] = ((char *)dma_buf)[offset2+9];
    ((char *)dma_buf)[offset+8] = ((char *)dma_buf)[offset2+8];
    ((char *)dma_buf)[offset+7] = ((char *)dma_buf)[offset2+7];
    ((char *)dma_buf)[offset+6] = ((char *)dma_buf)[offset2+6];
    ((char *)dma_buf)[offset+5] = ((char *)dma_buf)[offset2+5];
    ((char *)dma_buf)[offset+4] = ((char *)dma_buf)[offset2+4];
    ((char *)dma_buf)[offset+3] = ((char *)dma_buf)[offset2+3];
    ((char *)dma_buf)[offset+2] = ((char *)dma_buf)[offset2+2];
    ((char *)dma_buf)[offset+1] = ((char *)dma_buf)[offset2+1];
    ((char *)dma_buf)[offset] = ((char *)dma_buf)[offset2];
	memcpy(skb_put(skb,pkt_len-offset), (u8 *)(dma_buf+offset), pkt_len-offset);
  }
  else
  {
	  dprintk_rx("$$$$$$$$$$$$$$$$$  no vlan $$$$$$$$$$$$$$$$$ \n");
    if (0 == dmaflg)
    {
      memcpy(skb_put(skb,pkt_len-4), (u8 *)(dma_buf+4), pkt_len-4);
    }
    else
    {
 #ifdef CONFIG_ONU_FE_RESPIN_C13_ETH
        memcpy(skb_put(skb,pkt_len), (u8 *)(dma_buf), pkt_len);
 #else   /**we need to cut the atheros header for f23 **/
      ((char *)dma_buf)[13] = ((char *)dma_buf)[11];
      ((char *)dma_buf)[12] = ((char *)dma_buf)[10];
      ((char *)dma_buf)[11] = ((char *)dma_buf)[9];
      ((char *)dma_buf)[10] = ((char *)dma_buf)[8];
      ((char *)dma_buf)[9] = ((char *)dma_buf)[7];
      ((char *)dma_buf)[8] = ((char *)dma_buf)[6];
      ((char *)dma_buf)[7] = ((char *)dma_buf)[5];
      ((char *)dma_buf)[6] = ((char *)dma_buf)[4];
      ((char *)dma_buf)[5] = ((char *)dma_buf)[3];
      ((char *)dma_buf)[4] = ((char *)dma_buf)[2];
      ((char *)dma_buf)[3] = ((char *)dma_buf)[1];
      ((char *)dma_buf)[2] = ((char *)dma_buf)[0];
      
      memcpy(skb_put(skb,pkt_len-2), (u8 *)(dma_buf+2), pkt_len-2);
#endif
    }
  }
  
  skb->protocol = eth_type_trans(skb, devtmp);
#ifdef WORKQ_MOD
	netif_rx(skb);
#else
	netif_receive_skb(skb);
#endif
  dprintk_rx("dma:%d, receive to kernel\n", dmaflg);
  
  fep->stats[stats_pos].rx_packets++;
  fep->stats[stats_pos].rx_bytes += pkt_len;
}

extern el_appdma_t *getdmabufaddress(u8 dmanum);
static int dma_rx_receive(struct net_device *ndev, int budget)
{
  int received = 0;
  int pkt_len;
  int rev;
  void *pfe_pkt, *dma_buf, *debug_buf;
  u32 status, valid_index;
  struct ipmux_fe_priv *fep = netdev_priv(ndev);
  struct net_device *devtmp;
  struct atheros_head_t * arhead;
  int devstartnum, devendnum, devnownum;
  int offset0, offset1, offset2, offset;
  int with_vlan, i;
  int dmaflg = 0;
  int skip_cnt = 0;
  VoiceVlanTag_t *vlan_tag;
  u32 index = fep->rx_ring_read;
  
  dprintk_rx("start------index=%d\n", index);
  while ((received < budget) && find_valid_rx(received, fep, index, &valid_index))
  {
    index = valid_index;
    
    read_rx_bd(fep, index);
    if (!(fep->rx_buffers[index].bd_info & BD_VALID_BIT))
    {
      printk("invalid packed index=%d\n", index);
      goto done;
    }
    
    status = fep->rx_buffers[index].bd_info;
    dma_buf = debug_buf = (u8 *)fep->rx_buffers[index].vaddr;
    
    pkt_len = (status >> BD_LENGTH_SHIFT) & BD_LENGTH_MASK;
    if (pkt_len < 60)
    {
      printk("bogus packet, length=%d\n",  pkt_len);
      goto done;
    }
    else
    {
    #if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
      if (ndev->dev_id < MAX_DMA1_INTERFACE)
      {
        dprintk_rx("-----------------ndev->dev_id =%d\n", ndev->dev_id );
        dmaflg = 1;
      }
    #endif
      
      if (0 == dmaflg)
      {
//    	  printk("dma_buf[3] is 0x%x\r\n",((u8 *)dma_buf)[3]);
      #if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
        if (((u8 *)dma_buf)[3] == 0x0)
        {
#if 1  /*--- Modified by KnightDavion 2013-8-7 ---*/
          devstartnum = 0;  
          devendnum = 0;    /**  ignor pkt from ge to cpu **/
#else
          devstartnum = MAX_INTETFACE_NUM / 2;
          devendnum = MAX_INTETFACE_NUM;    /**  let pkt from ge to cpu **/
#endif
        }
        else
        {
          devstartnum = MAX_DMA1_INTERFACE;
          devendnum = MAX_INTETFACE_NUM;
        }
      #else
        if (((u8 *)dma_buf)[3] == 0x0)
        {
          devstartnum = 0;
          devendnum = MAX_INTETFACE_NUM / 2;
        }
        else
        {
          devstartnum = MAX_INTETFACE_NUM / 2;
          devendnum = MAX_INTETFACE_NUM;
        }
      #endif
        offset0 = 4;
        offset1 = 0;
      }
      else   /**when use dma1 as fe interface**/
      {
    	  dprintk_rx("444-----------------dmaflg =%d\n", dmaflg );
#if 0
    	int i=0;
    	if((((unsigned char *)dma_buf)[0] == 0x0)&&	(((unsigned char *)dma_buf)[1] == 0x0f)&&
    			(((unsigned char *)dma_buf)[2] == 0xe9))
    	{
    for(i=0;i<20;i++)
    	{
    		if(i%16 == 0)
    			printk("\r\n");
    		printk("0x%02x ",((unsigned char *)dma_buf)[i]);
    	}
    printk("\r\n\n");
    	}
#endif
        devstartnum = 0;
        devendnum = MAX_DMA1_INTERFACE;
        offset0 = 0;
#if defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
       offset1 = 2;      /**f23 need to add atheros header**/
#else
        offset1 = 0;     /**c13 need not **/
#endif
      }
#if 0
if((((unsigned char *)dma_buf)[offset0] == 0x0)&&	(((unsigned char *)dma_buf)[offset0+1] == 0x0f)&&
		(((unsigned char *)dma_buf)[offset0+2] == 0xe9))
{
	int i=0;
	printk("((char *)dma_buf+offset0)[0] is  0x%x...\r\n",((char *)dma_buf+offset0)[0]);
	for(i=0;i<20;i++)
	{
		if(i%16 == 0)
			printk("\r\n");
		printk("0x%02x ",((unsigned char *)dma_buf)[i]);
	}
    printk("\r\n\npkt_len=%d, dmaflg=%d, with_vlan=%d, offset0=%d, offset1=%d, offset2=%d, devstartnum=%d, devendnum=%d, devnownum=%d\n",
                pkt_len, dmaflg, with_vlan, offset0, offset1, offset2, devstartnum, devendnum, devnownum);
}
#endif
      devnownum = 0xFF;
      
      if ((((u8 *)dma_buf)[12+offset0+offset1] == 0x81) && (((u8 *)dma_buf)[13+offset0+offset1] == 0x00))
      {
        offset2 = 4;
        with_vlan = 1;
        
        vlan_tag = (VoiceVlanTag_t *)((u8 *)dma_buf+12+offset0+offset1);
        
        dprintk_rx("vlan_tag=%x\n", vlan_tag->bfVoiceVlan);
        if (0xFFF == vlan_tag->bfVoiceVlan)
        {
          devnownum = 0xF;
        }
        else
        {
          for (i=devstartnum; i<devendnum; i++)
          {
			if ((gSysVoiceVlan[i].VoipQinQEn==1) && 
				(gSysVoiceVlan[i].VoiceSVlanId==vlan_tag->bfVoiceVlan))
            {
				dprintk_rx("index=%d, qinq, vlan_tag=%x\n", i, gSysVoiceVlan[i].VoiceSVlanId);
              devnownum = i;
              goto process_pon_data;
            }
            else if ((gSysVoiceVlan[i].VoipQinQEn==0) && 
				(gSysVoiceVlan[i].VoipTaggedValid==1) && 
				(gSysVoiceVlan[i].VoiceVlanId==vlan_tag->bfVoiceVlan))
            {
				dprintk_rx("index=%d, vlan_tag=%x\n", i,
					gSysVoiceVlan[i].VoiceVlanId);
              devnownum = i;
              goto process_pon_data;
            }
          }
        }
      }
      else
      {
        offset2 = 0;
        with_vlan = 0;
      }
      dprintk_rx("pkt_len=%d, dmaflg=%d, with_vlan=%d, offset0=%d, offset1=%d, offset2=%d, devstartnum=%d, devendnum=%d, devnownum=%d\n",
                  pkt_len, dmaflg, with_vlan, offset0, offset1, offset2, devstartnum, devendnum, devnownum);
      print_mem(dma_buf+offset0, pkt_len, 1);

process_pon_data:
      /* Processing PON related data first */
#if 0
if((((unsigned char *)dma_buf)[offset0] == 0x0)&&	(((unsigned char *)dma_buf)[offset0+1] == 0x0f)&&
		(((unsigned char *)dma_buf)[offset0+2] == 0xe9))
{
	int i=0;
	printk("((char *)dma_buf+offset0)[0] is  0x%x...\r\n",((char *)dma_buf+offset0)[0]);
	for(i=0;i<20;i++)
	{
		if(i%16 == 0)
			printk("\r\n");
		printk("0x%02x ",((unsigned char *)dma_buf)[i]);
	}
    printk("\r\n\npkt_len=%d, dmaflg=%d, with_vlan=%d, offset0=%d, offset1=%d, offset2=%d, devstartnum=%d, devendnum=%d, devnownum=%d\n",
                pkt_len, dmaflg, with_vlan, offset0, offset1, offset2, devstartnum, devendnum, devnownum);
}
#endif
/*oam pkt
00 00 00 01 01 80 c2 00 00 02 00 0c d5 00 01 10
88 09 03 00 50 00 01 10 01 00 00 00 0d 05 ee 11
11 11 52 01 00 01 02 10 01 00 08 00 14 05 ee 11
11 11 47 57 44 4c 00 00 00 00 00 00 00 00 00 00
1d 01 80 c2 00 00 02 00 0f e9 11 22 33 88 09 03
00 50 00 01 10 01 00 08 00 14 05 ee 11 11 11 47
57 44 4c 02 10 01 00 00 00 0d 05 ee 11 11 11 52
01     00    01    00    00    00    00    00    00    00    00    00    00 */
//printk("8809=== 0x%02x  0x%02x  0x%02x  0x%02x  \r\n",((unsigned char *)dma_buf)[offset0+12],
	//	((char *)dma_buf)[offset0+13],((char *)dma_buf)[offset0+14],((char *)dma_buf)[offset0+15]);
#if 0
		if (((char *)dma_buf+offset0)[0] == 0x01)   /**pkt from pon **/
#else   /*------ send pkt OAM RCP and LoopBack PKT(from PON and UNI) to app modified by KnightDavion 2013-8-14 ------*/
    if (((((unsigned char *)dma_buf)[offset0+12] == 0x88)&&(((unsigned char *)dma_buf)[offset0+13] == 0x09))||
    		((((unsigned char *)dma_buf)[offset0+offset1+12] == 0x08)&&	(((unsigned char *)dma_buf)[offset0+offset1+13] == 0x0)&&
    		(((unsigned char *)dma_buf)[offset0+offset1+14] == 0x0)&&(((unsigned char *)dma_buf)[offset0+offset1+15] == 0x80))||
    		((((unsigned char *)dma_buf)[offset0+offset1+12+4] == 0x08)&&	(((unsigned char *)dma_buf)[offset0+offset1+13+4] == 0x0)&&
    	    		(((unsigned char *)dma_buf)[offset0+offset1+14+4] == 0x0)&&(((unsigned char *)dma_buf)[offset0+offset1+15+4] == 0x80))||
    	    		((((unsigned char *)dma_buf)[offset0+offset1+12] == 0x88)&&(((unsigned char *)dma_buf)[offset0+offset1+13] == 0x99))||
    	    		((((unsigned char *)dma_buf)[offset0+offset1+12+4] == 0x88)&&(((unsigned char *)dma_buf)[offset0+offset1+13+4] == 0x99)))
#endif
      {
//    	  printk("aaaaaaaa33\r\n");
        el_appdma_t *p_s_appdma = NULL;
        
        if ((p_s_appdma=getdmabufaddress(dmaflg)) != NULL)
        {
          p_s_appdma->len = pkt_len;
          memcpy((char *)&(p_s_appdma->data[0]), (char *)dma_buf, pkt_len);
          if (0 == dmaflg)
          {
            if (test_and_set_bit(0, &opl_host_dma0_irq_event))
            {
              ;
            }
            wake_up_interruptible(&opl_host_dma0_waitq);
            dprintk_rx("wakeupfromdma0\n");
          }
          else
          {
            if (test_and_set_bit(0, &opl_host_dma1_irq_event))
            {
              ;
            }
            wake_up_interruptible(&opl_host_dma1_waitq);
            dprintk_rx("wakeupfromdma1\n");
          }
          fep->stats[lanndev->dev_id].rx_packets++;
          fep->stats[lanndev->dev_id].rx_bytes += pkt_len;
          goto done;
        }
      }
      
      offset = offset0 + offset1 + offset2;

      skip_cnt = 0;
      for (i=devendnum-1; i>=devstartnum; i--)
      {
        devtmp = onundevs[i];
        /* Deleted by zhangjj as tag arp cannot send to cpu 2013-8-28
        if ((with_vlan) && (i!=devnownum) && (0xF!=devnownum))
        {
          devtmp = onundevs[i-1];
          skip_cnt++;
          continue;
        }
        */
        dprintk_rx("dev name=%s\n", devtmp->name);
        if (devtmp)
        {

          if (MACEQUAL((char *)devtmp->dev_addr, (char *)dma_buf+offset0))
          {
            struct iphdr* piphead;
            struct in_device *ipout=devtmp->ip_ptr;
            struct in_ifaddr *iplist;
            u32 iptmp, masktmp, ipin;
            
            if ((((u8 *)dma_buf)[12+offset]==0x08) && 
              (((u8 *)dma_buf)[13+offset]==0x00))
            {
            	dprintk_rx("onu_voip_fe 0800\r\n");
              if (((((u8 *)dma_buf)[34+offset] == 0x00) &&
                (((u8 *)dma_buf)[35+offset] == 0x43)) || 
                ((((u8 *)dma_buf)[34+offset] == 0x00) &&
                (((u8 *)dma_buf)[35+offset] == 0x44)))
              {
            	  dprintk_rx("dhcp offer package\n");
                dma_rx_receive_kernel(dmaflg, dma_buf, pkt_len, devtmp, with_vlan, fep);
              }
              else
              {
            	  dprintk_rx("ip package\n");
                piphead = (struct iphdr *)((u8 *)dma_buf+14+offset);
                if (ipout != NULL)
                {
                  iplist = ipout->ifa_list;
                  while(iplist != NULL)
                  {
                    ipin = piphead->daddr;
                    iptmp = iplist->ifa_address;
                    masktmp = iplist->ifa_mask;
                    dprintk_rx("ownip=%x, packageip=%x\n", ipin&masktmp, iptmp&masktmp);
                    if ((ipin&masktmp) == (iptmp&masktmp))
                    {
                      dma_rx_receive_kernel(dmaflg, dma_buf, pkt_len, devtmp, with_vlan, fep);
                      goto done;
                    }
                    iplist = iplist->ifa_next;
                  }
                }
              }
            }
            else if ((((u8 *)dma_buf)[12+offset] == 0x08) && (((u8 *)dma_buf)[13+offset] == 0x06))
            {
            	dprintk_rx("in onu_voip_fe 0806\r\n");
              struct in_device *ipout=devtmp->ip_ptr;
              struct in_ifaddr *iplist;
              u32 iptmp, ipin;
              u8 iptmp1,iptmp2,iptmp3,iptmp4;
              u8* dma_buftmp = (u8 *)dma_buf;
              
              dprintk_rx("arp response package\n");
              if (ipout != NULL)
              {
                iplist = ipout->ifa_list;
                while(iplist != NULL)
                {
                  iptmp = iplist->ifa_address;
                  iptmp4 = (u8)iptmp;
                  iptmp3 = (u8)(iptmp>>8);
                  iptmp2 = (u8)(iptmp>>16);
                  iptmp1 = (u8)(iptmp>>24);
                  dprintk_rx("ownip=%x.%x.%x.%x, packageip=%x.%x.%x.%x\n", iptmp1, iptmp2, iptmp3, iptmp4,
                     dma_buftmp[38+offset],dma_buftmp[39+offset],dma_buftmp[40+offset],dma_buftmp[41+offset]);
                  if ((dma_buftmp[38+offset]==iptmp1) && (dma_buftmp[39+offset]==iptmp2)
                   && (dma_buftmp[40+offset]==iptmp3) && (dma_buftmp[41+offset]==iptmp4))
                  {
                    dma_rx_receive_kernel(dmaflg, dma_buf, pkt_len, devtmp, with_vlan, fep);
                    goto done;
                  }
                  iplist = iplist->ifa_next;
                }
              }
            }
            else
            {
              dprintk_rx("other uni package\n");
              dma_rx_receive_kernel(dmaflg, dma_buf, pkt_len, devtmp, with_vlan, fep);
              //goto done;
            }
          }
          else if (MACEQUAL(auc_broadcast_mac, (char *)dma_buf+offset0))
          {
            if ((((char *)dma_buf)[12+offset] == 0x08) && (((char *)dma_buf)[13+offset] == 0x06))
            {
              struct in_device *ipout=devtmp->ip_ptr;
              struct in_ifaddr *iplist;
              u32 iptmp, ipin;
              u8 iptmp1,iptmp2,iptmp3,iptmp4;
              u8* dma_buftmp = (u8 *)dma_buf;
              
              dprintk_rx("arp request package\n");
              if (ipout != NULL)
              {
                iplist = ipout->ifa_list;
                while(iplist != NULL)
                {
                  iptmp = iplist->ifa_address;
                  iptmp4 = (u8)iptmp;
                  iptmp3 = (u8)(iptmp>>8);
                  iptmp2 = (u8)(iptmp>>16);
                  iptmp1 = (u8)(iptmp>>24);
                  
                  dprintk_rx("ownip=%x.%x.%x.%x, packageip=%x.%x.%x.%x\n", iptmp1, iptmp2, iptmp3, iptmp4,
                   dma_buftmp[38+offset],dma_buftmp[39+offset],dma_buftmp[40+offset],dma_buftmp[41+offset]);
                  if ((dma_buftmp[38+offset]==iptmp1) && (dma_buftmp[39+offset]==iptmp2) 
                   && (dma_buftmp[40+offset]==iptmp3) && (dma_buftmp[41+offset]==iptmp4))
                  {
                    dma_rx_receive_kernel(dmaflg, dma_buf, pkt_len, devtmp, with_vlan, fep);
                    goto done;
                  }
                  iplist = iplist->ifa_next;
                }
              }
            }
            else if ((((u8 *)dma_buf)[16+offset] == 0x08) && 
              (((u8 *)dma_buf)[17+offset] == 0x00))
            {
              struct iphdr* piphead;
              struct in_device *ipout=devtmp->ip_ptr;
              struct in_ifaddr *iplist;
              u32 iptmp, masktmp, ipin;

              if (((((u8 *)dma_buf)[34+offset] == 0x00) &&
                (((u8 *)dma_buf)[35+offset] == 0x43)) || 
                ((((u8 *)dma_buf)[34+offset] == 0x00) &&
                (((u8 *)dma_buf)[35+offset] == 0x44)))
              {
                dprintk_urx("  send dhcp offer to kernel, dev %d \n", i);
                dma_rx_receive_kernel(dmaflg, dma_buf, 
                  pkt_len, devtmp, with_vlan, fep);
              }
              else
              {
                piphead = (struct iphdr *)((u8 *)dma_buf+18+offset);
                if (ipout != NULL)
                {
                  iplist = ipout->ifa_list;
                  while(iplist != NULL)
                  {
                    ipin = piphead->daddr;
                    iptmp = iplist->ifa_address;
                    masktmp = iplist->ifa_mask;
                    if ((ipin&masktmp) == (iptmp&masktmp))
                    {
                      dprintk_urx("  send ip packet to kernel, dev %d \n", i);
                      dma_rx_receive_kernel(dmaflg, dma_buf, 
                        pkt_len, devtmp, with_vlan, fep);
                      goto done;
                    }
                    iplist = iplist->ifa_next;
                  }
                }
              }
            }
            else
            {
              dprintk_rx("other broadcast package\n");
              dma_rx_receive_kernel(dmaflg, dma_buf, pkt_len, devtmp, with_vlan, fep);
              //goto done;
            }
          }
          else
          {
            dprintk_rx("other unusefule package\n");
          }
        }
        devtmp = onundevs[i-1];
      }
    }

 #if defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH) /*added to support N:1 aggregation vlan mode.*/

    if (1 == dmaflg && (skip_cnt == devendnum-devstartnum)){
		arhead = (struct atheros_head_t *)((u8 *)dma_buf+12);
		if (arhead->usPackType == 0) {/*normal packet.*/
			el_appdma_t *p_s_appdma = NULL;
			if ((p_s_appdma=getdmabufaddress(dmaflg)) != NULL){
			  p_s_appdma->len = pkt_len;
			  memcpy((char *)&(p_s_appdma->data[0]), (char *)dma_buf, pkt_len);
		      if (test_and_set_bit(0, &opl_host_dma1_irq_event)){
				  ;
			  }
			  wake_up_interruptible(&opl_host_dma1_waitq);
			  dprintk_rx("wakeupfromdma1\n");
			  fep->stats[lanndev->dev_id].rx_packets++;
			  fep->stats[lanndev->dev_id].rx_bytes += pkt_len;
			}
		}
    }
	
#endif
	
    ndev->last_rx = jiffies;

done:
    fep->rx_buffers[index].bd_info &=~ BD_VALID_BIT;
    write_rx_bd(fep, index);
    index = (index+1) % MAX_RX_DESC;
    received++;
  }
  fep->rx_ring_read = index;
  dprintk_rx("end------index=%d\n", index);
  
  return check_rxbd_valid(fep, index);
}

static int dma_onu_poll(struct net_device *ndev, int *budget)
{
  int orig_budget;
  int done = 1;
  int work_done;
  struct ipmux_fe_priv *fep;
  struct net_device *devtmp;
  
#if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  if (ndev->dev_id < MAX_DMA1_INTERFACE)
  {
    devtmp = lanndev;
  }
  else
  {
    devtmp = wanndev;
  }
#else
   devtmp = lanndev;
#endif
  
  fep = netdev_priv(devtmp);
  orig_budget = min(*budget, devtmp->quota);
  dprintk_rx("dev_id=%d, orig_budget=%d\n", ndev->dev_id, orig_budget);
  spin_lock(&fep->rx_lock);
  
  work_done = dma_rx_receive(devtmp, orig_budget);
  
  dprintk_rx("work_done=%d\n", work_done);
  if (likely(work_done > 0))
  {
    *budget -= work_done;
    devtmp->quota -= work_done;
    done = (work_done < orig_budget);
  }
  
  dprintk_rx("done=%d\n", done);
  if (done)
  {
    unsigned long flags;
    
    local_irq_save(flags);
    __netif_rx_complete(devtmp);
    enable_rx_interrupt(fep);
    local_irq_restore(flags);
  }
  
  spin_unlock(&fep->rx_lock);
  
  return !done;
}

static inline void enable_tx_interrupt(struct ipmux_fe_priv *fep)
{
  u32 val = 0;
  
  val = fe_readl(fep->dmac_regs + FE_INT_ENABLE);
  val |= FEDMA_TX_INT;
  fe_writel(fep->dmac_regs + FE_INT_ENABLE, val);
}

static inline void disable_tx_interrupt(struct ipmux_fe_priv *fep)
{
  u32 val= 0;
  
  val = fe_readl(fep->dmac_regs + FE_INT_ENABLE);
  val &=~FEDMA_TX_INT;
  fe_writel(fep->dmac_regs + FE_INT_ENABLE, val);
}

#ifdef WORKQ_MOD
#define TX_TIMEOUT (20*HZ/100)

void el_tx_timeout(struct net_device *ndev)
{
  struct ipmux_fe_priv *fep = netdev_priv(ndev);
  
  schedule_work(&fep->tx_wq);
}

void el_xmit_workq(struct work_struct *work)
{
  struct ipmux_fe_priv *fep = container_of(work, struct ipmux_fe_priv, tx_wq);
  int dev_id, flags;
  struct ipmux_fe_priv *feptmp;
  struct net_device *devtmp;
  
  dev_id = fep->ndev->dev_id;
  
#if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  if (dev_id < MAX_DMA1_INTERFACE)
  {
    devtmp = lanndev;
  }
  else
  {
    devtmp = wanndev;
  }
#else
  devtmp = lanndev;
#endif
  
  feptmp = netdev_priv(devtmp);
  
  spin_lock_irqsave(&feptmp->tx_lock, flags);
  
  while(feptmp->tx_tail != feptmp->tx_head)
  {
	if(feptmp->tx_buffers[feptmp->tx_tail].skb != NULL)
    {
      dev_kfree_skb_any(feptmp->tx_buffers[feptmp->tx_tail].skb);
      feptmp->tx_buffers[feptmp->tx_tail].skb = NULL;
    }

    /* to next Tx descriptor */
    feptmp->tx_tail = (feptmp->tx_tail + 1) % MAX_TX_DESC;
  }

  spin_unlock_irqrestore(&feptmp->tx_lock, flags);

  if (netif_queue_stopped(fep->ndev))
  {
  	netif_wake_queue(fep->ndev);
  }

  //enable_tx_interrupt(fep);
}

static void el_receive_workq(struct work_struct *work)
{
  int orig_budget = 64;
  int needReschedule = 0, flags;
  struct net_device *ndev;
  struct ipmux_fe_priv *fep = container_of(work, struct ipmux_fe_priv, rx_wq);
  
  ndev = fep->ndev;
  //spin_lock_irqsave(&fep->rx_lock, flags);
  needReschedule = dma_rx_receive(ndev, orig_budget);
  dprintk_rx("needReschedule=%d\n", needReschedule);
  if (needReschedule)
  {
    schedule_work(&fep->rx_wq);
  }
  else
  {
    enable_rx_interrupt(fep);
  }
  //spin_unlock_irqrestore(&fep->rx_lock, flags);
}
#endif

static irqreturn_t dma_onu_interrupt(int irq, void *dev_id)
{
  struct net_device *ndev = (struct net_device *)dev_id;
  struct ipmux_fe_priv *fep = netdev_priv(ndev);
  
  u32 status, flags;

  spin_lock_irqsave(&fep->lock, flags);
  
  status = fe_readl(fep->dmac_regs + FE_INT_STATUS);
  dprintk("ndevname=%s, status=%x\n", ndev->name, status);
  
  if (status & FEDMA_RX_INT || status & RXBD_FULL_INT)
  {
  #ifdef WORKQ_MOD
    disable_rx_interrupt(fep);
    schedule_work(&fep->rx_wq);
  #else
    if (netif_rx_schedule_prep(ndev))
    {
      disable_rx_interrupt(fep);
      __netif_rx_schedule(ndev);
    }
  #endif
  }
  
  if (status & FEDMA_TX_INT)
  {
  #ifdef WORKQ_MOD
    disable_tx_interrupt(fep);
    schedule_work(&fep->tx_wq);
  #else
    onu_tx_interrupt(fep);
  #endif
  }
  
  spin_unlock_irqrestore(&fep->lock, flags);
  
  return IRQ_HANDLED;
}

static int onu_open(struct net_device *dev)
{
  int retval = 0;
  unsigned long val = 0;
  struct ipmux_fe_priv *fep = netdev_priv(dev);
  
  if (fep->irq == OPL_FEDMA1_IRQ)
  {
    fep->tx_tail = fep->tx_head = fe_readl(fep->dmac_regs + FEDMA_TXBD_POINTER);
  }
  else
  {
    fep->tx_tail = fep->tx_head = fe_readl(fep->dmac_regs + CPDMA_TXBD_POINTER);
  }
  fep->rx_ring_read = 0;
  
#if defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  atheros_head.usVersionHi = 2;
  atheros_head.usPriority = 3;
  atheros_head.usPackType = 0;
  atheros_head.usFromCpu = 1;
  atheros_head.usPortNum = 0x1e;
#endif
  
  if ((HW_DMA0_INITED == 0) || (HW_DMA1_INITED == 0))
  {
  #ifdef WORKQ_MOD
    INIT_WORK(&fep->rx_wq, el_receive_workq);
    INIT_WORK(&fep->tx_wq, el_xmit_workq);
  #endif

    if ((fep->irq == OPL_FEDMA1_IRQ) && (HW_DMA1_INITED == 0))
    {
      retval = request_irq(fep->irq, &dma_onu_interrupt, IRQF_DISABLED/*IRQF_SHARED*/, "onu-fe", dev);
    }
    else if ((fep->irq == OPL_HOSTDMA0_IRQ) && (HW_DMA0_INITED == 0))
    {
      retval = request_irq(fep->irq, &dma_onu_interrupt, IRQF_DISABLED/*IRQF_SHARED*/, "onu-ge", dev);
    }
    
    if (retval)
    {
      printk( "failure to request IRQ %d\n", fep->irq);
      return retval;
    }
    
    enable_fe_dmac(fep);
    
    /*enable interrupt*/
    val = fe_readl(fep->dmac_regs + FE_INT_ENABLE);
    val |= ( RXBD_FULL_INT | FEDMA_RX_INT | FEDMA_TX_INT);
    fe_writel(fep->dmac_regs + FE_INT_ENABLE, val);
    
    if (fep->irq == OPL_FEDMA1_IRQ)
    {
      HW_DMA1_INITED = 1;
    }
    else
    {
      HW_DMA0_INITED = 1;
    }
  }
  netif_start_queue(dev);
  
  return 0;
}

static int onu_close(struct net_device *dev)
{
  struct ipmux_fe_priv *fep = netdev_priv(dev);
  
  fep->tx_head = 0;
  fep->tx_tail = 0;
  fep->rx_ring_read = 0;
  
  netif_stop_queue(dev); 
  free_irq(fep->irq, dev);
  disable_fe_dmac(fep);
  
  if (fep->irq == OPL_FEDMA1_IRQ)
  {
    HW_DMA1_INITED = 0;
  }
  else
  {
    HW_DMA0_INITED = 0;
  }
  
  return 0;
}

static int onu_set_mac(struct net_device *ndev, void *addr)
{
  struct sockaddr *address = addr;
  
  if (!is_valid_ether_addr(address->sa_data))
  {
    return -EADDRNOTAVAIL;
  }
  
  memcpy(ndev->dev_addr, address->sa_data, ndev->addr_len);
  if (ndev->dev_id < (MAX_INTETFACE_NUM/2))
  {
    memcpy((u8 *)fe_addr, (u8 *)ndev->dev_addr, ndev->addr_len);
  }
  else
  {
    memcpy((u8 *)ge_addr, (u8 *)ndev->dev_addr, ndev->addr_len);
  }
  
  printk("%s: Updated MAC address to %pM\n", ndev->name, ndev->dev_addr);
  
  return 0;
}

static int onu_set_mac_from_boot(char *line)
{
  char *ep;
  u32 i;
  
  for (i=0; i<6; i++)
  {
    fe_addr[i] = line ? simple_strtoul(line, &ep, 16) : 0;
    if (line)
    {
      line = (*ep) ? ep+1 : ep;
    }
  }
  printk("Updated LAN MAC address from u-boot: %pM\n",fe_addr);
  
  return 0;
}
__setup("mac=", onu_set_mac_from_boot);

static int onu_set_mac1_from_boot(char *line)
{
  char *ep;
  u32 i;
  
  for (i=0; i<6; i++)
  {
    ge_addr[i] = line ? simple_strtoul(line, &ep, 16) : 0;
    if (line)
    {
      line = (*ep) ? ep+1 : ep;
    }
  }
  printk("Updated WAN MAC address from u-boot: %pM\n",ge_addr);
  
  return 0;
}
__setup("mac1=", onu_set_mac1_from_boot);

/*read mac addr*/
static void read_mac_addr(struct net_device *ndev)
{
	if (ndev->dev_id < (MAX_INTETFACE_NUM/2))
	{
		memcpy((u8 *)ndev->dev_addr, (u8 *)fe_addr, 6);

		printk ("LAN MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
			ndev->dev_addr[0], ndev->dev_addr[1],
			ndev->dev_addr[2], ndev->dev_addr[3],
			ndev->dev_addr[4], ndev->dev_addr[5]);
	}
	else
	{
		memcpy((u8 *)ndev->dev_addr, (u8 *)ge_addr, 6);
		printk ("WAN MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
			ndev->dev_addr[0], ndev->dev_addr[1],
			ndev->dev_addr[2], ndev->dev_addr[3],
			ndev->dev_addr[4], ndev->dev_addr[5]);
	}

	if (!is_valid_ether_addr(ndev->dev_addr))
		dprintk("%s: Invalid ethernet MAC address. Please "
				"set using ifconfig\n", ndev->name);

}

/****************************************USER START****************************************/
void ResevMacInit(void)
{
#ifndef CONFIG_ONU_FE_VOIP_F23_ETH
#ifndef CONFIG_ONU_FE_RESPIN_F23_ETH
#ifndef CONFIG_ONU_FE_RESPIN_C13_ETH 
  u32 regdata1, regdata2;
  *(volatile u32*)0xbf00c0c0 = 0xFFFFFFFF;
  *(volatile u32*)0xbf00c0c4 = 0x0000FFFF;
  regdata2 = fe_addr[0]*0x100 + fe_addr[1];
  regdata1 = fe_addr[2]*0x1000000 + fe_addr[3]*0x10000 + fe_addr[4]*0x100 + fe_addr[5];
  *(volatile u32*)0xbf00c0c8 = regdata1;
  *(volatile u32*)0xbf00c0cc = regdata2;
  *(volatile u32*)0xbf00c0f4 = 0x00000808;
#endif
#endif
#endif
  
  *(volatile u32*)0xbf008000 = 0x00000003;
#ifdef CONFIG_ONU_F13P_VOIP_PCM_WDT_BOARD
  *(volatile u32*)(0xbf000000+0x909*4) = 0x0;  //1000m for f23,f13, 100m must be 0
#else
  *(volatile u32*)(0xbf000000+0x909*4) = 0x1;  //1000m for f23,f13, 100m must be 0
#endif
  
  /* CPDMA policing  */
  *(volatile u32*)0xbf0180cc = 10; //GE 320pps;
  *(volatile u32*)0xbf0180d0 = 10;
  *(volatile u32*)0xbf0180d4 = 19; //PON 640pps;
  *(volatile u32*)0xbf0180d8 = 10;
  *(volatile u32*)0xbf0180c0 = 0x03;

  /* FEDMA shaper */
  *(volatile u32*)0xbf01C044 = 1;
#if defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  *(volatile u32*)0xbf01C050 = 15; //fe 512pps for AR8228 N:1 aggregation
#else
  *(volatile u32*)0xbf01C050 = 30;
#endif
  *(volatile u32*)0xbf01C054 = 10;

  /* Since u-boot disabled FEMAC, we need to enable FEMAC in kernel */
#if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  *(volatile u32*)0xbf003400 = 0x3;
#endif
}

void DmaBufInit(u8 dmanum)
{
  int i;
  
  if (dmanum == 0)
  {
    init_waitqueue_head(&opl_host_dma0_waitq);
  }
  else
  {
    init_waitqueue_head(&opl_host_dma1_waitq);
  }
  dma_startnum[dmanum] = 0;
  dma_endnum[dmanum] = 0;
  
  memset((char *)&s_dmadata[dmanum][0], 0, sizeof(el_appdma_t)*H_MAXDMABUFNUM);
  
  for (i=0; i<(H_MAXDMABUFNUM-1); i++)
  {
    s_dmadata[dmanum][i].next = &s_dmadata[dmanum][i+1];
  }
  s_dmadata[dmanum][H_MAXDMABUFNUM].next = &s_dmadata[dmanum][0];
}

int getdmabufnum(u8 dmanum)
{
  if (((dma_endnum[dmanum]+1)==dma_startnum[dmanum]) || ((dma_endnum[dmanum]+1)==(dma_startnum[dmanum]+H_MAXDMABUFNUM)))
  {
    return 0;
  }
  
  if (dma_endnum[dmanum] < dma_startnum[dmanum])
  {
    return (dma_startnum[dmanum]-(dma_endnum[dmanum]+1));
  }
  else
  {
    return ((dma_startnum[dmanum]+H_MAXDMABUFNUM)-(dma_endnum[dmanum]+1));
  }
}

el_appdma_t *getdmabufaddress(u8 dmanum)
{
  int tmpnum;
  
  if (getdmabufnum(dmanum) == 0)
  {
    return NULL;
  }
  else
  {
    tmpnum = dma_endnum[dmanum];
    dma_endnum[dmanum] += 1;
    if (dma_endnum[dmanum] == H_MAXDMABUFNUM)
    {
      dma_endnum[dmanum] = 0;
    }
   return (&s_dmadata[dmanum][tmpnum]);
  }
}

int get_ipmux_dma_rx_buf_data(u8 dmanum, char *pbuf, int* plen)
{
  el_appdma_t *tmpbuf;
  
  if (dma_startnum[dmanum] != dma_endnum[dmanum])
  {
    tmpbuf = &s_dmadata[dmanum][dma_startnum[dmanum]];
    *plen = tmpbuf->len;
    
    copy_to_user(pbuf, tmpbuf, tmpbuf->len);
    dma_startnum[dmanum] += 1;
    if (dma_startnum[dmanum] == H_MAXDMABUFNUM)
    {
      dma_startnum[dmanum] = 0;
    }
    
    return 1;
  }
  *plen = 0;
  return 0;
}

int set_ipmux_dma_tx_buf_data(u8 dmanum, char *appbuf, int len)
{
  struct net_device *ndev;
//  if(dmanum == 1)
//  printk("Yeah!!!come on!!!\r\n");
#if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  if (1 == dmanum)
  {
    ndev = lanndev;
  }
  else
  {
    ndev = wanndev;
  }
#else
  ndev = lanndev;
#endif
  
  copy_from_user((char *)&TxBufdma[0], appbuf, len);
  onu_start_xmit_buf(ndev, TxBufdma, len);
  
  return 0;
}

void wait_for_host_dma1_interrupt(void)
{
  wait_event_interruptible(opl_host_dma1_waitq, opl_host_dma1_irq_event);
  test_and_clear_bit(0,&opl_host_dma1_irq_event);
}

int get_ipmux_dma1_rx_buf_data(char *pbuf, int* plen)
{
  return (get_ipmux_dma_rx_buf_data(1, pbuf, plen));
}

int set_ipmux_dma1_tx_buf_data(char *appbuf, int len)
{
  return (set_ipmux_dma_tx_buf_data(1, appbuf, len));
}

int get_ipmux_dma0_rx_buf_data(char *pbuf, int* plen)
{
  return (get_ipmux_dma_rx_buf_data(0, pbuf, plen));
}

int set_ipmux_dma0_tx_buf_data(char *appbuf, int len)
{
  return (set_ipmux_dma_tx_buf_data(0, appbuf, len));
}

EXPORT_SYMBOL(opl_host_dma0_waitq);
EXPORT_SYMBOL(opl_host_dma0_irq_event);
EXPORT_SYMBOL(get_ipmux_dma0_rx_buf_data);
EXPORT_SYMBOL(set_ipmux_dma0_tx_buf_data);

int copy_voicevlan_cfgdata(char *appbuf, int len)
{
  int port;
  VoiceVlanInfo_t tVoiceVlan;
  
  copy_from_user(&tVoiceVlan, appbuf, len);
  port = tVoiceVlan.VoicePort;
  if ((port<0) || (port>(MAX_INTETFACE_NUM-1)))
  {
    printk("copy_voicevlan_cfgdata port id %d return\n", port);
    
    return -1;
  }
  
  gSysVoiceVlan[port].VoicePort = port;
  gSysVoiceVlan[port].VoiceVlanCos = tVoiceVlan.VoiceVlanCos;
  gSysVoiceVlan[port].VoiceVlanId = tVoiceVlan.VoiceVlanId;
  gSysVoiceVlan[port].VoipTaggedValid = tVoiceVlan.VoipTaggedValid;
  gSysVoiceVlan[port].VoipTaggedValid = tVoiceVlan.VoipTaggedValid;
  gSysVoiceVlan[port].VoipQinQEn = tVoiceVlan.VoipQinQEn;
  gSysVoiceVlan[port].VoiceSVlanCos = tVoiceVlan.VoiceSVlanCos;
  gSysVoiceVlan[port].VoiceSVlanId = tVoiceVlan.VoiceSVlanId;
  dprintk("port=%d, VoiceVlanCos=%x, VoiceVlanId=%x, VoipTaggedValid=%x\n", port, tVoiceVlan.VoiceVlanCos,
          tVoiceVlan.VoiceVlanId, tVoiceVlan.VoipTaggedValid);
  
  return 0;
}
/****************************************USER END****************************************/
static int __init onu_fe_probe(struct platform_device *pdev)
{
  struct ipmux_fe_priv *fep;
  struct net_device *ndev;
  struct resource *pres;
  int ret = 0;
  int i;
  
#if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
  DmaBufInit(0);
  DmaBufInit(1);
#else
  DmaBufInit(0);
#endif
  ResevMacInit();
  
  for (i=0; i<MAX_INTETFACE_NUM; i++)
  {
    ndev = alloc_etherdev(sizeof (struct ipmux_fe_priv));
    if (ndev == NULL)
    {
      printk( "Unable to alloc new net device\n");
    }
    
    SET_MODULE_OWNER(ndev);
    SET_NETDEV_DEV(ndev, &pdev->dev);
    
    onundevs[i] = ndev;
    if (0 == i)
    {
      lanndev = ndev;
    }
    else if ((MAX_INTETFACE_NUM/2) == i)
    {
      wanndev = ndev;
    }
    
    fep = netdev_priv(ndev);
    memset(fep, 0, sizeof(struct ipmux_fe_priv));
    
    fep->ndev = ndev;
    
  #if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
    if (i < MAX_DMA1_INTERFACE)
    {
      pres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
      fep->dmac_regs = pres->start;
      
      pres = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
      fep->irq = pres->start;
    }
    else
    {
      pres = platform_get_resource(pdev, IORESOURCE_MEM, 1);
      fep->dmac_regs = pres->start;
       
      pres = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
      fep->irq = pres->start;
    }
  #else
    pres = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    fep->dmac_regs = pres->start;
    
    pres = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
    fep->irq = pres->start;
  #endif
    
    printk(KERN_INFO "%s Ethernet driver, V%s\n",MODULENAME, MODULEVERSION);
    printk(KERN_INFO "FE:dmac_regs =0x%x, irq=%d\n",fep->dmac_regs, fep->irq);
    
  #if defined  (CONFIG_ONU_FE_RESPIN_C13_ETH )||defined  (CONFIG_ONU_FE_RESPIN_F23_ETH )||defined(CONFIG_ONU_FE_VOIP_F23_ETH)
    if ((0 == i)  || (MAX_DMA1_INTERFACE == i))
    {
      onu_fe_init_dma(fep);
    }
  #else
    if (0 == i)
    {
      onu_fe_init_dma(fep);
    }
  #endif
  
    ndev->dev_id = i;
    
    read_mac_addr(ndev);
    
    spin_lock_init(&fep->tx_lock);
    spin_lock_init(&fep->rx_lock);
    spin_lock_init(&fep->lock);
    
    ether_setup(ndev);
    
    ndev->open = onu_open;
    ndev->hard_start_xmit = onu_start_xmit;
  #ifdef WORKQ_MOD
    ndev->watchdog_timeo = TX_TIMEOUT;
    ndev->tx_timeout = el_tx_timeout;
  #else
    ndev->watchdog_timeo = HZ;
    ndev->tx_timeout = onu_timeout;
  #endif
    ndev->stop = onu_close;
    ndev->get_stats = onu_get_stats;
    ndev->set_multicast_list = onu_multicast_list;
    ndev->do_ioctl = netdev_ioctl;
    ndev->set_mac_address = onu_set_mac;
    
  #ifdef WORKQ_MOD
    ndev->weight = 64;
  #else
    ndev->poll = dma_onu_poll;
    ndev->weight = 128;
  #endif
    
    //platform_set_drvdata(pdev, ndev);
    
    ret = register_netdev(ndev);
    if (ret != 0)
    {
      printk("ipmux: failed to register netdev.\n");
      goto failure;
    }
  }
  return 0;
  
failure:
  free_netdev(ndev);
  return ret;
} 

static void __exit onu_fe_remove(struct platform_device *pdev) 
{
  struct net_device *ndev;
  struct ipmux_fe_priv *fep;
  int i;
  
  for (i=0; i<MAX_INTETFACE_NUM; i++)
  {
    ndev = onundevs[i];
    unregister_netdev(ndev);
    kfree(ndev);
  }
  
  platform_set_drvdata(pdev, NULL);
}

static struct platform_driver onu_fe_driver = {
  .driver = {
  .name	= MODULENAME,
  .owner = THIS_MODULE,
  },
  .probe = onu_fe_probe,
  .remove = __devexit_p(onu_fe_remove),
};

static int __init onu_fe_init(void)
{
  return platform_driver_register(&onu_fe_driver);
}

static void __exit onu_fe_cleanup(void)
{
  platform_driver_unregister(&onu_fe_driver);
}

MODULE_AUTHOR("Figo.zhang <figo180@gmail.com>");
MODULE_DESCRIPTION("Opulan ONU FE Ethernet driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" MODULENAME);

module_init(onu_fe_init);
module_exit(onu_fe_cleanup);
