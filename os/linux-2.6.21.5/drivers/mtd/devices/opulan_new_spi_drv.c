/**
*
*  COPYRIGHT (C) 2003-2030 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products.
* 
* @file opulan_new_spi_drv.c :Driver source file for SPI module
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
 * Source file changeLog:
 *   - 2010-01-20:add initial code according to respin register document v1.0
 */
/**
 * @defgroup SPI SPI module
 * @{
 */
/***** INCLUDE FILES *****/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/interrupt.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <asm-mips/delay.h>

#include <linux/platform_device.h>
#include <asm/semaphore.h>
#include <linux/spinlock.h>

#ifdef debug_spi
#define PL printf(" %d ", __LINE__);udelay(10000);
#else
#define PL
#endif

#define OPL_STATUS int
#define OPL_OK 0
#define OPL_ERROR -1
#define OPL_NULL ((void *) 0)
#define SPI_AGENT_READ_OFS 0x10
#define SECT_4K     0x01

#define SPI_FLASH_BASE_ADDR 0xb0000000

#define OPCODE_WREN     0x06    /* Write enable */
#define OPCODE_RDSR     0x05    /* Read status register */
#define OPCODE_READ     0x03    /* Read data bytes (low frequency) */
#define OPCODE_FAST_READ    0x0b    /* Read data bytes (high frequency) */
#define OPCODE_PP       0x02    /* Page program (up to 256 bytes) */
#define OPCODE_BE_4K        0x20    /* Erase 4K block */
#define OPCODE_BE_32K       0x52    /* Erase 32K block */
#define OPCODE_SE       0xd8    /* Sector erase (usually 64K) */


#define REG_BASE_ADDR                                           0xbf000000

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

#if 1
typedef struct spi_int_status{
    __u8 ready_buff_id ;
	__u8 spi_trans_error_int ;
	__u8 spi_trans_done_int ;
	__u8 spi_config_error_int ;
	__u8 spi_complete_int ;
}SPI_INT_STATUS;
#endif

struct opulan_spi_prv {
	struct spi_device   *spi;
	struct semaphore    lock;
	struct mtd_info     mtd;
	unsigned        partitioned:1;
	u8          erase_opcode;
	u8          command[4];
	spinlock_t spi_lock;
	struct flash_info *flinfo;
};

struct flash_info{
				  char	  *name;
				  u32	  jedec_id;
				  unsigned	  sector_size;
				  u16	  n_sectors;
				  u16	  flags;
				  };

static struct flash_info __devinitdata respin_spi_data [] = {

	{ "at26df321",  0x1f4700, 64 * 1024, 64, 0},
	{ "mx25",  0xc22016, 64 * 1024, 64, SECT_4K},
	{"kh25l", 0xc22016, 64*1024, 64, SECT_4K,},
	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{ "sst25vf040b", 0xbf258d, 64 * 1024, 8, SECT_4K, },
	{ "sst25vf080b", 0xbf258e, 64 * 1024, 16, SECT_4K, },
	{ "sst25vf016b", 0xbf2541, 64 * 1024, 32, SECT_4K, },
	{ "sst25vf032b", 0xbf254a, 64 * 1024, 64, SECT_4K, },	
	{ "m25px64", 0x207117, 64 * 1024, 128, 0, },
	{ "mx25l160", 0xc22016, 64 * 1024, 64, 0, }
};

#ifdef CONFIG_MTD_PARTITIONS
#define mtd_has_partitions()    (1)
#else
#define mtd_has_partitions()    (0)
#endif

SPI_INT_STATUS P_int_status;

/***** PUBLIC FUNCTIONS ****/

/**
*@par Description
* This function init spi master.
*
 * @retval OPL_OK  Spi init success.
*/

OPL_STATUS oplRegRead(__u32 regAddr,__u32 *regVal)
{
	if(OPL_NULL == regVal){
		return -1;
	}
	*regVal = *(volatile __u32 *)((__u32)REG_BASE_ADDR + regAddr*4);
	return OPL_OK;
}

OPL_STATUS oplRegWrite(__u32 regAddr,__u32 regVal)
{
	OPL_STATUS retVal = OPL_OK;
	*(volatile __u32 *)((__u32)REG_BASE_ADDR + regAddr*4) = regVal;
	return retVal;
}

