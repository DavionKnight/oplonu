#include <asm/regdef.h>
#include <configs/opipmux.h>
#include "ipmux_ddr.h"

/*------------------------------------------------------------------
 Function:    ipmux_ddr_asm.S

 Description: tune ddr delay value
-------------------------------------------------------------------*/

/* --- tune the ddr just find the first right value --- */
/*#define TUNE_DDR_SIMPLE*/
/*#define DDR_TUNE_DOWNSTREAM*/

#define zw_change

#define DELAYASMDDR    //

#define DELAYASM la s5,0; \
                 or s5,ra; \
                 bal delayasmshort; \
                 nop;\
                 la ra,0;\
                 or ra,s5

/* --- using s5 store the ra value , s4 = 1 ,using ddr params --- */
#define DDR_TUNE_MEMTEST la s5,0; \
                         or s5,ra; \
                         la s4,1; \
                         bal sdram_test_asm; \
                         nop;\
                         la ra,0;\
                         or ra,s5
                         
#define MAB_TBL_ACC_CTRL 0XBF020030
#define MAB_TBL_ACC_DATA 0XBF020038
#define MAB_TBL_WRITE_FLAG 0Xc0000000
#define MAB_TBL_READ_FLAG 0X80000000
#define TBL_ACC_BUSY 0X80000000
#define TBL_GOT_DATA t7  /*read  tbl data to t7*/
	
#define MAB_TBL_DATA_SET(TBL_DATA)  \
						la t0, MAB_TBL_ACC_DATA;  \
						li t1, TBL_DATA;  \
						sw t1, 0(t0);  \
						nop
						
#define MAB_TBL_DATA_GET  \
						la t0, MAB_TBL_ACC_DATA;  \
						lw t7, 0(t0);  \
						nop

#define MAB_TBL_WRITE(TBL_ID,TBL_ACC_ADDR)  \
						la t0, MAB_TBL_ACC_CTRL;  \
						lui t1, TBL_ID;	\
						ori t1, t1, TBL_ACC_ADDR;	\
						addu t1, MAB_TBL_WRITE_FLAG;  \
						sw t1, 0(t0);  \
						nop 
													
#define MAB_TBL_READ(TBL_ID,TBL_ACC_ADDR)  \
						la t0, MAB_TBL_ACC_CTRL;  \
						lui t1, TBL_ID;	\
						ori t1, t1, TBL_ACC_ADDR;	\
						addu t1, MAB_TBL_READ_FLAG;	\
						sw t1, 0(t0);  \
						nop
						
#define WAIT_TBL_ACC_DONE  \
					 1:  \
						la t0, MAB_TBL_ACC_CTRL;  \
						lw t1, 0(t0);  \
						li t2, TBL_ACC_BUSY;  \
						and t1, t1, t2;  \
						bne t1, zero, 1b;  \
						nop;nop;nop;nop;nop;nop

#define mab_tbl_data_bit_check(arg0,arg1) \
	2:\
	MAB_TBL_READ(0,arg1);  \
	WAIT_TBL_ACC_DONE ; \
	MAB_TBL_DATA_GET; \
	li t3,arg0   ; \
	and TBL_GOT_DATA, TBL_GOT_DATA, t3; \
	bne TBL_GOT_DATA,zero,2b ;\
	nop;nop;nop;nop;nop;nop

#define mab_tbl_data_bit_poll(arg0,arg1) \
	2:\
	MAB_TBL_READ(0,arg1) ;\
	WAIT_TBL_ACC_DONE ;\
	MAB_TBL_DATA_GET ;\
	li t3,arg0	 ;\
	and TBL_GOT_DATA, TBL_GOT_DATA, t3 ;\
	bne TBL_GOT_DATA,t3,2b ;\
	nop;nop;nop;nop;nop;nop 
	

/***************************************************
    delay for a short period : using t8, t9
***************************************************/
    .globl  delayasmshort
    .ent    delayasmshort
delayasmshort:

1:  li t8, 0
    /* change from 0x2000 to 0x100 to save boot time */
    li t9, 0x2000

1:  add t8,1
    bne t8,t9,1b
    nop

    /*jump to caller*/
1:  j       ra
    nop

    .end    delayasmshort

/***************************************************
    tune the ddr ram
***************************************************/
    .globl  ipmux_ddr_tune
    .ent    ipmux_ddr_tune
ipmux_ddr_tune:

