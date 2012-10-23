#ifndef _INCLUDE_EXTTA_
#define _INCLUDE_EXTTA_

#ifndef _OPIPMUX_H_
#include <config.h>
#endif

/*
#define DEBUG_IN_WIN32

#ifndef ipMuxRegRead
#define ipMuxRegRead(regId, pval)   *pval=0x1000; //printf("-------------+++++++++++++++") //printf("       r a=%x\n",IPMUX_REG_ID2ADDR(regId));
#endif

#ifndef ipMuxRegWrite
#define ipMuxRegWrite(regId, val)   //printf("      w a=%x v=0x%x\n",IPMUX_REG_ID2ADDR(regId),val);   *((volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)) = (UINT32)(val)
#endif
*/

/*------------------- start of extra data  -------------------*/
/* ------ cyang ------  */
#define BRIDGE_VLAN_INDEX_LENGTH      3
#define BRIDGE_VLAN_ADDRESS_LENGTH    0x1FF
#define BRIDGE_PORTNUM_PERENTRY       32
#define BRIDGE_VLAN_MEM_ENTRY_LEN     12      /*12*32Bits.*/

/*List operation*/
#define LIST_NULL                               0x80001000
#define LIST_POINTER_NULL                       0x80001001
#define PARAMETER_INPUT_POINTER_NULL            0x80001002
#define TABLE_INIT_MEMORY_NULL                  0x80001003

#define OPL_BOOL                   int
#define OPL_TRUE            1
#define OPL_FALSE           0

#ifndef NULL
#define NULL 0
#endif
/* ---------------- start of extra 1-------------------*/
/*
OPL_STATUS ipmuxTableRead (IPMUX_TAB_e region, UINT16 offset, int nNum, UINT32* pVal)
{
    *pVal=region+0x10000*offset;
    return OPL_OK;
}
*/
//#define ipmuxTableRead(region,offset,nNum,pVal)   (offset==1?(*pVal=region+0x1*offset):(*pVal=region+0x1*offset))

#ifdef DEBUG_IN_WIN32
//#define ipMuxRegRead(regId, pval) *(UINT32*)(pval) = *(volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)
//#define ipMuxRegWrite(regId, val) *((volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)) = (UINT32)(val)

#define ipmuxTableRead(region,offset,nNum,pVal) (*pVal=offset+1)
#define ipmuxTableWrite(region,offset,nNum,pVal) (*pVal=region+0x10000*offset)
#endif

#ifndef u32
#define u32 unsigned int
#endif

#ifndef UINT32
#define UINT32 unsigned int
#endif

#ifndef UINT16
#define UINT16 unsigned short
#endif

#ifndef UINT8
#define UINT8 unsigned char
#endif

typedef signed char     INT8;
typedef short           INT16;
typedef int             INT32;


typedef struct {UINT32 low; UINT32 high;} UINT64;