OPL_STATUS oplRegFieldRead(__u32 regAddr,__u16 fieldOffset,__u16 fieldWidth,__u32 *data0)
{
	OPL_STATUS retVal;
	__u32 regVal;
	__u32 fieldMask;
	if(fieldWidth == 32){
		fieldMask = 0xFFFFFFFF;
	}else{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}
	retVal = oplRegRead(regAddr,&regVal);
	if(OPL_OK != retVal){
	return retVal;
	}	
	regVal = regVal&fieldMask;
	*data0 = regVal>>fieldOffset;
	return OPL_OK;	
}

OPL_STATUS oplRegFieldWrite(__u32 regAddr,__u16 fieldOffset,__u16 fieldWidth,__u32 data0)
{
	OPL_STATUS retVal;
	__u32 regVal;
	__u32 oldVal;
	__u32 fieldMask;
	if(fieldWidth == 32){	
		fieldMask = 0xFFFFFFFF;
	}else{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}
	retVal = oplRegRead(regAddr,&oldVal);
	if(OPL_OK != retVal){
		return retVal;
	}
    oldVal = oldVal&(~fieldMask);
	regVal = (data0<<fieldOffset)&fieldMask;
	regVal = regVal|oldVal;
	retVal = oplRegWrite(regAddr,regVal);
	return retVal;	
}	

OPL_STATUS spiFreqDivSet(__u32 div)
{
    OPL_STATUS retVal;
    retVal = oplRegFieldWrite(REG_SPI_FRQ_DIV, 0, 16, div );
    return retVal;
}

OPL_STATUS  spiTransLenSet (__u32 len) 
{
    OPL_STATUS retVal;
     retVal = oplRegFieldWrite(REG_SPI_LEN, 0, 16, len );
     return retVal;
}

OPL_STATUS  spiTransLenGet (__u32 *len) 
{
    OPL_STATUS retVal;
     retVal = oplRegFieldRead(REG_SPI_LEN, 0,16, len );
     return retVal;
}

OPL_STATUS spiIntEnable(void)
{
    OPL_STATUS retVal;
    retVal = oplRegFieldWrite(REG_SPI_CTL, 4, 1, 1 );
	printk("enable spi interrupt...\n");
    return retVal;
}

OPL_STATUS spiIntDisable(void)
{
    OPL_STATUS retVal;
    retVal = oplRegFieldWrite(REG_SPI_CTL, 4, 1, 0 );
	printk("disable spi interrupt...\n");
    return retVal;
}

OPL_STATUS  spiBufReadySet (__u32 buf_num) 
{
    OPL_STATUS retVal;
     retVal = oplRegFieldWrite(REG_SPI_BUF_READY, buf_num, 1, 1 );
     return retVal;
}

OPL_STATUS	spiIntStatusGet (void)
{
    OPL_STATUS retVal;
	OPL_STATUS tmpVal;
	retVal = oplRegRead(REG_SPI_INT,&tmpVal);
	printk("get spi int status : 0x%08x \n",tmpVal);
	P_int_status.ready_buff_id = (tmpVal&0x10)>>4 ;
	P_int_status.spi_trans_error_int = (tmpVal&0x08)>>3 ;
	P_int_status.spi_trans_done_int = (tmpVal&0x04)>>2 ;
	P_int_status.spi_config_error_int = (tmpVal&0x02)>>1 ;
	P_int_status.spi_complete_int = tmpVal&0x01 ;
	return retVal;
}

OPL_STATUS  spiReadyBufIdGet (__u32 *id) 
{
    OPL_STATUS retVal;
     retVal = oplRegFieldRead(REG_SPI_INT, 4, 1, id );
     return retVal;
}

OPL_STATUS spiAutoSelectChose(__u32 option)
{
    OPL_STATUS retVal;
    switch (option){
		case 0:
			printk("disable auto select... \n");
			break;
		case 1: 
			printk("enable auto select... \n");
			break;			
		default:
           printk("chu  cuo  la......... : valid option\n");
           return OPL_ERROR;		
		}
    retVal = oplRegFieldWrite(REG_SPI_CTL, 5, 1, option );
    return retVal;
}