#ifndef CONFIG_IPMUX_ONU
    /*----- start io tune for up stream ddr----------*/
    la  t8, DDR_CONFIG_START_UP
    la  t0, 1
    sw  t0, 0(t8)
    la  t0, 2
    sw  t0, 0(t8)

    /*----- start io tune for down stream ddr----------*/
    la  t8, DDR_CONFIG_START_DN
    la  t0, 1
    sw  t0, 0(t8)
    la  t0, 2
    sw  t0, 0(t8)
#endif

#ifdef CONFIG_IPMUX_ONU_A
    la t0, DDR_IO_REG
    la t1, 0x9e0f
    sw t1, 0(t0)
#endif

    /*----- config to DDR 1G MODE or 512M MODE ----*/
    /*64M = 3; 32M = 1*/
    la t0, DDR_CONFIG_1G_512M_UP

#ifdef DDR2_UP_1G
    la t1, 3
        
    #ifdef DDR2_USED_BY_CPU_64M
        la t1, 3    
    #endif

    #ifdef DDR2_USED_BY_CPU_96M
        la t1, 5    
    #endif
    
#else /*512M*/
    #ifdef CONFIG_IPMUX_ONU
    la t1, 2 /*modified by wgtian*/
    #else
    la t1, 0
    #endif
#endif
    sw t1, 0(t0)

#ifndef CONFIG_IPMUX_ONU
    /* dn */
    la t0, DDR_CONFIG_1G_512M_DN
#ifdef DDR2_DN_1G
    la t1, 1
#else
    la t1, 0
#endif
    sw t1, 0(t0)


    /*----- config DDR to strong driver MODE for up stream ddr----*/
    la t0, 0xbf004910
    la t1, 1
    sw t1, 0(t0)

    /*----- config DDR to strong driver MODE for down stream ddr----*/
    la t0, 0xbf00494c
    la t1, 1
    sw t1, 0(t0)
#endif


/*-------------------- config the control registers for up ddr -----------------------*/
la t0, REG_DDR_UP_CTLX( 0)
la t1, DENALI_CTL_00_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX( 1)
la t1, DENALI_CTL_01_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX( 2)
la t1, DENALI_CTL_02_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX( 3)
la t1, DENALI_CTL_03_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX( 4)
la t1, DENALI_CTL_04_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX( 5)
la t1, DENALI_CTL_05_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX( 6)
la t1, DENALI_CTL_06_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX( 7)
la t1, DENALI_CTL_07_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX( 8)
la t1, DENALI_CTL_08_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX( 9)
la t1, DENALI_CTL_09_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(10)
la t1, DENALI_CTL_10_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(11)
la t1, DENALI_CTL_11_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(12)
la t1, DENALI_CTL_12_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(13)
la t1, DENALI_CTL_13_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(15)
la t1, DENALI_CTL_15_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(16)
la t1, DENALI_CTL_16_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(17)
la t1, DENALI_CTL_17_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(18)
la t1, DENALI_CTL_18_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(19)
la t1, DENALI_CTL_19_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(20)
la t1, DENALI_CTL_20_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(21)
la t1, DENALI_CTL_21_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(22)
la t1, DENALI_CTL_22_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(23)
la t1, DENALI_CTL_23_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(24)
la t1, DENALI_CTL_24_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

#ifdef CONFIG_IPMUX_ONU_A

la t0, REG_DDR_UP_CTLX(25)
la t1, DENALI_CTL_25_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_UP_CTLX(26)
la t1, DENALI_CTL_26_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

#endif
/* config the delay value */
la t0, REG_DDR_UP_CTLX(14)
la t1, DENALI_CTL_14_DATA_UP
sw t1, 0(t0)
DELAYASMDDR

/* start */
la t0, REG_DDR_UP_CTLX(4)
la t1, DENALI_START_DATA_UP
sw t1, 0(t0)
DELAYASMDDR


/* ONU don't tune the value */
#ifndef CONFIG_IPMUX_ONU
/**************************************************************************
    tune the delay value of Up stream
    t0 : delay control register address.
    t1 : the current tuned delay value.( from 0 to IPMUX_DDR_MAX_DE     LAY_VAL)
    t3,t4 : the calulated tuned delay value

    a0 : rw test address
    a1 : rw test value
    a2 : rw test readback value
***************************************************************************/

/********************************************************************
        t3 : the min value
********************************************************************/
    la t1, 0
    la t2, IPMUX_DDR_MAX_DELAY_VAL

