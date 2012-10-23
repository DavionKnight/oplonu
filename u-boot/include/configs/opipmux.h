/*
 * (C) Copyright 2003-2005
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
 * This file contains the configuration parameters for the INCA-IP board.
 */

#ifndef _OPIPMUX_H_
#define _OPIPMUX_H_ 1

#include "debugmode.h"
#include "board.h"  /**zw add in 1119, used to load macro of the board**/
/**************************************************************
*           Board selection                          *
**************************************************************/

/*------choose the fpga board,just one,no multi select------
 *#define CONFIG_IPMUX
 *#define CONFIG_IPMUX_E
 *#define CONFIG_IPMUX_E2
 *#define CONFIG_IPMUX_A
 *#define CONFIG_IPMUX_E_MPW
 *#define CONFIG_IPMUX_CLT
 *#define CONFIG_IPMUX_E2_C
 *#define CONFIG_IPMUX_E2_ASKEY
 *#define CONFIG_IPMUX_E2_10G                   : MX flash
 *
 *#define CONFIG_IPMUX_ONU
 *  #define CONFIG_IPMUX_ONU_A_FPGA
 *  #define CONFIG_IPMUX_ONU_MPW
 *  #define CONFIG_IPMUX_ONU_8BIT
 *  #define CONFIG_IPMUX_ONU_16BIT
 *  #define CONFIG_IPMUX_ONU_MPW_SOCKET
 *  #define CONFIG_IPMUX_ONU_A_F13              : 512M DDR
 *  #define CONFIG_IPMUX_ONU_A_C13              : p flash
 *  #define CONFIG_IPMUX_ONU_A_F23              : atheros
 *  #define CONFIG_IPMUX_ATHEROS
 *  #define CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
 *  #define CONFIG_IPMUX_ONU_XUNSHI :xunshi FPLK 131072000
 *  #define CONFIG_IPMUX_2 : 10g onu/olt/optrann FPGA version
 *  #define CONFIG_10G_IPMUX_C2 : 10g onu/olt/optrann ASIC version
 *  #define CONFIG_IPMUX_ONU_A_F23_M7 : 6097
 *  #define CONFIG_IPMUX_ONU_A_F23_TW : atheros, p flash
 *  #define CONFIG_ONU_A_F13_P08B08_M064 : 1GE, p 8bits flash 8MB,  DDR2 64MB
 */

    //#define CONFIG_IPMUX_ONU
    //#define CONFIG_IPMUX_ONU_MPW
    //#define CONFIG_IPMUX_ONU_A
    //#define CONFIG_IPMUX_ONU_MPW_SOCKET
    //#define CONFIG_IPMUX_ONU_8BIT
    //#define  CONFIG_IPMUX_ONU_XUNSH
    //#define CONFIG_IPMUX_ATHEROS
    //#define CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
    //#define CONFIG_IPMUX_ATHEROS_HW_RESET
    //#define CONFIG_IPMUX_E2
    //#define CONFIG_IPMUX_E2_ASKEY
    //#define CONFIG_IPMUX_E2_C
 //   #define CONFIG_IPMUX_E2_10G




//#define defaultsize flash_4M
//#define FLASHSIZE defaultsize
#define CFG_SPI_FLASH   /*default spi*/
//#define IPMUX_E2_ETH_GE

#if defined(CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M_BC)
#define CONFIG_IPMUX_ONU_A_F13_BC
#define CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M
#define REG_GMAC_MODE                    (0x090c*4)
#define REG_GMAC_RGMII_TXDLL_CFG         (0x0996*4)
#endif 

#if defined(CONFIG_ONU_1_1_RF400_P)
// reuse RF400's configuration except for flash
#define CONFIG_ONU_1_1_RF400
#endif

#if defined(CONFIG_ONU_1_1_RF400)
#define REG_GMAC_MODE                    (0x090c*4)
#define REG_MDIO_SELECT                  (0x0052*4)
#define REG_DMA0_EN                         (0x6005*4)
#define REG_GMAC_CFG_TRANSFER_ENA (0x0900*4)
#endif

/*phy */
#ifdef CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M_BC
#define PHY_DEVICE_ADDR                  (0x07)
#else
#define PHY_DEVICE_ADDR                  (0x02)
#endif


#if defined(CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M)
#define CONFIG_ONU_1_1_C13_SPI
#endif

#if defined(CONFIG_ONU_1_1_F13)   
#define CONFIG_IPMUX_ONU_A_F13
#endif

#if defined(CONFIG_IPMUX_ONU_1_1_F23)||defined(CONFIG_IPMUX_ONU_1_1_DDR512M_F23)
#define CONFIG_IPMUX_ONU_A_F23
#endif

 
#if defined(CONFIG_IPMUX_ONU_A_F23_D2_131)    
     #define  CONFIG_IPMUX_ONU_A_F23_D2
#endif
#if  defined(CONFIG_IPMUX_ONU_A_F23_TW)||defined(CONFIG_IPMUX_ONU_A_F23_TW_EOC)||defined(CONFIG_IPMUX_ONU_A_F23_D2)||defined(CONFIG_IPMUX_ONU_A_F23_DAYA)
    #define  CONFIG_IPMUX_ONU_A_F23
#endif

#if defined(CONFIG_IPMUX_ONU_A_F13_TW)
    #define  CONFIG_ONU_A_F13_P08B08_M064
#endif

#if  defined(CONFIG_10G_IPMUX_C2)||defined(CONFIG_10G_IPMUX_C3) ||defined(CONFIG_10G_IPMUX_C3_NJZTE) \
    ||defined(CONFIG_IPMUX2_OPTRANN_FPGA)||defined(CONFIG_10G_IPMUX_T3) 
