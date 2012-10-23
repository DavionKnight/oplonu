/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * RTC, Date & Time support: get and set date & time
 */
#include <common.h>
#include <command.h>

#if (CONFIG_COMMANDS & CFG_CMD_UU)
#define printfreg(fmt,args...) //printf(fmt,##args)
#define printfreg1(fmt,args...) //printf(fmt,##args)
#define printfreg2(fmt,args...) //printf(fmt,##args)

#ifdef INCLUDE_REG_TEST
static void regtest_show_usage(void);
#endif

int do_uenpr (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int val;
	extern int NetBreak;

	switch (argc) {
	case 2:		
		val=(int)simple_strtoul(argv[1],NULL,16);

		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (val)
		{
			printf("	ping reply is enabled now!\n");
			NetBreak=0;
		}
	else
		{
			printf("	ping reply is disabled now!\n");
			NetBreak=1;
		}
	
	return 1;
}

int do_udbgrx (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{


	int val;
	extern u16 debug_rx;



	switch (argc) {
	case 2:		
		val=(int)simple_strtoul(argv[1],NULL,16);

		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (val)
		{
			printf("	debug rx now!\n");
			debug_rx=val;
		}
	else
		{
			printf("	exit debug rx now!\n");
			debug_rx=0;


		}
	
	return 1;
}

int do_udbgs (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern void debug_pck_clear_statistic(void);
	extern void debug_pck_display_statistic(void);
	int val;
	switch (argc) {
	case 2:		
		val=(int)simple_strtoul(argv[1],NULL,16);
		if (val==1) {
			puts("RX and TX statistic data was cleared\n");
			debug_pck_clear_statistic();
			
		}

		break;
	default:

		break;
	}
	debug_pck_display_statistic();	
	return 1;
}


int do_udbgtx (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int val;
	extern u16 debug_tx;



	switch (argc) {
	case 2:		
		val=(int)simple_strtoul(argv[1],NULL,16);
		break;
		
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (val)
	{
		printf("	debug tx now!\n");
		debug_tx=val;
	}
	else
	{
		printf("	exit debug tx now!\n");
		debug_tx=0;
	}
	
	return 1;
}


#ifdef INCLUDE_REG_TEST
int do_utestRegs(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern int ipmuxRegisterTest(UINT32 pu4RegId, UINT32 nNum, UINT32 u1TestValue, UINT32 bPrintAll, UINT32 nLoopTimes);
	int i;
	char *thisarg;
	

	char sFile[100]="";	 /* 0: test all register, 1:*/
	int iTestPatterns=4;
	unsigned int testPatterns[]={0xFFFFFFFF,0x00000000,0xAAAAAAAA,0x55555555};
	int iNum=0;	/* 0 to test all ; other test num*/
	int bAddr=0;
	int iexlude=0;
	int iloop=1;
	int iAddr=REGID_INIT_VALUE;
	int iPrintMode=0;/*0=print error;1=print all*/
	
	argc--;
	argv++;
	
	/* Parse any options */
	while (argc >= 1 && **argv == '-') {
		printfreg("argv=[%s]\n",argv);
		thisarg = *argv;
		thisarg++;
		printfreg("thisarg=[%s]\n",thisarg);
		switch (*thisarg) {
		case 'h':
			argv++;
			
			regtest_show_usage();
			return 1;	
			
			break;
		case 'f':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			argv++;
			printfreg("fargv=[%s]\n",argv);
			strcpy(sFile,*argv);
			break;
			
		case 'v':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			
			argv++;
			printfreg(" V argv=[%s]\n",argv);
			iTestPatterns=1;

			testPatterns[0]= strtoul(*argv,NULL,0);
			break;
			
		case 'p':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			
			argv++;
			printfreg(" P argv=[%s]\n",argv);
			
			iPrintMode=strtoul(*argv,NULL,0);
			break;
			
		case 'a':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			
			argv++;
			printfreg(" A argv=[%s]\n",argv);
            
			bAddr=1;
			iAddr=strtoul(*argv,NULL,0);
			break;
				case 'e':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			
			argv++;
			printfreg(" E argv=[%s]\n",argv);
            
			iexlude=strtoul(*argv,NULL,0);
			break;	
		case 'l':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			
			argv++;
			
			printfreg(" L argv=[%s]\n",argv);

			iloop=strtol(*argv,NULL,0);
			
			printfreg("argv =%x,iNum = %d\n",*((char *)argv),iNum);
			break;

			
		case 'n':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}

			argv++;
			
			printfreg(" N argv=[%s]\n",argv);

			iNum=strtol(*argv,NULL,0);
			
			printfreg("argv =%x,iNum = %d\n",*((char *)argv),iNum);
			break;
			
		default:
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
		}
		argc--;
		argv++;
		printfreg("argc =%d\n",argc);
		printfreg("argv=[%s]\n",argv);
	}
	
	printfreg2("iMode = %d\n",iMode);
	printfreg2("iNum = 0x%x\n",iNum);
	
	printfreg2("iAddr = 0x%x\n",iAddr);		
	printfreg2("bAddr = 0x%x\n",bAddr);
	printfreg2("sFile = %s\n",sFile);
	
	for(i=0; i< iTestPatterns ; i++)
	{
		printfreg2("TestPatterns[%d] = 0x%x\n",i,testPatterns[i]);   
	}	
	
	/* do default test */
	for(i=0; i< iTestPatterns ; i++)
	{
		ipmuxRegisterTest(iAddr,iNum,testPatterns[i],iPrintMode,iloop);        
	}
	return 1;
}
#endif

#ifdef INCLUDE_TAB_TEST
int do_utestTabs(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	
	char *thisarg;
	/*20 words*/
	char testVal[400]="0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";	 /* 0: test all register, 1:*/

	int iNum=0;		/* 0 to test all ; other test num*/
	int bAddr=1;
	int iRec=1;
	int iOffset=0;
	int iAddr=1;
	int iloop =1;
	int iPrintMode=0;	/*0=print error;1=print all*/

	argc--;
	argv++;
	
	/* Parse any options */
	while (argc >= 1 && **argv == '-') {
		printfreg("argv=[%s]\n",argv);
		thisarg = *argv;
		thisarg++;
		printfreg("thisarg=[%s]\n",thisarg);
		switch (*thisarg) {

		case 'h':
			argv++;
			
			regtest_show_usage();
			return 1;	
			
			break;
		case 'r':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			argv++;
			printfreg("fargr=[%s]\n",argv);
			iRec=strtoul(*argv,NULL,0);
			break;
			
		case 'v':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}

			argv++;

			printfreg(" V argv=[%s]\n",argv);
			strcpy(testVal,*argv);
			break;
			
		case 'p':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			
			argv++;
			printfreg(" P argv=[%s]\n",argv);
			
			iPrintMode=strtoul(*argv,NULL,0);
			break;
			
		case 'a':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}

			argv++;
			printfreg(" A argv=[%s]\n",argv);
			
			bAddr=1;
			iAddr=strtoul(*argv,NULL,0);
			break;
		case 'o':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}

			argv++;
			printfreg(" O argv=[%s]\n",argv);
		
			iOffset=strtoul(*argv,NULL,0);
			break;	
		case 'n':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			
			argv++;
			
			printfreg(" N argv=[%s]\n",argv);

			iNum=strtol(*argv,NULL,0);
			
			printfreg("argv =%x,iNum = %d\n",*((char *)argv),iNum);
			break;
		case 'l':
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
			
			argv++;
			
			printfreg(" N argv=[%s]\n",argv);

			iloop=strtol(*argv,NULL,0);
			
			break;
				
		default:
			if (--argc <= 0)
			{
				regtest_show_usage();
				return 1;
			}
		}
		argc--;
		argv++;
		printfreg("argc =%d\n",argc);
		printfreg("argv=[%s]\n",argv);
	}
	
	printfreg2("iMode = %d\n",iMode);
	printfreg2("iNum = 0x%x\n",iNum);
	
	printfreg2("iAddr = 0x%x\n",iAddr);		
	printfreg2("bAddr = 0x%x\n",bAddr);

	ipmuxTableTest(iAddr,iNum,iOffset,iRec,testVal,iPrintMode,iloop);
	return 1;
}
#endif
/*
static void regtest_show_usage(void)
{
	printf("Test registers by write and read values.\n");
	printf("Usage:regtest [-a REGID] [-n NUM] [-v TEST VALUE] [-f FILE] [-p PRINT MODE]\n");
	printf("\t-a REGID \n\t\tThe offset of the register.\n");
	printf("\t-n NUM \n\t\tThe numbers of the registers to be tested.\n\t\t0 : to test all;\n\t\tother : real num.\n");
	printf("\t-v TEST VALUE \n\t\tThe testvalue to be written to the registers.\n");
	printf("\t-f FILE \n\t\tThe file to be loaded. \n\t\tThe file format is: \"regid,num,rw_mask,wo_mask,ro_mask,default,flag\".\n");
	printf("\t-p PRINT MODE\n\t\t0 :only print err registers;\n\t\t1 : print all tested registers.\n");
}*/
#ifdef INCLUDE_PERIPHA_TEST	
int do_utesti (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int val=0;
	switch (argc) {
	case 2:		
		val=(int)simple_strtoul(argv[1],NULL,16);
		break;
	default:
		break;
	}

	switch (val) {
	
	case 0:		

		test_mips_timer_int();
	
		break;
	case 1:		
	
		test_fel_int();
		break;
	case 2:		
		test_uart_int();
		break;
	default:
		break;
	}
	return 1;	
}
#endif
#ifdef INCLUDE_PERIPHA_TEST
int do_utesthdlc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int val=0;
	int arg1=0;
	int arg2=0;
	switch (argc) {
	case 4:
		arg2=(int)simple_strtoul(argv[3],NULL,10);
	case 3:
		arg1=(int)simple_strtoul(argv[2],NULL,10);
	case 2:		
		val=(int)simple_strtoul(argv[1],NULL,10);
		break;
	default:
		break;
	}

	switch (val) {
	case 0:
		test_ipmux_hdlc_loopback();
		break;		
	case 1:
		test_ipmux_hdlc_recv();
		break;	
	case 2: 
		ipmuxHdlcCounterPrint();
		break;		
	case 3: 
		test_ipmux_hdlc_tx_disabletx();
		break;	
	case 4: 
		printf("hdlc send str:");
		ipmuxHdlcStrSend(argv[2]);
		break;	
	
	case 5: 
		printf("hdlc send mem addr 0x%08x..+0x%08x",arg1,arg2);
		ipmuxHdlcMemSend((char *)arg1,arg2);
		break;
	case 6:
		test_ipmux_hdlc_tx_entx();
		break;	

	}
	return 1;	
}
#endif



