#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/kmap_types.h>
#include <asm/page.h>
#include <asm/io.h>
#include <linux/slab.h>

#include "opl_hw_ops.h"

/*--------------------------Macro definition------------------------- */
#define OPL_UART_MAJOR    160
#define OPL_UART_DEVICE  "opl_uart"
#define CPLD_OFFSET	     0x12800000


#ifdef debug 
    #undef debug
#endif

#if 0
#define debug(x...) printk("  [DEBUG : %s @ %d]", __FUNCTION__, __LINE__);printk(x);
#else
#define debug(x...)
#endif

//typedef 	unsigned int	u32;



/*--------------local  function declaration------------- */
static int opl_uart_open(struct inode *inode, struct file *filp);
static ssize_t opl_uart_read(struct file *filp,char *buffer,size_t length,loff_t *offset);
static ssize_t opl_uart_write(struct file *filp, const char *buffer,size_t length, loff_t *offset);
static int opl_uart_mmap(struct file * file, struct vm_area_struct * vma);
static int opl_uart_release(struct inode *inode, struct file *filp);


/*--------------local  varible declaration and definition------------- */

static u32 opl_uart_open_count = 0;
static u32 opl_uart_is_open = 0;
struct file_operations opl_uart_fops = {
  .read = opl_uart_read,
  .write = opl_uart_write,
  .mmap = opl_uart_mmap,
  .open = opl_uart_open,
  .release = opl_uart_release, 
};


static ssize_t opl_uart_read(struct file *filp,char *buffer,size_t length,loff_t *offset)
{
	return 0;
}

static ssize_t opl_uart_write(struct file *filp, const char *buffer,size_t length, loff_t *offset)
{
	return 0;
}



static int opl_uart_mmap(struct file * file, struct vm_area_struct * vma)
{
  unsigned long offset = vma->vm_pgoff << 12;
  /*
   * Accessing memory above the top the kernel knows about or
   * through a file pointer that was marked O_SYNC will be
   * done non-cached.
   */
  offset += CPLD_OFFSET;
  if ((offset>__pa(high_memory)) || (file->f_flags & O_SYNC)) {
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
  }

  /* Don't try to swap out physical pages.. */
  vma->vm_flags |= VM_RESERVED;
  
  /* Don't dump addresses that are not real memory to a core file.*/
  if (offset >= __pa(high_memory) || (file->f_flags & O_SYNC))
    vma->vm_flags |= VM_IO;
  
#ifdef LINUX_24
  debug(">>>>memmap : physical(offset) 0x%x to userspace(vma->vm_start) 0x%x ~ 0x%x\n", offset, vma->vm_start, vma->vm_end);
  if (remap_page_range(vma->vm_start, offset, vma->vm_end-vma->vm_start,
               vma->vm_page_prot))
#else
  offset = offset >> PAGE_SHIFT;
  debug(">>>>memmap : physical(offset) 0x%x to userspace(vma->vm_start) 0x%x ~ 0x%x\n", offset, vma->vm_start, vma->vm_end);
  if (remap_pfn_range(vma, vma->vm_start, offset, vma->vm_end-vma->vm_start,
               vma->vm_page_prot))
#endif
  {
    return -EAGAIN;
  }
  return 0;
}

static int opl_uart_open(struct inode *inode, struct file *filp)         
{
  if (test_and_set_bit(0, &opl_uart_is_open))
    printk(KERN_DEBUG "the /dev/opl_cpld is already opened\n");
  opl_uart_open_count ++;
  return 0;
}

static int opl_uart_release(struct inode *inode, struct file *filp)
{
  opl_uart_open_count ++;
  if(!opl_uart_open_count)
    clear_bit(0, &opl_uart_is_open);
  return 0;
}
/** description: The driver module entry.to register the chrdev and 
    request irq. and some essential initialization.
 *  @param void
 *  @retval 0 success
 *  @retval !0  failure.the module cant be inserted into os.

 *  @see insmod rmmod
 *  @deprecated 
 */
static int __init opl_uart_init(void)
{
  int ret;
  ret = register_chrdev(OPL_UART_MAJOR,OPL_UART_DEVICE,&opl_uart_fops);
  if(ret<0){
    /* TBD add debug message */
  }
  return ret;
}
/** description: it is called when the module removed. do the reverse of
    opl_reg_module_init. 
 *  @param void
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void __exit opl_uart_exit(void)
{
  int ret;
  ret = unregister_chrdev(OPL_UART_MAJOR,OPL_UART_DEVICE);
  if(ret < 0){
    /*TBD:add debug message  */
  }
  return;
}

module_init(opl_uart_init);
module_exit(opl_uart_exit);
#ifdef LINUX_24
MODULE_LICENSE("GPL2");
#else
MODULE_LICENSE("Dual BSD/GPL");
#endif
MODULE_AUTHOR("opulan inc");
MODULE_DESCRIPTION("dedicated the module for mmap the opulan CPLD.");
/** @}*/
/** @}*/
