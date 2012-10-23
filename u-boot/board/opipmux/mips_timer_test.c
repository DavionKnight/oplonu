#include <common.h>

#ifdef INCLUDE_PERIPHA_TEST		
#include "ipmux_fe.h"
/************************int************************************/
#define INT_CTRL_ADDR 0xbf002a00
typedef struct S_INT_CTRL
{
	volatile u32 SOURCE1;
	volatile u32 SOURCE2;
	volatile u32 ENABLE1;
	volatile u32 ENABLE2;
	volatile u32 CLEAR1;
	volatile u32 CLEAR2;
	volatile u32 ACK;	
	volatile u32 TRIGGERMODE;
	volatile u32 PRI0;
	volatile u32 PRI1;
	volatile u32 PRI2;
	volatile u32 PRI3;
	volatile u32 PRI4;
	volatile u32 PRI5;
	volatile u32 PRI6;
	volatile u32 PRI7;
} INT_CTRL;


void test_udelay()
{
	printf("CFG_HZ = %d \n",CFG_HZ);
	printf(" ****** test udelay : udelay 10,000,000 = 10s ******\n");
		
	udelay_t(10000000);

	printf(" ****** DONE ****** \n\n");
}

void test_mips_timer()
{
	test_mips_timer_delay();
	test_mips_timer_int();
}

void test_mips_timer_delay()
{
	unsigned int oldvalue=0 ;
	unsigned int newvalue=0 ;

	printf(" ****** test mips timer max value ****** \n\n");
	/*cpu is 25MHz, so the 32 bit counter is max 6 minute. */
	printf("wait for  = 30,000,000\n");
	printf("	1 counter = %08x \n",get_timer(0));
	oldvalue=get_timer(0);
	udelay_t(60000000);
	newvalue=get_timer(0);
	printf("	2 counter = %08x \n",newvalue);
	
	printf(" 	>>> counter dis = %08x \n",newvalue-oldvalue);
	
	for (;;)
	{	
		udelay_t(5000000);
			if (ctrlc())
			{
				printf("interrupted!\n");
				return ;
			}
		printf("3 	counter = %08x \n",get_timer(0));
		if (get_timer(0)>0xf0000000)
		{
			printf(" 	@ wait for  = 30,000,000\n");
			oldvalue=get_timer(0);
			udelay_t(30000000);
			newvalue=get_timer(0);
			printf("4 	>   counter = %08x \n",newvalue);
			printf(" 	>>> counter dis = %08x \n",newvalue-oldvalue);
		}
	}
	
	printf(" ****** DONE ****** \n\n");

}
__inline__ mips_cp0_set(u32 reg_num,u32 v) 
{
switch (reg_num)
	{
	case 0:	asm volatile("mtc0 %0, $0":"=r"(v):"0"(v));break;
	case 1:	asm volatile("mtc0 %0, $1":"=r"(v):"0"(v));break;
	case 2:	asm volatile("mtc0 %0, $2":"=r"(v):"0"(v));break;
	case 3:	asm volatile("mtc0 %0, $3":"=r"(v):"0"(v));break;
	case 4:	asm volatile("mtc0 %0, $4":"=r"(v):"0"(v));break;
	case 5:	asm volatile("mtc0 %0, $5":"=r"(v):"0"(v));break;
	case 6:	asm volatile("mtc0 %0, $6":"=r"(v):"0"(v));break;
	case 7:	asm volatile("mtc0 %0, $7":"=r"(v):"0"(v));break;
	case 8:	asm volatile("mtc0 %0, $8":"=r"(v):"0"(v));break;
	case 9:	asm volatile("mtc0 %0, $9":"=r"(v):"0"(v));break;
	case 10:asm volatile("mtc0 %0, $10":"=r"(v):"0"(v));break;
	case 11:asm volatile("mtc0 %0, $11":"=r"(v):"0"(v));break;
	case 12:asm volatile("mtc0 %0, $12":"=r"(v):"0"(v));break;
	case 13:asm volatile("mtc0 %0, $13":"=r"(v):"0"(v));break;
	case 14:asm volatile("mtc0 %0, $14":"=r"(v):"0"(v));break;
	case 15:asm volatile("mtc0 %0, $15":"=r"(v):"0"(v));break;
	case 16:asm volatile("mtc0 %0, $16":"=r"(v):"0"(v));break;
	case 17:asm volatile("mtc0 %0, $17":"=r"(v):"0"(v));break;
	case 18:asm volatile("mtc0 %0, $18":"=r"(v):"0"(v));break;
	case 19:asm volatile("mtc0 %0, $19":"=r"(v):"0"(v));break;
	case 20:asm volatile("mtc0 %0, $20":"=r"(v):"0"(v));break;
	case 21:asm volatile("mtc0 %0, $21":"=r"(v):"0"(v));break;
	case 22:asm volatile("mtc0 %0, $22":"=r"(v):"0"(v));break;
	case 23:asm volatile("mtc0 %0, $23":"=r"(v):"0"(v));break;
	case 24:asm volatile("mtc0 %0, $24":"=r"(v):"0"(v));break;
	case 25:asm volatile("mtc0 %0, $25":"=r"(v):"0"(v));break;
	case 26:asm volatile("mtc0 %0, $26":"=r"(v):"0"(v));break;
	case 27:asm volatile("mtc0 %0, $27":"=r"(v):"0"(v));break;
	case 28:asm volatile("mtc0 %0, $28":"=r"(v):"0"(v));break;
	case 29:asm volatile("mtc0 %0, $29":"=r"(v):"0"(v));break;
	default:printf("Reg number invalid\n");break;
	}
}