1:  la t0, REG_DDR_UP_CTLX(14)
    /* calculate the delay value*/

    li t3, 0x0c00

    sll t4,t1,IPMUX_DDR_DELAY0_SHIFT
    or t3,t4

    sll t4,t1,IPMUX_DDR_DELAY1_SHIFT
    or t3,t4

    /* write the delay velue to ddr controll                            er */
    sw t3, 0(t0)

    DELAYASM

    DDR_TUNE_MEMTEST

    /* if not found the tune value, just go                             on*/
    beq t1,t2,1f

    /*inc current tune value*/
    add t1,1

    /* bne a1,a2,1b */
    /* if ram test failed, go back */
    beqz s4,1b
    nop

#ifndef TUNE_DDR_SIMPLE
/********************************************************************
        t5 : the max value
********************************************************************/
1:  la t1, IPMUX_DDR_MAX_DELAY_VAL
    la t2, 0

    la t6, 1

1:  la t0, REG_DDR_UP_CTLX(14)
    /* calculate the delay value*/

    li t5, 0x0c00

    sll t4,t1,IPMUX_DDR_DELAY0_SHIFT
    or t5,t4

    sll t4,t1,IPMUX_DDR_DELAY1_SHIFT
    or t5,t4

    /* write the delay velue to ddr controll                            er */
    sw t5, 0(t0)
    DELAYASM

    DDR_TUNE_MEMTEST

    /* if not found the tune value, just go                             on*/
    beq t1,t2,1f

    /*inc current tune value*/
    sub t1,t1,t6
    /*bne a1,a2,1b*/
    /* if ram test failed, go back */
    beqz s4,1b
    nop

/***************************************************
    calulate the average
***************************************************/

1:
#ifdef LOG_DDR_CONFIG_DELAY_VAL
    /* store min delay value for later using */
    la t9, 0xabcd0000
    or t9, t3
    la t8, DDR_CONFIG_DELAY_MIN
    sw t3, 0(t8)

    /* store max delay value for later using */
    la t9, 0xabcd0000
    or t9, t5
    la t8, DDR_CONFIG_DELAY_MAX
    sw t5, 0(t8)
#endif

    srl t5, t5, IPMUX_DDR_DELAY0_SHIFT
    srl t3, t3, IPMUX_DDR_DELAY0_SHIFT

    add t1, t3, t5
    srl t1, t1, 1

    li t5, 0x0c00

    sll t4, t1, IPMUX_DDR_DELAY0_SHIFT
    or  t5, t4

    sll t4, t1, IPMUX_DDR_DELAY1_SHIFT
    or  t5, t4
#endif    /*end ifndef TUNE_DDR_SIMPLE*/

#ifdef TUNE_DDR_SIMPLE
    /*t3 is the min value*/
    la t5, 0
    or t5, t3
#endif

#ifdef LOG_DDR_CONFIG_DELAY_VAL
    /* store average delay value for later using */
    la t8, 0
    or t8, t5
    srl t8, t8, IPMUX_DDR_DELAY0_SHIFT
    la t9, 0xabcd0000
    or t9, t8

    la t8, DDR_CONFIG_DELAY_AVERAGE
    sw t5, 0(t8)
#endif

    /* write the delay velue to ddr controller */
    la t0, REG_DDR_UP_CTLX(14)
    sw t5, 0(t0)
#endif /* CONFIG_IPMUX_ONU */

/* ONU don't have down stream ddr */
#ifndef CONFIG_IPMUX_ONU
/****************************************************
           ddr dn : don't change t5
 ***************************************************/
la t0, REG_DDR_DN_CTLX( 0)
la t1, DENALI_CTL_00_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX( 1)
la t1, DENALI_CTL_01_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX( 2)
la t1, DENALI_CTL_02_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX( 3)
la t1, DENALI_CTL_03_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX( 4)
la t1, DENALI_CTL_04_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX( 5)
la t1, DENALI_CTL_05_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX( 6)
la t1, DENALI_CTL_06_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX( 7)
la t1, DENALI_CTL_07_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX( 8)
la t1, DENALI_CTL_08_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX( 9)
la t1, DENALI_CTL_09_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(10)
la t1, DENALI_CTL_10_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(11)
la t1, DENALI_CTL_11_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(12)
la t1, DENALI_CTL_12_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(13)
la t1, DENALI_CTL_13_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(15)
la t1, DENALI_CTL_15_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(16)
la t1, DENALI_CTL_16_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(17)
la t1, DENALI_CTL_17_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(18)
la t1, DENALI_CTL_18_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(19)
la t1, DENALI_CTL_19_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(20)
la t1, DENALI_CTL_20_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(21)
la t1, DENALI_CTL_21_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(22)
la t1, DENALI_CTL_22_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(23)
la t1, DENALI_CTL_23_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

