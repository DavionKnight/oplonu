#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include "nf_fastpath.h"


u32 nfp_enable = 1;
u32 nfp_log_enable = 0;
unsigned long nfp_ct_mark = 1 << 16;

void nfp_register_netdev_receive_fastpath(void);
void nfp_unregister_netdev_receive_fastpath(void);

static int nfp_enable_proc_read(char *buf, char **bloc, off_t off,
                                int length, int *eof, void *data)
{
        int len = 0;
    
        /* finished reading regardless of anything else */
        if(off > 0)
                return 0;

        len += sprintf(buf, "%d\n", nfp_enable);

        return len;
}

static int nfp_enable_proc_write(struct file *file, const char __user *buffer,
                                 unsigned long count, void *data)
{
        /* kernel space buffer */
        char kernel_buffer[256];
        int enable = -1;
        int args;
   
        /* copy the user space data to the kernel space buffer */
        if (copy_from_user(kernel_buffer, buffer, count)) {
                printk(KERN_WARNING "copy_from_user failed\n");
                return -EFAULT;
        }

        /* IF wrong # of arguments or invalid argument */
        args = sscanf(kernel_buffer, "%d", &enable);
        if (args != 1 || enable < 0 || enable > 1) {
                printk(KERN_WARNING 
                       "echo 1 to enable fast-path\n"
                       "echo 0 to disable fast-path\n");
                return count;
        }
    
    
        if (nfp_enable == 0 && enable == 1) {
                /* enabling, register our hooks: */
                printk(KERN_INFO "Registering fast-path hooks\n");
                nfp_register_all();
        }

        if (nfp_enable == 1 && enable == 0) {
                printk (KERN_INFO "Disabling fast-path.  Unregistering hooks\n");
                nfp_unregister_all();
        }
    
        nfp_enable = enable;

        return count;
}

static int nfp_log_proc_read(char *buf, char **bloc, off_t off,
                             int length, int *eof, void *data)
{
        int len = 0;
    
        /* finished reading regardless of anything else */
        if(off > 0)
                return 0;

        len += sprintf(buf, "%d\n", nfp_log_enable);

        return len;
}

static int nfp_log_proc_write(struct file *file, const char __user *buffer,
                              unsigned long count, void *data)
{
        /* kernel space buffer */
        char kernel_buffer[256];
        int enable, args;
   
        /* copy the user space data to the kernel space buffer */
        if (copy_from_user(kernel_buffer, buffer, count)) {
                printk(KERN_WARNING "copy_from_user failed\n");
                return -EFAULT;
        }

        /* IF wrong # of arguments */
        args = sscanf(kernel_buffer, "%d", &enable);
        if (args != 1) {
                printk(KERN_WARNING 
                       "echo 1 to log fast-path info\n"
                       "echo 0 to log fast-path info\n");
                return count;
        }

        nfp_log_enable = enable ? 1 : 0;

        return count;
}

inline int nfp_log_enabled(void) {
        return nfp_log_enable;
}

static int nfp_ct_mark_proc_read(char *buf, char **bloc, off_t off, 
                                 int length, int *eof, void *data)
{
        int len = 0;
    
        /* finished reading regardless of anything else */
        if(off > 0)
                return 0;

    
        len += sprintf(buf, "%lu\n", nfp_ct_mark);

        return len;
}

static int nfp_ct_mark_proc_write(struct file *file, const char __user *buffer,
                                  unsigned long count, void *data)
{
        /* kernel space buffer */
        char kernel_buffer[256];
        int bitmask, args;
   
        /* copy the user space data to the kernel space buffer */
        if (copy_from_user(kernel_buffer, buffer, count)) {
                printk(KERN_WARNING "copy_from_user failed\n");
                return -EFAULT;
        }

        /* IF wrong # of arguments */
        args = sscanf(kernel_buffer, "%d", &bitmask);
        if (args != 1) {
                printk(KERN_WARNING 
                       "echo bit mask (as decimal integer value) for connmark"
                       " used to enable fast-path\n");
                return count;
        }

        nfp_ct_mark = bitmask;

        return count;
}

#ifdef CONFIG_NF_FASTPATH_STATS
static int nfp_stats_proc_read(char *buf, char **bloc, off_t off,
                               int length, int *eof, void *data)
{
        int len = 0;
    
        /* finished reading regardless of anything else */
        if(off > 0)
                return 0;

        len += sprintf(buf+len, "Active TCP FAST_PATH connections: %d\n",
                       NFP_STAT_GET(active_tcp));
        len += sprintf(buf+len, "Active UDP FAST_PATH connections: %d\n",
                       NFP_STAT_GET(active_udp));
        len += sprintf(buf+len, "FAST_PATH Packets handled: %d\n",
                       NFP_STAT_GET(handled));
        len += sprintf(buf+len, "Packets unhandled by FAST_PATH due to\n");
        len += sprintf(buf+len, "  Unsupported L4 protocol: %d\n",
                       NFP_STAT_GET(unhandled_l4proto));
        len += sprintf(buf+len, "  No FAST_PATH established: %d\n",
                       NFP_STAT_GET(unhandled_no_fp));
        len += sprintf(buf+len, "  Wrong CONNMARK: %d\n",
                       NFP_STAT_GET(unhandled_wrongmark));
        len += sprintf(buf+len, "  Other: %d\n", NFP_STAT_GET(unhandled_other));

        return len;
}

static int nfp_stats_proc_write(struct file *file, const char __user *buffer,
                                unsigned long count, void *data)
{
        NFP_STAT_CLEAR(handled);
        NFP_STAT_CLEAR(unhandled_l4proto);
        NFP_STAT_CLEAR(unhandled_no_fp);
        NFP_STAT_CLEAR(unhandled_wrongmark);
        NFP_STAT_CLEAR(unhandled_other);

        printk(KERN_INFO "Statistics cleared\n");

        return count;
}
#endif /* CONFIG_NF_FASTPATH_STATS */
    


void __exit nfp_proc_fini(void)
{
        remove_proc_entry("nfp_enable", proc_net);
        remove_proc_entry("nfp_log", proc_net);
        remove_proc_entry("nfp_mark", proc_net);
#ifdef CONFIG_NF_FASTPATH_STATS
        remove_proc_entry("nfp_stats", proc_net);
#endif
}


int __init nfp_proc_init(void)
{
        struct proc_dir_entry *proc_nfp;

        proc_nfp = create_proc_entry("nfp_enable", 0644, proc_net);
        if (proc_nfp) {
                proc_nfp->read_proc = nfp_enable_proc_read;
                proc_nfp->write_proc = nfp_enable_proc_write;
                proc_nfp->data = NULL;
        }

    
        proc_nfp = create_proc_entry("nfp_log", 0644, proc_net);
        if (proc_nfp) {
                proc_nfp->read_proc = nfp_log_proc_read;
                proc_nfp->write_proc = nfp_log_proc_write;
                proc_nfp->data = NULL;
        }

        proc_nfp = create_proc_entry("nfp_mark", 0644, proc_net);
        if (proc_nfp) {
                proc_nfp->read_proc = nfp_ct_mark_proc_read;
                proc_nfp->write_proc = nfp_ct_mark_proc_write;
                proc_nfp->data = NULL;
        }
    
#ifdef CONFIG_NF_FASTPATH_STATS
        proc_nfp = create_proc_entry("nfp_stats", 0644, proc_net);
        if (proc_nfp) {
                proc_nfp->read_proc = nfp_stats_proc_read;
                proc_nfp->write_proc = nfp_stats_proc_write;
                proc_nfp->data = NULL;
        }
#endif

        return 0;
}
