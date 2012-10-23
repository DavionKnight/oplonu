/*
=============================================================================
     Header Name: ipmux_ddr.h

     General Description:
IPMux DDR Controller module head file.
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 yzhang            08/21/06      Initial Version
 xwang             11/21/06      tune the parameter values of ddr controls 
----------------   ------------  ----------------------------------------------
*/
#include <configs/opipmux.h>

#define BIT0  (1<<0)
#define BIT1  (1<<1)
#define BIT2  (1<<2)
#define BIT3  (1<<3)
#define BIT4  (1<<4)
#define BIT5  (1<<5)
#define BIT6  (1<<6)
#define BIT7  (1<<7)
#define BIT8  (1<<8)
#define BIT9  (1<<9)
#define BIT10 (1<<10)
#define BIT11 (1<<11)
#define BIT12 (1<<12)
#define BIT13 (1<<13)
#define BIT14 (1<<14)
#define BIT15 (1<<15)
#define BIT16 (1<<16)
#define BIT17 (1<<17)
#define BIT18 (1<<18)
#define BIT19 (1<<19)
#define BIT20 (1<<20)
#define BIT21 (1<<21)
#define BIT22 (1<<22)
#define BIT23 (1<<23)
#define BIT24 (1<<24)
#define BIT25 (1<<25)
#define BIT26 (1<<26)
#define BIT27 (1<<27)
#define BIT28 (1<<28)
#define BIT29 (1<<29)
#define BIT30 (1<<30)
#define BIT31 (1<<31)

#ifndef DDRC_CONFIG_INCLUDE
#define DDRC_CONFIG_INCLUDE

/* !!! DDR 1G or 512M moved to opipmux.h */

/* #define LOG_DDR_CONFIG_DELAY_VAL */
#ifdef LOG_DDR_CONFIG_DELAY_VAL
#define DDR_CONFIG_DELAY_MIN     0xa1000010
#define DDR_CONFIG_DELAY_MAX     0xa1000014
#define DDR_CONFIG_DELAY_AVERAGE 0xa1000018
#endif        

/* ddr tune config */           
#define DDRTUNE_ADDR_TEST_REST 0xa0000004
#define DDRTUNE_VAL_TEST_REST 0xFFFFFFFF

#define DDRTUNE_ADDR_TEST 0xa0000000
#define DDRTUNE_VAL_TEST 0xaaaaaaaa

#define DDRTUNE_ADDR_TEST1 0xa0000004
#define DDRTUNE_VAL_TEST1 0x55555555

/* Registers address */
#ifdef CONFIG_IPMUX_ONU
    #define IPMUX_REG_BASE_DDR  0xbf014000
    #define DDR_CONFIG_1G_512M_UP 0xbf014084
    #define DDR_IO_REG 0xbf014170
#else
    #define IPMUX_REG_BASE_DDR  0xbf004800
    #define DDR_CONFIG_1G_512M_UP 0xbf004904
    #define DDR_CONFIG_1G_512M_DN 0xbf004944
    #define DDR_CONFIG_START_UP 0xbf004914
    #define DDR_CONFIG_START_DN 0xbf004950
#endif

/* Denali Control registers */
#define REG_DDR_UP_CTLX(x)       (IPMUX_REG_BASE_DDR + (x) * 4)
#define REG_DDR_DN_CTLX(x)       (IPMUX_REG_BASE_DDR + 0x80 + (x) * 4)

/* DDR Upstream Arbitration registers */
#define REG_DDRUPAE                 (IPMUX_REG_BASE_DDR + 0x100)
#define REG_DDRUPACFG               (IPMUX_REG_BASE_DDR + 0x104)

/* DDR Downstream Arbitration registers */
#define REG_DDRDNAE                 (IPMUX_REG_BASE_DDR + 0x140)
#define REG_DDRDNACFG               (IPMUX_REG_BASE_DDR + 0x144)

#define IPMUX_DDR_MAX_DELAY_VAL 0x7F
#define IPMUX_DDR_DELAY0_SHIFT 24
#define IPMUX_DDR_DELAY1_SHIFT 16

#define DENALI_START_DATA_UP  0xff000000
#define DENALI_START_DATA_DN  0xff000000 

#define DENALI_CTL_04_DATA_DN 0x00000000
#define DENALI_CTL_04_DATA_UP 0x00000000

/* DDR2 controller register config default vaule */
#ifdef CONFIG_IPMUX_ONU
#include "opulan_ddr_onu.h"
#else
#include "opulan_ddr_ipmux.h"
#endif

#ifndef UINT32
#define UINT32  unsigned int
#endif

/* define a magic number for terminator */
#define DDRC_MAGIC_NUM  0x123123ee

/* initialization process for DDR Controler.
 * user should put this MACRO into the initialization process before the Memory is accessed
 *
 * please redefine MACROs UINT32,
 */
#define IPMUX_DDR_INIT      {\
                            int ni = 0; \
                            UINT32 denaliDefVal[] = {   DENALI_CTL_00_DATA, \
                                                        DENALI_CTL_01_DATA, \
                                                        DENALI_CTL_02_DATA, \
                                                        DENALI_CTL_03_DATA, \
                                                        DENALI_CTL_04_DATA, \
                                                        DENALI_CTL_05_DATA, \
                                                        DENALI_CTL_06_DATA, \
                                                        DENALI_CTL_07_DATA, \
                                                        DENALI_CTL_08_DATA, \
                                                        DENALI_CTL_09_DATA, \
                                                        DENALI_CTL_10_DATA, \
                                                        DENALI_CTL_11_DATA, \
                                                        DENALI_CTL_12_DATA, \
                                                        DENALI_CTL_13_DATA, \
                                                        DENALI_CTL_14_DATA, \
                                                        DENALI_CTL_15_DATA, \
                                                        DENALI_CTL_16_DATA, \
                                                        DENALI_CTL_17_DATA, \
                                                        DENALI_CTL_18_DATA, \
                                                        DENALI_CTL_19_DATA, \
                                                        DENALI_CTL_20_DATA, \
                                                        DENALI_CTL_21_DATA, \
                                                        DENALI_CTL_22_DATA, \
                                                        DENALI_CTL_23_DATA, \
                                                        DENALI_CTL_24_DATA, \
                                                        DDRC_MAGIC_NUM}; \
                            while (DDRC_MAGIC_NUM != denaliDefVal[ni]) {ipMuxRegWrite(REG_DDR_UP_CTLX(ni), denaliDefVal[ni]); ni++;}; \
                            ni = 0; \
                            while (DDRC_MAGIC_NUM != denaliDefVal[ni]) {ipMuxRegWrite(REG_DDR_DN_CTLX(ni), denaliDefVal[ni]); ni++;}; \
                            ipMuxRegWrite(REG_DDR_UP_CTLX(4), DENALI_START_DATA); /*start controller upstream */ \
                            ipMuxRegWrite(REG_DDR_DN_CTLX(4), DENALI_START_DATA); /*start controller downstream */ \
                            udelay(20000);/*delay for 20 ms*/ \
                            ipMuxRegWrite(REG_DDRUPAE, 0x1); /* enable upstream arbitration */ \
                            ipMuxRegWrite(REG_DDRDNAE, 0x1); /* enable downstream arbitration */ \
                    }

#endif /* DDRC_CONFIG_INCLUDE */

