
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

#include "atheros_old.h"

#define D2_MOD   (1)

#ifdef D2_MOD
#include <linux/workqueue.h> 
#endif

#define MODULENAME	"onu_fe_ether"
#define MODULEVERSION	"1.00"

static unsigned int core_debug = 0;
module_param(core_debug, int, 0644);
MODULE_PARM_DESC(core_debug,"enable debug messages");
#define dprintk(fmt, arg...)	if (core_debug) \
	printk(KERN_EMERG " " fmt, ## arg)


#define MAX_RX_DESC 64
#define MAX_TX_DESC 64
#define TX_START_SHIFT 64
#define MAX_RXBUF_SIZE 0x800 /*2048 bytes*/
#define MAX_TXBUF_SIZE 0x800 /*2048 bytes*/

#define MAX_WAIT_CNT 0x2000

#ifdef D2_MOD
static struct net_device *devEther;
#endif

/**/
struct ipmux_skbuff {
	struct sk_buff	*skb; /*just for tx*/
	void *vaddr;
	dma_addr_t	dma_ptr;
	u32		length;
	u32 bd_info;
};

struct ipmux_fe_priv {
	struct net_device *ndev;

	struct resource *dmac_resource, *msg_resource;

	u32   dmac_regs; /*DMAC reg base addr*/
	u32   msg_regs;  /*MSG reg base addr*/

	int irq;
	spinlock_t lock;

	void *dma_vaddr;
	dma_addr_t dma_base;
	u32 order;

#ifdef D2_MOD
    /* workqueue */
    struct work_struct  rx_wq;
    struct work_struct  tx_wq;
#endif

	/*rx information*/
	struct ipmux_skbuff rx_buffers[MAX_RX_DESC];
	spinlock_t rx_lock;
	u32 rx_ring_read;

	/*tx information*/
	int tx_head;
	int tx_tail;
	struct ipmux_skbuff tx_buffers[MAX_TX_DESC];
	spinlock_t tx_lock;

	struct net_device_stats stats;
};


struct atheros_head_t {
	/* atheros 8228 header struct */
	u16 usVersionHi:2;
	u16 usPriority:2;
	u16 usPackType:4;
	u16 usFromCpu:1;
	u16 usPortNum:7;
};

static struct atheros_head_t atheros_head;
static u8 auc_broadcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static u8 g_ucportid = 0x1e;

static unsigned char fe_addr[6]={00,0xaa,0xbb,0xcc,0xcc,0xdd};

#define DRV_NAME    "onu_fe"
#define DRV_VERSION "2009/10/31"
static const char version[] =
DRV_NAME ".c: " DRV_VERSION " Figo.zhang (figo1802@gmail.com).\n";

#define fe_writel(reg, value) 	((*(volatile uint32_t *)(reg)) = value)
#define fe_readl(reg)			(*(volatile uint32_t *)( reg))

#define KSEG0_TO_KSEG1(x) ((unsigned long)(x)|0xa0000000)

#ifdef D2_MOD
#define FE_RX_MAX_PROCESS    (64)
#endif

/*----------------------------------------------*/

static void read_tx_bd(struct ipmux_fe_priv *fep, u32 index);
static void read_rx_bd(struct ipmux_fe_priv *fep, u32 index);

/*---------------------------------------------*/


static void printk_register(struct ipmux_fe_priv *fep)
{
#if 0
	int i;
	for(i = 0; i < DMAC_TF_TX_PKT_CNT; i+=4)
		dprintk("reg(0x%x)= 0x%x\n", i/4,fe_readl(fep->dmac_regs + i));
#endif
}

static void printk_bd(struct ipmux_fe_priv *fep)
{
	int i;	
#if 1
	for(i = 0; i< MAX_RX_DESC; i++){
		read_rx_bd(fep, i);
		dprintk("rx:i=%d, bd=0x%x\n", i, fep->rx_buffers[i].bd_info);
	}

	for(i = 0; i< MAX_TX_DESC; i++){
		read_tx_bd(fep, i);
		dprintk("tx:i=%d, bd =0x%x\n", i, fep->tx_buffers[i].bd_info);
	}
#endif
}

void print_mem(unsigned char *p,u32 len, u8 * s)
{
	u32 i;
#if 1
	printk("  %s   ram addr = %x , data len = %x",s, p, len);

	for (i=0;i<len;i++) {
		if (0==i%16) {
			printk("\n  0x%02x : ",(p+i));
		}
		printk("%02x",*(u8 *)(p+i));
		printk(" ");
	}

	printk("\n\n");
#endif

}

/*----------------------------------------------*/


static inline void enable_rx_interrupt(struct ipmux_fe_priv *fep)
{

	u32 val = 0;
	val = fe_readl(fep->dmac_regs + FE_INT_ENABLE);
	val |= ( RXBD_FULL_INT | FEDMA_RX_INT );
	fe_writel(fep->dmac_regs + FE_INT_ENABLE, val);

}

static inline void disable_rx_interrupt(struct ipmux_fe_priv *fep)
{

	u32 val= 0;
	val = fe_readl(fep->dmac_regs + FE_INT_ENABLE);
	val &=~ ( RXBD_FULL_INT | FEDMA_RX_INT );
	fe_writel(fep->dmac_regs + FE_INT_ENABLE, val);

}



static void disable_fe_dmac(struct ipmux_fe_priv *fep)
{
	fe_writel(fep->dmac_regs + FE_CONTRL, 0);
}

static void enable_fe_dmac(struct ipmux_fe_priv *fep)
{
	fe_writel(fep->dmac_regs + FE_CONTRL, 0x3);
}

static void read_tx_bd(struct ipmux_fe_priv *fep, u32 index)
{
	u32 i = 0;
	unsigned long flags;
	u32 ctrl = 0;

	spin_lock_irqsave(&fep->lock,flags);

	ctrl |= (TXBD_OP_BUSY | TXBD_OP_READ | ( index ));
	fe_writel(fep->dmac_regs + FE_TXBD_CTL, ctrl);

	while(fe_readl(fep->dmac_regs + FE_TXBD_CTL) & TXBD_OP_BUSY){
		i++;
		if(i > MAX_WAIT_CNT){
			printk(KERN_DEBUG "write bd index=%d time out\n", index);
			return;
		}
	}

	fep->tx_buffers[index].bd_info = fe_readl(fep->dmac_regs + FE_TXBD_SHADOW );

	spin_unlock_irqrestore(&fep->lock,flags);
}

static void write_tx_bd(struct ipmux_fe_priv *fep, u32 index)
{
	u32 i = 0;
	unsigned long flags;
	u32 ctrl = 0;

	fep->tx_buffers[index].bd_info |= (index+TX_START_SHIFT);

	if((index == MAX_TX_DESC -1 )){
		fep->tx_buffers[index].bd_info |= (BD_END_BIT);
	}

	spin_lock_irqsave(&fep->lock,flags);

	fe_writel(fep->dmac_regs + FE_TXBD_SHADOW, fep->tx_buffers[index].bd_info);

	ctrl |= (TXBD_OP_BUSY | TXBD_OP_WRITE |BD_TABLE_ID | (index ));
	fe_writel(fep->dmac_regs + FE_TXBD_CTL, ctrl);

	while(fe_readl(fep->dmac_regs + FE_TXBD_CTL)& TXBD_OP_BUSY){
		i++;
		if(i > MAX_WAIT_CNT){
			printk(KERN_DEBUG "write tx bd index=%d time out\n", index);
			return;
		}
	}

	spin_unlock_irqrestore(&fep->lock,flags);

}

static void read_rx_bd(struct ipmux_fe_priv *fep, u32 index)
{
	u32 i = 0;
	unsigned long flags;
	u32 ctrl = 0;

	spin_lock_irqsave(&fep->lock,flags);

	ctrl |= (RXBD_OP_BUSY | RXBD_OP_READ | index);
	fe_writel(fep->dmac_regs + FE_RXBD_CTL, ctrl);

	while(fe_readl(fep->dmac_regs + FE_RXBD_CTL)& RXBD_OP_BUSY){
		i++;
		if(i > MAX_WAIT_CNT){
			printk(KERN_DEBUG "write bd index=%d time out\n", index);
			return;
		}
	}
	fep->rx_buffers[index].bd_info = fe_readl(fep->dmac_regs + FE_RXBD_SHADOW );

	spin_unlock_irqrestore(&fep->lock,flags);
}

static void write_rx_bd(struct ipmux_fe_priv *fep, u32 index)
{
	u32 i = 0;
	unsigned long flags;
	u32 ctrl = 0;

	fep->rx_buffers[index].bd_info |= (BD_LENGTH_MASK << BD_LENGTH_SHIFT) | (index);

	if((index == MAX_RX_DESC -1 )){
		fep->rx_buffers[index].bd_info |= (BD_END_BIT);
	}

	spin_lock_irqsave(&fep->lock,flags);

	fe_writel(fep->dmac_regs + FE_RXBD_SHADOW, fep->rx_buffers[index].bd_info);

	ctrl |= (RXBD_OP_BUSY | RXBD_OP_WRITE|BD_TABLE_ID | index);
	fe_writel(fep->dmac_regs + FE_RXBD_CTL, ctrl);

	while(fe_readl(fep->dmac_regs + FE_RXBD_CTL)& RXBD_OP_BUSY){
		i++;
		if(i > MAX_WAIT_CNT){
			printk(KERN_DEBUG "write bd index=%d time out\n", index);
			return;
		}
	}

	spin_unlock_irqrestore(&fep->lock,flags);	 
}

/*return 1 is valid*/
static int check_rxbd_valid(struct ipmux_fe_priv *fep, u32 index)
{
	//read_bd(fep, bd, index);
	read_rx_bd(fep, index);

	return fep->rx_buffers[index].bd_info & BD_VALID_BIT;
}
#if 0
/*return 1 is valid*/
static int check_txbd_valid(struct ipmux_fe_priv *fep, u32 index)
{
	//read_bd(fep, bd, index + MAX_RX_DESC);
	read_tx_bd(fep, index);

	return (fep->tx_buffers[index].bd_info & BD_VALID_BIT );
}
#endif

static int onu_fe_init_dma(struct ipmux_fe_priv *fep)
{
	int i;
	u32  order,size,count;

	disable_fe_dmac(fep);
	udelay(10);

	/*clean interrupt status*/
	fe_readl(fep->dmac_regs + FE_INT_STATUS);
	
	/*init dma base addr 
	*/
#if 1
	size = MAX_RXBUF_SIZE*(MAX_RX_DESC+MAX_TX_DESC);
	fep->dma_vaddr = dma_alloc_coherent(NULL, size,
			&fep->dma_base, GFP_KERNEL);

	if (!fep->dma_vaddr) {
		printk("canot alloc coherent dma for rx\n");
		return -ENOMEM;
	}
	dprintk("dma_alloc_coherent::vaddr=0x%x,dma=0x%x\n", fep->dma_vaddr,fep->dma_base);

	fep->dma_base = KSEG0_TO_KSEG1(fep->dma_base);
	fep->dma_vaddr = KSEG0_TO_KSEG1(fep->dma_vaddr);
	memset(fep->dma_vaddr, 0, size);

#else
	size = MAX_RXBUF_SIZE*(MAX_RX_DESC+MAX_TX_DESC);
	for (count = PAGE_SIZE, order = 0; count < size; order++, count <<= 1)
		;
	fep->dma_vaddr =(char*) __get_free_pages(GFP_KERNEL|GFP_DMA, order);
	dprintk("__get_free_pages::vaddr=0x%x\n", fep->dma_vaddr);

	fep->dma_vaddr = KSEG0_TO_KSEG1(fep->dma_vaddr);
	fep->dma_base = fep->dma_vaddr ;
	fep->order = order;
	memset(fep->dma_vaddr, 0, size);

#endif

	printk("alloc dma done:vaddr=0x%x,dma=0x%x,size=%d\n", fep->dma_vaddr,fep->dma_base,
			MAX_RXBUF_SIZE*(MAX_RX_DESC +MAX_TX_DESC));

	/*write DMA base addr register*/
	fe_writel(fep->dmac_regs +FE_DMA_BASE_ADDR, fep->dma_base);

	for(i = 0; i< MAX_RX_DESC; i++){
		fep->rx_buffers[i].dma_ptr = fep->dma_base + MAX_RXBUF_SIZE * i;
		fep->rx_buffers[i].vaddr = fep->dma_vaddr + MAX_RXBUF_SIZE * i;

		dprintk("rx: index=%d, vaddr=0x%x\n",i,
				fep->rx_buffers[i].vaddr);
		write_rx_bd(fep, i);
	}


	for(i = 0; i< MAX_TX_DESC; i++){
		fep->tx_buffers[i].dma_ptr = fep->dma_base + MAX_RXBUF_SIZE*MAX_RX_DESC + MAX_TXBUF_SIZE * i;
		fep->tx_buffers[i].vaddr = fep->dma_vaddr + MAX_RXBUF_SIZE*MAX_RX_DESC + MAX_TXBUF_SIZE * i;

		dprintk("tx: index=%d, vaddr=0x%x\n",i,
				fep->tx_buffers[i].vaddr);
		write_tx_bd(fep, i);
	}

	dprintk("FE DMAC initial done\n");

#if 1
	printk_register(fep);
	printk_bd(fep);
#endif

	dprintk("enable fe dmac done\n");
}

static void onu_clean_dma(struct ipmux_fe_priv *fep)
{
	/*clean any pending Tx packets*/
#if 1
	dma_free_coherent(NULL, MAX_RXBUF_SIZE*(MAX_RX_DESC+MAX_TX_DESC),
			fep->dma_vaddr, fep->dma_base);
#else
	if(fep->dma_vaddr)
		__free_pages(fep->dma_vaddr, fep->order);
#endif

}

/*----------------------------------------------*/



static void onu_timeout(struct net_device *ndev)
{
	netif_wake_queue(ndev);
}

static void onu_multicast_list(struct net_device *dev)
{
}

static struct net_device_stats *onu_get_stats(struct net_device *ndev)
{
	struct ipmux_fe_priv *fep = netdev_priv(ndev);
#if 0
	fep->stats.rx_packets = fe_readl(fep->dmac_regs + DMAC_MSG_RX_PKT_CNT);
	fep->stats.tx_packets = fe_readl(fep->dmac_regs + DMAC_MSG_TX_PKT_CNT);
	fep->stats.rx_dropped = fe_readl(fep->dmac_regs + DMAC_MSG_RX_DRP_PKT);
#endif
	return &fep->stats;
}

/**
 * netdev_ethtool_ioctl: Handle network interface SIOCETHTOOL ioctls
 * @dev: network interface on which out-of-band action is to be performed
 * @useraddr: userspace address to which data is to be read and returned
 *
 * Process the various commands of the SIOCETHTOOL interface.
 */

static int netdev_ethtool_ioctl (struct net_device *ndev, void *useraddr)
{
	u32 ethcmd;

	/* dev_ioctl() in ../../net/core/dev.c has already checked
	   capable(CAP_NET_ADMIN), so don't bother with that here.  */

	if (get_user(ethcmd, (u32 *)useraddr))
		return -EFAULT;

	switch (ethcmd) {

		case ETHTOOL_GDRVINFO: {
					       struct ethtool_drvinfo info = { ETHTOOL_GDRVINFO };
					       strcpy (info.driver, DRV_NAME);
					       strcpy (info.version, DRV_VERSION);
					       sprintf(info.bus_info, "ISA 0x%lx", ndev->base_addr);
					       if (copy_to_user (useraddr, &info, sizeof (info)))
						       return -EFAULT;
					       return 0;
				       }

				       /* get message-level */
		case ETHTOOL_GMSGLVL: {
					      struct ethtool_value edata = {ETHTOOL_GMSGLVL};
					      //edata.data = debug;
					      if (copy_to_user(useraddr, &edata, sizeof(edata)))
						      return -EFAULT;
					      return 0;
				      }
				      /* set message-level */
		case ETHTOOL_SMSGLVL: {
					      struct ethtool_value edata;
					      if (copy_from_user(&edata, useraddr, sizeof(edata)))
						      return -EFAULT;
					      // debug = edata.data;
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

	switch (cmd) {
		case SIOCETHTOOL:
			rc = netdev_ethtool_ioctl(dev, (void *) rq->ifr_data);
			break;
		default:
			rc = -EOPNOTSUPP;
			break;
	}

	return rc;
}

static u32  atheros_tx_add_head( void *buf, void *skb_buf, int len)
{
	//u8 *buf_tmp = (u8 *)&atheros_head;

	memcpy((u8 *)buf, (u8 *)skb_buf, 12);

	if(0 == memcmp(auc_broadcast_mac, (u8 *)skb_buf, 6)){
		atheros_head.usPortNum = 0x1e;
	} else {
		atheros_head.usPortNum = 1<< g_ucportid;
	}

	//printk("usport=0x%x,atheros_head = 0x%x, 0x%x\n", atheros_head.usPortNum, *buf_tmp, *(buf_tmp +1));
	memcpy((u8 *)(buf+12), (u8 *)&atheros_head, 2);
	memcpy((u8 *)(buf+14), (u8 *)(skb_buf+12), len-12);

	return (len+2);

}

int onu_start_xmit_buf(struct net_device *ndev, u8 * buf, u32 len)
{
	struct ipmux_fe_priv *fep = netdev_priv(ndev);
	u32 index, status= 0, i;
	unsigned char *buffer;
	unsigned long flags;

	spin_lock_irqsave(&fep->tx_lock, flags);

	index = fep->tx_head;
	buffer = (unsigned char *)fep->tx_buffers[index].vaddr;

	dprintk("%s head=%d, tail=%d, skb->len=%d\n", __FUNCTION__, index, fep->tx_tail, len);

	/*wait for DMA ready for send*/
	read_tx_bd(fep, index);
	while(fep->tx_buffers[index].bd_info & BD_VALID_BIT){
		read_tx_bd(fep, index);
		i++;
		if(i > MAX_WAIT_CNT){
			dprintk("dma not ready for send\n");
			goto fail;
		}
	}

	memcpy((unsigned char *)buffer, (unsigned char *)buf, len);
	/*write bd to reg, and start DMA*/
	status |= ( (len<< BD_LENGTH_SHIFT ) | BD_VALID_BIT);
	fep->tx_buffers[index].bd_info = status;
	write_tx_bd(fep, index);

	dprintk("write BD, status=0x%x,start dma send\n" ,fep->tx_buffers[index].bd_info);

	fep->tx_buffers[index].length = len;
	/*wait for DMA send ok*/
	read_tx_bd(fep, index);
	while(fep->tx_buffers[index].bd_info & BD_VALID_BIT){
		read_tx_bd(fep, index);
		i++;
		if(i > MAX_WAIT_CNT){
			dprintk("dma send fail\n");
			goto fail;
		}
	}
	dprintk("dma send ok, status =0x%x\n",fep->tx_buffers[index].bd_info);

	/*update for avail pointer*/
	fep->tx_head = (index+1) % MAX_TX_DESC;

fail:
	spin_unlock_irqrestore(&fep->tx_lock, flags);
	return NETDEV_TX_BUSY;	
}

EXPORT_SYMBOL(onu_start_xmit_buf);

static int onu_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	struct ipmux_fe_priv *fep = netdev_priv(ndev);
	u32 index, i = 0, status= 0;
	unsigned char *buf;
	unsigned long flags, length;

	spin_lock_irqsave(&fep->tx_lock, flags);

	index = fep->tx_head;

	buf = (unsigned char *)fep->tx_buffers[index].vaddr;
	length = skb->len;

	dprintk("start xmit head=%d, tail=%d, skb->len=%d\n", index, fep->tx_tail, length);

	/*wait for DMA ready for send*/
	read_tx_bd(fep, index);
	while(fep->tx_buffers[index].bd_info & BD_VALID_BIT){
		read_tx_bd(fep, index);
		i++;
		if(i > MAX_WAIT_CNT){
			dprintk("dma not ready for send\n");
			goto fail;
		}
	}

#ifdef CONFIG_ATHEROS_SWITCH
	length = atheros_tx_add_head((unsigned char *)buf, (unsigned char *)skb->data, length);
	dprintk("start tx atheros lenght: %d\n", length);
#else
	memcpy((unsigned char *)buf, (unsigned char *)skb->data, length);
#endif

	/*write bd to reg, and start DMA*/
	status |= ( (length<< BD_LENGTH_SHIFT ) | BD_VALID_BIT);
	fep->tx_buffers[index].bd_info = status;
	write_tx_bd(fep, index);

	dprintk("write BD, status=0x%x,start dma send\n" ,fep->tx_buffers[index].bd_info);

	fep->tx_buffers[index].skb = skb;
	fep->tx_buffers[index].length = length;

#if 0
	/*wait for DMA send ok*/
	read_tx_bd(fep, index);
	while(fep->tx_buffers[index].bd_info & BD_VALID_BIT){
		read_tx_bd(fep, index);
		i++;
		if(i > MAX_WAIT_CNT){
			dprintk("dma send fail\n");
			goto fail;
		}
	}
	dprintk("dma send ok, status =0x%x\n",fep->tx_buffers[index].bd_info);
#endif
#if 0
	print_mem(buf, length, "tx");
#endif

	//dev_kfree_skb(skb);

	/*update for avail pointer*/
	fep->tx_head = (index+1) % MAX_TX_DESC;

	if(fep->tx_head == fep->tx_tail)
		netif_stop_queue(ndev);

	ndev->trans_start = jiffies;

	spin_unlock_irqrestore(&fep->tx_lock, flags);
	return NETDEV_TX_OK;

fail:
	spin_unlock_irqrestore(&fep->tx_lock, flags);
	return NETDEV_TX_BUSY;
}

static void onu_tx_interrupt(struct ipmux_fe_priv *fep)
{
	unsigned tx_head = fep->tx_head;
	unsigned tx_tail = fep->tx_tail;

	//dprintk("%s :: tx_head=%d, tx_tail=%d\n",__FUNCTION__, tx_head, tx_tail);

	while(tx_tail != tx_head){
		fep->stats.tx_packets++;
		fep->stats.tx_bytes += fep->tx_buffers[tx_tail].length;

		dev_kfree_skb_irq(fep->tx_buffers[tx_tail].skb);

		dprintk("have a tx packet %d xmit done\n", tx_tail);

		//print_mem(fep->tx_buffers[tx_tail].vaddr, fep->tx_buffers[tx_tail].length, "tx");

		/* to next Tx descriptor */
		tx_tail = (tx_tail + 1) % MAX_TX_DESC;
	}

	fep->tx_tail= tx_tail;

	dprintk("%s :: tx_head=%d, tx_tail=%d\n",__FUNCTION__, tx_head, fep->tx_tail);

	/*if netif queue stop ,wake it up!!!*/
	if (netif_queue_stopped(fep->ndev))
		netif_wake_queue(fep->ndev);

}



static int find_valid_rx(struct ipmux_fe_priv *fep, u32 index, u32 *valid_index)
{
	int i;
	for(i = index; i< MAX_RX_DESC + index; i++){
		if(check_rxbd_valid(fep, i% MAX_RX_DESC)){
			*valid_index = i%MAX_RX_DESC;
			//dprintk("find_valid_rx:: index=%d\n",*valid_index);
			return 1;
		}
	}

	return 0;
}


/*
*/
static int atheros_rx_add_head(struct net_device *ndev, u8 *skb_buf, u8 *buf, int len)
{
	struct atheros_head_t *atheros_head_tx = NULL;

	memcpy((u8 *)skb_buf, (u8*)buf, 12);

	atheros_head_tx = (struct atheros_head_t  *)(buf+12);
	if((0== memcmp(auc_broadcast_mac, (u8 *)buf, 6)) || 
			0 == memcmp((u8*)ndev->dev_addr, (u8*)buf, 6)){

		g_ucportid = atheros_head_tx->usPortNum;
		memcpy((u8 *)(skb_buf+12), (u8 *)(buf+14), len-14);

	}else{
		el_apptest_t *p_s_apptest=NULL;
              if ((p_s_apptest=gettestbufaddress()) != NULL){
                  p_s_apptest->len = len;
                  memcpy ((char *)&(p_s_apptest->data[0]), (char *)buf, len);
                  wake_up_interruptible(&opl_host_el_waitq);
                }
	}

	return 0;
	
}

static int rx_receive(struct net_device *ndev, int budget)
{
	
#ifdef D2_MOD
    int reschedule = 0;  
    unsigned long flags;
#endif   
        int received = 0;
	int pkt_len;
	int rev;
	struct sk_buff *skb;
	void *pfe_pkt, *dma_buf, *debug_buf;
	u32 status, valid_index;
	struct ipmux_fe_priv *fep = netdev_priv(ndev);
    
	u32 index = fep->rx_ring_read;
#ifdef D2_MOD
        while(find_valid_rx(fep, index, &valid_index)){
#else
	while(received < budget && find_valid_rx(fep, index, &valid_index)){
#endif
		index = valid_index;

		read_rx_bd(fep, index);
		if(!(fep->rx_buffers[index].bd_info & BD_VALID_BIT)){
			dprintk("invalid packed index=%d\n", index);
			goto done;
		}

		status = fep->rx_buffers[index].bd_info;
		dma_buf = debug_buf = (u8 *)fep->rx_buffers[index].vaddr;

		pkt_len = (status >> BD_LENGTH_SHIFT) & BD_LENGTH_MASK;

		dprintk("index=%d; status=0x%x, pkt_len=%d\n", index, status, pkt_len);

		if (pkt_len < 60)
		{
			printk(KERN_DEBUG " bogus packet, length=%d\n",  pkt_len);
			goto done;

		}
		else
		{
#ifdef D2_MOD
                        skb = __dev_alloc_skb(pkt_len+2,GFP_DMA|GFP_ATOMIC);
                        if( skb == NULL) {
                            printk(KERN_ERR "skb not available...\n");
                            reschedule = 1;
                            break;
                        }
#else
                        skb = dev_alloc_skb(pkt_len+2);
#endif
			skb_reserve(skb,2);
			skb->dev = ndev;
#ifdef D2_MOD
            spin_lock_irqsave(&fep->lock,flags);
#endif
#ifdef CONFIG_ATHEROS_SWITCH
			pfe_pkt = skb_put(skb,pkt_len);
			rev = atheros_rx_add_head(ndev, (u8 *)pfe_pkt , (u8 *)dma_buf, 
					pkt_len);
#else
			memcpy(skb_put(skb,pkt_len), (unsigned char *)fep->rx_buffers[index].vaddr, pkt_len );
#endif

			skb->protocol=eth_type_trans(skb,ndev);
#ifdef D2_MOD
                        netif_rx(skb);
#else
                       netif_receive_skb(skb);
#endif
			//dprintk( "receive skb --- \n");
		}
#ifdef D2_MOD
        spin_unlock_irqrestore(&fep->lock,flags);
#endif
#if 0
		print_mem(debug_buf, pkt_len, "rx");
#endif

#ifdef D2_MOD
        if (received++ > MAX_RX_DESC) {
            reschedule = 1;
            break;
        } 
#else
                received++;
#endif
		ndev->last_rx = jiffies;
		fep->stats.rx_packets++;
		fep->stats.rx_bytes+=pkt_len;

done:
		/*let this BD invalid*/
		fep->rx_buffers[index].bd_info &=~ BD_VALID_BIT;
		write_rx_bd(fep, index);
		index = (index+1) % MAX_RX_DESC;
	}

	fep->rx_ring_read = index;

	return received;
}

static int onu_poll(struct net_device *ndev, int *budget)
{
	int orig_budget = min(*budget, ndev->quota);
	int done = 1;
	int work_done;

	struct ipmux_fe_priv *fep = netdev_priv(ndev);

	spin_lock(&fep->lock);
	work_done = rx_receive(ndev, orig_budget);
	dprintk( "work_done =%d, orig_budget=%d\n", work_done, orig_budget);
	if (likely(work_done > 0)) {
		*budget -= work_done;
		ndev->quota -= work_done;
		done = (work_done < orig_budget);
	}

	if(done){
		unsigned long flags;
		/*
		   open interrupt
		   */
		local_irq_save(flags);
		__netif_rx_complete(ndev);
		enable_rx_interrupt(fep);
		dprintk("__netif_rx_complete\n");
		local_irq_restore(flags);
	}

	spin_unlock(&fep->lock);

	return !done;
}

#ifdef D2_MOD
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

#define TX_TIMEOUT (20*HZ/100)
void el_tx_timeout(struct net_device *ndev)
{
    struct ipmux_fe_priv *fep = netdev_priv(ndev);

    schedule_work(&fep->tx_wq);
}

void el_xmit_workq(struct work_struct *work)
{
    struct net_device *ndev = devEther;
    struct ipmux_fe_priv *fep = netdev_priv(ndev);

    unsigned tx_head = fep->tx_head;
    unsigned tx_tail = fep->tx_tail;

    while(tx_tail != tx_head){
        fep->stats.tx_packets++;
        fep->stats.tx_bytes += fep->tx_buffers[tx_tail].length;

        dev_kfree_skb_irq(fep->tx_buffers[tx_tail].skb);

        dprintk("have a tx packet %d xmit done\n", tx_tail);
        /* to next Tx descriptor */
        tx_tail = (tx_tail + 1) % MAX_TX_DESC;
    }

    fep->tx_tail= tx_tail;
#if 1
    /*if netif queue stop ,wake it up!!!*/
    if (netif_queue_stopped(fep->ndev))
        netif_wake_queue(fep->ndev);
#else
    enable_tx_interrupt(fep);
#endif
}


static void el_receive_workq(struct work_struct *work)
{
    int orig_budget = 64;
    int work_done;
    u32 val;
    unsigned long flags;    
    int needReschedule = 0;
    struct net_device *ndev = devEther;

    struct ipmux_fe_priv *fep = netdev_priv(ndev);

    spin_lock_irqsave(&fep->lock, flags);
    needReschedule = rx_receive(ndev, orig_budget);

    dprintk( "work_done =%d, orig_budget=%d\n", work_done, orig_budget); 

    if (needReschedule) {
        schedule_work(&fep->rx_wq);
    }    
    else {    
        enable_rx_interrupt(fep);
    }
    spin_unlock_irqrestore(&fep->lock, flags);
}
#endif

static irqreturn_t onu_interrupt(int irq, void *dev_id)
{
	struct net_device *ndev = (struct net_device *)dev_id;
	struct ipmux_fe_priv *fep = netdev_priv(ndev);

	u32 status, flags;

	spin_lock_irqsave(&fep->lock, flags);

	status = fe_readl(fep->dmac_regs + FE_INT_STATUS);

	dprintk("%s, status=0x%x\n", __FUNCTION__, status);

	if(status & FEDMA_RX_INT || status & RXBD_FULL_INT){
#ifdef D2_MOD
        disable_rx_interrupt(fep);
        schedule_work(&fep->rx_wq);
#else        
		if (netif_rx_schedule_prep(ndev)) {
			disable_rx_interrupt(fep);
			__netif_rx_schedule(ndev);
			dprintk("__netif_rx_schedule\n");
		}
#endif        
	}

	if(status & FEDMA_TX_INT){
#ifdef D2_MOD
        disable_tx_interrupt(fep);
        schedule_work(&fep->tx_wq);
#else
	    dprintk("have tx interrupt--\n");
	    onu_tx_interrupt(fep);
#endif
	}

	spin_unlock_irqrestore(&fep->lock, flags);
	return IRQ_HANDLED;
}

static int onu_open(struct net_device *dev)
{
	int retval;
	unsigned long val = 0;
	struct ipmux_fe_priv *fep = netdev_priv(dev);
#ifdef D2_MOD
    unsigned int flags;
#endif
	fep->tx_tail = fep->tx_head = fe_readl(fep->dmac_regs + FEDMA_TXBD_POINTER);
	//fep->tx_tail= 0;
	fep->rx_ring_read = 0;

#ifdef CONFIG_ATHEROS_SWITCH
	atheros_head.usVersionHi = 2;
	atheros_head.usPriority = 3;
	atheros_head.usPackType = 0;
	atheros_head.usFromCpu = 1;
	atheros_head.usPortNum = 0x1e;  
	//u8 *buf_tmp = (u8 *)&atheros_head;
	//printk("atheros_head = 0x%x, 0x%x\n", *buf_tmp, *(buf_tmp +1));
	opl_host_el_init(dev);
#endif

	/*regist irq*/
    spin_lock_irqsave(&fep->lock, flags);
	retval = request_irq(fep->irq, &onu_interrupt, IRQF_DISABLED/*IRQF_SHARED*/, "onu-fe", dev);
	if (retval) {
		dprintk( "failure to request IRQ %d\n", fep->irq);
		return retval;
	} 
	dprintk("requst irq %d done\n", fep->irq);

	enable_fe_dmac(fep);
	dprintk("enable dmac fe done\n");

	/*enable interrupt*/
	val = fe_readl(fep->dmac_regs + FE_INT_ENABLE);
	val |= ( RXBD_FULL_INT |FEDMA_RX_INT | FEDMA_TX_INT);
	fe_writel(fep->dmac_regs + FE_INT_ENABLE, val);

	dprintk("enable interrupt done\n");

#ifdef D2_MOD
    INIT_WORK(&fep->rx_wq, el_receive_workq);
    INIT_WORK(&fep->tx_wq, el_xmit_workq);

#endif
	netif_start_queue(dev);
#ifdef D2_MOD
    spin_unlock_irqrestore(&fep->lock, flags);
#endif
	dprintk("open finish\n");

	return 0;
}

static int onu_close(struct net_device *dev)
{
	struct ipmux_fe_priv *fep = netdev_priv(dev);

	fep->tx_head = 0;
	fep->tx_tail= 0;
	fep->rx_ring_read = 0;

	netif_stop_queue(dev); 
	free_irq(fep->irq, dev);

	disable_fe_dmac(fep);

	return 0;
}

static int onu_set_mac(struct net_device *ndev, void *addr)
{
	struct sockaddr *address = addr;

	if (!is_valid_ether_addr(address->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(ndev->dev_addr, address->sa_data, ndev->addr_len);
	memcpy((u8 *)fe_addr, (u8 *)ndev->dev_addr, ndev->addr_len);

	printk("%s: Updated MAC address to %pM\n",
			ndev->name, ndev->dev_addr);

	return 0;
}

static int onu_set_mac_from_boot(char *line)
{
	char *ep;
	u32 i;

	/* there should really be routines to do this stuff */
	for (i = 0; i < 6; i++)
	{
		fe_addr[i] = line  ? simple_strtoul(line, &ep, 16) : 0;
		if (line)
			line = (*ep) ? ep+1 : ep;
	}

	printk("Updated MAC address from u-boot: %pM\n",fe_addr);

	return 0;
}
__setup("mac=", onu_set_mac_from_boot);


/*read mac addr*/
static void read_mac_addr(struct net_device *ndev)
{
	memcpy((u8 *)ndev->dev_addr, (u8 *)fe_addr, 6);

	printk ("FE MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
			ndev->dev_addr[0], ndev->dev_addr[1],
			ndev->dev_addr[2], ndev->dev_addr[3],
			ndev->dev_addr[4], ndev->dev_addr[5]);

	if (!is_valid_ether_addr(ndev->dev_addr))
		dprintk("%s: Invalid ethernet MAC address. Please "
				"set using ifconfig\n", ndev->name);

}

static int __init onu_fe_probe(struct platform_device *pdev)
{
	struct ipmux_fe_priv *fep;
	struct net_device *ndev;
	struct resource *dmac_res;
	int ret = 0;

	dprintk("%s in\n", __FUNCTION__);

	/* dev and priv zeroed in alloc_etherdev */
	ndev = alloc_etherdev (sizeof (struct ipmux_fe_priv));
	if (ndev == NULL) {
		dprintk( "Unable to alloc new net device\n");
		return -ENOMEM;
	}

	SET_MODULE_OWNER(ndev);
	SET_NETDEV_DEV(ndev, &pdev->dev);

	/* Configure our private structure a little */
	fep = netdev_priv(ndev);
	memset(fep, 0, sizeof(struct ipmux_fe_priv));

	dprintk("onu_fe_probe in\n");

	//ndev->name = "fe eth";
	fep->ndev = ndev;

	/*get DMAC reg base addr*/
	dmac_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	fep->dmac_regs = dmac_res->start;

	/*get FE irq*/
	fep->irq = platform_get_irq(pdev, 0);

	printk(KERN_INFO "%s Ethernet driver, V%s\n",
			MODULENAME, MODULEVERSION);
	printk(KERN_INFO "FE:dmac_regs =0x%x, irq=%d\n",fep->dmac_regs, fep->irq);

	onu_fe_init_dma(fep);

	/*set mac addr*/
	read_mac_addr(ndev);

	spin_lock_init(&fep->tx_lock);
	spin_lock_init(&fep->rx_lock);
	spin_lock_init(&fep->lock);

	/* driver system function */
	ether_setup(ndev);

	ndev->open = onu_open;
	ndev->hard_start_xmit = onu_start_xmit;
	ndev->tx_timeout = onu_timeout;
#ifdef D2_MOD
	ndev->watchdog_timeo = TX_TIMEOUT;
#else
	ndev->watchdog_timeo = HZ;
#endif
	ndev->stop = onu_close;
	ndev->get_stats = onu_get_stats;
	ndev->set_multicast_list = onu_multicast_list;
	ndev->do_ioctl = netdev_ioctl;
	ndev->set_mac_address = onu_set_mac;
#ifdef D2_MOD
	ndev->weight = 64;
#else
	ndev->poll = onu_poll;
	ndev->weight = 128;
#endif

	platform_set_drvdata(pdev, ndev);
	ret = register_netdev(ndev);
	if (ret == 0) {
		dprintk( "ipmux ethernet  MAC M\n");
	} else {
		/* Report the failure to register the net_device */
		dprintk("ipmux: failed to register netdev.\n");
		goto failure;
	}

#ifdef D2_MOD
    devEther=ndev;
#endif

	return 0;

failure:

	free_netdev(ndev);
	return ret;
}

static void __exit onu_fe_remove(struct platform_device *pdev) 
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct ipmux_fe_priv *fep = netdev_priv(ndev);

	onu_clean_dma(fep);

	platform_set_drvdata(pdev, NULL);

	unregister_netdev(ndev);

	kfree(ndev);		/* free device structure */

	printk(KERN_INFO "%s:clean_module() exit\n",__FUNCTION__);
}

static struct platform_driver onu_fe_driver = {
	.driver = {
		.name	= MODULENAME,
		.owner	= THIS_MODULE,
	},
	.probe		= onu_fe_probe,
	.remove		= __devexit_p(onu_fe_remove),
};

/* Module interface */
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

