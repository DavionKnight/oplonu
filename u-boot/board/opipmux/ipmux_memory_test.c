//#include <stdio.h>
/* 16M  */ 
#include <configs/opipmux.h>
#ifdef INCLUDE_MEM_TEST
	#define MAXPATTERN 2
	#include <common.h>
	#define OPLSYNC //asm("sync")
	#define KSEG0_TO_KSEG1(x) (u32 *)((x)>=(u32 *)0xa0000000?(x):(x+0x20000000))
	#define KSEG1_TO_KSEG0(x) (u32 *)((x)<(u32 *)0xa0000000?(x-0x20000000):(x))

/* 80000000 - 80090000 protected*/
/* 80350000 - 80400000 protected*/
/* 80750000 - 80800000 protected*/
/* 80f50000 - 81000000 protected*/
/*static void print_config(void);*/
static void print_testrange(void);
int is_sys_mem(u32 mem_addr);
void set_memtestrange(u32 start,u32 end,u32 copyto);
void memcheck32(void);
#if 0
void testcopyipmuxa(void);
void testcopy32err2(void);
void testcopy32err1(void);
void testcopy32err(void);
#endif
void testcopy32(void);


void mem_search_word(u32 mem_start,u32 mem_end,u32 val);
int checkcach(void);
int logProcessSum(int at);
int logProcess(int* at);
void testcopy(void);
void debug_halt(void);



u32 proc=0x1000;
u32 as1=0x80200000;
u32 ae1=0x80F00000;

u32 as2=0x81000000;
u32 ae2=0x81D00000;

u32 adis=0xE00000;
/*
static void print_config(void)
{

	printf("	uncach = %x ",*(u32 *)KSEG0_TO_KSEG1(&as1));
	printf("	cach = %x ",as1);
	printf("	addr = %x \n",&as1);

}
*/
static void print_testrange(void)
{
	printf(" start=%x end=%x, cp to start=%x end=%x, dis=%x\n",as1,ae1,as2,ae2,adis);   
}

int is_sys_mem(u32 mem_addr){
	/*
	u32 gp,sp;
	
	printf(" in %s\t",info);
	asm ("li $9,0x8000000c");
	asm ("sw $28, 0($9)");
	gp=*(u32 *)0x8000000c;
	
	asm ("li $9,0x8000000c");
	asm ("sw $29, 0($9)");
	sp=*(u32 *)0x8000000c;

	printf("\n");
	
	if (mem_addr>gp-0x50000) && (mem_addr<gp + 0x50000) return 1;
	if (mem_addr>sp-0x50000) && (mem_addr<sp + 0x50000) return 1;
	*/
	return 0;

}
void set_memtestrange(u32 start,u32 end,u32 copyto)
{
	printf("CFG_SDRAM_BASE=%x,CFG_INIT_SP_OFFSET=%x,sp = %x\n",CFG_SDRAM_BASE , CFG_INIT_SP_OFFSET,CFG_SDRAM_BASE + CFG_INIT_SP_OFFSET);

	asm ("li $9,0x8000000c");
	asm ("sw $28, 0($9)");
	printf("gp=%x ",*(u32 *)0x8000000c);

	asm ("li $9,0x8000000c");
	asm ("sw $29, 0($9)");
	printf("sp=%x ",*(u32 *)0x8000000c);

	printf("\n");


	as1=start;
	ae1=end;

	as2=copyto;
	ae2=copyto+end-start;

	adis=as2-as1;
}

void memcheck32(void)
{
	int i,j,err=0;
	
	unsigned long *p32;
	

	logProcessSum(0);
	i=0;
	err=0;
	j=0;

	printf("	start check32  ...\n",err);

	printf("	check mem 1 ...\n",err);
	for (p32=(unsigned long *)(as1); p32<(unsigned long *)ae1; p32++) {
		i++;

		if (*p32 !=(unsigned long)p32) {
			printf("\n err at 0x%x val 0x%x\n",p32,*p32);
			err++;
		}

		if ((i % proc)==0) {
			puts(".");
		};
	}

	i=0;
	j=0;

	printf("\n	check mem 2 ...\n",err);
	for (p32=(unsigned long *)as2; p32<(unsigned long *)ae2  ; p32++) {
		//logProcess(i);
		
		i++;
		if (*p32!=*(p32-adis/4)) {
			err ++;
			puts(" \n*** error ");
			printf(" dev addr = %x ",p32);
			printf(" source addr = %x ",p32-adis/4);

			printf(" source val = %x ",*(p32-adis/4));
			printf(" dev val = %x\n",*p32);
		}
		if ((i % proc)==0) {
			puts(".");
		};
	}

/*
	printf("	check except mem 1,2 ...\n",err);
	for(p32=(unsigned long *)(0); p32<0x1000000; p32++)
	{
		if !(((p32>=as1) &&  (p32<ae1)) ||  ((p32>=as2) &&  (p32<ae2)))
		{
			if (*p32==(unsigned long)p32)
			{
				printf(" find at 0x%x val 0x%x\n",p32,*p32);
				err++;
			}
		}
		if ((i % proc)==0) 
		{
			puts(".");
		};
		
	}
*/
	printf("\n----------------------------------------------\n");

	printf("	MEM check word OVER : Found %d errors\n",err);

	printf("----------------------------------------------\n");



}


