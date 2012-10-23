
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/inetdevice.h>

#define MAX_RX_DESC 64
#define MAX_TX_DESC 64
#define MAX_RXBUF_SIZE 0x800 /*2048 bytes*/
#define MAX_TXBUF_SIZE 0x800 /*2048 bytes*/

#define MAX_WAIT_CNT 0x2000

static unsigned char fe_addr[6]={0,0,0,0,0,0};

struct bd_desc {
	u32 addr; /*table access addr , DMA addr*/
	u32 status;   /*buffer len and sta/last  bit */

	u32 table_id;  /*0:msg    1: TF;*/
};

/**/
struct ipmux_skbuff {
	struct sk_buff	*skb; /*just for tx*/
	void *vaddr;
	dma_addr_t	dma_ptr;
	u32		length;
	struct bd_desc bd_info;
};

struct ipmux_fe_priv {
	struct net_device *ndev;

	struct resource *dmac_resource, *msg_resource;

	u32 channel;  /*0: msg dma  1: TF dma*/
	u32 table_id; /*0: msg dma  1: TF dma*/

	u32   dmac_regs; /*DMAC reg base addr*/
	u32   msg_regs;  /*MSG reg base addr*/

	int irq;
	spinlock_t lock;

	void *dma_vaddr;
	dma_addr_t dma_base;
	u32 order;


	/*rx information*/
	//struct bd_desc *rx_ring;
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

#define fe_writel(reg, value) 	((*(volatile uint32_t *)(reg)) = value)
#define fe_readl(reg)			(*(volatile uint32_t *)( reg))

#define KSEG0_TO_KSEG1(x) ((unsigned long)(x)|0xa0000000)