la t0, REG_DDR_DN_CTLX(24)
la t1, DENALI_CTL_24_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

/* config the delay value */
la t0, REG_DDR_DN_CTLX(14)
la t1, DENALI_CTL_14_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

/* start */
la t0, REG_DDR_DN_CTLX(4)
la t1, DENALI_START_DATA_DN
sw t1, 0(t0)
DELAYASMDDR

#ifndef DDR_TUNE_DOWNSTREAM
    /* write the delay velue to ddr controller : just write the UP delay values to down*/
    la t0, REG_DDR_DN_CTLX(14)
    sw t5, 0(t0)
    DELAYASM
#else

/**************************************************************************
    tune the delay value of DownStream
    t0 : delay control register address.
    t1 : the current tuned delay value.( from 0 to IPMUX_DDR_MAX_DE     LAY_VAL)
    t3,t4 : the calulated tuned delay value

    a0 : rw test address
    a1 : rw test value
    a2 : rw test readback value
***************************************************************************/

/********************************************************************
        t3 : the min value
********************************************************************/
    la t1, 0
    la t2, IPMUX_DDR_MAX_DELAY_VAL

1:  la t0, REG_DDR_DN_CTLX(14)
    /* calculate the delay value*/

    li t3, 0x0c00

    sll t4,t1,IPMUX_DDR_DELAY0_SHIFT
    or t3,t4

    sll t4,t1,IPMUX_DDR_DELAY1_SHIFT
    or t3,t4

    /* write the delay velue to ddr controll                            er */
    sw t3, 0(t0)

    DELAYASM

    DDR_TUNE_MEMTEST

    /* if not found the tune value, just go                             on*/
    beq t1,t2,1f

    /*inc current tune value*/
    add t1,1

    /* bne a1,a2,1b */
    /* if ram test failed, go back */
    beqz s4,1b
    nop

#ifndef TUNE_DDR_SIMPLE
/********************************************************************
        t5 : the max value
********************************************************************/
1:  la t1, IPMUX_DDR_MAX_DELAY_VAL
    la t2, 0

    la t6, 1

1:  la t0, REG_DDR_DN_CTLX(14)
    /* calculate the delay value*/

    li t5, 0x0c00

    sll t4,t1,IPMUX_DDR_DELAY0_SHIFT
    or t5,t4

    sll t4,t1,IPMUX_DDR_DELAY1_SHIFT
    or t5,t4

    /* write the delay velue to ddr controll                            er */
    sw t5, 0(t0)
    DELAYASM

    DDR_TUNE_MEMTEST

    /* if not found the tune value, just go                             on*/
    beq t1,t2,1f

    /*inc current tune value*/
    sub t1,t1,t6
    /*bne a1,a2,1b*/
    /* if ram test failed, go back */
    beqz s4,1b
    nop

/***************************************************
    calulate the average
***************************************************/

1:
#ifdef LOG_DDR_CONFIG_DELAY_VAL
    /* store min delay value for later using */
    la t9, 0xabcd0000
    or t9, t3
    la t8, DDR_CONFIG_DELAY_MIN
    sw t3, 0(t8)

    /* store max delay value for later using */
    la t9, 0xabcd0000
    or t9, t5
    la t8, DDR_CONFIG_DELAY_MAX
    sw t5, 0(t8)
#endif

    srl t5, t5, IPMUX_DDR_DELAY0_SHIFT
    srl t3, t3, IPMUX_DDR_DELAY0_SHIFT

    add t1, t3, t5
    srl t1, t1, 1

    li t5, 0x0c00

    sll t4, t1, IPMUX_DDR_DELAY0_SHIFT
    or  t5, t4

    sll t4, t1, IPMUX_DDR_DELAY1_SHIFT
    or  t5, t4
#endif    /*end ifndef TUNE_DDR_SIMPLE*/

#ifdef TUNE_DDR_SIMPLE
    /*t3 is the min value*/
    la t5, 0
    or t5, t3
#endif

#ifdef LOG_DDR_CONFIG_DELAY_VAL
    /* store average delay value for later using */
    la t8, 0
    or t8, t5
    srl t8, t8, IPMUX_DDR_DELAY0_SHIFT
    la t9, 0xabcd0000
    or t9, t8

    la t8, DDR_CONFIG_DELAY_AVERAGE
    sw t5, 0(t8)
