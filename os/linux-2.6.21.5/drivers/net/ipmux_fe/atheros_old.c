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

#include "atheros_old.h"

volatile u8 TxBufTmp[2048];    /* each 2048 byte */

u8 aucBroadcastMac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

volatile u8 TxBuftest[2048];    /* each 2048 byte */

static struct net_device * netdev;

el_apptest_t s_testdata[H_MAXTESTBUFNUM];
int test_startnum;
int test_endnum;

wait_queue_head_t opl_host_el_waitq;
u32 opl_host_el_irq_event = 0;

void TestBufInit(void)
{
  int i;
  
  test_startnum = 0;
  test_endnum = 0;
  
  memset((char *)&s_testdata[0], 0, sizeof(el_apptest_t)*H_MAXTESTBUFNUM);
  for (i=0; i<(H_MAXTESTBUFNUM-1); i++)
  {
    s_testdata[i].next = &s_testdata[i+1];
  }
  s_testdata[H_MAXTESTBUFNUM].next = &s_testdata[0];
}

int gettestbufnum(void)
{
  if (((test_endnum+1)==test_startnum) || ((test_endnum+1)==(test_startnum+H_MAXTESTBUFNUM)))
  {
    return 0;
  }
  
  if (test_endnum < test_startnum)
  {
    return (test_startnum-(test_endnum+1));
  }
  else
  {
    return ((test_startnum+H_MAXTESTBUFNUM)-(test_endnum+1));
  }
}

el_apptest_t *gettestbufaddress(void)
{
  int tmpnum;
  
  if (gettestbufnum() == 0)
  {
    return NULL;
  }
  else
  {
    tmpnum = test_endnum;
    test_endnum += 1;
    if (test_endnum == H_MAXTESTBUFNUM)
    {
      test_endnum = 0;
    }
    return (&s_testdata[tmpnum]);
  }
}

void opl_host_el_init(struct net_device *dev)
{
	netdev = dev; 
	TestBufInit();
	init_waitqueue_head(&opl_host_el_waitq);
}

void wait_for_host_test_interrupt(void)
{
  wait_event_interruptible(opl_host_el_waitq, opl_host_el_irq_event);
  
  if(!test_and_clear_bit(0, &opl_host_el_irq_event))
  {
    return;
  }
  return;
}

int get_ipmux_el_rx_buf_data(char *pbuf, int* plen)
{
  el_apptest_t *tmpbuf;
  
  if (test_startnum != test_endnum)
  {
    tmpbuf = &s_testdata[test_startnum];
    *plen = tmpbuf->len;
    /*if(!copy_to_user(pbuf, tmpbuf, tmpbuf->len))
    {
      return 1;
    }*/
    copy_to_user(pbuf, tmpbuf, tmpbuf->len);
    test_startnum += 1;
    if (test_startnum == H_MAXTESTBUFNUM)
    {
      test_startnum = 0;
    }
    return 1;
  }
  return 0;
}

int set_ipmux_el_tx_buf_data(char *appbuf, int len)
{

  copy_from_user((char *)&TxBuftest[0], appbuf, len);
  onu_start_xmit_buf(netdev, TxBuftest,  len);
  return 0;
}

EXPORT_SYMBOL(opl_host_el_init);
EXPORT_SYMBOL(wait_for_host_test_interrupt);
EXPORT_SYMBOL(get_ipmux_el_rx_buf_data);
EXPORT_SYMBOL(set_ipmux_el_tx_buf_data);