int do_udelay (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int val=0;
	switch (argc) {
	case 2:		
		val=(int)simple_strtoul(argv[1],NULL,10);
		break;
	default:
		break;
		return;
	}

	printf("Now delay for %dus = %dms = %ds.\n", val, val/1000, val/1000000);
	udelay(val);
	printf("----------------Finished---------------\n");
	return 1;
}

int do_utest (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern void ipmuxGpioTest(void);
	int val=0;
	//char s[]="Uabc";
	switch (argc) {
	case 2:		
		val=(int)simple_strtoul(argv[1],NULL,16);
		break;
	default:
		break;
	}

	switch (val) {
	
	case 0:	
#ifdef INCLUDE_PERIPHA_TEST	
		test_udelay();
#endif		
		break;
	case 1:		
#ifdef INCLUDE_PERIPHA_TEST	
		test_mips_timer();
#endif		
		break;
	case 2:	
#ifdef INCLUDE_PERIPHA_TEST	
		test_ipmux_wdt();
#endif
		break;
	case 3:		
#ifdef INCLUDE_PERIPHA_TEST	
		test_ipmux_timer();
#endif		
		break;
	case 4:
		/*other test is in the test trc.tcl*/
#ifdef INCLUDE_PERIPHA_TEST	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             	
		test_ipmux_rtc();
#endif		
		break;
	case 5:
#ifdef INCLUDE_SDRAM_COUNTER
		//testSdramCounter();
#else
		printf("Not include sram counter routings !\n");
#endif
		break;
	case 6:
		ipmuxGpioTest();
		
	default:
		break;
	}
	return 1;
}