typedef enum IPMUX_TAB_s
{
    IPMUX_TAB_PORT,         /* 0,  Bridge Port table (PTable) */
        IPMUX_TAB_ARL,          /* 1,  Bridge Address Resolution logic table  */
        IPMUX_TAB_VLAN_PTR    ,     /* 2,  Bridge Vlan Pointer Table */
        IPMUX_TAB_VLAN_HASH_B   ,   /* 3,  Bridge Vlan Hash Table B */
        IPMUX_TAB_VLAN_HASH_A   ,   /* 4,  Bridge Vlan Hash Table A */
        IPMUX_TAB_VLAN,         /* 5,  Bridge VLAN table (VTable) */
        IPMUX_TAB_VLAN_REP,     /* 6,  Bridge CTAG replacement table */
        IPMUX_TAB_VLAN_MEM,     /* 7,  Bridge VLAN Cross Connection Enable table */
        IPMUX_TAB_SACT  ,           /* 8,  Bridge MAC Filter Table, also source address control talbe, SACT */
        IPMUX_TAB_REMARK,           /* 9,  Bridge remark table */
        IPMUX_TAB_MC_PTR,           /* 10, Bridge MC pointer table, Multicast Group table */
        IPMUX_TAB_MC,               /* 11, Bridge MC Table, Multicast Group member table */
        IPMUX_TAB_EGRESS_PORT  ,    /* 12, Bridge Port Egress Contrl table (ECT) */
        IPMUX_TAB_LIMIT,            /* 13, Bridge MAC limitation table */
        IPMUX_TAB_LIMIT_CNT,        /* 14, Bridge MAC limitation counter table */
        IPMUX_TAB_PRIORITY_TAG, /* 15, Bridge VLAN priority tag table */

        IPMUX_TAB_RULE_MASK=16, /* 16, Classify Rule Mask table */
        IPMUX_TAB_RULE_HASH,        /* 17, Classify rule hash address table */
        IPMUX_TAB_RULE,         /* 18, Classify rule table */
        IPMUX_TAB_FLOW_MASK=20, /* 20, Classify Flow ID mask table */
        IPMUX_TAB_FLOW_HASH,        /* 21, Classify flow address pointer (hash) table */
        IPMUX_TAB_FLOW,         /* 22, Classify Flow table */
        IPMUX_TAB_FLOW_RMK,     /* 23, Classify Flow Remark rule table */
        IPMUX_TAB_CLS_CNT_HIT,  /* 24, Classify Hit Counter table */

        IPMUX_TAB_US_FSHP=32,       /* 32, Upstream Flow Shaper Table */
        IPMUX_TAB_US_FWRED=34,  /* 34, Upstream Flow WRED Table */
        IPMUX_TAB_US_FLOW_R=36, /* 36, Upstream Flow Table (Read Only) */
        IPMUX_TAB_US_FLOW_W=38, /* 38, Upstream Flow Table (Write Only) */
        IPMUX_TAB_US_FCNT=40,       /* 40, Upstream Flow Queue Counter table */
        IPMUX_TAB_US_FSTATUS=42,    /* 42, Upstream Flow Queue Status Table */
        IPMUX_TAB_US_FCNT_DROP=44,  /* 44, Upstream Flow Rx Packet Drop Counter Table */

        IPMUX_TAB_DS_FSHP=33/* 49*/,        /* 49, Downstream Flow Shaper Table */
        IPMUX_TAB_DS_FWRED=35 /*51*/,   /* 51, Downstream Flow WRED Table */
        IPMUX_TAB_DS_FPSEQ=37 /*53*/,   /* 53, Downstream Flow Port Sequence Table (Read Only) */
        IPMUX_TAB_DS_FPWPRI=39 /*55*/,  /* 55, Downstream Port Weight and Priority Table (Read Only) */
        IPMUX_TAB_DS_FPWSUM=41 /*57*/,  /* 57, Downstream Port Weight Sum Table */
        IPMUX_TAB_DS_FLOW_W=43 /*59*/,  /* 59, Downstream Flow Table (Write Only) */
        IPMUX_TAB_DS_PSEQ_R=45 /*45*/,  /* 59, Downstream Port Sequence Table (Read Only) */
        IPMUX_TAB_DS_FCNT=47,       /* 47, Downstream Flow Queue Counter table */
        IPMUX_TAB_DS_FSTATUS=49,    /* 49, Downstream Flow Queue Status Table */
        IPMUX_TAB_DS_FCNT_DROP=51,  /* 51, Downstream Flow Rx Packet Drop Counter Table */

        IPMUX_TAB_UPPT=64,      /* 64, SAR UPPT */
        IPMUX_TAB_EPPT,         /* 65, SAR EPPT */
        IPMUX_TAB_SAR_QS=67,            /* 67, SAR Queue Status Table */
        IPMUX_TAB_SAR_CNT_APCTRL,   /* 68, SAR Access Port Tx/Rx Counter Control Table */
        IPMUX_TAB_SAR_CNT_APGRPRX,  /* 69, SAR Access Port Rx Counter Group Table */
        IPMUX_TAB_SAR_CNT_APGRPTX,  /* 70, SAR Access Port Tx Counter Group Table */
        IPMUX_TAB_SAR_CNT_PVPTR,        /* 71, SAR Port VLAN Counter Pointer Table */
        IPMUX_TAB_SAR_CNT_PVHASH,   /* 72, SAR Port VLAN Counter Hash Table */
        IPMUX_TAB_SAR_CNT_PVIN,     /* 73, SAR Port VLAN In Frame Statistics Group Table */
        IPMUX_TAB_SAR_CNT_PVOUT,        /* 74, SAR Port VLAN Out Frame Statistics Group Table */
        IPMUX_TAB_SAR_CNT_VLANPTR,  /* 75, SAR GEW VLAN Counter Pointer Table */
        IPMUX_TAB_SAR_CNT_VLANHASH, /* 76, SAR GEW VLAN Counter Hash Table */
        IPMUX_TAB_SAR_CNT_VLANIN,   /* 77, SAR GEW VLAN In Frame Statistics Group Table */
        IPMUX_TAB_SAR_CNT_VLANOUT,  /* 78, SAR GEW VLAN Out Frame Statistics Group Table */

        IPMUX_TAB_HOST_RX=80,       /* 80, HOST Rx BD table, DMA0 BD table */
        IPMUX_TAB_HOST_TX,      /* 81, HOST Tx BD table, DMA0 BD table */
        IPMUX_TAB_FE_RX=96,     /* 96, Fast Ethernet Rx BD table, DMA1 BD table */
        IPMUX_TAB_FE_TX,            /* 97, Fast Ethernet Tx BD table, DMA1 BD table */

        IPMUX_TAB_ATB_CNT_RXCL=112, /* 112, Rx cells per Link,except asm */
        IPMUX_TAB_ATB_CNT_RXCG, /* 113, Rx cells per Group,except asm */
        IPMUX_TAB_ATB_CNT_RXAL, /* 114, Rx asm cells per Link */
        IPMUX_TAB_ATB_CNT_TXCL, /* 115, Tx cells per Link,except asm */
        IPMUX_TAB_ATB_CNT_TXCG, /* 116, Tx cells per Group,except asm */
        IPMUX_TAB_ATB_CNT_TXAL, /* 117, Tx asm cells per Link */

        IPMUX_TAB_QUEUE,            /* Queue propey */
        IPMUX_TAB_DRAM,         /* External SDRAM */

        IPMUX_TAB_ATB_XWANG_PORT_UP, /* add by xwang   */
        IPMUX_TAB_ATB_XWANG_PORT_DOWN, /* add by xwang   */

        IPMUX_TAB_END
}IPMUX_TAB_e;


