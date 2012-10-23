/**
*
*  COPYRIGHT (C) 2003-2020 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products.
*
* @file spi.h
*
* DESCRIPTION:
*
* Date Created:
*
* Authors(optional): 
*
* Reviewed by (optional):
*
* Edit History:
*
*/

/**
 * Header file changeLog:
 *   - 2010-01-20:add initial code according to ONU2 register document v1.0
 */
/**
 * @defgroup SPI SPI module
 * @{
 */

 //#if defined(CFG_ONU2_SPI_FLASH)

#ifndef SPI_H
#define SPI_H

/***** INCLUDE FILES *****/

/***** DEFINES and ENUMS *****/
#define SPI_CMD_READ_MODE_1          0x03
#define SPI_CMD_READ_MODE_2          0x0B
#define SPI_CMD_READ_MODE_3          0x0D
#define SPI_CMD_READ_MODE_4          0x1D
#define SPI_CMD_WRITE_MODE_1         0xA3
#define SPI_CMD_WRITE_MODE_2         0xAB
#define SPI_CMD_WRITE_MODE_3         0xAD
#define SPI_CMD_WRITE_MODE_4         0xBD
#define SPI_CMD_WRITE_MODE_5         0xCD
#define SPI_CMD_WRITE_MODE_6         0xFD

#define SPI_CTRL_RESET_OFFSET    31
#define SPI_CTRL_CMD_OFFSET      27
#define SPI_CTRL_READ_OK_OFFSET  26
#define SPI_CTRL_WRITE_OK_OFFSET 25

 #define NORMAL_WRITE 0xa3
 #define NORMAL_READ  0x03
 #define DUALADDR_2IO_WRITE  0xfd
 #define SINGADDR_2IO_WRITE  0xcd

#define OPL_STATUS int

#define regBaseAddr 0xbf000000

#if defined(CONFIG_OPTRAN)

#define SPI_BASE_ADDR                                           0x3800

#define REG_SPI_TX_(n)                                          (SPI_BASE_ADDR + 0X0020 + n)
#define REG_SPI_RX_(n)                                          (SPI_BASE_ADDR + 0X0028 + n)
#define REG_SPI_FRQ_DIV                                         (SPI_BASE_ADDR + 0X0030)
#define REG_SPI_LEN                                             (SPI_BASE_ADDR + 0X0031)
#define REG_SPI_CTL                                             (SPI_BASE_ADDR + 0X0032)
#define REG_SPI_SS                                              (SPI_BASE_ADDR + 0X0033)
#define REG_SPI_BUSY                                            (SPI_BASE_ADDR + 0X0034)
#define REG_SPI_INT                                             (SPI_BASE_ADDR + 0X0000)
#define REG_SPI_CS_DELAY                                        (SPI_BASE_ADDR + 0X0035)
#define REG_SPI_BUF_READY                                       (SPI_BASE_ADDR + 0X0036)
#define REG_SPI_INT_ENABLE                                      (SPI_BASE_ADDR + 0X0004)
#define REG_SPI_MASTER_BOOT_SCK_RATE                            (SPI_BASE_ADDR + 0X0037)
#define REG_SPI_MASTER_BOOT_RD_CMD                              (SPI_BASE_ADDR + 0X0038)
#define REG_SPI_MASTER_BOOT_SUB_MODE                            (SPI_BASE_ADDR + 0X0039)
#define REG_SPI_MASTER_BOOT_DUMMY_CYL                           (SPI_BASE_ADDR + 0X003a)
#define REG_SPI_MASTER_BOOT_OFFSET                              (SPI_BASE_ADDR + 0X003b)

//#elif defined(CONFIG_ONU_1_1_C13_SPI)||defined(CONFIG_ONU_1_1_C13_SPI_DDR_CLK_300)
#elif defined(ONU1_1_SPI_REG)
#define SPI_BASE_ADDR                                           0x300