#ifdef INCLUDE_COUNTER
int do_ucounter (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int oper=0x001;
	int kind=-1;
	int hexDisplay=1;
	switch (argc) {
	case 4:
		hexDisplay=(int)simple_strtol(argv[3],NULL,0);
	case 3:		
		kind=(int)simple_strtol(argv[2],NULL,0);
	case 2:		
		oper=(int)simple_strtoul(argv[1],NULL,0);
		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 0;
		break;
	}
	
	CounterProcess(kind, oper & 0x001, oper & 0x100, oper & 0x010,hexDisplay);
	
	return 1;
	
}
#endif

#ifdef INCLUDE_SDRAM_COUNTER
int do_usdcounter (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int oper=0;
	int kind=-1;

	int arg1=0;
	int arg2=0;
	int arg3=0;

	
	switch (argc) {
	case 6:		
		arg3=(int)simple_strtol(argv[5],NULL,0);	
	case 5:		
		arg2=(int)simple_strtol(argv[4],NULL,0);	
	case 4:		
		arg1=(int)simple_strtol(argv[3],NULL,0);	
	case 3:		
		kind=(int)simple_strtol(argv[2],NULL,0);
	case 2:		
		oper=(int)simple_strtoul(argv[1],NULL,0);
		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 0;
		break;
	}
	
	switch (kind) {
	case 0:		
		flowCounterProcess(oper,1,arg1,arg2);
		break;
	case 1:		
		clsHitCounterProcess(oper,1,arg1,arg2);
		break;
	case 2:		
		accessPortCounterProcess(oper,1,arg1,arg2,0);
		break;
	case 3:
		//accessPortVlanCounterProcess(oper,1,arg1,arg2,0);
		break;
	case 4:
		gewPortVlanCounterProcess(oper,1,arg1,arg2,0);
		break;
	case 5:
		linkCounterProcess(oper,1,arg1,arg2,0);
		break;
	case 6:
		groupCounterProcess(oper,1,arg1,arg2,0);
		break;
	case -1:
		flowCounterProcess(oper,1,arg1,arg2);
		clsHitCounterProcess(oper,1,arg1,arg2);
		accessPortCounterProcess(oper,1,arg1,arg2,0);
		//accessPortVlanCounterProcess(oper,1,arg1,arg2,0);
		gewPortVlanCounterProcess(oper,1,arg1,arg2,0);
		linkCounterProcess(oper,1,arg1,arg2,0);
		groupCounterProcess(oper,1,arg1,arg2,0);
		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 0;
		break;
	}
	return 1;
	
}
#endif