#define CONFIG_IPMUX_2
#endif 
 
#if defined(CONFIG_IPMUX_ONU_A_FPGA)||defined(CONFIG_IPMUX_ONU_A_F13)   	   \
	||defined(CONFIG_IPMUX_ONU_A_C13)||defined(CONFIG_IPMUX_ONU_A_F23)   \
	||defined(CONFIG_IPMUX_ONU_A_F23_M7)||defined(CONFIG_IPMUX_ONU_A_F23_TW)|| defined(CONFIG_IPMUX_ONU_A_F23_TW_EOC)   \
       ||defined(CONFIG_IPMUX_ONU_A_XUNSHI)||defined(CONFIG_IPMUX_ONU_A_F23_D2)    \
       ||defined(CONFIG_IPMUX_ONU_A_F23_DAYA)||defined(CONFIG_IPMUX_ONU_A_F23_D2_131) \
	||defined(CONFIG_ONU_A_F13_P08B08_M064)||defined(CONFIG_ONU_1_1_C13) \
       ||defined(CONFIG_ONU_1_1_C13_SPI)   ||defined(CONFIG_ONU_1_1_C13_DDR_CLK_300)\
       ||defined(CONFIG_ONU_1_1_C13_SPI_DDR_CLK_300)||defined(CONFIG_ONU_1_1_C13_SPI_DDR_CLK_300_512M)\
       || defined( CONFIG_IPMUX_ONU_1_1_F23_DDR_CLK300M)||defined(CONFIG_ONU_1_1_RF400)
    #define CONFIG_IPMUX_ONU
    #define CONFIG_IPMUX_ONU_MPW
    #define CONFIG_IPMUX_ONU_A
#endif
#if defined(CONFIG_IPMUX_E2_10G)||defined(CONFIG_IPMUX_E2_ASKEY)||defined(CONFIG_IPMUX_E2_C)     \
	||defined(CONFIG_IPMUX_E2_ZTE)
      #define CONFIG_IPMUX_E2

#endif
 
#if defined(CONFIG_IPMUX_ONU_A_F13)||defined(CONFIG_ONU_A_F13_P08B08_M064)||defined(CONFIG_ONU_1_1_RF400)
		#define CONFIG_IPMUX_ONU_A_F13_BC
#endif

#if defined(CONFIG_IPMUX_ONU_A_F23)||defined(CONFIG_ONU_1_1_RF400)
    #define CONFIG_IPMUX_ATHEROS
    #ifndef CONFIG_ONU_1_1_RF400
    #define CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
    #endif
    #ifndef CONFIG_IPMUX_ONU_A_F23_TW 
        #define CONFIG_IPMUX_ATHEROS_HW_RESET
    #endif
    #ifndef CONFIG_IPMUX_ONU_A_F23_TW_EOC
        #define CONFIG_IPMUX_ATHEROS_HW_RESET
    #endif 
    //#define CONFIG_IPMUX_ATHEROS_HW_RESET
#endif


#define uboffset  0x0
#define envoffset  0x40000
#if defined(CONFIG_IPMUX_E2_10G)||defined(CONFIG_IPMUX_ONU_A_F23_D2)     \
       ||defined(CONFIG_ONU_2) ||defined(CONFIG_IPMUX_2)||defined(CONFIG_IPMUX_ONU_A_F23_TW_EOC) || defined(CONFIG_ONU_1_1_RF400_P)
    #define cfg_offset 0x60000     
#else
   #define cfg_offset 0x50000
#endif
#if defined(CONFIG_IPMUX_E2_10G)||defined(CONFIG_IPMUX_ONU_A_F23_D2)       \
       ||defined(CONFIG_ONU_2) ||defined(CONFIG_IPMUX_2)||defined(CONFIG_IPMUX_ONU_A_F23_TW_EOC)|| defined(CONFIG_ONU_1_1_RF400_P)
   #define knl_1offset 0x80000  
#elif (defined(CONFIG_IPMUX_E2_C))||defined(CONFIG_IPMUX_E2_ZTE)
   #define knl_1offset  0xe000000 
    #else
  #define  knl_1offset 0x60000
#endif
#define knl_2offset  ((flash_info[0].size+knl_1offset)/2)                       /*knl_1offset+(flashsize-knl_1offset)/2)*/

/***/
//#define uboffset  0x0
//#define envoffset  0x40000

#define flash_2M    0x200000
#define flash_4M    0x400000
#define flash_8M    0x800000
#define flash_16M   0x1000000
#define flash_32M   0x2000000
#define flash_64M   0x4000000
#define flash_128M 0x8000000


#ifdef   CPU_CLOCK_RATE
#undef   CPU_CLOCK_RATE
#endif

#if defined(CONFIG_IPMUX_ONU_A_C13) || defined(CONFIG_IPMUX_ONU_A_F23_TW)||defined(CONFIG_IPMUX_ONU_A_F23_TW_EOC) || defined(CONFIG_IPMUX_ONU_XUNSHI)   \
     || defined(CONFIG_IPMUX_ONU_A_F23_D2)||defined(CONFIG_ONU_2) \
	 || defined(CONFIG_ONU_A_F13_P08B08_M064)||defined(CONFIG_ONU_1_1_C13)\
	 ||defined(CONFIG_ONU_1_1_C13_DDR_CLK_300) ||defined (CONFIG_IPMUX_ONU_A_F23_TW_EOC)/*8bit flash*/ 
    #define CONFIG_IPMUX_ONU_8BIT
    #undef CFG_SPI_FLASH