#define REG_SPI_TX_(n)                                          (SPI_BASE_ADDR + 0X0000 + n)
#define REG_SPI_RX_(n)                                          (SPI_BASE_ADDR + 0X0008 + n)
#define REG_SPI_FRQ_DIV                                         (SPI_BASE_ADDR + 0X0010)
#define REG_SPI_LEN                                             (SPI_BASE_ADDR + 0X0011)
#define REG_SPI_CTL                                             (SPI_BASE_ADDR + 0X0012)
#define REG_SPI_SS                                              (SPI_BASE_ADDR + 0X0013)
#define REG_SPI_BUSY                                            (SPI_BASE_ADDR + 0X0014)
#define REG_SPI_INT                                             (SPI_BASE_ADDR + 0X0015)
#define REG_SPI_CS_DELAY                                        (SPI_BASE_ADDR + 0X0016)
#define REG_SPI_BUF_READY                                       (SPI_BASE_ADDR + 0X0017)
#define REG_SPI_INT_ENABLE                                      (SPI_BASE_ADDR + 0X0018)
#define REG_SPI_MASTER_BOOT_SCK_RATE                            (SPI_BASE_ADDR + 0X0020)
#define REG_SPI_MASTER_BOOT_RD_CMD                              (SPI_BASE_ADDR + 0X0021)
#define REG_SPI_MASTER_BOOT_SUB_MODE                            (SPI_BASE_ADDR + 0X0022)
#define REG_SPI_MASTER_BOOT_DUMMY_CYL                           (SPI_BASE_ADDR + 0X0023)
#define REG_SPI_MASTER_BOOT_OFFSET                              (SPI_BASE_ADDR + 0X0024)
#define REG_SPI_SLAVE_ADDR                                      (SPI_BASE_ADDR + 0x0050)
#define REG_SPI_SLAVE_CTRL                                      (SPI_BASE_ADDR + 0x0051)
#define REG_SPI_SLAVE_DATA                                      (SPI_BASE_ADDR + 0x0052)
#define REG_SPI_SLAVE_SRAM_BUF_START                            (SPI_BASE_ADDR + 0x0053)
#define REG_SPI_SLAVE_SRAM_PAGE_SIZE                            (SPI_BASE_ADDR + 0x0054)



#endif

#define  	FLASH_UNKNOWN  	0xFFFF
#define 	FLASH_MAN_ATMEL	0x1f
#define 	FLASH_AT26_ATMEL	0x47
#define  FLASH_MAN_MX 0xc2
#define  FLASH_MX_L320 0x20
#define  FLASH_MAN_WB 0xef
#define  FLASH_WB_25X16 0x30
#define  FLASH_MAN_SST 0xbf
#define  FLASH_MAN_ST 0x20
#define  FLASH_SST_25VF 0x25

#define SECTOR_SIZE 0x10000
#define BOOTLOADER_ADDR 0xbfc00000

#define BOOTLOADER_BEGIN 0
#ifdef CONFIG_IPMUX_ATHEROS
#define BOOTLOADER_SECTOR 4
#else
#define BOOTLOADER_SECTOR 3
#endif

#define SPI_AGENT_READ_OFS 0x10


#define ENV_BEGIN 3
#define ENV_SECTOR 1
/***** STRUCTURES and UNIONS *****/

/***** TYPEDEFS *****/
typedef  __u32 DATA32;
typedef struct data64
{
	__u32 data0;	
    __u32 data1;
}DATA64;

typedef struct data96
{
	__u32 data0;	
    __u32 data1;
	__u32 data2;
}DATA96;

typedef struct data128
{
	__u32 data0;	
    __u32 data1;
	__u32 data2;
	__u32 data3;
}DATA128;

typedef struct spi_int_status{
    __u8 ready_buff_id ;
	__u8 spi_trans_error_int ;
	__u8 spi_trans_done_int ;
	__u8 spi_config_error_int ;
	__u8 spi_complete_int ;
}SPI_INT_STATUS;

/***** EXTERNS *****/
OPL_STATUS spiInit(void);
OPL_STATUS spiFreqDivSet(__u32 div);
OPL_STATUS spiTransLenSet (__u32 len) ;
OPL_STATUS  spiTransLenGet (__u32 *len) ;
OPL_STATUS spiIntEnable(void);
OPL_STATUS spiIntDisable(void);
OPL_STATUS  spiBufReadySet (__u32 buf_num) ;
OPL_STATUS spiAutoSelectChose(__u32 option);
OPL_STATUS spiSlaveSelect(__u32 cs);
OPL_STATUS waitTransDone( void );
OPL_STATUS  waitBusy( void );
OPL_STATUS spiStartTrans(void);
OPL_STATUS spiTransDataFill( int buff_num, __u32 len, void *data );
OPL_STATUS spiRcvDataGet( int buff_num, __u32 len,void *data );

unsigned long spi_read_one_word(unsigned long addr_ofs);
OPL_STATUS spi_read_status(__u32 *status);
OPL_STATUS spi_write_status(__u32 status);
OPL_STATUS spiWriteEnable(void);
OPL_STATUS spiWriteDisable(void);
OPL_STATUS spi_erase_one_sector(__u32 addr_ofs);
OPL_STATUS spi_write_one_word(__u32 *value,__u32 addr_ofs);
OPL_STATUS spiWordProgram(__u32 *value,__u32 addr_ofs);
OPL_STATUS sst_write_one_word(__u32 *value,__u32 addr_ofs);
//OPL_STATUS sstWordProgram(__u32 *value,__u32 addr_ofs);

//OPL_STATUS spiProgramUb(void);
//OPL_STATUS spiStatusShow();
OPL_STATUS spi_read_man(void);

/***** PROTOTYPES *****/

#endif /* SPI_H */
/** 
 *@} 
 */
/* END FILE */

//#endif
