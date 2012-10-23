
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

#include <asm/mach-optrann/optrann_reg.h>
#include <asm/mach-optrann/optrann_irq.h>

#include "optrann.h"

#define MODULENAME	"optrann_fe_ether"
#define MODULEVERSION	"1.00"


static unsigned int core_debug = 0;
module_param(core_debug, int, 0644);
MODULE_PARM_DESC(core_debug,"enable debug messages");
#define dprintk(fmt, arg...)	if (core_debug) \
	printk(KERN_EMERG " " fmt, ## arg)

#define DRV_NAME    "optrann_fe"
#define DRV_VERSION "2009/10/31"
static const char version[] =
DRV_NAME ".c: " DRV_VERSION " Figo.zhang (tfzhang@opulan.com).\n";

/*for pass TF interrupt status fo app*/
#define TF_NO_INT  0
#define TF_HAVE_INT 1

static u32 opl_interrupt_status;
static u32 opl_tf_int_status = TF_NO_INT;
static wait_queue_head_t opl_tf_int_wq;


/*----------------------------------------------*/

static void read_tx_bd(struct ipmux_fe_priv *fep, u32 index);
static void read_rx_bd(struct ipmux_fe_priv *fep, u32 index);
static void read_bd(struct ipmux_fe_priv *fep, struct bd_desc *bd, u32 index);
static void write_bd(struct ipmux_fe_priv *fep, struct bd_desc *bd, u32 index);

/*---------------------------------------------*/


static void printk_register(struct ipmux_fe_priv *fep)
{
	int i;
	for(i = 0; i < DMAC_TF_TX_PKT_CNT; i+=4)
		dprintk("reg(0x%x)= 0x%x\n", i/4,fe_readl(fep->dmac_regs + i));
}

static void printk_bd(struct ipmux_fe_priv *fep)
{
	int i;	

	for(i = 0; i< MAX_RX_DESC; i++){
		read_rx_bd(fep, i);
		dprintk("rx:i=%d, addr=0x%x, status=0x%x\n", i, fep->rx_buffers[i].bd_info.addr,
				fep->rx_buffers[i].bd_info.status);
	}

	for(i = 0; i< MAX_TX_DESC; i++){
		read_tx_bd(fep, i);
		dprintk("tx:i=%d, addr=0x%x, status=0x%x\n", i, fep->tx_buffers[i].bd_info.addr,
				fep->tx_buffers[i].bd_info.status);
	}
}

void print_mem(unsigned char *p,u32 len)
{
	u32 i;

	printk("     ram addr = %x , data len = %x",p,len);

	for (i=0;i<len;i++) {
		if (0==i%16) {
			printk("\n  0x%02x : ",(p+i));
		}
		printk("%02x",*(u8 *)(p+i));
		printk(" ");
	}

	printk("\n\n");

}

/*----------------------------------------------*/
static inline void enable_rx_interrupt(struct ipmux_fe_priv *fep)
{

	u32 val = 0;
	val = fe_readl(fep->dmac_regs + DMAC_INT_ENABLE);
	val |= (MSG_RX_FULL_INT_ENABLE | MSG_RX_INT_ENABLE);
	fe_writel(fep->dmac_regs + DMAC_INT_ENABLE, val);

}

static inline void disable_rx_interrupt(struct ipmux_fe_priv *fep)
{

	u32 val= 0;
	val = fe_readl(fep->dmac_regs + DMAC_INT_ENABLE);
	val &=~ (MSG_RX_FULL_INT_ENABLE| MSG_RX_INT_ENABLE);
	fe_writel(fep->dmac_regs + DMAC_INT_ENABLE, val);

}


static void disable_fe_dmac(struct ipmux_fe_priv *fep)
{
	fe_writel(fep->dmac_regs + DMAC_MSG_CFG, 0);
}

static void enable_fe_dmac(struct ipmux_fe_priv *fep)
{
	fe_writel(fep->dmac_regs + DMAC_MSG_CFG, 0x1);
}

void optrann_read_bd(struct ipmux_fe_priv *fep, struct bd_desc *bd, u32 index)
{
	u32 i = 0;
	u32 ctrl = 0;

	unsigned long flags;
	spin_lock_irqsave(&fep->lock,flags);

	//ctrl = fe_readl(fep->dmac_regs + DMAC_TBL_ACC_CTL);
	ctrl |= (TBL_BUSY | TBL_ACC_MODE_READ| (bd->table_id <<TBL_ID_SHIFT) | index);
	fe_writel(fep->dmac_regs + DMAC_TBL_ACC_CTL, ctrl);

	/*wait for ready*/
	while(fe_readl(fep->dmac_regs + DMAC_TBL_ACC_CTL) 
			& TBL_BUSY){
		i++;
		if(i > MAX_WAIT_CNT){
			printk(KERN_DEBUG "read bd index=%d time out\n", index);
			return;
		}
	}

	bd->addr = fe_readl(fep->dmac_regs + DMAC_TBL_ACC_DATA_0);
	bd->status = fe_readl(fep->dmac_regs + DMAC_TBL_ACC_DATA_1);

	spin_unlock_irqrestore(&fep->lock,flags);
}
EXPORT_SYMBOL(optrann_read_bd);

void optrann_write_bd(struct ipmux_fe_priv *fep, struct bd_desc *bd, u32 index)
{
	u32 i = 0;
	u32 ctrl = 0, val = 0;
	unsigned long flags;

	spin_lock_irqsave(&fep->lock,flags);

	fe_writel(fep->dmac_regs + DMAC_TBL_ACC_DATA_0, bd->addr);
	fe_writel(fep->dmac_regs + DMAC_TBL_ACC_DATA_1, bd->status);

	//dprintk("%s: index=%d, add=0x%x, status=0x%x\n",__FUNCTION__, index, bd->addr, bd->status);

	//ctrl = fe_readl(fep->dmac_regs + DMAC_TBL_ACC_CTL);
	ctrl |= (TBL_BUSY | TBL_ACC_MOD_WRITE | (bd->table_id <<TBL_ID_SHIFT) | index);
	fe_writel(fep->dmac_regs + DMAC_TBL_ACC_CTL, ctrl);

	while(fe_readl(fep->dmac_regs + DMAC_TBL_ACC_CTL)& TBL_BUSY){
		i++;
		if(i > MAX_WAIT_CNT){
			printk(KERN_DEBUG "write bd index=%d time out\n", index);
			return;
		}
	}

	//dprintk("write bd done\n");
	spin_unlock_irqrestore(&fep->lock,flags);
}
EXPORT_SYMBOL(optrann_write_bd);



static void read_tx_bd(struct ipmux_fe_priv *fep, u32 index)
{
	optrann_read_bd(fep, &fep->tx_buffers[index].bd_info, index + MAX_RX_DESC);
}

static void read_rx_bd(struct ipmux_fe_priv *fep, u32 index)
{
	optrann_read_bd(fep, &fep->rx_buffers[index].bd_info, index);
}

static void write_tx_bd(struct ipmux_fe_priv *fep, u32 index)
{
	u32 ctrl;

	if((index == MAX_TX_DESC -1 )){
		fep->tx_buffers[index].bd_info.status |= MSG_BD_LAST;
	}
	/*rx bd range : 0~63; tx bd range: 64~127*/
	optrann_write_bd(fep, &fep->tx_buffers[index].bd_info, index + MAX_RX_DESC);	 
}

static void write_rx_bd(struct ipmux_fe_priv *fep, u32 index)
{
	u32 ctrl;

	if((index == MAX_RX_DESC -1 )){
		fep->rx_buffers[index].bd_info.status |= MSG_BD_LAST;
	}

	optrann_write_bd(fep, &fep->rx_buffers[index].bd_info, index);	 
}

/*return 1 is valid*/
static int check_rxbd_valid(struct ipmux_fe_priv *fep, u32 index)
{
	//read_bd(fep, bd, index);
	read_rx_bd(fep, index);

	return fep->rx_buffers[index].bd_info.status & MSG_BD_STA;
}

/*return 1 is valid*/
static int check_txbd_valid(struct ipmux_fe_priv *fep, u32 index)
{
	//read_bd(fep, bd, index + MAX_RX_DESC);
	read_tx_bd(fep, index);

	return (fep->tx_buffers[index].bd_info.status & MSG_BD_STA );
}


static void ipmux_init_dma(struct ipmux_fe_priv *fep)
{
	int i;
	u32 val, ret;
	u32  order,size,count;

	fe_writel(fep->dmac_regs + DMAC_INT_ENABLE, 0);

	disable_fe_dmac(fep);
	udelay(10);

	/*clean interrupt status*/
	fe_readl(fep->dmac_regs + DMAC_INT_STATUS);

	dprintk("start init dma\n");

	/*enable initial bd*/ 
	fe_writel(fep->dmac_regs + DMAC_INIT, 0x1);
	udelay(10);
	while(fe_readl(fep->dmac_regs + DMAC_INIT) & DMAC_INIT_BUSY){
		i++;
		if(i > MAX_WAIT_CNT){
			printk("fail:canot initial BD table\n");
			return;
		}
	}


	/*init dma base addr 
	*/
#if 1
	size = MAX_RXBUF_SIZE*(MAX_RX_DESC+MAX_TX_DESC);
	fep->dma_vaddr = dma_alloc_coherent(NULL, size,
			&fep->dma_base, GFP_KERNEL);

	if (!fep->dma_vaddr) {
		printk("canot alloc coherent dma for rx\n");
		ret = -ENOMEM;
	}
	dprintk("dma_alloc_coherent::vaddr=0x%x,dma=0x%x\n", fep->dma_vaddr,fep->dma_base);

	fep->dma_base = KSEG0_TO_KSEG1(fep->dma_base);
	fep->dma_vaddr = KSEG0_TO_KSEG1(fep->dma_vaddr);
	memset(fep->dma_vaddr, 0, size);

	/*write kuseg1 addr to DMAC_BA*/
	fe_writel(fep->dmac_regs + DMAC_BA, 0xa0000000);

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

	/*write kuseg1 addr to DMAC_BA*/
	fe_writel(fep->dmac_regs + DMAC_BA, 0xa0000000);

#endif

	dprintk("alloc dma done:vaddr=0x%x,dma=0x%x,size=%d\n", fep->dma_vaddr,fep->dma_base,
			MAX_RXBUF_SIZE*(MAX_RX_DESC +MAX_TX_DESC));

	for(i = 0; i< MAX_RX_DESC; i++){
		fep->rx_buffers[i].dma_ptr = fep->dma_base + MAX_RXBUF_SIZE * i;
		fep->rx_buffers[i].vaddr = fep->dma_vaddr + MAX_RXBUF_SIZE * i;

		fep->rx_buffers[i].bd_info.addr = fep->rx_buffers[i].dma_ptr; 
		fep->rx_buffers[i].bd_info.status = 0; 
		fep->rx_buffers[i].bd_info.table_id = 0;
		dprintk("rx: index=%d, dma=0x%x, vaddr=0x%x\n",i, fep->rx_buffers[i].bd_info.addr,
				fep->rx_buffers[i].vaddr);
		write_rx_bd(fep, i);
	}


	for(i = 0; i< MAX_TX_DESC; i++){
		fep->tx_buffers[i].dma_ptr = fep->dma_base + MAX_RXBUF_SIZE*MAX_RX_DESC + MAX_TXBUF_SIZE * i;
		fep->tx_buffers[i].vaddr = fep->dma_vaddr + MAX_RXBUF_SIZE*MAX_RX_DESC + MAX_TXBUF_SIZE * i;

		fep->tx_buffers[i].bd_info.addr = fep->tx_buffers[i].dma_ptr;  
		fep->tx_buffers[i].bd_info.status = 0;
		fep->rx_buffers[i].bd_info.table_id = 0;
		dprintk("tx: index=%d, dma=0x%x, vaddr=0x%x\n",i, fep->tx_buffers[i].bd_info.addr,
				fep->tx_buffers[i].vaddr);
		write_tx_bd(fep, i);
	}

	dprintk("FE DMAC initial done\n");

#if core_debug
	printk_register(fep);
	printk_bd(fep);
#endif

	dprintk("enable fe dmac done\n");
}

static void ipmux_clean_dma(struct ipmux_fe_priv *fep)
{

	int index;

	/*clean any pending Tx packets*/
	dma_free_coherent(NULL, MAX_RXBUF_SIZE*(MAX_RX_DESC+MAX_TX_DESC),
			fep->dma_vaddr, fep->dma_base);

}

/*----------------------------------------------*/



static void ipmux_timeout(struct net_device *ndev)
{
	struct ipmux_fe_priv *fep = netdev_priv(ndev);

	netif_wake_queue(ndev);
}

static void ipmux_multicast_list(struct net_device *dev)
{
}

static struct net_device_stats *ipmux_get_stats(struct net_device *ndev)
{
	struct ipmux_fe_priv *fep = netdev_priv(ndev);

	fep->stats.rx_packets = fe_readl(fep->dmac_regs + DMAC_MSG_RX_PKT_CNT);
	fep->stats.tx_packets = fe_readl(fep->dmac_regs + DMAC_MSG_TX_PKT_CNT);
	fep->stats.rx_dropped = fe_readl(fep->dmac_regs + DMAC_MSG_RX_DRP_PKT);

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

static int ipmux_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	struct ipmux_fe_priv *fep = netdev_priv(ndev);
	u32 index, i = 0, status= 0;
	dma_addr_t dmap;
	unsigned char *buf;
	unsigned long flags, length;

	spin_lock_irqsave(&fep->tx_lock, flags);

	index = fep->tx_head;

	buf = (unsigned char *)fep->tx_buffers[index].vaddr;
	length = (skb->len & MSG_BD_LEN);

	dprintk("start xmit head=%d, tail=%d, skb->len=%d\n", index, fep->tx_tail, length);

	/*wait for DMA ready for send*/
	read_tx_bd(fep, index);
	while(fep->tx_buffers[index].bd_info.status & MSG_BD_STA){
		read_tx_bd(fep, index);
		i++;
		if(i > MAX_WAIT_CNT){
			dprintk("dma not ready for send\n");
			goto fail;
		}
	}

	memcpy((unsigned char *)buf, (unsigned char *)skb->data, length);

	/*write bd to reg, and start DMA*/
	status |= (length | MSG_BD_STA);
	fep->tx_buffers[index].bd_info.status = status;
	write_tx_bd(fep, index);

	dprintk("write BD, status=0x%x,start dma send\n" ,fep->tx_buffers[index].bd_info.status);

	fep->tx_buffers[index].skb = skb;
	fep->tx_buffers[index].length = length;
#if 0
	/*wait for DMA send ok*/
	read_tx_bd(fep, index);
	while(fep->tx_buffers[index].bd_info.status & MSG_BD_STA){
		read_tx_bd(fep, index);
		i++;
		if(i > MAX_WAIT_CNT){
			dprintk("dma send fail\n");
			goto fail;
		}
	}
	dprintk("dma send ok, status =0x%x\n",fep->tx_buffers[index].bd_info.status);
	//print_mem(buf, length);

	dev_kfree_skb(skb);
#endif
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

static void ipmux_tx_interrupt(struct ipmux_fe_priv *fep)
{
	unsigned tx_head = fep->tx_head;
	unsigned tx_tail = fep->tx_tail;

	//dprintk("%s :: tx_head=%d, tx_tail=%d\n",__FUNCTION__, tx_head, tx_tail);

	while(tx_tail != tx_head){
		fep->stats.tx_packets++;
		fep->stats.tx_bytes += fep->tx_buffers[tx_tail].length;

		dev_kfree_skb_irq(fep->tx_buffers[tx_tail].skb);

		dprintk("have a tx packet %d xmit done\n", tx_tail);

		//print_mem(fep->tx_buffers[tx_tail].vaddr, fep->tx_buffers[tx_tail].length);

		/* to next Tx descriptor */
		tx_tail = (tx_tail + 1) % MAX_TX_DESC;
	}

	fep->tx_tail= tx_tail;
	//printk("%s :: tx_head=%d, tx_tail=%d\n",__FUNCTION__, tx_head, fep->tx_tail);

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

static int rx_receive(struct net_device *ndev, int budget)
{
	int received = 0;
	int pkt_len;
	struct sk_buff *skb;
	void *pfe_pkt, *tmp;
	u32 bd_data;
	int reval;
	u32 status, valid_index;
	u32 error ;
	struct ipmux_fe_priv *fep = netdev_priv(ndev);

	struct ipmux_skbuff * rxbuf;

	u32 index = fep->rx_ring_read;
	while(received < budget && find_valid_rx(fep, index, &valid_index)){

		index = valid_index;

		read_rx_bd(fep, index);
		if(!(fep->rx_buffers[index].bd_info.status & MSG_BD_STA)){
			dprintk("invalid packed index=%d\n", index);
			goto done;
		}

		error = *(volatile u32 *)(0xbf000000 + MSG_SA_BASE + MSG_INT_STATUS);
		if(error & (0xff)){
			if(error & LEN_MISMATCH_INT_STATUS) fep->stats.rx_length_errors++;
			if(error & CRC_ERR_INT_STATUS) fep->stats.rx_crc_errors++;
			if(error & MII_ERR_DROP_INT_STATUS) fep->stats.rx_missed_errors++;
			dprintk("error packed index=%d\n", index);
			goto done;
		}

		status = fep->rx_buffers[index].bd_info.status;

		pkt_len = status & MSG_BD_LEN;

		dprintk("index=%d; status=0x%x, pkt_len=%d\n", index, status, pkt_len);

		if (pkt_len < 60)
		{
			printk(KERN_DEBUG " bogus packet, length=%d\n",  pkt_len);
			goto done;

		}
		else
		{
			skb = dev_alloc_skb(pkt_len+2);
			skb_reserve(skb,2);
			skb->dev = ndev;

			memcpy(skb_put(skb,pkt_len), (unsigned char *)fep->rx_buffers[index].vaddr, pkt_len );

			skb->protocol=eth_type_trans(skb,ndev);
			netif_receive_skb(skb);
			//dprintk( "receive skb --- \n");
		}

#if 0
		tmp = (char *)fep->rx_buffers[index].vaddr;
		print_mem(tmp, pkt_len);
#endif
		received++;
		ndev->last_rx = jiffies;
		fep->stats.rx_packets++;
		fep->stats.rx_bytes+=pkt_len;

done:
		/*let this BD invalid*/
		fep->rx_buffers[index].bd_info.status &=~ MSG_BD_STA;
		write_rx_bd(fep, index);
		index = (++index) % MAX_RX_DESC;
	}

	fep->rx_ring_read = index;

	return received;
}

static int ipmux_poll(struct net_device *ndev, int *budget)
{
	int orig_budget = min(*budget, ndev->quota);
	int done = 1;
	int work_done;
	u32 val;

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


int tf_interrupt_check()
{
	      wait_event_interruptible(opl_tf_int_wq, (opl_tf_int_status == TF_HAVE_INT));
	  	opl_tf_int_status = TF_NO_INT;

		return opl_interrupt_status;
}
EXPORT_SYMBOL(tf_interrupt_check);

static irqreturn_t ipmux_interrupt(int irq, void *dev_id)
{
	struct net_device *ndev = (struct net_device *)dev_id;
	struct ipmux_fe_priv *fep = netdev_priv(ndev);

	u32 status, flags;
	static u32 i;

	i++;
	spin_lock_irqsave(&fep->lock, flags);

	status = fe_readl(fep->dmac_regs + DMAC_INT_STATUS);

	opl_interrupt_status = status;

	if(status & MSG_RX_INT_STATUS || status & MSG_RX_FULL_INT_STATUS){
		if (netif_rx_schedule_prep(ndev)) {
			disable_rx_interrupt(fep);
			__netif_rx_schedule(ndev);
			dprintk("__netif_rx_schedule\n");
		}
	}

	if((status & TF_RX_INT_STATUS) || (status & TF_RX_FULL_INT_STATUS) || (status & PPE_RX_FULL_INT_ENABLE) ||
		                              (status & PPE_RX_INT_ENABLE)){
	       	dprintk(KERN_EMERG "kern: have TF/PPE interrupts, status=0x%x\n", opl_interrupt_status);
		
		opl_tf_int_status = TF_HAVE_INT;
		wake_up_interruptible(&opl_tf_int_wq);
	}

	if(status & MSG_TX_INT_STATUS){
		//dprintk("have tx interrupt--\n");
		ipmux_tx_interrupt(fep);
	}

done:
	spin_unlock_irqrestore(&fep->lock, flags);
	return IRQ_HANDLED;
}

static int ipmux_open(struct net_device *dev)
{
	int retval;
	unsigned long flags,val = 0;
	struct ipmux_fe_priv *fep = netdev_priv(dev);

	int i;

	fep->tx_head = 0;
	fep->tx_tail= 0;
	fep->rx_ring_read = 0;
	fep->channel = 0; 
	fep->table_id = 0;

	/*for TF interrpt*/
	init_waitqueue_head(&opl_tf_int_wq);

	/*for dma bd app*/
	optrann_app_init();

	/*regist irq*/
	retval = request_irq(fep->irq, &ipmux_interrupt, IRQF_DISABLED/*IRQF_SHARED*/, "optrann-fe", dev);
	if (retval) {
		dprintk( "failure to request IRQ %d\n", fep->irq);
		return retval;
	} 
	dprintk("requst irq %d done\n", fep->irq);

	/*enable interrupt*/
	val = fe_readl(fep->dmac_regs + DMAC_INT_ENABLE);
	val |= (MSG_TX_INT_ENABLE | MSG_RX_FULL_INT_ENABLE | MSG_RX_INT_ENABLE);
	fe_writel(fep->dmac_regs + DMAC_INT_ENABLE, val);

	/*unmask interrupt*/
	val = fe_readl(fep->dmac_regs + DMAC_INT_MASK);
	val &=~ (MSG_TX_INT_MASK | MSG_RX_FULL_INT_MASK | MSG_RX_INT_MASK);
	fe_writel(fep->dmac_regs + DMAC_INT_MASK, val);
	dprintk("enable interrupt done\n");

	/*bit 8~9 set to 0x1, to add CRC*/
	*(volatile u32 *)(0xbf000000 + MSG_SA_BASE + MSG_CFG) = 0xd03;
	dprintk("enable MSG_CFG done\n");

	enable_fe_dmac(fep);
	dprintk("enable dmac fe done\n");

	netif_start_queue(dev);

	dprintk("open finish\n");

	return 0;
}

static int ipmux_close(struct net_device *dev)
{
	struct ipmux_fe_priv *fep = netdev_priv(dev);

	fep->tx_head = 0;
	fep->tx_tail= 0;
	fep->rx_ring_read = 0;

	netif_stop_queue(dev); 
	free_irq(fep->irq, dev);

	disable_fe_dmac(fep);

	ipmux_clean_dma(fep);

	return 0;
}

static int ipmux_set_mac(struct net_device *ndev, void *addr)
{
	struct ipmux_fe_priv *fep = netdev_priv(ndev);
	struct sockaddr *address = addr;
	u32 machigh, maclow;

	if (!is_valid_ether_addr(address->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(ndev->dev_addr, address->sa_data, ndev->addr_len);

	maclow	= ((ndev->dev_addr[2] << 24) | (ndev->dev_addr[3] << 16) |
			(ndev->dev_addr[4] <<  8) | (ndev->dev_addr[5] <<  0));
	machigh = ((ndev->dev_addr[0] <<  8) | (ndev->dev_addr[1] <<  0));

	fe_writel(0xbf000000 + MSG_SA_BASE + MSG_MAC_ADDR_L, maclow);
	fe_writel(0xbf000000 + MSG_SA_BASE + MSG_MAC_ADDR_H, machigh);

	printk("%s: Updated MAC address to %pM\n",
			ndev->name, ndev->dev_addr);

	return 0;
}

static int ipmux_set_mac_from_boot(char *line)
{
	char *ep;
	u32 i, machigh, maclow;

	unsigned char fe_addr[6]={0,0,0,0,0,0};

	/* there should really be routines to do this stuff */
	for (i = 0; i < 6; i++)
	{
		fe_addr[i] = line  ? simple_strtoul(line, &ep, 16) : 0;
		if (line)
			line = (*ep) ? ep+1 : ep;
	}

	maclow	= ((fe_addr[2] << 24) | (fe_addr[3] << 16) |
			(fe_addr[4] <<  8) | (fe_addr[5] <<  0));
	machigh = ((fe_addr[0] <<  8) | (fe_addr[1] <<  0));

	fe_writel(0xbf000000 + MSG_SA_BASE + MSG_MAC_ADDR_L, maclow);
	fe_writel(0xbf000000 + MSG_SA_BASE + MSG_MAC_ADDR_H, machigh);

	printk("Updated MAC address from u-boot: %pM\n",fe_addr);


}
__setup("mac=", ipmux_set_mac_from_boot);



/*read mac addr*/
static void read_mac_addr(struct net_device *ndev)
{

	u32 machigh, maclow;
	machigh = fe_readl(0xbf000000 + MSG_SA_BASE + MSG_MAC_ADDR_H);
	maclow = fe_readl(0xbf000000 + MSG_SA_BASE + MSG_MAC_ADDR_L);

	ndev->dev_addr[0] = (machigh >> 8) & 0xFF;
	ndev->dev_addr[1] = machigh & 0xFF;
	ndev->dev_addr[2] = (maclow >> 24) & 0xFF;
	ndev->dev_addr[3] = (maclow >> 16) & 0xFF;
	ndev->dev_addr[4] = (maclow >> 8) & 0xFF;
	ndev->dev_addr[5] = maclow & 0xFF;

	printk ("FE MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
			ndev->dev_addr[0], ndev->dev_addr[1],
			ndev->dev_addr[2], ndev->dev_addr[3],
			ndev->dev_addr[4], ndev->dev_addr[5]);

	if (!is_valid_ether_addr(ndev->dev_addr))
		dprintk("%s: Invalid ethernet MAC address. Please "
				"set using ifconfig\n", ndev->name);

}

static void ipmux_hw_init(struct ipmux_fe_priv *fep)
{
	ipmux_init_dma(fep);
}

struct ipmux_fe_priv *fep_tf;
static int __init optrann_fe_probe(struct platform_device *pdev)
{
	struct ipmux_fe_priv *fep;
	struct net_device *ndev;
	struct resource *dmac_res, *msg_res;
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

	fep_tf = fep;

	dprintk("optrann_fe_probe in\n");

	//ndev->name = "fe eth";
	fep->ndev = ndev;

	/*get DMAC reg base addr*/
	dmac_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	msg_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);

	fep->dmac_regs = dmac_res->start;

	fep->msg_regs = msg_res->start;

	/*get FE irq*/
	fep->irq = platform_get_irq(pdev, 0);

	//fep->dmac_regs = 0xbf000000 + IPMUX_DMAC_REG;
	//fep->irq = 26;

	printk(KERN_INFO "%s Ethernet driver, V%s\n",
			MODULENAME, MODULEVERSION);
	printk(KERN_INFO "FE:dmac_regs =0x%x,msg_regs=0x%x, irq=%d\n",fep->dmac_regs,
			fep->msg_regs, fep->irq);

	ipmux_hw_init(fep);

	/*set mac addr*/
	read_mac_addr(ndev);

	spin_lock_init(&fep->tx_lock);
	spin_lock_init(&fep->rx_lock);
	spin_lock_init(&fep->lock);

	/* driver system function */
	ether_setup(ndev);

	ndev->open = ipmux_open;
	ndev->hard_start_xmit = ipmux_start_xmit;
	ndev->tx_timeout = ipmux_timeout;
	ndev->watchdog_timeo = 400*HZ/1000;
	ndev->stop = ipmux_close;
	ndev->get_stats = ipmux_get_stats;
	ndev->set_multicast_list = ipmux_multicast_list;
	ndev->do_ioctl = netdev_ioctl;
	ndev->set_mac_address = ipmux_set_mac;

	ndev->poll = ipmux_poll;
	ndev->weight = 64;

	platform_set_drvdata(pdev, ndev);
	ret = register_netdev(ndev);
	if (ret == 0) {
		dprintk( "ipmux ethernet  MAC M\n");
	} else {
		/* Report the failure to register the net_device */
		dprintk("ipmux: failed to register netdev.\n");
		goto failure;
	}

	return 0;

failure:

	free_netdev(ndev);
	return ret;
}

static void __exit optrann_fe_remove(struct platform_device *pdev) 
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct ipmux_fe_priv *fep = netdev_priv(ndev);

	free_pages(fep->dma_vaddr, fep->order);

	platform_set_drvdata(pdev, NULL);

	unregister_netdev(ndev);

	kfree(ndev);		/* free device structure */

	printk("clean_module() exit\n");

	return 0;
}

static struct platform_driver optrann_fe_driver = {
	.driver = {
		.name	= MODULENAME,
		.owner	= THIS_MODULE,
	},
	.probe		= optrann_fe_probe,
	.remove		= __devexit_p(optrann_fe_remove),
};

/* Module interface */
static int __init optrann_fe_init(void)
{

	return platform_driver_register(&optrann_fe_driver);
}

static void __exit optrann_fe_cleanup(void)
{
	platform_driver_unregister(&optrann_fe_driver);
}


MODULE_AUTHOR("Figo.zhang <tfzhang@opulan.com>");
MODULE_DESCRIPTION("Opulan Optrann FE Ethernet driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" MODULENAME);

module_init(optrann_fe_init);
module_exit(optrann_fe_cleanup);