__inline__ mips_cp0_get(u32 reg_num) 
{
u32 v;
switch (reg_num)
	{
	case 0:	asm volatile("mfc0 %0, $0":"=r"(v):"0"(v));break;
	case 1:	asm volatile("mfc0 %0, $1":"=r"(v):"0"(v));break;
	case 2:	asm volatile("mfc0 %0, $2":"=r"(v):"0"(v));break;
	case 3:	asm volatile("mfc0 %0, $3":"=r"(v):"0"(v));break;
	case 4:	asm volatile("mfc0 %0, $4":"=r"(v):"0"(v));break;
	case 5:	asm volatile("mfc0 %0, $5":"=r"(v):"0"(v));break;
	case 6:	asm volatile("mfc0 %0, $6":"=r"(v):"0"(v));break;
	case 7:	asm volatile("mfc0 %0, $7":"=r"(v):"0"(v));break;
	case 8:	asm volatile("mfc0 %0, $8":"=r"(v):"0"(v));break;
	case 9:	asm volatile("mfc0 %0, $9":"=r"(v):"0"(v));break;
	case 10:asm volatile("mfc0 %0, $10":"=r"(v):"0"(v));break;
	case 11:asm volatile("mfc0 %0, $11":"=r"(v):"0"(v));break;
	case 12:asm volatile("mfc0 %0, $12":"=r"(v):"0"(v));break;
	case 13:asm volatile("mfc0 %0, $13":"=r"(v):"0"(v));break;
	case 14:asm volatile("mfc0 %0, $14":"=r"(v):"0"(v));break;
	case 15:asm volatile("mfc0 %0, $15":"=r"(v):"0"(v));break;
	case 16:asm volatile("mfc0 %0, $16":"=r"(v):"0"(v));break;
	case 17:asm volatile("mfc0 %0, $17":"=r"(v):"0"(v));break;
	case 18:asm volatile("mfc0 %0, $18":"=r"(v):"0"(v));break;
	case 19:asm volatile("mfc0 %0, $19":"=r"(v):"0"(v));break;
	case 20:asm volatile("mfc0 %0, $20":"=r"(v):"0"(v));break;
	case 21:asm volatile("mfc0 %0, $21":"=r"(v):"0"(v));break;
	case 22:asm volatile("mfc0 %0, $22":"=r"(v):"0"(v));break;
	case 23:asm volatile("mfc0 %0, $23":"=r"(v):"0"(v));break;
	case 24:asm volatile("mfc0 %0, $24":"=r"(v):"0"(v));break;
	case 25:asm volatile("mfc0 %0, $25":"=r"(v):"0"(v));break;
	case 26:asm volatile("mfc0 %0, $26":"=r"(v):"0"(v));break;
	case 27:asm volatile("mfc0 %0, $27":"=r"(v):"0"(v));break;
	case 28:asm volatile("mfc0 %0, $28":"=r"(v):"0"(v));break;
	case 29:asm volatile("mfc0 %0, $29":"=r"(v):"0"(v));break;

	default:printf("Reg number invalid\n");v=0;break;
	}
	return v;
}

