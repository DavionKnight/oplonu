#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/delay.h>
#include <string.h>

#include <fcntl.h>
#include <stdio.h>

#include "opl_regmmap.h"
//#include "ringbuf.h"
#include <pthread.h>




#define OPL_REG_DEVICE "/dev/opl_reg"
#define OPL_REG_SIZE 0x300000

char *opl_reg_base = NULL;


char *OPL_REG_USAGE = "\nreg usage:\n\
\topl_uart_test -r offset  or\n\
\topl_uart_test --read offset  or\n\
\topl_uart_test -w offset value   or\n\
\topl_uart_test --write offset value  \n\
Note: the offset and value can be decimal or hex!\n" ;


/** global fd of the "/dev/opl_reg"  */
int opl_reg_fd,opl_uart_fd,opl_mem_fd;

u32 reg_read(char *reg_base,int offset)
{
  u32 value = 0;
  if(offset%4){
    printf("the alignment is not 4 bytes \n");
    return -1;
  }

  value = *(volatile u32 *)(reg_base + offset);

//  debug("reg_base 0x%x, offset 0x%x, address 0x%x, value 0x%x.\n", reg_base, offset, reg_base + offset, value);
  return value;
}

void reg_write(char *reg_base, int offset, u32 value)
{
//printf("reg_write start write ..\noffset is :%d\nvalue is %d:",offset,value);
  *(volatile u32 *)(reg_base + offset) = value;

}




void opl_reg_main(int argc,char ** argv)
{
//printf("read or write:%s\naddress is:%s\nvalue is:%s\n",argv[1],argv[2],argv[3]);
  int i = 0;
  int offset = 0;
  u32 value = 0;

  if(argc < 3){
    printf("ERROR:the number of argument is not correct\n");
    printf(OPL_REG_USAGE);
    return;
  }

  if((argv[2][0] == '0') && ((argv[2][1] == 'x')||(argv[2][1] == 'X'))){
    /* TBD:should be check the ranged */
    sscanf(argv[2],"%x",&offset);
  }else{
    sscanf(argv[2],"%d",&offset);
  }

  if(!strcmp(argv[1],"--read")||!strcmp(argv[1], "-r")){
    if(argc != 3){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    printf("%x\t0x%08x\n",offset,reg_read(opl_reg_base,offset));

  }if(!strcmp(argv[1],"--read4")||!strcmp(argv[1], "-r4")){
    if(argc != 3){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    printf("%x\t0x%08x\n",offset,reg_read(opl_reg_base,(offset*4)));

  }else if(!strcmp(argv[1],"--write")||!strcmp(argv[1], "-w")){
    if(argc != 4){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    if(argv[3][0] == '0' && ((argv[3][1] == 'x')||(argv[3][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[3],"%x",&value);
    }else{
      sscanf(argv[3],"%d",&value);
    }
    reg_write(opl_reg_base,offset,value);
  }else if(!strcmp(argv[1],"--write4")||!strcmp(argv[1], "-w4")){
    if(argc != 4){
      printf("ERROR:the number of argument is not correct\n");
      printf(OPL_REG_USAGE);
      return;
    }
    if(argv[3][0] == '0' && ((argv[3][1] == 'x')||(argv[3][1] == 'X'))){
      /* TBD:should be check the ranged */
      sscanf(argv[3],"%x",&value);
    }else{
      sscanf(argv[3],"%d",&value);
    }
    reg_write(opl_reg_base,(offset*4),value);
  }
}


int opl_reg_init(int *opl_reg_initialized)
{
	
  opl_reg_fd = open(OPL_REG_DEVICE,O_RDWR);
  if(opl_reg_fd<0){
    printf("ERROR:open device file failed\n",OPL_REG_DEVICE);
    return -1;
  }

  opl_reg_base = (char *)mmap(0,OPL_REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,opl_reg_fd,0);
	
  if(!opl_reg_base){
    printf("ERROR:mmap the opl register failed\n");
    close(opl_reg_fd);
    return -1;
  }
  *opl_reg_initialized = 1;
//printf("opl_reg_base is 0x%x\n",opl_reg_base);

  return 0;
}



int opl_reg_exit(int *opl_reg_initialized)
{
  int ret = 0;
  if(*opl_reg_initialized == 1){
    *opl_reg_initialized = 0;
    munmap(opl_reg_base,OPL_REG_SIZE);
    ret = close(opl_reg_fd);
  }
  return ret;
}