#ifndef INCLUDE_EXTRA_H
#define INCLUDE_EXTRA_H 1
#define ulong unsigned long
#define  printfreg2 printf
#define  printfreg1 printf
#define  printfreg printf
#define  printfRegTest //printf
#define  printfCounter //printf
#define  opl_log(x) //
#define  opl_logerr(x) printf(" ERR:");printf x
#define  oprintf printf

//typedef unsigned int UINT32

#define DEBUG 0
/* register and bitwise operation */
#define IPMUX_REG_SIZE          32          /* 32 bits register for IPMux */
#define FIELD_BITS(n)               (0xffffffff >> (IPMUX_REG_SIZE - (n)))
/* ---------------- end of extra 1---------------------*/
#define FPGA_MEM_INDIRECT_ACCESS_TIMEOUT        10
#define IPMUX_MEM_ACCESS_POLLING(regid, maskVal)        //

/*
#define IPMUX_MEM_ACCESS_POLLING(regid, maskVal)        {\
    int ii;\
    int jj;\
    UINT32 temp;\
    for (ii = 0; ii < FPGA_MEM_INDIRECT_ACCESS_TIMEOUT; ii++)\
                                                        {\
                                                        for (jj = 0; jj < 10; jj++);    \
                                                        ipMuxRegRead (regid, &temp);\
                                                        if (!(temp & maskVal)) break;\
                                                    }\
                                                    if (ii >= FPGA_MEM_INDIRECT_ACCESS_TIMEOUT)\
                                                        {\
                                                        opl_log(("Access Talbe time out, reg %x = %x#.\n", (regid), temp));\
                                                        return OPL_ERR_NO(OPL_ERR_TIMEOUT);\
                                                    }\
                                                }
                                                */
#define REG_TAB_SHDW_REG_SET0_DATA17    (IPMUX_REG_BASE_HOST + 0x100)

#define IPMUX_REG_BASE          0xBF000000  /* tbd */
#define REG_ADDR_ALLIGNED       4           /* 32bits word */
#define REGID_INIT_VALUE        0xFFFFAAAA
#define OPL_ERR_NO(x) OPL_ERROR
#define OPL_OK              0
#define OPL_ERROR           (-1)
#define OPL_STATUS          int
#define OPL_ZERO            0
#define OPL_NULL            ((void *) 0)    /* a null pointer */
#define IPMUX_REG_ID2ADDR(regId)    (IPMUX_REG_BASE + (regId))
#ifdef CONFIG_IPMUX_ONU

#define IPMUX_REG_BASE_GLOBAL       0x0000 /* wwei mark, All not care *//* Global register               */
#define IPMUX_REG_BASE_SWD          0x2c00 /* wwei mark *//* Watch dog timer               */
#define IPMUX_REG_BASE_GPIO         0x2c80 /* wwei mark *//* General purpose IO block      */
#define IPMUX_REG_BASE_RTC          0x2d80 /* wwei mark *//* Real time clock               */
#define IPMUX_REG_BASE_INTC         0x2e00 /* wwei mark *//* Interrupt Controller          */
#define IPMUX_REG_BASE_DMA0         0x18000/* wwei mark, All not care */ /* DMA0 channel interface, Host bus  interface*/
#define IPMUX_REG_BASE_HOST         IPMUX_REG_BASE_DMA0 
#define IPMUX_REG_BASE_DMA1         0x1c000/* wwei mark, All not care */ /* DMA1 channel interface         */
#define IPMUX_REG_BASE_HDLC         0x2C00 /* HDLC interface                */
#define IPMUX_REG_BASE_UART         0x1800 /* wwei mark *//* Serial UART                   */
#define IPMUX_REG_BASE_I2C          0x2000 /* wwei mark *//* I2C interface                 */
#define IPMUX_REG_BASE_DDR          0x4800 /* wwei mark, ONU not care *//* DDR controller register access*/
#define IPMUX_REG_BASE_RESERVED0    0x24000/* wwei mark, All not care *//*                               */
#else
    /* Register Section Address Map */