OPL_STATUS spiSlaveSelect(__u32 cs)
{
    OPL_STATUS retVal;
	__u32 cs_val;
    switch (cs){
		case 0:
			printk("select slave 0 ... \n");
			cs_val = 0x1;
			break;
		case 1: 
			printk("select slave 1 ... \n");
		    cs_val = 0x2;
			break;	
		case 2: 
			printk("select slave 2 ... \n");
			cs_val = 0x4;
			break;	
		default:
           printk("chu  cuo  la......... : valid CS\n");
           return OPL_ERROR;		
		}
    retVal = oplRegFieldWrite(REG_SPI_SS, 0, 3, cs_val );
    return retVal;
}

OPL_STATUS  waitTransDone( void )
{
	OPL_STATUS retVal;
	__u32 timeout=50000;
	__u32 transdone=0;
    while (timeout--)
    {
        retVal = oplRegFieldRead(REG_SPI_INT, 2, 1, &transdone );
        if (transdone )
        {
           goto OUT;
        }
    }
	printk("chu  cuo  la......... : wait spi trans done timeout\n");
	return OPL_ERROR;
OUT:
    return retVal;
}

OPL_STATUS  waitBusy( void )
{
	OPL_STATUS retVal;
	__u32 timeout=500000;
	__u32 busy=1;
//	__u32 transdone=1;
    while (timeout--)
    {
        retVal = oplRegFieldRead(REG_SPI_BUSY, 1, 1, &busy );
        if (!busy )
        {
           goto OUT;
        }
    }
	printk("chu  cuo  la......... : wait spi busy timeout\n");
	return OPL_ERROR;
OUT:
    return retVal;
}

OPL_STATUS spiStartTrans(void)
{
    OPL_STATUS retVal;
	if(0 != waitBusy()){
	   printk("spi bus is busy...\n");
	   return OPL_ERROR;
	  }
	retVal = oplRegFieldWrite(REG_SPI_BUSY, 1, 1, 1);
	return retVal;		
}

OPL_STATUS spiTransDataFill( int buff_num, __u32 len, void *data )
{
    OPL_STATUS retVal;
#if 1
    if((buff_num!=0)&&(buff_num!=1)){
	     printk("invaild buffer number\n");
	     return OPL_ERROR;
    	}	
	switch (((len>>8)-1)/32){
		case 0:
			retVal = oplRegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, *(DATA32 *)data);
			break;
		case 1:
			retVal = oplRegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, ((DATA64 *)data)->data0);
			retVal = oplRegFieldWrite(REG_SPI_TX_(1+4*buff_num), 0, 32, ((DATA64 *)data)->data1);
			break;
		case 2:
			retVal = oplRegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, ((DATA96 *)data)->data0);
			retVal = oplRegFieldWrite(REG_SPI_TX_(1+4*buff_num), 0, 32, ((DATA96 *)data)->data1);
			retVal = oplRegFieldWrite(REG_SPI_TX_(2+4*buff_num), 0, 32, ((DATA96 *)data)->data2);
			break;
		case 3:
			retVal = oplRegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, ((DATA128 *)data)->data0);
			retVal = oplRegFieldWrite(REG_SPI_TX_(1+4*buff_num), 0, 32, ((DATA128 *)data)->data1);
			retVal = oplRegFieldWrite(REG_SPI_TX_(2+4*buff_num), 0, 32, ((DATA128 *)data)->data2);
			retVal = oplRegFieldWrite(REG_SPI_TX_(3+4*buff_num), 0, 32, ((DATA128 *)data)->data3);
			break;
		default:
			printk("invalid length...\n");
		}
#endif
	return retVal;
}