#if 0
void testcopyipmuxa(void)
{
#define myputs puts
#define myprintf printf
	int j=0;
	int i;
	unsigned int *p32;

	myprintf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.\n");
	puts("start ipmuxa memory test in 32 bit mode \n");

	j=0;
	p32=(unsigned int *)(0xa0207f98);

	for (j=0; j<40000; j++) {
		*p32 = j;

		if ((j % proc)==0) {
			*(unsigned int *)0xBF004000='.';
			udelay(20000);
		};
	}

	myputs("------------------die ?-----------------------");


	for (i=0; i<10; i++) {
		printf("loop %d.\n",i);
		for (j=0; j<10000; j++) {
			*p32 = j;
			//i=j;
			printf(".%d",j);
		}
	}
   
	myputs("-----------------------------------------");
	udelay(120000000);
	myputs("-----------------------------------------");

	for (j=0; j<10000; j++) {
		*p32 = j;
		//i=j;
		printf(".%d",j);
	}
	udelay(120000000);
	myputs("-----------------------------------------");

	for (j=10000; j<20000; j++) {
		*p32 = j;
		//i=j;
		printf(".%d",j);
	}


	myputs(" init ipmuxa ddr mem 1.1 ok\n");
	myprintf("***************************************************************\n");

}
#endif

void testcopy32(void)
{
	int i,j,err=0;
	
	unsigned int p32tmp;

	unsigned int *p32;
#define myputs puts
#define myprintf printf

	myprintf("***************************************************************\n");
	puts("start memory test in 32 bit mode \n");
	print_testrange();
	myprintf("***************************************************************\n");
	
	i=0;
	j=0;
	p32=(unsigned int *)(as1);
	/*
	for (j=0; j<0x150000; j++) {
		//j++;
	
		*p32 = j;
		udelay(2000);
		if ((j % proc)==0) {
			myputs(".");
			printf("%d",j);
		};
	
	}
	myputs(" init mem 1.1 ok\n");
   	*/

	j=0;
	logProcessSum(2);
	for (p32=(unsigned int *)as1; p32<(unsigned int *)ae1  ; p32++) {
		if (p32==(unsigned int *)0x80207f98) {
			debug_halt();
		}
		logProcess(p32);
		p32tmp=(UINT32)p32 & 0xffffff;
		
		i++;
		*p32=(unsigned int)p32;
		if ((i % proc)==0) {
			myputs(".");
			//printf(" %x",p32tmp);
		};
	}

	OPLSYNC;
	myputs("  write source mem ok \n");

	i=0;
	j=0;
	logProcessSum(3);
	for (p32=(unsigned int *)as2; p32<(unsigned int *)ae2  ; p32++) {
		*((u32 *)0x80000000)=(unsigned int)p32;

		logProcess(p32);
		i++;

		//myprintf("[ %x",p32);
		*p32=*(p32-adis/4);
		/*
		p32tmp=*(p32-adis/4);
		*p32=p32tmp;
		*/
		//myprintf(" %x",p32-adis/4);
		//udelay(100000);
		//OPLSYNC;


		if ((i % proc)==0) {
			myputs(".");
		};

		//myprintf(" %x]",p32tmp);
	}

	i=0;
	OPLSYNC;
	logProcessSum(5);
	myputs(" copy to dest ok \n");

	OPLSYNC;
	for (p32=(unsigned int *)as2; p32<(unsigned int *)ae2  ; p32++) {
		logProcess(p32);
		i++;
		if (*p32!=*((unsigned int *)(p32-adis/4))) {
			err ++;
			myputs(" **** error ");
			myprintf(" dev addr = %x ",p32);
			myprintf(" source addr = %x ",p32-adis/4);

			myprintf(" source = %x ",*(p32-adis/4));
			myprintf(" dev = %x\n",*p32);
		}
		if ((i % proc)==0) {
			myputs(".");
		};
	}
	logProcessSum(6);

	myprintf("\n-------------enter memcheck32-----------------\n");
	memcheck32();

	myprintf("\n----------------------------------------------\n");
	if (err>0) {
		printf("	MEM TEST32 OVER : Found memery error %d \n",err);
	} else {
		printf("	MEM TEST32 OVER : All passed \n",err);

	}
   	
	myprintf("----------------------------------------------\n");
}

