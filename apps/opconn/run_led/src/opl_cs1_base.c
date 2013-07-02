#include <stdio.h>
#include <fcntl.h>
#include "opl_cs1_base.h"
#include "opl_regmmap.h"
#include <linux/init.h>
#include <linux/kernel.h>
//#include <linux/module.h>
//#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/fs.h>
//#include <asm/uaccess.h>
#include <asm/kmap_types.h>
#include <asm/page.h>
//#include <asm/io.h>
#include <asm/mman.h>
#include <linux/slab.h>

#define OPL_REG_DEVICE "/dev/opl_reg"
#define OPL_REG_SIZE 0x300000
#define OPL_CS1_DEVICE "/dev/opl_cs1"
#define OPL_CS1_SIZE 0x300000


/** global fd of the "/dev/opl_reg"  */
int opl_reg_fd,opl_cs1_fd;


int opl_cs1_init(int *opl_cs1_initialized)
{
	int virt_addr,phy_addr;

  opl_cs1_fd = open(OPL_CS1_DEVICE,O_RDWR);

  if(opl_cs1_fd<0){
    printf("ERROR:open device file failed\n",OPL_CS1_DEVICE);
    return -1;
  }


  opl_cs1_cpldbase = (char *)mmap(0,OPL_CS1_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,opl_cs1_fd,0);
printf("opl_cs1_cpldbase is %#x\n",opl_cs1_cpldbase);
  if(!opl_cs1_cpldbase){
    printf("ERROR:mmap the opl register failed\n");
  //  close(opl_uart_fd);
    return -1;
  }

  *opl_cs1_initialized = 1;
  return 0;
}

int opl_cs1_exit(int *opl_cs1_initialized)
{
  int ret = 0;
  if(*opl_cs1_initialized == 1){
    *opl_cs1_initialized = 0;
    munmap(opl_cs1_cpldbase,OPL_CS1_SIZE);
    ret = close(opl_cs1_fd);
  }
  return ret;
}


void cs1_base_init()
{
	int num=4;
	char *cmd_base[4]={"self_exe","-w","0x3044","0x12000"};                     //start from 40M 
	char *cmd_mask[4]={"self_exe","-w","0x3064","0xf800"};	
	char *cmd_portsize[4]={"self_exe","-w","0x3008","0x80000000"};				//set portsize is 8bits
	int init_reg = 0,init_cs1 = 0;

	 opl_reg_init(&init_reg);
	if(init_reg){
		opl_reg_main(num,cmd_base);
		opl_reg_main(num,cmd_mask);
		opl_reg_main(num,cmd_portsize);
	}
	opl_reg_exit(&init_reg);
//printf("cs1 start\n");
	opl_cs1_init(&init_cs1);
//printf("cs1 init end\n");
	if(!init_cs1){
		printf("error\n");
	} 
//	opl_cs1_exit(&init_cs1);	
}