#ifdef INCLUDE_TABLE_RW_FUNC	
int do_urt (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern void rTab(int tid,int  item);
	int i,item=0,tid,itemend=-1;

	switch (argc) {
	case 4:
		itemend=(int)simple_strtol(argv[3],NULL,0);
	case 3:		
		item=(int)simple_strtol(argv[2],NULL,0);
		if(-1==itemend) itemend=item;
			
	case 2:		
		tid=(int)simple_strtoul(argv[1],NULL,0);
		printf("\n");break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 0;
		printf("\n");break;
	}

	for(i=item;i<=itemend;i++)
		{
			rTab(tid,i);
		}
	
	return 1;	
}


int do_uwt (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern void wtab(int tid, int offset,int val,int val1);
	int item=0,val=0,tid,val1=0xAAAA3333;

	switch (argc) {
	case 5:		
		val1=(int)simple_strtol(argv[4],NULL,0);
	case 4:		
		val=(int)simple_strtol(argv[3],NULL,0);
	case 3:		
		item=(int)simple_strtol(argv[2],NULL,0);
	case 2:		
		tid=(int)simple_strtoul(argv[1],NULL,0);
		printf("\n");break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 0;
		printf("\n");break;
	}
	wtab(tid,item,val,val1);	
	return 1;
}
#endif

/***************************************************/