#define IPMUX_REG_BASE_GLOBAL       0x0000 /* Global register               */
#define IPMUX_REG_BASE_SWD          0x2800 /* Watch dog timer               */
#define IPMUX_REG_BASE_GPIO         0x2880 /* General purpose IO block      */
#define IPMUX_REG_BASE_RTC          0x2980 /* Real time clock               */
#define IPMUX_REG_BASE_INTC         0x2A00 /* Interrupt Controller          */
#define IPMUX_REG_BASE_DMA0         0x2B40 /* DMA0 channel interface, Host bus  interface*/
#define IPMUX_REG_BASE_HOST         IPMUX_REG_BASE_DMA0
#define IPMUX_REG_BASE_DMA1         0x2BC0 /* DMA1 channel interface         */
#define IPMUX_REG_BASE_HDLC         0x2C00 /* HDLC interface                */
#define IPMUX_REG_BASE_UART         0x4000 /* Serial UART                   */
#define IPMUX_REG_BASE_I2C              0x4400 /* I2C interface                 */
#define IPMUX_REG_BASE_DDR          0x4800 /* DDR controller register access*/
#define IPMUX_REG_BASE_RESERVED0    0x4C00 /*                               */
#endif /*CONFIG_IPMUX_ONU_8BIT*/
#define IPMUX_REG_BASE_GE0              0x8000 /* Gigbits Ethernet 0            */
#define IPMUX_REG_BASE_GE1              0xA000 /* Gigbits Ethernet 1            */
#define IPMUX_REG_BASE_GEL              0xC000 /* Gigbits Ethernet Local        */
#define IPMUX_REG_BASE_UTOPIA           0x10000 /* UTOPIA interface              */
#define IPMUX_REG_BASE_POSPHY           0x10000 /* POS-PHY interface             */
#define IPMUX_REG_BASE_TRAFFIC          0x14000 /* Traffic management block      */
#define IPMUX_REG_BASE_RESERVED1        0x16000 /*                               */
#define IPMUX_REG_BASE_CLASSIFY         0x18000 /* Classfy engine block          */
#define IPMUX_REG_BASE_RESERVED2        0x1A000 /*                               */
#define IPMUX_REG_BASE_REMARK           0x1C000 /* Remark block                  */
#define IPMUX_REG_BASE_RESERVED3        0x1E000 /*                               */
#define IPMUX_REG_BASE_MACBRIDGE        0x20000 /* Mac learning bridge block     */
#define IPMUX_REG_BASE_RESERVED4        0x40000 /*                               */


    /* IPMux Table IDs */

#define IPMUX_TAB_DATA_UNIT_SIZE        IPMUX_REG_SIZE  /* 32 bits data unit for IPMux tables */
    /* table alias */
#define IPMUX_TAB_DS_FLOW_R1    IPMUX_TAB_DS_FPSEQ
#define IPMUX_TAB_DS_FLOW_R2    IPMUX_TAB_DS_FPWPRI
#define IPMUX_TAB_DS_FLOW_R3    IPMUX_TAB_DS_FPWSUM

    /* IPMux Table dimensions: width, in 32-bit word */
#define IPMUX_TAB_PORT_WIDTH            3 /* 0,  Bridge Port table (PTable) */
#define IPMUX_TAB_ARL_WIDTH             3 /* 1,  Bridge Address Resolution logic table  */
#define IPMUX_TAB_VLAN_PTR_WIDTH        1 /* 2,  Bridge Vlan Pointer Table */
#define IPMUX_TAB_VLAN_HASH_B_WIDTH     2 /* 3,  Bridge Vlan Hash Table B */
#define IPMUX_TAB_VLAN_HASH_A_WIDTH     2 /* 4,  Bridge Vlan Hash Table A */
#define IPMUX_TAB_VLAN_WIDTH            12 /* 5,  Bridge VLAN table (VTable) */
#define IPMUX_TAB_VLAN_REP_WIDTH        1 /* 6,  Bridge CTAG replacement table */
#define IPMUX_TAB_VLAN_MEM_WIDTH    4 /* 7,  Bridge VLAN Cross Connection Enable table */
#define IPMUX_TAB_SACT_WIDTH            2 /* 8,  Bridge MAC Filter Table, also source address control talbe, SACT */
#define IPMUX_TAB_REMARK_WIDTH      1 /* 9,  Bridge remark table */
#define IPMUX_TAB_MC_PTR_WIDTH      1   /* 10, Bridge MC pointer table, Multicast Group table */
#define IPMUX_TAB_MC_WIDTH          1   /* 11, Bridge MC Table, Multicast Group member table */
#define IPMUX_TAB_EGRESS_PORT_WIDTH     1 /* 12, Bridge Port Egress Contrl table (ECT) */
#define IPMUX_TAB_LIMIT_WIDTH           1 /* 13, Bridge MAC limitation table */
#define IPMUX_TAB_LIMIT_CNT_WIDTH   1   /* 14, Bridge MAC limitation counter table */
#define IPMUX_TAB_PRIORITY_TAG_WIDTH 1  /* 15, Bridge VLAN priority tag table */

#define IPMUX_TAB_RULE_MASK_WIDTH   18 /* 16, Classify Rule Mask table */
#define IPMUX_TAB_RULE_HASH_WIDTH   2 /* 17, Classify rule hash address table */
#define IPMUX_TAB_RULE_WIDTH            18 /* 18, Classify rule table */
#define IPMUX_TAB_FLOW_MASK_WIDTH   2 /* 20, Classify Flow ID mask table */
#define IPMUX_TAB_FLOW_HASH_WIDTH   1 /* 21, Classify flow address pointer (hash) table */
#define IPMUX_TAB_FLOW_WIDTH            2 /* 22, Classify Flow table */
#define IPMUX_TAB_FLOW_RMK_WIDTH    1 /* 23, Classify Flow Remark rule table */
#define IPMUX_TAB_CLS_CNT_HIT_WIDTH 1 /* 24, Classify Hit Counter table */