OPL_STATUS spiRcvDataGet( int buff_num, __u32 len,void *data )
{
    OPL_STATUS retVal;
#if 1
    if((buff_num!=0)&&(buff_num!=1)){
	     printk("invaild buffer number\n");
	     return OPL_ERROR;
    	}
	switch (((len>>8)-1)/32){
		case 0:
			retVal = oplRegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, (DATA32 *)data);
			break;
		case 1:
			retVal = oplRegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, &((DATA64 *)data)->data0);
			retVal = oplRegFieldRead(REG_SPI_RX_(1+4*buff_num), 0, 32, &((DATA64 *)data)->data1);
			break;
		case 2:
			retVal = oplRegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, &((DATA96 *)data)->data0);
			retVal = oplRegFieldRead(REG_SPI_RX_(1+4*buff_num), 0, 32, &((DATA96 *)data)->data1);
			retVal = oplRegFieldRead(REG_SPI_RX_(2+4*buff_num), 0, 32, &((DATA96 *)data)->data2);
			break;
		case 3:
			retVal = oplRegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, &((DATA128 *)data)->data0);
			retVal = oplRegFieldRead(REG_SPI_RX_(1+4*buff_num), 0, 32, &((DATA128 *)data)->data1);
			retVal = oplRegFieldRead(REG_SPI_RX_(2+4*buff_num), 0, 32, &((DATA128 *)data)->data2);
			retVal = oplRegFieldRead(REG_SPI_RX_(3+4*buff_num), 0, 32, &((DATA128 *)data)->data3);
			break;
		}
#endif
	return retVal;
}

OPL_STATUS spi_freq_set( unsigned int div)
{
    OPL_STATUS retVal;
	PL
 	//retVal = oplRegFieldWrite(REG_SPI_CTL, 2, 1, 1 );
//	*(volatile unsigned int*)(0xbf00e0c8) = 0x24;
	retVal = spiFreqDivSet(div);
	PL
//	printf("set spi  clock 1/4 of sysclk\n",;
	if(OPL_OK != retVal)
	{
		return retVal;
	}
//	retVal = spiTransLenSet(0x8080);

         spi_write_status(0x0);
//	spi_read_man();
	return retVal;
}

/**
*@par Description
* This function read spi flash jedec ID.
*
 * @retval  jedec ID of the spi flash.
*/

OPL_STATUS spi_read_man(void)
{
	OPL_STATUS retVal;
    DATA128 cmd ,tmp_data;
	__u32 len;
	__u32 ready_buf_id = 0;

	retVal = spiTransLenSet(0x8080);
	PL
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	PL
	/*set cmd for spi flash*/
	(&cmd)->data0 =0 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =0x9f000000 ;
	PL
    /*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	PL
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	PL

	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	PL
	/*start spi trans*/
	retVal = spiStartTrans();
	PL
	/*wait for spi tx done*/
	retVal = waitBusy();
	PL
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
	PL
	/*get data from rx buffer*/
	retVal = spiRcvDataGet(ready_buf_id,len,&tmp_data);
	PL
//	printk("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	printk("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	printk("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	printk("debug :read back data0  = 0x%x\n",(tmp_data.data0));	
	retVal=tmp_data.data3;
       printk("read spi flash manufacture ID = 0x%x\n",retVal);
    udelay(100000);
	return retVal;
}

/**
*@par Description
* This function read spi flash status.
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spi_read_status(__u32 *status)
{
	OPL_STATUS retVal;
	DATA128 cmd ,tmp_data;
	__u32 len;
	__u32 ready_buf_id = 0;

	retVal = spiTransLenSet(0x1010);	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	/*set cmd for spi flash*/
	(&cmd)->data0 =0x0500 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =0x05000000 ;
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	/*start spi trans*/
	retVal = spiStartTrans();
	/*wait for spi tx done*/
	retVal = waitBusy();
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
	/*get data from rx buffer*/
	retVal = spiRcvDataGet(ready_buf_id,len,&tmp_data);	
	/*just for debug*/
//	printk("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	printk("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	printk("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	printk("debug :read back data0  = 0x%x\n",(tmp_data.data0));
	*status = (tmp_data.data0<<8)>>8 ;
//	printk("read spi flash status  = 0x%x\n",tmp_data.data0);
	return retVal;
}

/**
*@par Description
* This function write spi flash status.
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spi_write_status(__u32 status)
{
	OPL_STATUS retVal;
    DATA128 cmd ;
	__u32 len;
	__u32 ready_buf_id = 0;
	__u32 delay = 0x100000;

	retVal = spiTransLenSet(0x8080);	
	/*get spi valid len */
	retVal = spiTransLenGet(&len);	
	/*set cmd for spi flash*/
	(&cmd)->data0 =0 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =(0x01000000|(( status & 0xf )<<16)) ;
    /*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	/*start spi trans*/
	retVal = spiStartTrans();
	/*wait for spi tx done*/
	retVal = waitBusy();
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
  
    while(delay--);
	/*just for debug*/