#ifdef INCLUDE_TABLE_RW_FUNC	
U_BOOT_CMD(
	urt,	4,	1,	do_urt,
	"urt     - read ipmux table.\n",
	" tabid,offset  -tabid=-1 to show tabids.\n  "
);

U_BOOT_CMD(
	uwt,	5,	1,	do_uwt,
	"uwt     - write ipmux table.\n",
	" tabid,offset val -\n  "
);
#endif

#ifdef INCLUDE_COUNTER
U_BOOT_CMD(
	uc,	4,	1,	do_ucounter,
	"uc      - ipmux counter operations.\n",
	" oper,kind,hexshow  -oper 0x111 clear update show.\n\t-kind 	0=GE0 ,1=GE1 , 2=FE ,3=UTOPIA,4=POSPHY,\n\t\t5=MULTICAST, 6=TRAFFIC_MANAGEMENT, 7=PACKET_PROCESS_ENGINE ,8=BRIDGE ,9=IWF .\n  "
);
#endif

#ifdef INCLUDE_SDRAM_COUNTER
U_BOOT_CMD(
	usdc,	6,	1,	do_usdcounter,
	"usdc    - ipmux sdram counter operations.\n",
	" oper,kind,hexshow  -oper 0x111 clear update show.\n\t-kind 	FLOW=0,	CLASSIFY=1,	PORT=2,\n\t\tPORT_VLAN=3, LINK_BONDING=4, ALL=-1.\n  "
);
#endif
#ifdef INCLUDE_PERIPHA_TEST	
U_BOOT_CMD(
	utesti,	2,	1,	do_utesti,
	"utesti  - a collection of int test program.\n",
	" dest  -0:mips timer int; 1:fe; 2:uart.\n"
);
#endif
#ifdef INCLUDE_PERIPHA_TEST
U_BOOT_CMD(
	utesth,	4,	1,	do_utesthdlc,
	"utesth  - hdlc test program.\n",
	" cata  - 0:hdlc loop; 1:hdlc rx. 2:hdlc counter. 3,4:tx. 5:tx 0x80100000..+0x200000.\n"
);
#endif

U_BOOT_CMD(
	utest,	2,	1,	do_utest,
	"utest   - a collection of test program.\n",
	" dest  -0:udelay; 1:mips timer; 2:wdt; 3:ipmux timer; 4:rtc; 5:sramcounter; 6:gpio\n"
);

U_BOOT_CMD(
	udelay,	2,	1,	do_udelay,
	"udelay	- delay for N us.\n",
	" time	- delay for time us.\n"
);

#ifdef INCLUDE_TAB_TEST
U_BOOT_CMD(
	uttab,	12,	1,	do_utestTabs,
	"uttab   - test ipmux tables\n",
	"[-a eTableId] [-n NUM] [-r uNumRec] [-v TEST VALUE] [-p PRINT MODE]\n"
);
#endif

#ifdef INCLUDE_REG_TEST
U_BOOT_CMD(
	utreg,	10,	1,	do_utestRegs,
	"utreg   - test ipmux registers\n",
	"[-a REGID] [-n NUM] [-v TEST VALUE] [-f FILE] [-p PRINT MODE][0x1111-know ind_ctr ind_shadow skip]\n"
);
#endif
U_BOOT_CMD(
	udbgs,	2,	1,	do_udbgs,
	"udbgs   - clear or display RX and TX statistic\n",
	"clear   - rx and tx statistic, clear: 1 to clear, 0 to display\n"
);


U_BOOT_CMD(
	udbgrx,	2,	1,	do_udbgrx,
	"udbgrx  - enable/disable rx debug\n",
	"v       - enable/disable rx debug\n"
);

U_BOOT_CMD(
	udbgtx,	2,	1,	do_udbgtx,
	"udbgtx  - enable/disable ping reply\n",
	"v       - enable/disable ping reply\n"
);

U_BOOT_CMD(
	uenpr,	2,	1,	do_uenpr,
	"uenpr   - enable/disable ping reply\n",
	"v       - enable/disable ping reply\n"
);

#endif	/* CFG_CMD_UU */