#define IPMUX_TAB_US_FSHP_WIDTH         1 /* 32, Upstream Flow Shaper Table */
#define IPMUX_TAB_US_FWRED_WIDTH    1   /* 34, Upstream Flow WRED Table */
#define IPMUX_TAB_US_FLOW_R_WIDTH   1 /* 36, Upstream Flow Table (Read Only) */
#define IPMUX_TAB_US_FLOW_W_WIDTH   1 /* 38, Upstream Flow Table (Write Only) */
#define IPMUX_TAB_US_FCNT_WIDTH     1 /* 40, Upstream Flow Queue Counter table */
#define IPMUX_TAB_US_FSTATUS_WIDTH  1 /* 42, Upstream Flow Queue Status Table */
#define IPMUX_TAB_US_FCNT_DROP_WIDTH 1  /* 44, Upstream Flow Rx Packet Drop Counter Table */

#define IPMUX_TAB_DS_FSHP_WIDTH         1 /* 49, Downstream Flow Shaper Table */
#define IPMUX_TAB_DS_FWRED_WIDTH    1 /* 51, Downstream Flow WRED Table */
#define IPMUX_TAB_DS_FPSEQ_WIDTH        1 /* 53, Downstream Flow Port Sequence Table (Read Only) */
#define IPMUX_TAB_DS_FPWPRI_WIDTH   10 /* 55, Downstream Port Weight and Priority Table (Read Only) */
#define IPMUX_TAB_DS_FPWSUM_WIDTH   4 /* 57, Downstream Port Weight Sum Table */
#define IPMUX_TAB_DS_FLOW_W_WIDTH   1 /* 59, Downstream Flow Table (Write Only) */
#define IPMUX_TAB_DS_PSEQ_R_WIDTH   1 /*45, Downstream Port Sequence Table (Read Only) */
#define IPMUX_TAB_DS_FCNT_WIDTH     1 /* 47, Downstream Flow Queue Counter table */
#define IPMUX_TAB_DS_FSTATUS_WIDTH  1 /* 49, Downstream Flow Queue Status Table */
#define IPMUX_TAB_DS_FCNT_DROP_WIDTH 1  /* 51, Downstream Flow Rx Packet Drop Counter Table */

#define IPMUX_TAB_UPPT_WIDTH            1 /* 64, SAR UPPT */
#define IPMUX_TAB_EPPT_WIDTH            4 /* 65, SAR EPPT */
#define IPMUX_TAB_SAR_QS_WIDTH      1 /* 67, SAR Queue Status Table */
#define IPMUX_TAB_SAR_CNT_APCTRL_WIDTH      1 /* 68, SAR Access Port Tx/Rx Counter Control Table */
#define IPMUX_TAB_SAR_CNT_APGRPRX_WIDTH     8 /* 69, SAR Access Port Rx Counter Group Table */
#define IPMUX_TAB_SAR_CNT_APGRPTX_WIDTH     7 /* 70, SAR Access Port Tx Counter Group Table */
#define IPMUX_TAB_SAR_CNT_PVPTR_WIDTH       1 /* 71, SAR Port VLAN Counter Pointer Table */
#define IPMUX_TAB_SAR_CNT_PVHASH_WIDTH      2 /* 72, SAR Port VLAN Counter Hash Table */
#define IPMUX_TAB_SAR_CNT_PVIN_WIDTH            1 /* 73, SAR Port VLAN In Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_PVOUT_WIDTH       1 /* 74, SAR Port VLAN Out Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_VLANPTR_WIDTH     1 /* 75, SAR GEW VLAN Counter Pointer Table */
#define IPMUX_TAB_SAR_CNT_VLANHASH_WIDTH    2 /* 76, SAR GEW VLAN Counter Hash Table */
#define IPMUX_TAB_SAR_CNT_VLANIN_WIDTH      1 /* 77, SAR GEW VLAN In Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_VLANOUT_WIDTH     1 /* 78, SAR GEW VLAN Out Frame Statistics Group Table */

#define IPMUX_TAB_HOST_RX_WIDTH         1 /* 80, HOST Rx BD table, DMA0 BD table */
#define IPMUX_TAB_HOST_TX_WIDTH         1 /* 81, HOST Tx BD table, DMA0 BD table */
#define IPMUX_TAB_FE_RX_WIDTH       1 /* 96, Fast Ethernet Rx BD table, DMA1 BD table */
#define IPMUX_TAB_FE_TX_WIDTH       1 /* 97, Fast Ethernet Tx BD table, DMA1 BD table */