#endif

    /* write the delay velue to ddr controller */
    la t0, REG_DDR_DN_CTLX(14)
    sw t5, 0(t0)
#endif  /* end of tunning Down stream */

#endif /*endif CONFIG_IPMUX_ONU */
    /*jump to caller*/
1:  j       ra
    nop

    .end    ipmux_ddr_tune


/* begin added by jiangmingli */
/***************************************************
    tune the ddr ram
	a0 : store first available value
	a1 : store available value count
	a2 : store temp first available value
	a3 : store temp available value count
	
	t0 : delay or shift control register address
	t1 : delay or shift value for test
	t2 : max test value
	t3 : delay or shift register value
	t4 : temp delay or shift register value
	t5 : ddr test address
	t6 : max ddr test address

	s0 : test address
	s1 : test value
	s2 : read value
	s3 : test data pattern0
	s4 : test data pattern1
	s5 : test result	1  : pass	0 : fail

***************************************************/
    .globl  onu_ddr_tune
    .ent    onu_ddr_tune
onu_ddr_tune:	
/* set WR_DQS_SHIFT, DDR_CTRL 25 */
	li t0, 0xbf014064
	li t3, 0x38385858
	sw t3, 0(t0)
    DELAYASM
    
	la t1, 0
	la t2, 0x7f
	li s3, 0x10101010
	li s4, 0xEFEFEFEF
	la a0, 0
	la a1, 0
	la a2, 0
	la a3, 0
	
/* tune DLL_DQS_DELAY */
1:	li t0, 0xbf014038

	li t3, 0x0c00
	sll t4,t1,16
    or t3,t4
    sll t4,t1,24
    or t3,t4

    sw t3, 0(t0)
    DELAYASM

    /* test delay value */
	li t5, 0xa0000000
	li t6, 0xa1ffffff

    /* test 1 */
5:	add s0, t5, 0x0
    add s1, s3, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 2 */
    add s0, t5, 0x4
    add s1, s4, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 3 */
    add s0, t5, 0x8
    add s1, s3, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 4 */
    add s0, t5, 0xc
    add s1, s4, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 5 */
	add s0, t5, 0x10
    add s1, s3, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 6 */
    add s0, t5, 0x14
    add s1, s4, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 7 */
    add s0, t5, 0x18
    add s1, s3, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 8 */
    add s0, t5, 0x1c
    add s1, s4, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    add t5, 0x100000
    ble t5, t6, 5b
    nop

   	/* test pass */
   	bnez a3, 2f
   	nop
   	add a2, t1, 0
2:	add a3, 1
	ble a3, a1, 4f
	nop
	add a0, a2, 0
	add a1, a3, 0
	bnez a3, 4f
	nop
	/* clear temp count */
3:	la a3, 0
	/* try the next value */
4:	add t1, 4
	add t7, s3, 0
	add s3, s4, 0
	add s4, t7, 0
	ble t1, t2, 1b
	nop


	/* set DLL_DQS_DELAY */
	srl t4, a1, 1
	sll a1, t4, 2
	addu t1, a0, a1

	li t0, 0xbf014038

	li t3, 0x0c00
	sll t4,t1,16
    or t3,t4
    sll t4,t1,24
    or t3,t4

    sw t3, 0(t0)
    DELAYASM

/* tune WR_DQS_SHIFT */
	la t1, 0
	la t2, 0x7f
	la s3, 0x10101010
	la s4, 0xEFEFEFEF
	la a0, 0
	la a1, 0
	la a2, 0
	la a3, 0
	
1:	li t0, 0xbf014064

	li t3, 0x5858
	sll t4,t1,16
    or t3,t4
    sll t4,t1,24
    or t3,t4

    sw t3, 0(t0)
    DELAYASM

    /* test shift value */
	li t5, 0xa0000000
	li t6, 0xa1ffffff

    /* test 1 */
5:	add s0, t5, 0x0
    add s1, s3, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 2 */
    add s0, t5, 0x4
    add s1, s4, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 3 */
    add s0, t5, 0x8
    add s1, s3, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 4 */
    add s0, t5, 0xc
    add s1, s4, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 5 */
	add s0, t5, 0x10
    add s1, s3, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 6 */
    add s0, t5, 0x14
    add s1, s4, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 7 */
    add s0, t5, 0x18
    add s1, s3, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    /* test 8 */
    add s0, t5, 0x1c
    add s1, s4, 0
    sw s1, 0(s0)
    DELAYASMDDR

    lw s2, 0(s0)
    bne s2, s1, 3f
    nop

    add t5, 0x100000
    ble t5, t6, 5b
    nop

   	/* test pass */
   	bnez a3, 2f
   	nop
   	add a2, t1, 0