//	printk("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	printk("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	printk("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	printk("debug :read back data0  = 0x%x\n",(tmp_data.data0));	
	return retVal;
}

/**
*@par Description
* This function enable spi flash writing.
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiWriteEnable(void)
{
	OPL_STATUS retVal;
	DATA128 cmd ;
	__u32 len;
	__u32 ready_buf_id = 0;
	
	retVal = spiTransLenSet(0x0808);
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	/*set cmd for spi flash*/
	(&cmd)->data0 =0x06 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =0x06000000 ;
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	/*set spi buffer  ready*/
	retVal = spiBufReadySet(ready_buf_id);
	/*start spi trans*/
	retVal = spiStartTrans();
	/*wait for spi tx done*/
	retVal = waitBusy();
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
	/*just for debug*/	
//	printk("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	printk("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	printk("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	printk("debug :read back data0  = 0x%x\n",(tmp_data.data0));
//  spiWriteStatus(0x9c);
//  printk("enable spi flash write...\n");
	return retVal;
}

/**
*@par Description
* This function disable spi flash writing.
*
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spiWriteDisable(void)
{
	OPL_STATUS retVal;
//	OPL_STATUS regVal = 0;
	DATA128 cmd ;
	__u32 len;
	__u32 ready_buf_id = 0;
	
	retVal = spiTransLenSet(0x0808);
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
	/*set cmd for spi flash*/
	(&cmd)->data0 =0x04 ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =0x04000000 ;
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);	
	/*set spi buffer  ready*/
	retVal = spiBufReadySet(ready_buf_id);
	/*start spi trans*/
	retVal = spiStartTrans();
	/*wait for spi tx done*/
	retVal = waitBusy();
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
	/*just for debug*/
//	printk("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	printk("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	printk("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	printk("debug :read back data0  = 0x%x\n",(tmp_data.data0));
         printk("disable spi flash write...\n");
	return retVal;

}

/**
*@par Description
* This function erase a sector of spi flash .
*
* @param [in] addr_ofs
*	   address offset of spi flash.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spi_erase_one_sector(__u32 sec_num)
{
	OPL_STATUS retVal;
	DATA128 cmd;
	__u32 len;
	__u32 ready_buf_id = 0;
	__u32 flash_status;
	__u32 timeout=5000000;	

    /*enable spi flash write*/
	retVal = spiWriteEnable();

	retVal = spiTransLenSet(0x2020);
	/*get spi valid len */
	retVal = spiTransLenGet(&len);		
	/*set cmd for spi flash*/
         (&cmd)->data0 =(0xd8000000 | (sec_num*0x10000)) ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =(0xd8000000 | (sec_num*0x10000)) ;
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	/*start spi trans*/
	retVal = spiStartTrans();
	/*wait for spi tx done*/
	retVal = waitBusy();
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
//	printk("start erase spi flash block %d...",addr_ofs);	
    while (timeout--)
    {
        retVal = spi_read_status(&flash_status);
//		printk("flash_status = 0x%x \n",flash_status);
		if(!(timeout%100))
		{
		//	printk(".");
		}
        if (!(flash_status&0x3))
        {
           goto OUT;
        }
    }
	printk("\n chu  cuo  la......... : erase spi flash timeout\n");
	return OPL_ERROR;
	
OUT:
//	printk("done\n");

	return retVal;

}

