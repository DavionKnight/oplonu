#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include "vos_timer.h"
#include <stdlib.h>
#include <string.h>
#include "opl_cs1_base.h"
#include "opl_regmmap.h"
#include "run_led.h"
#include "warning_check.h"
#include "vos_time.h"

unsigned int count=0,count2=0;
unsigned int test_led_flag=0;

int cpldRead(int offset,unsigned char *setData)
{

  unsigned char value = 0;
*(volatile unsigned char *)(opl_cs1_cpldbase+ 5) = 0x55;
  value = *(volatile unsigned char*)(opl_cs1_cpldbase+offset);
  *(volatile unsigned char *)(opl_cs1_cpldbase + 5) = 0x0;
*setData=value;
  return 1;
}


int cpldWrite(int offset,unsigned char setData)
{
	char data=0;
*(volatile unsigned char *)(opl_cs1_cpldbase + 5) = 0x55;
	*(volatile unsigned char*)(opl_cs1_cpldbase+offset) = setData;
*(volatile unsigned char *)(opl_cs1_cpldbase + 5) = 0x0;
	//*(volatile unsigned char*)(opl_uart_base+offset*2+1) = setData;

return 1;
}

char BoardReset()
{
	char uData;

	cpldRead(CS1_RESET_REG,&uData);
	cpldWrite(CS1_RESET_REG, (uData & 0xfe));
      vosSleep(1);;
	cpldWrite(CS1_RESET_REG,(uData | 0x01));

	return OK;
}

char get_serialcard_status()
{
	unsigned char card_available;

	cpldRead(CS1_SRIALCARD_STATUS,&card_available);
	return card_available&0x1;
}

void voip_POTS1_led_on()
{
	char led_status;
	
	cpldRead(CS1_VOIP_STATUS,&led_status);
	led_status &= 0xfe;
	cpldWrite(CS1_VOIP_STATUS,led_status);
}
void voip_POTS1_led_off()
{
	char led_status;
	
	cpldRead(CS1_VOIP_STATUS,&led_status);
	led_status |= 0x1;
	cpldWrite(CS1_VOIP_STATUS,led_status);
}
void voip_POTS2_led_on()
{
	char led_status;
	
	cpldRead(CS1_VOIP_STATUS,&led_status);
	led_status &= 0xfd;
	cpldWrite(CS1_VOIP_STATUS,led_status);
}
void voip_POTS2_led_off()
{
	char led_status;
	
	cpldRead(CS1_VOIP_STATUS,&led_status);
	led_status |= 0x2;
	cpldWrite(CS1_VOIP_STATUS,led_status);
}

int SetAlarmEnable(int enable)
{
	cpldWrite(CS1_RACK_WARNING,enable);
}

void powerStatus_check()
{
	char powerStatus;
	static char powerOldStatus=3;

	
	
	cpldRead(CS1_POWER_STATUS,&powerStatus);
	powerStatus &= 0x03;
		
	if(powerStatus != powerOldStatus)
	{
//		printf("power Status is 0x%x..\r\n",powerStatus);
		if((powerStatus & 0x01) != (powerOldStatus & 0x01))
		{
			if((powerStatus & 0x01) == 0x0)
			{
				printf("POWER_2 RECOVER\r\n");
				powerOldStatus &= 0x02;
			}
			else
			{
				printf("POWER_2 LOSE\r\n");
				powerOldStatus |= 0x01;
			}
		}
		if((powerStatus & 0x02) != (powerOldStatus & 0x02))
		{
			if((powerStatus & 0x02) == 0x0)
			{
				printf("POWER_1 RECOVER\r\n");
				powerOldStatus &= 0x01;
			}
			else
			{
				printf("POWER_1 LOSE\r\n");
				powerOldStatus |= 0x02;
			}
		}
	}
}

static void led_change_status()
{

#if 0
char cmd[50];
char reg_char[5];
int reg_num=0;
char data;
int w_data;

while(1)
{
printf("please input cmd...\n");
gets(cmd);

//printf("the cmd is \n%s\n",cmd);

if(strlen(cmd)==3)
{
//printf("len equal  3\n");
	reg_num=cmd[2]-'0';
//	sscanf(cmd+4,"%x",&data);
}
else if(strlen(cmd)>3)
{
//printf("len more than 3\n");
	if(cmd[3]!=' ')
	{
		reg_num=(cmd[2]-'0')*10+cmd[3]-'0';
		if(cmd[0]=='w')
		sscanf(cmd+5,"%x",&w_data);
		data=w_data;
	}
	else if(cmd[3]==' ')
	{
		reg_num=cmd[2]-'0';
		if(cmd[0]=='w')
		{
			strcpy(reg_char,cmd+4);
			//printf("reg_char is \n%s\n",reg_char);
			sscanf(reg_char,"%x",&w_data);
			data=w_data;
			printf("data is 0x%x\n",data);
		}
	}
}
printf("reg num is %d\n\n",reg_num);	

if(cmd[0]=='r')
{
	cpldRead(reg_num,&data);
	printf("data in reg %d is 0x%x\n\n",reg_num,data);
}
else if(cmd[0]=='w')
{
printf("data is 0x%x\n",data);
	cpldWrite(reg_num, data);
	printf("write now...\n\n");
}
else
	printf("cmd error\n\n");
}
#endif
	unsigned char uData;

count++;
//printf("count is %d\n",count);

// test_led_flag != 0 , the command line is testing the arm led now,so stop warning_check
	if(0 == test_led_flag)
	{
		warning_check();
	}
	else
	{
		alarm_led_enable(test_led_flag);
	}

	powerStatus_check();

	if(count >= 5)
	{
		cpldRead(CS1_RUN_LED,&uData);

		if(0 == (uData & 0x01))
		{
			uData = 0x01|uData;
		}
		else
		{
			uData = 0xfe & uData;
		}
		cpldWrite(CS1_RUN_LED,uData);
		count = 0;
	}
//	signal(SIGALRM,led_change_status);
}


void run_led()
{
	int res=0;
	struct timeval t;
	static TIMER_OBJ_t *g_pstTimer=NULL;

	cs1_base_init();

	t.tv_sec=2;
	t.tv_usec=0;

	if(NULL == g_pstTimer)
	{
//		select(0,0,0,0,&t);
		g_pstTimer = vosTimerGetFree();
		if(g_pstTimer)
		{
			vosTimerStart(g_pstTimer,REOCURRING_TIMER,100,(FUNCPTR)led_change_status,0,0,0,0,0,0,0,0);//every 100ms
		}
	}
}
/*
int main()
{

	run_led();
	printf("alarm end\n");
	while(1);
	return 0;

}

*/