#endif

#if defined(CONFIG_ONU_1_1_C13) ||defined(CONFIG_ONU_1_1_C13_DDR_CLK_300)\
    ||defined(CONFIG_ONU_1_1_C13_SPI) ||defined(CONFIG_ONU_1_1_C13_SPI_DDR_CLK_300)\
    ||defined(CONFIG_IPMUX_ONU_1_1_F23)||defined(CONFIG_ONU_1_1_C13_SPI_DDR_CLK_300_512M)||defined(CONFIG_IPMUX_ONU_1_1_DDR512M_F23) \
    || defined(CONFIG_IPMUX_ONU_1_1_F23_DDR_CLK300M)
//#define ONU1_1_SPI_REG
//#define DRV_NEW_SPI
#endif


#if defined(CONFIG_ONU_1_1_C13_SPI) ||defined(CONFIG_ONU_1_1_C13_SPI_DDR_CLK_300)\
||defined(CONFIG_IPMUX_ONU_1_1_F23) ||defined(CONFIG_ONU_1_1_C13_SPI_DDR_CLK_300_512M)||defined(CONFIG_IPMUX_ONU_1_1_DDR512M_F23) \
|| defined( CONFIG_IPMUX_ONU_1_1_F23_DDR_CLK300M)||defined(CONFIG_ONU_1_1_RF400)
 #undef CONFIG_IPMUX_ONU_8BIT
 #undef CONFIG_IPMUX_ONU_16BIT
 //#define CFG_NEW_SPI_FLASH
 #define CFG_SPI_FLASH
#endif

#if defined(CONFIG_ONU_1_1_RF400_P)
#define CONFIG_IPMUX_ONU_8BIT
#undef CFG_SPI_FLASH
#endif

#if defined(CONFIG_IPMUX_2)||defined(CONFIG_IPMUX_E2)||defined(CONFIG_ONU_2)  /*16bit flash*/
 #undef CFG_SPI_FLASH

#endif

/*kxu 20110301*/
#if  defined(CONFIG_10G_IPMUX_C2)
 #undef CFG_SPI_FLASH
 #undef CONFIG_IPMUX_ONU_8BIT
 #undef CFG_NEW_SPI_FLASH
 #define CONFIG_IPMUX_ONU_16BIT
#elif (defined(CONFIG_10G_IPMUX_C3))
 #undef CFG_SPI_FLASH
 #undef CONFIG_IPMUX_ONU_8BIT
 #undef CONFIG_IPMUX_ONU_16BIT
 #define CFG_NEW_SPI_FLASH
 #define CONFIG_OPTRAN
#elif (defined(CONFIG_10G_IPMUX_C3_NJZTE))
 #undef CFG_SPI_FLASH
 #undef CONFIG_IPMUX_ONU_8BIT
 #undef CONFIG_IPMUX_ONU_16BIT
 #undef CFG_NEW_SPI_FLASH
 #define CONFIG_SPI_NJZTE
 #define CONFIG_OPTRAN
#endif

#if defined(CONFIG_10G_IPMUX_T3) 
#define  FPCLK          275000000
//#define  FPCLK          200000000
//#define  FPCLK           175000000
//#define  FPCLK           209375000
//#define  FPCLK           225000000
//#define  FPCLK           259375000
//#define  FPCLK          243750000
#define CPU_CLOCK_RATE  (300000000)  	

#define CONFIG_PLL_PARA
        
 #undef CFG_SPI_FLASH
 #undef CONFIG_IPMUX_ONU_8BIT
 #undef CFG_NEW_SPI_FLASH
 #define CONFIG_IPMUX_ONU_16BIT
 
#endif

#if defined(CONFIG_ONU_1_1_F13)   
 #define CFG_SPI_FLASH
 #undef CONFIG_IPMUX_ONU_8BIT
 #undef CONFIG_IPMUX_ONU_16BIT
//#define CFG_NEW_SPI_FLASH
//#define ONU1_1_SPI_REG
//#define DRV_NEW_SPI
//#define CONFIG_ONU_1_1_DDR_SIZE_512M
#endif

#if defined(CONFIG_IPMUX2_OPTRANN_FPGA)
//#define  FPCLK       156000000
#define  FPCLK          71000000   
//#define  FPCLK          50000000   
//#define  FPCLK          100000000   
#define CPU_CLOCK_RATE  (30000000)  	 
#define CONFIG_IPMUX_ONU_16BIT
#define FPGA_VERSION
#endif

#if defined(CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M) || defined(CONFIG_IPMUX_ONU_1_1_DDR512M_F23)
//#define CONFIG_ONU_1_1_DDR_SIZE_512M
#endif


/* begin added by jiangmingli */
//#define CONFIG_IPMUX_ONU_DDR_TUNE

/* Pattern 1 */
#define DDR_TEST_VAL1   0x10101010
#define DDR_TEST_VAL2   0xEFEFEFEF
#define DDR_TEST_VAL3   0x10101010
#define DDR_TEST_VAL4   0xEFEFEFEF
/* end added by jiangmingli */

/**************************************************************
 *                           Debug Switch                     *
 **************************************************************/
//#define ET_DEBUG 1
//#define DEBUG_HARDWARE 1

/* debug early boot */
#define DBG printf("%s()............\n",__FUNCTION__);

/* debug board.c*/
#ifdef DEBUG_HARDWARE
#define debug_boot_ram(x) serial_puts(x)
#else
#define debug_boot_ram(x)
#endif