OPL_STATUS spiByteProgram(__u8 vchar,__u32 addr_ofs)
{
	OPL_STATUS retVal;
	DATA128 cmd;
	__u32 len;
	__u32 ready_buf_id = 0;
	__u32 flash_status;
	__u32 timeout=5000000;	
	/*enable spi flash write*/
	retVal = spiWriteEnable();

	retVal = spiTransLenSet(0x2828);
	/*get spi valid len */
	retVal = spiTransLenGet(&len);	
//	printk("*vchar = 0x%x\n",vchar);
//	printk("*addr_ofs = 0x%x\n",addr_ofs);
	(&cmd)->data0 = (addr_ofs<<8)|(vchar);
	(&cmd)->data1 = 0x02 ;
	(&cmd)->data2 = 0 ;
	(&cmd)->data3 = 0 ;
	/*get spi ready buffer id*/
	retVal = spiReadyBufIdGet(&ready_buf_id);	
	/*write data to spi buffer */
	retVal = spiTransDataFill(ready_buf_id,len,&cmd);
	/*set spi buffer 0 ready*/
	retVal = spiBufReadySet(ready_buf_id);
	/*start spi trans*/
	retVal = spiStartTrans();
	/*wait for spi tx done*/
	retVal = waitBusy();
	/*wait spi trans(rx)  finished*/
	retVal = waitTransDone();
		
	while (timeout--)
	{
		retVal = spi_read_status(&flash_status);

        if (!(flash_status&0x3))
        {
           goto OUT;
        }

	}
	printk("\n chu	cuo  la......... : write to spi flash timeout\n");
	return OPL_ERROR;
	OUT:
//	printk("done\n");	
	return retVal;

}

static inline struct opulan_spi_prv *mtd_to_respin_spi(struct mtd_info *mtd)
{
	return container_of(mtd, struct opulan_spi_prv, mtd);
}

/*
 * Erase an address range on the flash chip.  The address range may extend
 * one or more erase sectors.  Return an error is there is a problem erasing.
 */
static int respin_spi_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct opulan_spi_prv *flash = mtd_to_respin_spi(mtd);

	u32 addr,len;
#ifdef debug_spi
	printk("%s: %s %s 0x%08x, len %d\n",
			flash->spi->dev.bus_id, __FUNCTION__, "at",
			(u32)instr->addr, instr->len);
#endif
	/* sanity checks */
	if (instr->addr + instr->len > flash->mtd.size)
		{
		printk("error: out of flash memry...\n");
		return -EINVAL;
		}
	if ((instr->addr % mtd->erasesize) != 0
			|| (instr->len % mtd->erasesize) != 0) {
		printk("error:erase not on sector band...\n");
		return -EINVAL;
	}

	addr = instr->addr;
	len = instr->len;

	down(&flash->lock);

	/* REVISIT in some cases we could speed up erasing large regions
	 * by using OPCODE_SE instead of OPCODE_BE_4K
	 */
        spi_write_status(0x0);
	/* now erase those sectors */
	while (len) {
#ifdef debug_spi	
		printk("erase addr 0x%x, code : flash->erase_opcode 0x%x.\n", addr  , flash->erase_opcode);
                  printk("start to erase sector number = 0x%x \n",(addr&0xffff0000)/0x10000);
//		bdump(0xb0000000, 10);
#else	
                  if ((addr&0xffff0000)/0x10000 == 0) {
		         printk("error: deadly erase operation, the first sector  was to be erased!!!\n");	
			return  EPERM;
                  	}
				  
		if (spi_erase_one_sector((addr&0xffff0000)/0x10000)) {
			instr->state = MTD_ERASE_FAILED;
			up(&flash->lock);
			return -EIO;
		}
#endif		
#if 0		
		if (addr == (flash->mtd.size - (flash->flinfo)->sector_size){
                    printk("waring: this is the last sector of our flash,addr = 0x%x\n.",addr);	
		}
#endif		
		//bdump(0xbfc00000, 10);

		addr += mtd->erasesize;
		len -= mtd->erasesize;
		
		
	}

	up(&flash->lock);

	instr->state = MTD_ERASE_DONE;
#if 0	
	mtd_erase_callback(instr);
#endif
	return 0;
}

/*
 * Read an address range from the flash chip.  The address range
 * may be any size provided it is within the physical boundaries.
 */
static int respin_spi_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
         struct opulan_spi_prv *flash = mtd_to_respin_spi(mtd);

	*retlen = len;

	down(&flash->lock);
#ifdef debug_spi
	printk("spi cfg param: 0x%x   0x%x  0x%x  0x%x  0x%x\n", \
		    *(unsigned int*)0xbf000c80, \
		    *(unsigned int*)0xbf000c84, \
		    *(unsigned int*)0xbf000c88, \
		    *(unsigned int*)0xbf000c8c, \
		    *(unsigned int*)0xbf000c90);
	 udelay(5000);