2:	add a3, 1
	ble a3, a1, 4f
	nop
	add a0, a2, 0
	add a1, a3, 0
	bnez a3, 4f
	nop
	/* clear temp count */
3:	la a3, 0
	/* try the next value */
4:	add t1, 4
	add t7, s3, 0
	add s3, s4, 0
	add s4, t7, 0	
	ble t1, t2, 1b
	nop

/* set WR_DQS_SHIFT */
	srl t4, a1, 1
	sll a1, t4, 2
	addu t1, a0, a1

	la t0, 0xbf014064

	li t3, 0x5858
	sll t4,t1,16
    or t3,t4
    sll t4,t1,24
    or t3,t4

    sw t3, 0(t0)
    DELAYASM

#if 0    
/* temp for test */
    /* set DLL_DQS_DELAY */
    la t1, 0x16
	li t0, 0xbf014038

	li t3, 0x0c00
	sll t4,t1,16
    or t3,t4
    sll t4,t1,24
    or t3,t4

    sw t3, 0(t0)
    DELAYASM
    /* set DLL_DQS_SHIFT */
	la t1, 0x78
	la t0, 0xbf014040

	li t3, 0x6A0C0000
	sll t4,t1,8
    or t3,t4

    sw t3, 0(t0)
    DELAYASM
/* end */
#endif

    j       ra
    nop
	.end    onu_ddr_tune
/* end added by jiangmingli */

/***************************************************
            tune the ddr2 ram on 10g_c2 platform
    
***************************************************/              


/***************************************************
            tune the ddr3 ram on 10g_c3 platform
    
***************************************************/              
    .globl  optrann_ddr3_tune16bit
    .ent    optrann_ddr3_tune16bit
    
optrann_ddr3_tune16bit:

//step 1:   
    MAB_TBL_DATA_SET(266)
	MAB_TBL_WRITE(0,0xc0)
	WAIT_TBL_ACC_DONE
	
    MAB_TBL_DATA_SET(200)
	MAB_TBL_WRITE(0,0xc4)
	WAIT_TBL_ACC_DONE
	
    MAB_TBL_DATA_SET(0x1a)
	MAB_TBL_WRITE(0,0xcc)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(500)
	MAB_TBL_WRITE(0,0xc8)
	WAIT_TBL_ACC_DONE

//step 2:
#if 0
   MAB_TBL_DATA_SET(0x3ff00000)
	MAB_TBL_WRITE(0,0x484)
	WAIT_TBL_ACC_DONE	
    MAB_TBL_DATA_SET(0x10000010)
	MAB_TBL_WRITE(0,0x41c)
	WAIT_TBL_ACC_DONE
	
    MAB_TBL_DATA_SET(0x00800080)
	MAB_TBL_WRITE(0,0x420)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x0010f111)
	MAB_TBL_WRITE(0,0x424)
	WAIT_TBL_ACC_DONE
#endif	
    //step 3:
	// poll general status register until pll initialization flag is set bit[1]
	mab_tbl_data_bit_poll(0x2, 0x410)
	mab_tbl_data_bit_poll(0x4, 0x410)
	mab_tbl_data_bit_poll(0x1, 0x410)	

//step 4:
    //Check that Master SM has moved on from ST0
    mab_tbl_data_bit_poll(0x10000, 0x484)

//step 5:
    // Write '1' into the POWCTL register
    MAB_TBL_DATA_SET(0x1)
	MAB_TBL_WRITE(0,0x44)
	WAIT_TBL_ACC_DONE

	// Poll the POWSTAT register until the power-up stage is completed
	mab_tbl_data_bit_poll(0x1, 0x48)

//step 6:
    MAB_TBL_DATA_SET(0x6e)
	MAB_TBL_WRITE(0,0x5e8)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x6e)
	MAB_TBL_WRITE(0,0x628)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x6e)
	MAB_TBL_WRITE(0,0x668)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x6e)
	MAB_TBL_WRITE(0,0x6a8)
	WAIT_TBL_ACC_DONE
	
