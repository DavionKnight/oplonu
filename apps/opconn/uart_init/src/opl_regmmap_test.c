#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/delay.h>
#include <string.h>

#include <fcntl.h>
#include <stdio.h>

#include "test.h"
#include "uart_default.h"
#include "ringbuf.h"
#include "serial.h"
#include <pthread.h>
#include "opl_regmmap.h"
#include "run_led.h"



#ifdef debug 
    #undef debug
#endif

pthread_mutex_t read_sem=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_sem=PTHREAD_MUTEX_INITIALIZER;
#define OPL_UART_DEVICE "/dev/opl_uart"
#define OPL_UART_SIZE 0x300000



char *opl_uart_base = NULL;




/** global fd of the "/dev/opl_reg"  */
int opl_uart_fd;

#if 0
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
#endif

int cpldcs2Read(int offset,unsigned char *setData)
{
//printf("opl_uart_base is 0x%x",opl_uart_base);
//pthread_mutex_lock(&read_sem);
  unsigned char value = 0;
*(volatile unsigned char *)(opl_uart_base + 7) = 0x55;
  value = *(volatile unsigned char*)(opl_uart_base+offset);
*(volatile unsigned char *)(opl_uart_base + 7) = 0x0;
*setData=value;
//pthread_mutex_unlock(&read_sem);
  return OK;
}


int cpldcs2Write(int offset,unsigned char setData)
{
//printf("opl_uart_base is 0x%x",opl_uart_base);
//pthread_mutex_lock(&write_sem);
	char data=0;
*(volatile unsigned char *)(opl_uart_base + 7) = 0x55;
	*(volatile unsigned char*)(opl_uart_base+offset) = setData;
*(volatile unsigned char *)(opl_uart_base + 7) = 0x0;
	//*(volatile unsigned char*)(opl_uart_base+offset*2+1) = setData;
//pthread_mutex_unlock(&write_sem);
return OK;
}

#if 0
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


void *new_thread(void *arg)
{
	int i=0,j=0;
	char buf[200];

while(1)
{
	SERIAL_INT_TYPE int_Status;
	if(OK == UARTIntStatus(&int_Status))
		{
		//	printf("interrupt  get int_status is %d\n",int_Status);			

			if(SERIAL_INT_TYPE_RX == int_Status)
			{
				printf("receive data\n");
				while((i<200)&&(OK == UARTCharsAvail()))
				{
					UARTCharGet(&buf[i]);
	//	udelay(10);
					i++;
				}

			}
				printf("the get data is :\n");
				while(j<i)
				{
					printf("%c",buf[j]);
					j++;
				}
				printf("\n");
				i=0;
				j=0;
		}
}
}
#endif
void opl_uart_main()
{
//	printf("opl_uart_main start...\n");
//	unsigned long serial_type = 0;	
//	unsigned char setdata=0,port_mode=0;
//	int ulPort,i;

//	char str[100];
	
	S2E_init();


}

int opl_uart_init(int *opl_uart_initialized)
{
	int virt_addr,phy_addr;

  opl_uart_fd = open(OPL_UART_DEVICE,O_RDWR);

  if(opl_uart_fd<0){
    printf("ERROR:open device file failed\n",OPL_UART_DEVICE);
    return -1;
  }


  opl_uart_base = (char *)mmap(0,OPL_UART_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,opl_uart_fd,0);
//printf("opl_uart_base is 0x%x==============\n",opl_uart_base);
  if(!opl_uart_base){
    printf("ERROR:mmap the opl register failed\n");
  //  close(opl_uart_fd);
    return -1;
  }

  *opl_uart_initialized = 1;

  return 0;
}
#if 0
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
#endif
int opl_uart_exit(int *opl_uart_initialized)
{
  int ret = 0;
  if(*opl_uart_initialized == 1){
    *opl_uart_initialized = 0;
    munmap(opl_uart_base,OPL_UART_SIZE);
    ret = close(opl_uart_fd);
  }
  return ret;
}


int init_main()
{ 
	 int num=4;
	int ret=0;
	 
	 char *cmd_base[4]={"self_exe","-w","0x3048","0x12800"};                      //start from 40M 
	char *cmd_mask[4]={"self_exe","-w","0x3068","0xf800"};	
	char *cmd_portsize[4]={"self_exe","-w","0x3010","0x80000000"};		//set portsize is 8bits
	char *cmd_cshold[4] = {"self_exe","-w","0x3014","0x0b981f8f"};    //if not   then some wrong code
	int init_reg = 0,init_uart = 0;

	 if((ret =opl_reg_init(&init_reg))==-1)
	 	return -1;
	if(init_reg){
		opl_reg_main(num,cmd_base);
		opl_reg_main(num,cmd_mask);
		opl_reg_main(num,cmd_portsize);
		opl_reg_main(num,cmd_cshold);
	}
	opl_reg_exit(&init_reg);
	opl_uart_init(&init_uart);
	if(init_uart){
		opl_uart_main();
	} 
//	opl_uart_exit(&init_uart);	
	return 0;
}



/** @}*/
/** @}*/