#define IPMUX_TAB_ATB_CNT_RXCL_WIDTH        1 /* 112, Rx cells per Link,except asm */
#define IPMUX_TAB_ATB_CNT_RXCG_WIDTH    1 /* 113, Rx cells per Group,except asm */
#define IPMUX_TAB_ATB_CNT_RXAL_WIDTH    1 /* 114, Rx asm cells per Link */
#define IPMUX_TAB_ATB_CNT_TXCL_WIDTH        1 /* 115, Tx cells per Link,except asm */
#define IPMUX_TAB_ATB_CNT_TXCG_WIDTH    1 /* 116, Tx cells per Group,except asm */
#define IPMUX_TAB_ATB_CNT_TXAL_WIDTH    1 /* 117, Tx asm cells per Link */

    /* IPMux Table dimensions: length, in records */
#define IPMUX_TAB_PORT_LEN          721/*1024*/ /* 0,  Bridge Port table (PTable) */
#define IPMUX_TAB_ARL_LEN               4096 /* 1,  Bridge Address Resolution logic table  */
#define IPMUX_TAB_VLAN_PTR_LEN      512 /* 2,  Bridge Vlan Pointer Table */
#define IPMUX_TAB_VLAN_HASH_B_LEN   512 /* 3,  Bridge Vlan Hash Table B */
#define IPMUX_TAB_VLAN_HASH_A_LEN   512 /* 4,  Bridge Vlan Hash Table A */
#define IPMUX_TAB_VLAN_LEN          1024 /* 5,  Bridge VLAN table (VTable) */
#define IPMUX_TAB_VLAN_REP_LEN      4608 /* 6,  Bridge CTAG replacement table */
#define IPMUX_TAB_VLAN_MEM_LEN      64 /* 7,  Bridge VLAN Cross Connection Enable table */
#define IPMUX_TAB_SACT_LEN          4608 /* 8,  Bridge MAC Filter Table, also source address control talbe, SACT */
#define IPMUX_TAB_REMARK_LEN        512 /* 9,  Bridge remark table */
#define IPMUX_TAB_MC_PTR_LEN            1024    /* 10, Bridge MC pointer table, Multicast Group table */
#define IPMUX_TAB_MC_LEN                4096    /* 11, Bridge MC Table, Multicast Group member table */
#define IPMUX_TAB_EGRESS_PORT_LEN   576 /* 12, Bridge Port Egress Contrl table (ECT) */
#define IPMUX_TAB_LIMIT_LEN             4096 /* 13, Bridge MAC limitation table */
#define IPMUX_TAB_LIMIT_CNT_LEN         576 /* 14, Bridge MAC limitation counter table */
#define IPMUX_TAB_PRIORITY_TAG_LEN  36  /* 15, Bridge VLAN priority tag table */

#define IPMUX_TAB_RULE_MASK_LEN     32 /* 16, Classify Rule Mask table */
#define IPMUX_TAB_RULE_HASH_LEN     256 /* 17, Classify rule hash address table */
#define IPMUX_TAB_RULE_LEN      512  /* 18, Classify rule table */
#define IPMUX_TAB_FLOW_MASK_LEN     8/*16*/ /* 20, Classify Flow ID mask table */
#define IPMUX_TAB_FLOW_HASH_LEN     (1024*8)    /* 21, Classify flow address pointer (hash) table */
#define IPMUX_TAB_FLOW_LEN      (1024*8)     /* 22, Classify Flow table */
#define IPMUX_TAB_FLOW_RMK_LEN  512 /* 23, Classify Flow Remark rule table */
#define IPMUX_TAB_CLS_CNT_HIT_LEN   512 /* 24, Classify Hit Counter table */

#if 0
#define IPMUX_TAB_US_FSHP_LEN       512 /* 32, Upstream Flow Shaper Table */
#define IPMUX_TAB_US_FWRED_LEN  512 /* 34, Upstream Flow WRED Table */
#define IPMUX_TAB_DS_FSHP_LEN       1024 /* 49, Downstream Flow Shaper Table */
#define IPMUX_TAB_DS_FWRED_LEN  1024 /* 51, Downstream Flow WRED Table */
#endif

#define IPMUX_TAB_US_FSHP_LEN   512 /* 32, Upstream Flow Shaper Table */
#define IPMUX_TAB_US_FWRED_LEN  512 /* 34, Upstream Flow WRED Table */
#define IPMUX_TAB_US_FLOW_R_LEN 512 /* 36, Upstream Flow Table (Read Only) */
#define IPMUX_TAB_US_FLOW_W_LEN 1 /* 38, Upstream Flow Table (Write Only) */
#define IPMUX_TAB_US_FCNT_LEN       512 /* 40, Upstream Flow Queue Counter table */
#define IPMUX_TAB_US_FSTATUS_LEN    512 /* 42, Upstream Flow Queue Status Table */
#define IPMUX_TAB_US_FCNT_DROP_LEN 512  /* 44, Upstream Flow Rx Packet Drop Counter Table */