/* for early debug of hardware, in asm level, direct put char to uart */
#ifdef DEBUG_HARDWARE
   #define inituart9600  la t5,0xBF00400c;  li t7,0x80;  sw t7,0(t5); delay;\
                       la t5,0xBF004000;  li t7,0x95;  sw t7,0(t5); delay;\
                       la t5,0xBF004004;  li t7,0x00;  sw t7,0(t5); delay;\
                       la t5,0xBF00400c;  li t7,0x03;  sw t7,0(t5); delay;
    
   #define inituart38400 la t5,0xBF00400c;  li t7,0x80;  sw t7,0(t5); delay;\
                       la t5,0xBF004000;  li t7,0x95;  sw t7,0(t5); delay;\
                       la t5,0xBF004004;  li t7,0x00;  sw t7,0(t5); delay;\
                       la t5,0xBF00400c;  li t7,0x03;  sw t7,0(t5); delay;

    #define printa(x)  la t5,0xBF004000;  li t7,x;  sw t7,0(t5); \
                       la t7,0; \
                       or t7,ra; \
                       nop;nop;nop;\
                       la ra,0;\
                       or ra,t7
   #ifdef CONFIG_ONU_2       
   11111
    #define printl(x)  la t5,0xBF070000;  li t7,x;  sw t7,0(t5); \
                       la t7,0; \
                       or t7,ra; \
					   nop;nop;nop;\
                       la ra,0;\
                       or ra,t7
  #else
    #define printl(x)  printa(x)
  #endif
#else
    #define inituart9600
    #define inituart38400
     
    #define printa(x)  
    #define inituarta  
    #define printl(x)  
#endif

//#define DEBUG_FLASH 1
#ifdef  DEBUG_FLASH
#define dbgFlash(fmt,args...)   printf (fmt ,##args)
#else
#define dbgFlash(fmt,args...)
#endif  

/**************************************************************
 *                           OS Configurations                *
 **************************************************************/


/**************************************************************
 *                           Commond Define                   *
 **************************************************************/
/* Cache Configuration */
/* old : 4096*/
#if  defined(CONFIG_IPMUX_2)||defined(CONFIG_ONU_2)
#define CFG_DCACHE_SIZE 8192*4
#define CFG_ICACHE_SIZE 8192*4
#define CFG_CACHELINE_SIZE 16*2
#else
#define CFG_DCACHE_SIZE 8192
#define CFG_ICACHE_SIZE 8192
#define CFG_CACHELINE_SIZE 16
#endif

/*env configuraton*/
#define CONFIG_ENV_OVERWRITE 1
#define CONFIG_RELEASE 1

/* Use the HUSH parser */
#if 0
#define CFG_HUSH_PARSER
#ifdef  CFG_HUSH_PARSER
#define CFG_PROMPT_HUSH_PS2 "> "
#endif
#endif

#define CONFIG_MIPS32       1   /* MIPS 4Kc CPU core    */
#define CONFIG_OP_IPMUX 1   /* on a OPIPMUX Board   */

#define CONFIG_IPADDR 192.168.1.98

#define CONFIG_ETHADDR  00:00:62:ee:ee:ee
#define CONFIG_OVERWRITE_ETHADDR_ONCE   1

#define INFINEON_EBU_BOOTCFG    0x40C4  /* CMULT = 8 */

#ifdef DEBUGMODE
#define CONFIG_BOOTDELAY    5   /* autoboot after 5 seconds */
#else
#define CONFIG_BOOTDELAY  1
#endif

#define CONFIG_BAUDRATE 115200

/* valid baudrates */
#define CFG_BAUDRATE_TABLE  { 9600, 19200, 38400, 57600, 115200 }

#define CONFIG_TIMESTAMP        /* Print image info with timestamp */

#define CONFIG_PREBOOT  ""

#undef  CONFIG_BOOTARGS

#define CONFIG_EXTRA_ENV_SETTINGS                   \
    "nfsargs=setenv bootargs root=/dev/nfs rw "         \
        "nfsroot=$(serverip):$(rootpath)\0"         \
    "ramargs=setenv bootargs root=/dev/ram rw\0"            \
    "addip=setenv bootargs $(bootargs) "                \
        "ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask)"  \
        ":$(hostname):$(netdev):off\0"              \
    "addmisc=setenv bootargs $(bootargs) "              \
        "console=ttyS0,$(baudrate) "                \
        "ethaddr=$(ethaddr) "                   \
        "panic=1\0"                     \
    "flash_nfs=run nfsargs addip addmisc;"              \
        "bootm $(kernel_addr)\0"                \
    "flash_self=run ramargs addip addmisc;"             \
        "bootm $(kernel_addr) $(ramdisk_addr)\0"        \
    "net_nfs=tftp 80500000 $(bootfile);"                \
        "run nfsargs addip addmisc;bootm\0"         \
    "rootpath=/opt/eldk/mips_4KC\0"                 \
    "bootfile=/tftpboot/INCA/uImage\0"              \
    "kernel_addr=B0040000\0"                    \
    "ramdisk_addr=B0100000\0"                   \
    "u-boot=/tftpboot/INCA/u-boot.bin\0"                \
    "load=tftp 80500000 $(u-boot)\0"                \
    "update=protect off 1:0-2;era 1:0-2;"               \
        "cp.b 80500000 B0000000 $(filesize)\0"          \
    ""
#define CONFIG_NFSBOOTCOMMAND  "tftp 0x80a00000 uImage.a\\;bootm 0x80a00000"
#define CONFIG_BOOTARGS  "root=/dev/nfs rw ip=192.168.1.98 mac=00:00:62:ee:ee:ee nfsroot=192.168.1.58:/home/ychen/workbench/mips_4KC_new mem=32M console=ttyS0,115200 nof"
#define CONFIG_SERVERIP  192.168.1.58
#define CONFIG_ETHADDR 00:00:62:ee:ee:ee