#if 0
    MAB_TBL_DATA_SET(0x1)
	MAB_TBL_WRITE(0,0x44) 
	WAIT_TBL_ACC_DONE
#endif


    MAB_TBL_DATA_SET(0x40021)
	MAB_TBL_WRITE(0,0x80)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x4e)
	MAB_TBL_WRITE(0,0xd0)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x4)
	MAB_TBL_WRITE(0,0xd4)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x56)
	MAB_TBL_WRITE(0,0xd8)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x8)
	MAB_TBL_WRITE(0,0xdc)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x7)
	MAB_TBL_WRITE(0,0xe4)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x8)
	MAB_TBL_WRITE(0,0xe8)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x6)
	MAB_TBL_WRITE(0,0xec)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x14)
	MAB_TBL_WRITE(0,0xf0)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x1C) 
	MAB_TBL_WRITE(0,0xf4)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x8)
	MAB_TBL_WRITE(0,0xf8)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x6)
	MAB_TBL_WRITE(0,0xfc)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x4)
	MAB_TBL_WRITE(0,0x100)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x8)
	MAB_TBL_WRITE(0,0x104)
	WAIT_TBL_ACC_DONE

    //MAB_TBL_DATA_SET(0x4)
        MAB_TBL_DATA_SET(0x6)
	MAB_TBL_WRITE(0,0x108)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x200)
	MAB_TBL_WRITE(0,0x10c)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x4)
	MAB_TBL_WRITE(0,0x110)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x1)
	MAB_TBL_WRITE(0,0x120)
	WAIT_TBL_ACC_DONE

    //MAB_TBL_DATA_SET(0x2)
    MAB_TBL_DATA_SET(0x7)
	MAB_TBL_WRITE(0,0xe0)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x6)
	MAB_TBL_WRITE(0,0x124)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x6)
	MAB_TBL_WRITE(0,0x128)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0xc)
	MAB_TBL_WRITE(0,0x130)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x3)
	MAB_TBL_WRITE(0,0x12c)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x36)
	MAB_TBL_WRITE(0,0x134)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x40)
	MAB_TBL_WRITE(0,0x118)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x200)
	MAB_TBL_WRITE(0,0x138)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0xd)
	MAB_TBL_WRITE(0,0x114)
	WAIT_TBL_ACC_DONE

    MAB_TBL_DATA_SET(0x5)
	MAB_TBL_WRITE(0,0x11c)
	WAIT_TBL_ACC_DONE

//setp 7:
    MAB_TBL_DATA_SET(0x83f00000)
	MAB_TBL_WRITE(0,0x40)
	WAIT_TBL_ACC_DONE
	mab_tbl_data_bit_check(0x80000000, 0x40)
	
    //  **********************************
	MAB_TBL_DATA_SET(0x80f42083)
	MAB_TBL_WRITE(0,0x40)
	WAIT_TBL_ACC_DONE
	mab_tbl_data_bit_check(0x80000000, 0x40)

	//	**********************************
	MAB_TBL_DATA_SET(0x80f60003)
	MAB_TBL_WRITE(0,0x40)
	WAIT_TBL_ACC_DONE
	mab_tbl_data_bit_check(0x80000000, 0x40)


	//	**********************************
	MAB_TBL_DATA_SET(0x80f200e3)
	MAB_TBL_WRITE(0,0x40)
	WAIT_TBL_ACC_DONE
	mab_tbl_data_bit_check(0x80000000, 0x40)

	//	**********************************
	MAB_TBL_DATA_SET(0xA)
	MAB_TBL_WRITE(0,0x48C)
	WAIT_TBL_ACC_DONE
	mab_tbl_data_bit_check(0x80000000, 0x40) /*tbd:???*/
	

	//	**********************************
	MAB_TBL_DATA_SET(0x80f09403)
	MAB_TBL_WRITE(0,0x40)
	WAIT_TBL_ACC_DONE
	mab_tbl_data_bit_check(0x80000000, 0x40)
		

	//	**********************************
	MAB_TBL_DATA_SET(0x80f00005)
	MAB_TBL_WRITE(0,0x40)
	WAIT_TBL_ACC_DONE
	mab_tbl_data_bit_check(0x80000000, 0x40)

//step 8:
	MAB_TBL_DATA_SET(0x1)
	MAB_TBL_WRITE(0,0x4)
	WAIT_TBL_ACC_DONE

	mab_tbl_data_bit_poll(0x1, 0x8)	
	mab_tbl_data_bit_check(0x6, 0x8)