#endif
         spi_write_status(0x0);

//     printk("addr 0x%x : 0x%x\n",(unsigned int)from + SPI_FLASH_BASE_ADDR, \
//       *(unsigned volatile int*)((unsigned int)from + SPI_FLASH_BASE_ADDR));
	memcpy(buf, (char *)((unsigned int)from + SPI_FLASH_BASE_ADDR), len);      
//	udelay(5000);
	up(&flash->lock);
	return 0;
}

/*
 * Write an address range to the flash chip.  Data must be written in
 * FLASH_PAGESIZE chunks.  The address range may be any size provided
 * it is within the physical boundaries.
 */
 
static int respin_spi_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	unsigned int i;
	struct opulan_spi_prv *flash = mtd_to_respin_spi(mtd);
	*retlen = len;
//	printk("\nspi write %d (hex 0x%x) bytes to 0x%x.\n",len,len,to+SPI_AGENT_READ_OFS);
	down(&flash->lock);
        spi_freq_set(1);
	for (i=0; i<len; i++) {
		spiByteProgram(buf[i],to + i +SPI_AGENT_READ_OFS);
	}
	up(&flash->lock);
	return 0;
}

static int __init respin_spi_probe(struct platform_device *plat_dev)
{
	struct flash_platform_data *data = plat_dev->dev.platform_data;
	struct opulan_spi_prv   *flash;
	unsigned  int    i;
	struct spi_device spi_dev;
	struct spi_device *spi = &spi_dev;
	spi->dev = plat_dev->dev;
	/*tbd : Got FS_OFFSET from device */
	//plat_dev->dev.platform_data.flash_platform_data.parts[0].offset
	//printk("@@@@ Dev name : %s, dev : 0x%p 0x%p.\n", plat_dev->name, &(plat_dev->dev), spi->dev.platform_data);
	/* Platform data helps sort out which chip type we have, as
	 * well as how this board partitions it.  If we don't have
	 * a chip ID, try the JEDEC id commands; they'll work for most
	 * newer chips, even if we don't recognize the particular chip.
	 */ 
	printk("respin_spi_probe  **********************************\n");
	flash = kzalloc(sizeof *flash, GFP_KERNEL);
	if (!flash){
		return -ENOMEM;
		}

	if (data && data->type) {
		for (i = 0, flash->flinfo = respin_spi_data;
				i < ARRAY_SIZE(respin_spi_data);
				i++, (flash->flinfo)++) {
			// debugKernelSpi("data->type 0x%s, info->name 0x%s.\n", data->type, info->name);
			if ((strcmp(data->type, (flash->flinfo)->name) == 0)||((flash->flinfo)->jedec_id==(spi_read_man())))  break;
		    }
		}
	/* unrecognized chip? */
	if (i == ARRAY_SIZE(respin_spi_data)) {
	#if 0	
		DEBUG(MTD_DEBUG_LEVEL0, "%s: unrecognized id %s\n",
					spi->dev.bus_id, data->type);
		flash->flinfo = NULL;
	#endif	
	 printk("%s: unrecognized id %s\n",
					spi->dev.bus_id, data->type);
	 kfree(flash);
	 return -ENODEV;
	} 
    /*unmatched flash*/
	 printk("probe flash type...\n");
	if((flash->flinfo)->jedec_id != spi_read_man())
	{
		printk("warning:the flash may be not match...\n");
	//	flash->flinfo = NULL;		
	//	return -ENODEV;
	}
	//flash->spi = spi;
	init_MUTEX(&flash->lock);
	dev_set_drvdata(&spi->dev, flash);

	if (data && data->name)
	   flash->mtd.name = data->name;
	else
	   flash->mtd.name = spi->dev.bus_id;

    flash->mtd.type = MTD_NORFLASH;
	flash->mtd.writesize = 1;
	/* writeable */
	flash->mtd.flags = MTD_CAP_NORFLASH | MTD_WRITEABLE;
	flash->mtd.size = (flash->flinfo)->sector_size * (flash->flinfo)->n_sectors;

	flash->mtd.erase = respin_spi_erase;
	flash->mtd.read = respin_spi_read;
	flash->mtd.write = respin_spi_write;

	/* prefer "small sector" erase if possible */
	if ((flash->flinfo)->flags & SECT_4K) {
		flash->erase_opcode = OPCODE_BE_4K;
		flash->mtd.erasesize = 4096;
	} else {
		flash->erase_opcode = OPCODE_SE;
		flash->mtd.erasesize = (flash->flinfo)->sector_size;
	}

	dev_info(&spi->dev, "%s (%d Kbytes)\n", (flash->flinfo)->name,
			flash->mtd.size / 1024);
 
	DEBUG(MTD_DEBUG_LEVEL2,
			"mtd .name = %s, .size = 0x%.8x (%uM) "
			 ".erasesize = 0x%.8x (%uK) .numeraseregions = %d\n",
			flash->mtd.name,
			flash->mtd.size, flash->mtd.size / (1024*1024),
			flash->mtd.erasesize, flash->mtd.erasesize / 1024,
			flash->mtd.numeraseregions);

	if (flash->mtd.numeraseregions)
		for (i = 0; i < flash->mtd.numeraseregions; i++)
			DEBUG(MTD_DEBUG_LEVEL2,
					"mtd.eraseregions[%d] = { .offset = 0x%.8x, "
					".erasesize = 0x%.8x (%uK), "
					".numblocks = %d }\n",
					i, flash->mtd.eraseregions[i].offset,
					flash->mtd.eraseregions[i].erasesize,
					flash->mtd.eraseregions[i].erasesize / 1024,
					flash->mtd.eraseregions[i].numblocks);

	   /* partitions should match sector boundaries; and it may be good to
	    * use readonly partitions for writeprotected sectors (BP2..BP0).
	    */
	if (mtd_has_partitions()) {
		struct mtd_partition    *parts = NULL;
		int         nr_parts = 0;

#ifdef CONFIG_MTD_CMDLINE_PARTS
		static const char *part_probes[] = { "cmdlinepart", NULL, };
		printk("CONFIG_MTD_CMDLINE_PARTS\n");
		nr_parts = parse_mtd_partitions(&flash->mtd,
				part_probes, &parts, 0);
#endif
		if (nr_parts <= 0 && data && data->parts) {
			parts = data->parts;
			nr_parts = data->nr_parts;
		}

		if (nr_parts > 0) {
			for (i = 0; i < nr_parts; i++) {
				printk( "partitions[%d] = "	    
					    "{.name = %s, .offset = 0x%.8x, "
						".size = 0x%.8x (%uK) }\n",
						i, parts[i].name,
						parts[i].offset,
						parts[i].size,
						parts[i].size / 1024);
			   }
			flash->partitioned = 1;
			return add_mtd_partitions(&flash->mtd, parts, nr_parts);
		}
	} else if (data->nr_parts)
		printk(&spi->dev, "ignoring %d default partitions on %s\n",
				data->nr_parts, data->name);
         printk("flash probe finished.\n");
	return add_mtd_device(&flash->mtd) == 1 ? -ENODEV : 0;
}


static int __exit respin_spi_remove(struct spi_device *spi)
{
	struct opulan_spi_prv *flash = dev_get_drvdata(&spi->dev);
	int     status;

	/* Clean up MTD stuff. */
	if (mtd_has_partitions() && flash->partitioned)
		status = del_mtd_partitions(&flash->mtd);
	else
		status = del_mtd_device(&flash->mtd);
	if (status == 0)
		kfree(flash);
	return 0;
}

static struct platform_driver respin_spi_drv = {
	.probe      = respin_spi_probe,
	.remove     = __devexit_p(respin_spi_remove),
	.driver     = {
		.name   = "respin-spi-flash",
		.owner  = THIS_MODULE,
	},
};

int respin_spi_drv_init(void)
{
	return platform_driver_register(&respin_spi_drv);
}

static void respin_spi_drv_exit(void)
{
	platform_driver_unregister(&respin_spi_drv);
}

module_init(respin_spi_drv_init);
module_exit(respin_spi_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("stone.zhang");
MODULE_DESCRIPTION("MTD SPI driver for spi flash chips on respin board");