#define IPMUX_TAB_DS_FSHP_LEN   1024 /* 49, Downstream Flow Shaper Table */
#define IPMUX_TAB_DS_FWRED_LEN  1024 /* 51, Downstream Flow WRED Table */
#define IPMUX_TAB_DS_FPSEQ_LEN  1024 /* 53, Downstream Flow Port Sequence Table (Read Only) */
#define IPMUX_TAB_DS_FPWPRI_LEN 72 /* 55, Downstream Port Weight and Priority Table (Read Only) */
#define IPMUX_TAB_DS_FPWSUM_LEN 144 /* 57, Downstream Port Weight Sum Table */
#define IPMUX_TAB_DS_FLOW_W_LEN 1 /* 59, Downstream Flow Table (Write Only) */
#define IPMUX_TAB_DS_PSEQ_R_LEN (72*32) /*45, Downstream Port Sequence Table (Read Only) */
#define IPMUX_TAB_DS_FCNT_LEN       1024 /* 47, Downstream Flow Queue Counter table */
#define IPMUX_TAB_DS_FSTATUS_LEN    1024 /* 49, Downstream Flow Queue Status Table */
#define IPMUX_TAB_DS_FCNT_DROP_LEN 1024 /* 51, Downstream Flow Rx Packet Drop Counter Table */

#define IPMUX_TAB_UPPT_LEN          576 /* 64, SAR UPPT */
#define IPMUX_TAB_EPPT_LEN          720 /* 65, SAR EPPT */
#define IPMUX_TAB_SAR_QS_LEN            578 /* 67, SAR Queue Status Table */
#define IPMUX_TAB_SAR_CNT_APCTRL_LEN        576 /* 68, SAR Access Port Tx/Rx Counter Control Table */
#define IPMUX_TAB_SAR_CNT_APGRPRX_LEN   256 /* 69, SAR Access Port Rx Counter Group Table */
#define IPMUX_TAB_SAR_CNT_APGRPTX_LEN   256 /* 70, SAR Access Port Tx Counter Group Table */
#define IPMUX_TAB_SAR_CNT_PVPTR_LEN     1024 /* 71, SAR Port VLAN Counter Pointer Table */
#define IPMUX_TAB_SAR_CNT_PVHASH_LEN        1024 /* 72, SAR Port VLAN Counter Hash Table */
#define IPMUX_TAB_SAR_CNT_PVIN_LEN      1024 /* 73, SAR Port VLAN In Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_PVOUT_LEN     1024 /* 74, SAR Port VLAN Out Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_VLANPTR_LEN   1024 /* 75, SAR GEW VLAN Counter Pointer Table */
#define IPMUX_TAB_SAR_CNT_VLANHASH_LEN  1024 /* 76, SAR GEW VLAN Counter Hash Table */
#define IPMUX_TAB_SAR_CNT_VLANIN_LEN        1024 /* 77, SAR GEW VLAN In Frame Statistics Group Table */
#define IPMUX_TAB_SAR_CNT_VLANOUT_LEN   1024 /* 78, SAR GEW VLAN Out Frame Statistics Group Table */

#define IPMUX_TAB_HOST_RX_LEN       16 /* 80, HOST Rx BD table, DMA0 BD table */
#define IPMUX_TAB_HOST_TX_LEN       4 /* 81, HOST Tx BD table, DMA0 BD table */
#define IPMUX_TAB_FE_RX_LEN         64 /* 96, Fast Ethernet Rx BD table, DMA1 BD table */
#define IPMUX_TAB_FE_TX_LEN         64 /* 97, Fast Ethernet Tx BD table, DMA1 BD table */

#define IPMUX_TAB_ATB_CNT_RXCL_LEN  144 /* 112, Rx cells per Link,except asm */
#define IPMUX_TAB_ATB_CNT_RXCG_LEN  36 /* 113, Rx cells per Group,except asm */
#define IPMUX_TAB_ATB_CNT_RXAL_LEN  144 /* 114, Rx asm cells per Link */
#define IPMUX_TAB_ATB_CNT_TXCL_LEN  144 /* 115, Tx cells per Link,except asm */
#define IPMUX_TAB_ATB_CNT_TXCG_LEN  36 /* 116, Tx cells per Group,except asm */
#define IPMUX_TAB_ATB_CNT_TXAL_LEN  144 /* 117, Tx asm cells per Link */

    /* Indirect Table Access Related Register definitions */
#define REG_BIT_TBUSY_OFF                           31
#define REG_BIT_TBUSY                               (FIELD_BITS(1) << REG_BIT_TBUSY_OFF)
#define REG_BIT_TTYPE_OFF                           30
#define REG_BIT_TTYPE                               (FIELD_BITS(1) << REG_BIT_TTYPE_OFF)
#define REG_BIT_TID_OFF                             16
#define REG_BIT_TID                                 (FIELD_BITS(7) << REG_BIT_TID_OFF)
#define REG_BIT_RECNO_OFF                           0
#define REG_BIT_RECNO                               (FIELD_BITS(16) << REG_BIT_RECNO_OFF)
#define IPMUX_TABLE_READ                            0
#define IPMUX_TABLE_WRITE                           REG_BIT_TTYPE
#define REG_BIT_SRS_OFF                         4
#define REG_BIT_SRS                                 (FIELD_BITS(3) << REG_BIT_SRS_OFF)

#define NUM_OF_SET                                  8
#define MAX_LEN_OF_TAB_REC                      19