#define CONFIG_BOOTCOMMAND  "run nfsboot"
#define CONFIG_DDR2_USED_BY_CPU  32 //added by kxu  for new envirnmental variable  "mem"
#define CONFIG_OPL_HEAD_SWITCH   /*kxu 20110429*/

#if 0
#define CONFIG_COMMANDS        (CONFIG_CMD_DFL  | \
                CFG_CMD_ASKENV  | \
                CFG_CMD_DHCP    | \
                CFG_CMD_ELF | \
                CFG_CMD_JFFS2   | \
                CFG_CMD_NFS | \
                CFG_CMD_PING    | \
                CFG_CMD_SNTP    )
#else
#define CONFIG_COMMANDS        (CONFIG_CMD_DFL  | \
                CFG_CMD_ASKENV  | \
                CFG_CMD_ELF | \
                CFG_CMD_PING    | \
                CFG_CMD_SNTP   |\
                CFG_CMD_XDOWN  |\
                CFG_CMD_UPDATE )     /***zw change for xdown support***/
#endif
#include <cmd_confdefs.h>

/**************************************************************
 *                        OPL Style Code Defines              *
 **************************************************************/
/*extra : for port driver code to uboot*/
#define UINT32   unsigned int
#define IPMUX_REG_BASE          0xBF000000
#define OPL_OK              0
#define OPL_ERROR           (-1)
#define OPL_STATUS          int
#define OPL_ZERO            0
#define OPL_NULL            ((void *) 0)    /* a null pointer */
#define REGID_INIT_VALUE 0xFFFFAAAA
#define REG_ADDR_ALLIGNED       4
#define IPMUX_REG_ID2ADDR(regId)    (IPMUX_REG_BASE + (regId))

#define DBGREG printf
/*
#define ipMuxRegWrite(regId, val) DBGREG("write 0x%08x val=0x%08x\n",IPMUX_REG_ID2ADDR(regId),val);*((volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)) = (UINT32)(val)
#define ipMuxRegRead(regId, pval) DBGREG("read  0x%08x\n",IPMUX_REG_ID2ADDR(regId));*(UINT32*)(pval) = *(volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)
*/
#define ipMuxRegWrite(regId, val)   *((volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)) = (UINT32)(val)
#define ipMuxRegRead(regId, pval)   *(volatile UINT32*)(pval) = *(volatile UINT32 *)IPMUX_REG_ID2ADDR(regId)
#define SWAP(x)	 ((((x) & 0x000000ff) << 24) | (((x) & 0x0000ff00) << 8)|\
			  (((x) & 0x00ff0000) >>  8) | (((x) & 0xff000000) >> 24) )      /***zw add in 1205***/

#define strtoul simple_strtol
#define strtol simple_strtol

/**************************************************************
 *             Miscellaneous configurable options             *
 **************************************************************/
#define CFG_LONGHELP                /* undef to save memory      */
#define CFG_PROMPT      "Opulan > " /* Monitor Command Prompt    */
#define CFG_CBSIZE      256     /* Console I/O Buffer Size   */
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#define CFG_MAXARGS     16      /* max number of command args*/

#define CFG_MALLOC_LEN      4*128*1024

#define CFG_BOOTPARAMS_LEN  128*1024

#define CFG_HZ          (opipmux_get_cpuclk()/2)

#define CFG_SDRAM_BASE      0x80000000     /*change 0x80000000 to test relocate routine*/

#define CFG_LOAD_ADDR       0x80100000  /* default load address */

#define CFG_MEMTEST_START   0x80100000
#define CFG_MEMTEST_END     0x81D00000

/**************************************************************
 *             FLASH and environment organization             *
 **************************************************************/

#if defined( CONFIG_IPMUX_E2_C)||defined(CONFIG_IPMUX_E2_ZTE)
#define CFG_MAX_FLASH_BANKS 2   /* max number of memory banks */
#else
#define CFG_MAX_FLASH_BANKS 1
#endif


#define PHYS_FLASH_1      0xb0000000  /*   (0xbfc00000 -(FLASHSIZE-flash_4M)) *//* Flash Bank #1 */ /*zw chang*/
#define PHYS_FLASH_2      0xbe000000 /* Flash Bank #2 */
#define PHYS_FLASH_3      0x800000   