void test_mips_timer_int(u32 compare)
{
	volatile INT_CTRL *p_int=(volatile INT_CTRL *)INT_CTRL_ADDR;
	
	/* !!! set eic mode in the future */
	printf("******test mips timer int******\n");
	/****** init int ctrl ******/
	/* clear source */
	print_cp0reg_int();
	p_int->CLEAR1=0xFFFFFFFF;
	p_int->CLEAR2=0xFFFFFFFF;

	/* enable all interrupt */
	p_int->ENABLE1=0xFFFFFFFF;
	p_int->ENABLE2=0xFFFFFFFF;

	/****** test mips timer interrupt ******/
	printf("set count 0\n");
	mips_cp0_set(CP0_ID_COUNT,0x0);   	    //count
	print_cp0reg_int();
	
	printf("set compare 10000000\n");
	mips_cp0_set(CP0_ID_COMPARE,10000000);    //compare
	print_cp0reg_int();

	printf("set STATUS 0x40fb05\n");
	mips_cp0_set(CP0_ID_STATUS,0x40fb05);   //ie = bit0	    
	print_cp0reg_int();	
	*((u32 *)0x80000080)=0x40895800;
	*((u32 *)0x80000100)=0x40895800;
	*((u32 *)0x80000180)=0x40895800;
	*((u32 *)0x80000200)=0x40895800;
	*((u32 *)0x80000300)=0x40895800;
	
	/* wait for timer interrupt */
	for(;;)	{
		print_cp0reg_int();
		if (ctrlc())
			{
				printf("interrupted!\n");
				return ;
			}
		udelay(2000000);
	}


}

void test_fel_int()
{
	//volatile INT_CTRL *p_int=(volatile INT_CTRL *)INT_CTRL_ADDR;
	volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;
	
	
	p_gel_dma->BDIE=0x7;
	printf("******test fe int******\n");
	/* !!! set eic mode in the future */
	/****** init int ctrl ******/
	/* clear source */
	print_cp0reg_int();
	//p_int->CLEAR1=0xFFFFFFFF;
	//p_int->CLEAR2=0xFFFFFFFF;

	/* enable all interrupt */
	//p_int->ENABLE1=0xFFFFFFFF;
	//p_int->ENABLE2=0xFFFFFFFF;
	
	//printf("int control source=%08x %08x  \n",	p_int->SOURCE1 ,p_int->SOURCE2 );

	
	/****** test mips timer interrupt ******/
	//mips_cp0_set(CP0_ID_STATUS,0x40fb05);   	    
	//mips_cp0_set(CP0_ID_STATUS,0x10008401);   
	
	print_cp0reg_int();

	printf("init status :\n");
	printf("	BDINT=%08x\n",p_gel_dma->BDINT);

	printf("send packet :\n");
	opipmux_gel_send(NULL,(char *)0x80800000,71);

	printf("	BDINT=%08x\n",p_gel_dma->BDINT);

	printf("	BDINT=%08x\n",p_gel_dma->BDINT);

	printf("set BDINT =0:\n");
	p_gel_dma->BDINT=1;

	printf("	BDINT=%08x\n",p_gel_dma->BDINT);

	/* wait for timer interrupt */
	for(;;)	{
		printf("-----------------------\n");
		printf("send pck\n");
		eth_rx();
		opipmux_gel_send(NULL,(char *)0x80800000,71);
		print_cp0reg_int();
		printf("	BDIE =%08x\n",p_gel_dma->BDIE);
		printf("	BDINT=%08x\n",p_gel_dma->BDINT);
		printf("set BDINT =0\n");
		
		p_gel_dma->BDINT=3;
		printf("	BDINT=%08x\n",p_gel_dma->BDINT);

		//opipmux_gel_send(NULL,(char *)0x80800000,71);
		//printf("int control source=%08x %08x  \n",	p_int->SOURCE1 ,p_int->SOURCE2 );
		//p_int->CLEAR1=0xFFFFFFFF;
		//p_int->CLEAR2=0xFFFFFFFF;
		//printf("int control source=%08x %08x  \n",	p_int->SOURCE1 ,p_int->SOURCE2 );
		if (ctrlc())
			{
				printf("interrupted!\n");
				return ;
			}
		udelay(1000000);
	}

}

