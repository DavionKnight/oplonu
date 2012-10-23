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

#define H_MAXTESTBUF  1536 
#define H_MAXTESTBUFNUM 128

typedef struct el_apptest_s{
  char data[H_MAXTESTBUF];
  int len;
  struct el_apptest_s *next;
}el_apptest_t;


extern wait_queue_head_t opl_host_el_waitq;
extern u32 opl_host_el_irq_event;