/* The following #defines are needed to get flash environment right */
#define CFG_MONITOR_BASE    TEXT_BASE
#define CFG_MONITOR_LEN     (192 << 10)
#define OPIPMUX_MAX_DRAM_SZ              0x2000000
#define CFG_INIT_SP_OFFSET  OPIPMUX_MAX_DRAM_SZ-0x150000
#define CFG_FLASH_BASE      PHYS_FLASH_1

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT    (20 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT    (2 * CFG_HZ) /* Timeout for Flash Write */

#define CFG_ENV_IS_IN_FLASH 1

/* Configure console */
#define CFG_CONSOLE_IS_IN_ENV   1

/* Address and size of Primary Environment Sector   */

#if (defined(CONFIG_IPMUX_E2_10G))||(defined(CONFIG_IPMUX_ONU_A_F23_D2))||defined(CONFIG_ONU_2) ||defined(CONFIG_IPMUX_2)||defined(CONFIG_IPMUX_ONU_A_F23_TW_EOC) || defined(CONFIG_ONU_1_1_RF400_P)
    #define ENV_OFFSET 0x40000
    #define CFG_ENV_SIZE 0x20000
    #define CFG_ENV_SECTOR_SIZE 0x20000
    #define FLASH_SECTOR_MASK 0x1FFFF
    #define CFG_MAX_FLASH_SECT  (256)    /* max number of sectors on one chip */
    #define CFG_ENV_ADDR        (ENV_OFFSET + CS0_REDEFINED)
#elif (defined(CONFIG_IPMUX_E2_C))||defined(CONFIG_IPMUX_E2_ZTE)  /*zw add CONFIG_IPMUX_E2_C*/
   #define ENV_OFFSET 0x40000
   #define CFG_ENV_SIZE 0x10000
    #define FLASH_SECTOR_MASK 0x7FFF
    #define CFG_ENV_SECTOR_SIZE 0x8000
    #define CFG_MAX_FLASH_SECT  (256)    /* max number of sectors on one chip */
   #define CFG_ENV_ADDR        (ENV_OFFSET + CS0_REDEFINED)

    
#else
    #define ENV_OFFSET 0x40000   /*zw 925*/
    #define CFG_ENV_SIZE 0x10000  /*zw 925*/
    #define CFG_ENV_SECTOR_SIZE 0x10000  /*zw 925*/
    #define FLASH_SECTOR_MASK 0xFFFF
/* Since we use SPI flash, the CRC32 to the ENV variables would be really slow. We would take a simple action to 
* reduce the size of the ENV variables, this is it, 0x10000--->0x1000 */
    #define CFG_MAX_FLASH_SECT  (256)    /* max number of sectors on one chip */  /*zw 925*/
    #define CFG_ENV_ADDR        (ENV_OFFSET + CS0_REDEFINED)
#endif

/* actually the 4k size for env variable would be enough, reduce it in order to accelarate the boot speed in spi flash 
 * to a comfortable speed */
#define KERNEL_OFFSET (ENV_OFFSET+CFG_ENV_SECTOR_SIZE ) 
#define KERNEL_ADDR (0xB0000000 + KERNEL_OFFSET) /*zw change*/

#ifdef CFG_ENV_SIZE 
#undef CFG_ENV_SIZE
#define CFG_ENV_SIZE 0x1000
#endif


#define CFG_ENV_ADDR_INIT   (ENV_OFFSET + PHYS_FLASH_1) 

#if 0
#define CONFIG_FLASH_16BIT
#endif


#define CONFIG_NR_DRAM_BANKS    1
#define CONFIG_OP_IPMUX     1

#define CONFIG_OPIPMUX_GMAC
#define CONFIG_NET_MULTI

/*
 * JFFS2 partitions
 */
/* No command line, one static partition, use all space on the device */
#undef CONFIG_JFFS2_CMDLINE
#define CONFIG_JFFS2_DEV        "nor1"
#define CONFIG_JFFS2_PART_SIZE      0xFFFFFFFF
#define CONFIG_JFFS2_PART_OFFSET    0x00000000

/*************************************************************
 *           Board Speciafied Configuration                  *
 *************************************************************/ 
/* choose the DDR Memory type : 1G or 512M */
#define DDR2_UP_1G
#define DDR2_DN_1G

/* DDR Memory Size: 1G, Clock: 266M*/
#if defined(CONFIG_IPMUX_ONU_A_C13)||defined(CONFIG_IPMUX_ONU_A_F23) \
	||defined(CONFIG_IPMUX_ONU_A_XUNSHI)||defined(CONFIG_IPMUX_ONU_A_F23_D2) \
	||defined(CONFIG_IPMUX_ONU_A_F23_D2_131) \
	||defined(CONFIG_ONU_1_1_C13) \
	||defined(CONFIG_ONU_1_1_C13_SPI)\
	||defined(CONFIG_ONU_1_1_RF400)

#define CONFIG_ONU_DDR_1G_CLK_266
#define CONFIG_ONU_DDR

#endif

/* Size: 512M, Clock: 266M*/
#if defined(CONFIG_IPMUX_ONU_A_F23_TW)||defined(CONFIG_IPMUX_ONU_A_F23_TW_EOC) \
	||defined(CONFIG_IPMUX_ONU_A_F23_DAYA)||defined(CONFIG_IPMUX_ONU_A_F23_M7) \
	||defined(CONFIG_IPMUX_ONU_A_F13)||defined(CONFIG_ONU_A_F13_P08B08_M064) \
	||defined(CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M) \
	||defined(CONFIG_IPMUX_ONU_1_1_DDR512M_F23)

#ifdef DDR2_UP_1G
#undef DDR2_UP_1G
#undef DDR2_DN_1G
#endif

#undef CONFIG_ONU_DDR_1G_CLK_266

#define CONFIG_ONU_DDR_512M_CLK_266
#define CONFIG_ONU_DDR

#endif

/* DDR Memory Size: 512M, Clock: 300M*/
#if defined(CONFIG_ONU_1_1_C13_SPI_DDR_CLK_300_512M)

#ifdef DDR2_UP_1G
#undef DDR2_UP_1G
#undef DDR2_DN_1G
#endif

#undef CONFIG_ONU_DDR_1G_CLK_266
#undef CONFIG_ONU_DDR_512M_CLK_266

#define CONFIG_ONU_DDR_512M_CLK_300
#define CONFIG_ONU_DDR

#endif

/* DDR Memory Size: 1G, Clock: 300M*/
#if defined(CONFIG_ONU_1_1_C13_DDR_CLK_300) \
	||defined(CONFIG_ONU_1_1_C13_SPI_DDR_CLK_300) \
	||defined( CONFIG_IPMUX_ONU_1_1_F23_DDR_CLK300M)

#undef CONFIG_ONU_DDR_1G_CLK_266
#undef CONFIG_ONU_DDR_512M_CLK_266
#undef CONFIG_ONU_DDR_512M_CLK_300

#define CONFIG_ONU_DDR_1G_CLK_300
#define CONFIG_ONU_DDR

#endif

/* DDR Memory default configuration */
#ifdef DDR2_UP_1G
#ifndef CONFIG_ONU_DDR
#define CONFIG_ONU_DDR_1G_CLK_266
#endif
#endif


  //  #undef DDR2_UP_1G
/* DDR2 size configure, if defined DDR2_USED_BY_CPU_64M indicates that can use 64M ddr2 for mips, 
 * if not defined only 32Mb size can be used.
 */
#define DDR2_USED_BY_CPU_32M
/*
#define DDR2_USED_BY_CPU_64M
#define DDR2_USED_BY_CPU_96M
*/

/*
 * Flash size configure, if defined BOOTFLASH_LARGER_THAN_4M indicates that can use 8M flash for mips, if not defined 
 *  only 4Mb size can be used. 
 */
#ifndef CONFIG_IPMUX_ONU
/* begin modified by jiangmingli for OS more than 5M */
#define OS_MAX_SIZE 0x800000
/* end modified by jiangmingli for OS more than 5M */
#else
#define OS_MAX_SIZE 0x3D0000  /*CHANGED IN1105*/
#endif

/*------choose the code company------ */
#define CONFIG_IPMUX_OPULAN
/*
#define CONFIG_IPMUX_OPULAN
#define CONFIG_IPMUX_H3C
*/



/*------board related config, don,t change------*/
#if defined(CONFIG_IPMUX_A) || defined(CONFIG_IPMUX_E_MPW) || defined (CONFIG_IPMUX_E2)
    #define CONFIG_TUNE_DDR 1
    #define CPU_CLOCK_RATE  400000000    /* default: 150 MHz clock for the MIPS core */
#else
#if defined(CONFIG_IPMUX_CLT) 
    #define CPU_CLOCK_RATE  25000000    /* default: 50 MHz clock for the MIPS core */
#endif
#if defined(CONFIG_IPMUX_ONU) 
#ifdef CONFIG_IPMUX_ONU_MPW
    #define CONFIG_TUNE_DDR 1
    #if defined(CONFIG_IPMUX_ONU_A_F13_TW)
        #define CPU_CLOCK_RATE  ((((*(volatile u32 *)(0xBF000000))&0xF)==1)?(212992000*2):(300000000*2))
    #else  
        #define CPU_CLOCK_RATE  ((((*(volatile u32 *)(0xBF000000))&0xF)==1)?(203125000*2):(200000000*2))
    #endif  
#else
    #define CPU_CLOCK_RATE  31250000    /* default: 50 MHz clock for the MIPS core */
#endif
#endif
#endif

//#ifdef CONFIG_IPMUX_2
	#define CONFIG_TUNE_DDR 1
//#endif


#if defined(CONFIG_IPMUX_E_MPW)    /* || defined(CONFIG_IPMUX_E_MPW) */
#define FPCLK           150000000
#endif
#if   defined(CONFIG_IPMUX_E_MPW) 
#define FPCLK           125000000
#endif

#if defined(CONFIG_IPMUX_E2_10G)  /**zw change in 1215**/
#define FPCLK           150000000
#endif

#if defined(CONFIG_IPMUX_E2_C)
#define FPCLK           150000000
//#define FPCLK           71000000
//#define FPCLK           50000000
#endif
#if defined(CONFIG_IPMUX_E2_ZTE)
#define FPCLK           (28224000*5)
  #ifdef   CPU_CLOCK_RATE
  #undef   CPU_CLOCK_RATE
  #endif
  #define  CPU_CLOCK_RATE  32000000
#endif

#if defined(CONFIG_IPMUX_E)
#define FPCLK           150000000
#endif

#if defined(CONFIG_IPMUX_CLT) 
#define FPCLK           62500000
#endif

#if defined(CONFIG_IPMUX_ONU) 
#if defined(CONFIG_IPMUX_ONU_MPW)
	#if defined(CONFIG_IPMUX_ONU_XUNSHI)||defined(CONFIG_IPMUX_ONU_A_F13_TW)
     #define FPCLK         ((((*(volatile u32 *)(0xBF000000))&0xF)==1)?(131072000):(125000000))   
   #else  
     #define  FPCLK       125000000
#endif  
#elif defined(CONFIG_IPMUX_ONU_A_FPGA)
#define FPCLK           75000000
//#elif defined(CONFIG_IPMUX_ONU_A_F13)||defined(CONFIG_IPMUX_ONU_A_F23)
// #define  FPCLK       125000000 
#else
#define FPCLK           62500000
#endif
#endif

#ifdef CONFIG_IPMUX_FH
#undef FPCLK
#define FPCLK           150000000
#endif

#ifdef CONFIG_IPMUX_ONU_A_F23_D2_131
  #ifdef FPCLK
   #undef FPCLK
  #endif
  #define FPCLK 131072000
#endif

#if defined(CONFIG_10G_IPMUX_C2) 
#define  FPCLK          275000000
//#define  FPCLK          200000000
#define CPU_CLOCK_RATE  (300000000)  	 
#elif defined(CONFIG_10G_IPMUX_C3)
#define  FPCLK          275000000
//#define  FPCLK          200000000
//#define  FPCLK           209375000
//#define  FPCLK           225000000
//#define  FPCLK           259375000
//#define  FPCLK          243750000
#define CPU_CLOCK_RATE  (150000000)  	 
#elif defined(CONFIG_10G_IPMUX_C3_NJZTE)
#define  FPCLK          275000000
//#define  FPCLK          200000000
#define CPU_CLOCK_RATE  (150000000)  	 
#endif

#if defined(CONFIG_10G_IPMUX_C3)||defined(CONFIG_10G_IPMUX_C3_NJZTE)
#define NO_CP_RELO_CODE_TO_RAM
#endif

#if  defined(CONFIG_10G_IPMUX_C2)||defined(CONFIG_10G_IPMUX_C3) ||defined(CONFIG_10G_IPMUX_C3_NJZTE)
#define CONFIG_PLL_PARA
#endif 


 #ifdef CONFIG_ONU_2
	#define  FPCLK          100000000   
    #ifdef   CPU_CLOCK_RATE
        #undef   CPU_CLOCK_RATE
    #endif
	 #define CPU_CLOCK_RATE  (50000000)  
#endif
/************************************************************
*                                     CS0 DEFINE                                                 *
************************************************************/
#if defined(CONFIG_IPMUX_E2)      /*zw changed in 1215*/
    #define REG_CS0_BASE_ADDR 0x1020
    #define REG_CS0_SIZE      0x1048

    #define REG_CS1_BASE_ADDR 0x1024
    #define REG_CS1_SIZE      0x104C
#endif

#ifdef  CONFIG_IPMUX_E_MPW
    #define REG_CS0_BASE_ADDR 0x1020
    #define REG_CS0_SIZE      0x1048
    #undef CFG_SPI_FLASH
#endif
#ifdef CONFIG_IPMUX_ONU
    #define REG_CS0_BASE_ADDR ((((*(volatile u32 *)(0xBF000000))&0xF)==1)?0x3020:0x3040) 
    #define REG_CS0_SIZE    ((((*(volatile u32 *)(0xBF000000))&0xF)==1)?0x3048:0x3060)
#endif

 #ifdef CONFIG_IPMUX_2
     #define REG_CS0_BASE_ADDR 0x100
     #define REG_CS0_SIZE    0x104
 #endif

 #ifdef CONFIG_ONU_2
     #define REG_CS0_BASE_ADDR 0xa0040
     #define REG_CS0_SIZE    0xa0060
 #endif
/**************************************************************
*           System working mode                               *
**************************************************************/
/*BIG endian/little endian*/
#define BIGENDIAN 1     
#define ENABLE_CACHE 1
/*
#define LOCK_CACHE_SP 1
#define UNLOCK_CACHE_SP 1
*/
#ifndef CONFIG_IPMUX_E2_ASKEY
#define OPULAN_BOOT_MENU_MODE
#define OPULAN_BOOT_BANNER
#define CONFIG_BOOT_TRANSFER_PARA
#endif

#ifdef CONFIG_BOOT_TRANSFER_PARA
#define startParaAddr 0x80400000
#endif

#define CONFIG_OPIPMUX_SYNC
#ifdef CONFIG_OPIPMUX_SYNC
#define opipmux_sync() __asm__ __volatile__ ("sync");
#else
#define opipmux_sync()
#endif







/**************************************************************
*           ulities                                           *
**************************************************************/
#define record_time_def u32 opipmux_time_stamp=0;u32 opipmux_time_stamp_last=0
#define record_time opipmux_time_stamp=get_timer(0);opipmux_time_stamp_last=opipmux_time_stamp
#define print_record_time printf("  ~~~~~~TIME ELAPSE %d MS~~~~~~\n",get_timer_usec(opipmux_time_stamp)/1000)

#define period(msec) (get_timer_usec(opipmux_time_stamp_last)/1000)>msec?(opipmux_time_stamp_last=get_timer(0)):0
#define delay nop
#define delay10 nop;nop;nop;nop;nop;nop;nop;nop;nop;nop

/**************************************************************
*           config if build the test modules                  *
**************************************************************/
/*------early memory tst: just after cache inited------*/
#define TEST_MEM_IN_EARLY_INIT 1

/*Boot Ram Test*/
#define OPIPMUX_DRAM_TEST_START          0xa1500000
/*change from 0xa1b10000 to 0xa1b01000 to save boot time */
#define OPIPMUX_DRAM_TEST_END            0xa1530000

/*DDR Tune Ram Test*/
#define OPIPMUX_DDRTUNE_TEST_START          0xa1d00000
#define OPIPMUX_DDRTUNE_TEST_END            0xa1d00050

/*------prihpa tests------*/
/*
#define INCLUDE_PERIPHA_TEST    1
#define INCLUDE_TAB_TEST    1
#define INCLUDE_REG_TEST    1
#define INCLUDE_COUNTER 1
#define INCLUDE_SDRAM_COUNTER    1
#define INCLUDE_UART_TEST   1
#define INCLUDE_MEM_EVALUE_RW 1  (test mem rw speed)
#define DO_MEM_TEST      1
*/
#define INCLUDE_MEM_TEST      1
#define chipInit        la t5,0xBF000008;  li t7,0x1;  sw t7,0(t5);delay;delay;delay;delay;
#define linklinkEN      la t5,0xBF00000c;  li t7,0x1;  sw t7,0(t5)

#define OPULAN_IMAGE_HEADER

#endif  /* __OPIPMUX_H */

/**************************************************************
 *   mips register using                                      *
 *        s5 - keep ra before bal                             *
 *                                                            *
 *   memory usage                                             *
 *        0xa1000010-0xa100001c : log DDR delay value         *
 **************************************************************/

/**************************************************************
*           END                                               *
**************************************************************/