print_cp0reg_int()
{
	//printf("DEBUG=%08x,DM=%x\t",mips_cp0_get(CP0_ID_DEBUG),mips_cp0_get(CP0_ID_DEBUG)>>30 & 0x1);
	//printf("	CNT=%08x\t",mips_cp0_get(CP0_ID_COUNT));
	//printf(" CMP=%08x\t",mips_cp0_get(CP0_ID_COMPARE));

	//printf(" STS=%08x\t",mips_cp0_get(CP0_ID_STATUS));
	printf("	CAUSE=%08x(IP=%x)(EXEC=%x)",mips_cp0_get(CP0_ID_CAUSE),\
		mips_cp0_get(CP0_ID_CAUSE)>>10 & 0x3F,mips_cp0_get(CP0_ID_CAUSE)>>2 & 0x1F);
	//printf("EPC=%08x\t",mips_cp0_get(CP0_ID_EPC));
	printf("\n");
}

void test_uart_int()
{
	volatile INT_CTRL *p_int=(volatile INT_CTRL *)INT_CTRL_ADDR;
	volatile gmac_dmal_reg *p_gel_dma = (volatile gmac_dmal_reg *)OP_IPMUX_GEL_DMA_ADDR;
	u32 uart_int=0;
	
	printf("******test uart int******\n");
	/*init uart int*/
	*((volatile u32 *)0xbf004004)=0xc0;

	/* !!! set eic mode in the future */
	/****** init int ctrl ******/
	/* clear source */
	print_cp0reg_int();
	p_int->CLEAR1=0xFFFFFFFF;
	p_int->CLEAR2=0xFFFFFFFF;

	/* enable all interrupt */
	p_int->ENABLE1=0xFFFFFFFF;
	p_int->ENABLE2=0xFFFFFFFF;
	
	
	/****** init hw0 interrupt ******/
	mips_cp0_set(CP0_ID_STATUS,0x40fb05);   	    
	
	print_cp0reg_int();

	printf("init status :\n");
	uart_int= *((volatile u32 *)0xbf004008);
	printf("	int =%x,rx int =%08x,tx int =%08x\n",uart_int ,uart_int & 0x40, uart_int & 0x01);

	printf(" ^_^ \n");

	uart_int= *((volatile u32 *)0xbf004008);
	mips_cp0_set(CP0_ID_COUNT,uart_int);   	    //count

	uart_int= *((volatile u32 *)0xbf004008);
	printf("	int =%x,rx int =%08x,tx int =%08x\n",uart_int ,uart_int & 0x40, uart_int & 0x01);
	udelay(1000000000);
}


#endif
