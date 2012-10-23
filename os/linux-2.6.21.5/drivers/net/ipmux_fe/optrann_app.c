
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>

#include "optrann.h"

#define OPTRANN_APP_MAJOR    151
#define OPTRANN_APP_DEVICE  "optrann_app"

#define OPTRANN_APP_READ_BD		_IOWR('v', 0, struct bd_req)

#define OPTRANN_APP_WRITE_BD    _IOWR('v', 1, struct bd_req)

struct bd_req{
	u32 index;
	struct bd_desc bd_info;
};

extern struct ipmux_fe_priv *fep_tf;

static int optrann_app_ioctl(struct inode *inode, struct file *file,
		     unsigned int cmd, unsigned long arg)
{

	switch(cmd){
		case OPTRANN_APP_READ_BD:
			{
				struct bd_req req;
				copy_from_user(&req, (u8 *)arg, sizeof(struct bd_req));

				//printk(KERN_EMERG "kern:from read :index=%d,  bd->status=0x%x, bd->add=0x%x, bd->table_id=%d\n",
		  		//				req.index,  req.bd_info.status, req.bd_info.addr, req.bd_info.table_id);

				optrann_read_bd(fep_tf, &req.bd_info, req.index);

			    // printk(KERN_EMERG "kern:read done: index=%d, status=0x%x, add=0x%x, table_id=%d\n",
		  	 //					req.index, req.bd_info.status, req.bd_info.addr, req.bd_info.table_id);
				
				copy_to_user((u8 *)arg, &req, sizeof(struct bd_req));
			}
			break;

		case OPTRANN_APP_WRITE_BD:
			{
				struct bd_req req;
				copy_from_user(&req, arg, sizeof(struct bd_req));
				
				//printk("kern:write_bd:index=%d, status=0x%x, add=0x%x, table_id=%d\n",
		             //           req.index, req.bd_info.status, req.bd_info.addr, req.bd_info.table_id);	
				optrann_write_bd(fep_tf, &req.bd_info, req.index);
				//copy_to_user((u8 *)arg, &req, sizeof(struct bd_req));
			}
			break;

		default:
		break;
	}
	

	return 0;
}

static int optrann_app_open(struct inode *inode, struct file *filp)         
{
  	if(fep_tf == NULL){
		printk("canot initial modules: fep_tf NULL\n");
		return -1;
  	}
	printk("optrann_fe_app open ok\n");
  return 0;
}

static int optrann_app_release(struct inode *inode, struct file *filp)
{
  
  return 0;
}


struct file_operations  optrann_app_fops = {
	.owner = THIS_MODULE,
  	.open = optrann_app_open,
  	.release = optrann_app_release, 
  	.ioctl = optrann_app_ioctl,
};

 int  optrann_app_init(void)
{
	  int ret;
  ret = register_chrdev(OPTRANN_APP_MAJOR, OPTRANN_APP_DEVICE, &optrann_app_fops);
  if(ret<0){
    	
  }
  return ret;
}

 void  optrann_app_cleanup(void)
{
	  int ret;
  ret = unregister_chrdev(OPTRANN_APP_MAJOR, OPTRANN_APP_DEVICE);
  if(ret < 0){
  
  }

}