//step 9:
	MAB_TBL_DATA_SET(0x111100)
	MAB_TBL_WRITE(0,0x4ac)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x111111)
	MAB_TBL_WRITE(0,0x90)
	WAIT_TBL_ACC_DONE	

//step 10:
	MAB_TBL_DATA_SET(0x20)
	MAB_TBL_WRITE(0,0x0)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x40061)
	MAB_TBL_WRITE(0,0x80)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x1fc)
	MAB_TBL_WRITE(0,0x84)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x1fc)
	MAB_TBL_WRITE(0,0x4a8)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x00)
	MAB_TBL_WRITE(0,0x180)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0xc4c4)
	MAB_TBL_WRITE(0,0x300)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x3)
	MAB_TBL_WRITE(0,0x320)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x3)
	MAB_TBL_WRITE(0,0x324)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x80f08403)
	MAB_TBL_WRITE(0,0x40)
	WAIT_TBL_ACC_DONE
	mab_tbl_data_bit_check(0x80000000, 0x40)
		

	MAB_TBL_DATA_SET(0x80f200e3)
	MAB_TBL_WRITE(0,0x40)
	WAIT_TBL_ACC_DONE
	mab_tbl_data_bit_check(0x80000000, 0x40)
		

	MAB_TBL_DATA_SET(0xA)
	MAB_TBL_WRITE(0,0x48c)
	WAIT_TBL_ACC_DONE

	
//step 11:  for training
	MAB_TBL_DATA_SET(0x07b6b529)
	MAB_TBL_WRITE(0,0x5a0)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x1100c461)
	MAB_TBL_WRITE(0,0x404)
	WAIT_TBL_ACC_DONE

        mab_tbl_data_bit_check(0x1, 0x404)

	mab_tbl_data_bit_poll(0x2, 0x410)
	mab_tbl_data_bit_poll(0x4, 0x410)
	mab_tbl_data_bit_poll(0x1, 0x410)
	
	MAB_TBL_DATA_SET(0x4201)
	MAB_TBL_WRITE(0,0x31c)
	WAIT_TBL_ACC_DONE	
	        
	MAB_TBL_READ(0,0x484)
	WAIT_TBL_ACC_DONE
	MAB_TBL_DATA_GET
	li t3, 0xffff 
	and TBL_GOT_DATA, TBL_GOT_DATA, t3
	li t3, 0xa4530000
	or TBL_GOT_DATA, TBL_GOT_DATA, t3
	la t0, MAB_TBL_ACC_DATA;
	sw TBL_GOT_DATA, 0(t0);
	MAB_TBL_WRITE(0,0x484)
        WAIT_TBL_ACC_DONE
    
	MAB_TBL_DATA_SET(0x1)
	MAB_TBL_WRITE(0,0x4c)
	WAIT_TBL_ACC_DONE	

//step 12:  for training
211:
	MAB_TBL_READ(0,0x484)
	WAIT_TBL_ACC_DONE
	MAB_TBL_DATA_GET
	li t3,0xffff0000	 
	and t4,TBL_GOT_DATA,t3
	bne t4,t3,211b		   
	nop

    //  **********************************
	MAB_TBL_DATA_SET(0x8)
	MAB_TBL_WRITE(0,0x8c)
	WAIT_TBL_ACC_DONE

	MAB_TBL_DATA_SET(0x4204)
	MAB_TBL_WRITE(0,0x31c)
	WAIT_TBL_ACC_DONE	

//step 13:
	MAB_TBL_DATA_SET(0x0100c640)
	MAB_TBL_WRITE(0,0x404)
	WAIT_TBL_ACC_DONE
	MAB_TBL_DATA_SET(0x2)
	MAB_TBL_WRITE(0,0x4)
	WAIT_TBL_ACC_DONE
22:
	MAB_TBL_READ(0,0x8)
	WAIT_TBL_ACC_DONE
	MAB_TBL_DATA_GET
	li t3,0x3	 
	and t4,TBL_GOT_DATA,t3
	bne t4,t3,22b		   /*check bit[0:1] ?= 0x11*/
	li t3,BIT2	 
	and t4,TBL_GOT_DATA,t3
	bne t4,zero,22b 	   /*check bit[2] ?= 0x0*/
	nop
	
	la t0, 0xbf0000c0
        la t1, 0x80021002
        sw t1, 0(t0)
// finish ddr2 init,return caller
    j ra      
    nop
	.end    optrann_ddr3_tune16bit