#define NUM_OF_REG_PER_SET                  19
#define REG_TAB_SHDW_REG_SET0_CTRL              0xA148


#define REG_TAB_SHDW_REG_SETx_CTRL(set)         mod_tab[set].ctrl
#define REG_TAB_SHDW_REG_SETx_DATAx(set, reg)   ((unsigned int)mod_tab[set].data0 + 4 * (unsigned int)(reg))

//#define REG_TAB_SHDW_REG_SETx_DATAx(set, reg) (REG_TAB_SHDW_REG_SETx_DATA17(set) + (17 -(reg)) * 4)


    /* struct for access to IPMux Internal Table */
typedef struct IPMUX_TAB_DESC_s
    {
    int id;         /* table id */
    int len;            /* number of table records */
    int width;      /* length of table record, in words */
    char * desc;        /* description to table */
}IPMUX_TAB_DESC_t;

    /* asic modules information */
typedef struct IPMUX_MOD_TAB_s
    {
    int modid;          /* module id */
    int tab_num;            /* the number of tables in this module */
    int width;          /* the max number of words in a record of table in that modules */
    unsigned int ctrl;      /* ctrl register address for Indirect Access of Table */
    unsigned int data0; /* data0 register address for Indirect Access of Table */
    char * desc;            /* description to this module */
}IPMUX_MOD_TAB_t;

    /* user interface stands for POSPHY or UTOPIA
    *   in IPMux, one interface out of the 2 types can be used
    */
typedef enum IPMUX_IF_MODE_s
    {
    IPMUX_DUAL_LATENCY,
    IPMUX_SINGLE_LATENCY,
    MODE_END
}IPMUX_IF_MODE_e;
#define IF_GET_MODE(phyNum)         (IPMUX_NUM_OF_PHY_SL == (phyNum) ? IPMUX_SINGLE_LATENCY : IPMUX_DUAL_LATENCY)

    /* struct for access to IPMux-CPU interface */
        /* for Tx */
#define REG_BIT_HOST_PTYPE_OFF      30
#define REG_BIT_HOST_PTYPE          (FIELD_BITS(2) << REG_BIT_HOST_PTYPE_OFF)
#define REG_BIT_HOST_FMIDPID_OFF    20
#define REG_BIT_HOST_FMIDPID        (FIELD_BITS(10) << REG_BIT_HOST_FMIDPID_OFF)
#define REG_BIT_HOST_FMSEL_OFF      16
#define REG_BIT_HOST_FMSEL          (FIELD_BITS(4) << REG_BIT_HOST_FMSEL_OFF)
#define REG_BIT_HOST_UC_OFF         15
#define REG_BIT_HOST_UC             (FIELD_BITS(1) << REG_BIT_HOST_UC_OFF)
#define REG_BIT_HOST_US_OFF         14
#define REG_BIT_HOST_US             (FIELD_BITS(1) << REG_BIT_HOST_US_OFF)
#define REG_BIT_HOST_CB_OFF         13
#define REG_BIT_HOST_CB             (FIELD_BITS(1) << REG_BIT_HOST_CB_OFF)
        /* for Rx */
#define REG_BIT_HOST_IPORT_OFF      22
#define REG_BIT_HOST_IPORT          (FIELD_BITS(10) << REG_BIT_HOST_IPORT_OFF)


#define LITTLE_ENDIAN

#ifndef ntohl
#ifdef LITTLE_ENDIAN
#define ntohl(x)    ((((x) & 0x000000ff) << 24) | \
             (((x) & 0x0000ff00) <<  8) | \
             (((x) & 0x00ff0000) >>  8) | \
(((x) & 0xff000000) >> 24))

#define htonl(x)    ((((x) & 0x000000ff) << 24) | \
             (((x) & 0x0000ff00) <<  8) | \
             (((x) & 0x00ff0000) >>  8) | \
(((x) & 0xff000000) >> 24))

#define ntohs(x)    ((((x) & 0x00ff) << 8) | \
(((x) & 0xff00) >> 8))

#define htons(x)    ((((x) & 0x00ff) << 8) | \
(((x) & 0xff00) >> 8))
#define NTOHL(x)    (x) = ntohl((u_long)(x))
#define NTOHS(x)    (x) = ntohs((u_short)(x))
#define HTONL(x)    (x) = htonl((u_long)(x))
#define HTONS(x)    (x) = htons((u_short)(x))

#else /* _BYTE_ORDER==_BIG_ENDIAN */

#define ntohl(x)    (x)
#define ntohs(x)    (x)
#define htonl(x)    (x)
#define htons(x)    (x)

#define NTOHL(x)    (x) = ntohl((u_long)(x))
#define NTOHS(x)    (x) = ntohs((u_short)(x))
#define HTONL(x)    (x) = htonl((u_long)(x))
#define HTONS(x)    (x) = htons((u_short)(x))

#endif
#endif


#define bzero(pcBase, nBytes)       memset ((char *)(pcBase), OPL_ZERO, (int)(nBytes))
/*------------------- end of extra data  -------------------*/
#endif

#endif