#if 0
void testcopy32err(void)
{

	unsigned long *p32;

	volatile u32 val;
	for (p32=(unsigned long *)0x80700000; p32 < (unsigned long *)0x80850000; p32++) {
		myprintf("[ %x",p32);
		val=*(p32-0x500000/4);
		*p32=val;
	}
	printf("test ok.\n");
}


void testcopy32err2(void)
{

	unsigned long *p32;

	for (p32=(unsigned long *)0x80700000; p32<(unsigned long *)ae2  ; p32++) {

		*((u32 *)0x80000000)=(unsigned long)p32;

		myprintf("[ %x",(unsigned long)p32);

		//*p32=*(p32-0x500000/4);
	}
}

void testcopy32err1(void)
{
	unsigned long *p32;

	for (p32=(unsigned long *)0x80700000; p32<(unsigned long *)0x80850000  ; p32++) {

		*((u32 *)0x80000000)=(unsigned long)p32;

		myprintf("[ %x",(unsigned long)p32);

		*p32=*(p32-0x500000/4);
	}
}

#endif




void mem_search_word(u32 mem_start,u32 mem_end,u32 val)
{
	int i,j,err=0;
	unsigned long init;
	unsigned long *p32;


	logProcessSum(0);
	i=0;
	err=0;
	j=0;
	init=0xEEEEEEEE;

	j=0;

	logProcessSum(1);

	for (p32=(unsigned long *)(mem_start); p32<(unsigned long *)mem_end; p32++) {
		i ++;

		if (val==*p32 ) {
			printf(" find at 0x%x val 0x%x\n",p32,*p32);
			err++;
		}

		if ((i % proc)==0) {
			myputs(".");
		};

		if (ctrlc()) {
			printf("user interrupted at p32=0x%x\n",p32);
			return ;
		}


	}
	printf("\n----------------------------------------------\n");
	if (err>0) {
		printf("	MEM search OVER : Found %d errors\n",err);
	}
	printf("----------------------------------------------\n");



}

int checkcach(void)
{
	if (*(u32 *)0x80f3fe90!=*(u32 *)0xa0f3fe90) {
		printf(" chch error : 0x80f3fe90=%x,0xa0f3fe90=%x\n", *(u32 *)0x80f3fe90,*(u32 *)0xa0f3fe90);
		return 1;
	} else {
		return 0;

	}
}

int logProcessSum(int at)
{
	*((u32 *)0x80000050)=at;
	return 0;
}

int logProcess(int* at)
{
	*((u32 *)0x80120000)=(int)at;
	return 0;
}

void testcopy(void)
{
	int i,j,err=0;
	unsigned long init;
	char *p;

	printf("\n***************************************************************\n");
	printf("*	start memory test in 8 bit mode 		*\n");
	print_testrange();
	printf("***************************************************************\n");

	i=0;
	err=0;
	j=0;
	init=0xaaaaaaaa;

	checkcach();

	j=0;
	for (p=(char *)as1; p<(char *)ae1  ; p++) {

		i++;

		*p=(char)p;	

		if (0==(i % proc)) {
			puts(".");
		}

	}

	puts("\n---- write source mem ok ---- \n");
	checkcach();

	i=0;
	j=0;

	printf("\ncp 0x%x to 0x%x, debug info:*0x80000000=p;*0x80000004=*p;*0x80000008=i;\n",p-adis,p);

	checkcach();
	for (p=(char *)as2; p<(char *)ae2  ; p++) {
		i++;

		*p=*(p-adis);

		*((u32 *)0x80000000)=(u32)p;
		*((u32 *)0x80000004)=*p;
		*((u32 *)0x80000008)=i;

		if ((i % proc)==0) {
			puts(".");
		};

	}
	i=0;

	puts("\n---- copy to dest ok ---- \n");
	for (p=(char *)as2; p<(char *)ae2  ; p++) {
		i++;

		if (*p!=*((char *)(p-adis))) {
			err ++;
			*((u32 *)0x8000000c)=*((char *)(p-adis));

			puts(" **** error ");
			printf(" dev addr = %x ",p);
			printf(" source addr = %x ",p-adis);

			printf(" source = %x ",*(p-adis));
			printf(" dev = %x\n",*p);

			debug_halt();
		}
		if ((i % proc)==0) {
			puts(".");
		};
	}




	printf("\n***************************************************************\n");
	if (err>0) {
		printf("*	MEM TEST8 OVER : Found memery error %d \n",err);

	} else {
		printf("*	MEM TEST8 OVER : All passed \n",err);

	}
	printf("***************************************************************\n");


}

void debug_halt(void)
{
	//print_gp("in halt1");
	//puts("in dbg!\n");
}

#endif

