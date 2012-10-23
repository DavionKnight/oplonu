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
* @file spi.c Driver source file for SPI module
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
 *   - 2010-01-20:add initial code according to ONU2 register document v1.0
 */
/**
 * @defgroup SPI SPI module
 * @{
 */
/***** INCLUDE FILES *****/

#include <common.h>
#include <configs/opipmux.h>
#include "asm-mips/types.h"

#if defined(CONFIG_SPI_NJZTE)
#include "onu_respin_spi.h"

#ifdef debug_spi
#define PL printf(" %d ", __LINE__);udelay(10000);
#else
#define PL
#endif

//#if defined(CFG_ONU2_SPI_FLASH)
SPI_INT_STATUS P_int_status;

/***** LOCAL TYPEDEFS ****/

/***** LOCAL (PRIVATE) PROTOTYPES *****/

flash_info_t flash_info[CFG_MAX_FLASH_BANKS];

UINT32 CS0_REDEFINED = (PHYS_FLASH_1);
/***** DATA ALLOCATION *****/

/***** PUBLIC FUNCTIONS ****/

/**
*@par Description
* This function init spi master.
*
 * @retval OPL_OK  Spi init success.
*/


OPL_STATUS spiInit(void)
{
    OPL_STATUS retVal;

	PL
 	//retVal = RegFieldWrite(REG_SPI_CTL, 2, 1, 1 );
 //	*(volatile unsigned int*)(0xbf00e0c8) = 0x24;
 	retVal = oplRegWrite(REG_SPI_CTL, 0x24); //This step is most importment
	PL
	retVal = spiFreqDivSet(3);
	PL
//	printf("set spi  clock 1/4 of sysclk\n",;
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	retVal = spiSlaveSelect(0);
	PL
//	retVal = spiTransLenSet(0x8080);
	PL
    spi_write_status(0x0);
	PL
//	spi_read_man();
	PL

	return retVal;

}


OPL_STATUS spiFreqDivSet(__u32 div)
{
    OPL_STATUS retVal;
    retVal = RegFieldWrite(REG_SPI_FRQ_DIV, 0, 16, div );
    return retVal;
}
OPL_STATUS  spiTransLenSet (__u32 len)
{
    OPL_STATUS retVal;
     retVal = RegFieldWrite(REG_SPI_LEN, 0, 16, len );
     return retVal;
}
OPL_STATUS  spiTransLenGet (__u32 *len)
{
    OPL_STATUS retVal;
     retVal = RegFieldRead(REG_SPI_LEN, 0,16, len );
     return retVal;
}
OPL_STATUS spiIntEnable(void)
{
    OPL_STATUS retVal;
    retVal = RegFieldWrite(REG_SPI_CTL, 4, 1, 1 );
	printf("enable spi interrupt...\n");
    return retVal;
}
OPL_STATUS spiIntDisable(void)
{
    OPL_STATUS retVal;
    retVal = RegFieldWrite(REG_SPI_CTL, 4, 1, 0 );
	printf("disable spi interrupt...\n");
    return retVal;
}
OPL_STATUS  spiBufReadySet (__u32 buf_num)
{
    OPL_STATUS retVal;
     retVal = RegFieldWrite(REG_SPI_BUF_READY, buf_num, 1, 1 );
     return retVal;
}
OPL_STATUS	spiIntStatusGet (void)
{
    OPL_STATUS retVal;
	OPL_STATUS tmpVal;
	retVal = oplRegRead(REG_SPI_INT,&tmpVal);
	printf("get spi int status : 0x%08x \n",tmpVal);
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
     retVal = RegFieldRead(REG_SPI_INT, 4, 1, id );
     return retVal;
}
OPL_STATUS spiAutoSelectChose(__u32 option)
{
    OPL_STATUS retVal;
    switch (option){
		case 0:
			printf("disable auto select... \n");
			break;
		case 1:
			printf("enable auto select... \n");
			break;
		default:
           printf("chu  cuo  la......... : valid option\n");
           return OPL_ERROR;
		}
    retVal = RegFieldWrite(REG_SPI_CTL, 5, 1, option );
    return retVal;
}
OPL_STATUS spiSlaveSelect(__u32 cs)
{
    OPL_STATUS retVal;
	__u32 cs_val;
    switch (cs){
		case 0:
			printf("select slave 0 ... \n");
			cs_val = 0x1;
			break;
		case 1:
			printf("select slave 1 ... \n");
		    cs_val = 0x2;
			break;
		case 2:
			printf("select slave 2 ... \n");
			cs_val = 0x4;
			break;
		default:
           printf("chu  cuo  la......... : valid CS\n");
           return OPL_ERROR;
		}
    retVal = RegFieldWrite(REG_SPI_SS, 0, 3, cs_val );
    return retVal;
}
OPL_STATUS  waitTransDone( void )
{
	OPL_STATUS retVal;
	UINT32 timeout=50000;
	UINT32 transdone=0;
    while (timeout--)
    {
        retVal = RegFieldRead(REG_SPI_INT, 2, 1, &transdone );
        if (transdone )
        {
           goto OUT;
        }
    }
	printf("chu  cuo  la......... : wait spi trans done timeout\n");
	return OPL_ERROR;
OUT:
    return retVal;
}
OPL_STATUS  waitBusy( void )
{
	OPL_STATUS retVal;
	UINT32 timeout=500000;
	UINT32 busy=1;
	UINT32 transdone=1;
    while (timeout--)
    {
        retVal = RegFieldRead(REG_SPI_BUSY, 1, 1, &busy );
        if (!busy )
        {
           goto OUT;
        }
    }
	printf("chu  cuo  la......... : wait spi busy timeout\n");
	return OPL_ERROR;
OUT:
    return retVal;
}
OPL_STATUS spiStartTrans(void)
{
    OPL_STATUS retVal;
	if(0 != waitBusy()){
	   printf("spi bus is busy...\n");
	   return OPL_ERROR;
	  }
	retVal = RegFieldWrite(REG_SPI_BUSY, 1, 1, 1);
	return retVal;
}
OPL_STATUS spiTransDataFill( int buff_num, __u32 len, void *data )
{
    OPL_STATUS retVal;
#if 1
    if((buff_num!=0)&&(buff_num!=1)){
	     printf("invaild buffer number\n");
	     return OPL_ERROR;
    	}
	switch (((len>>8)-1)/32){
		case 0:
			retVal = RegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, *(DATA32 *)data);
			break;
		case 1:
			retVal = RegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, ((DATA64 *)data)->data0);
			retVal = RegFieldWrite(REG_SPI_TX_(1+4*buff_num), 0, 32, ((DATA64 *)data)->data1);
			break;
		case 2:
			retVal = RegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, ((DATA96 *)data)->data0);
			retVal = RegFieldWrite(REG_SPI_TX_(1+4*buff_num), 0, 32, ((DATA96 *)data)->data1);
			retVal = RegFieldWrite(REG_SPI_TX_(2+4*buff_num), 0, 32, ((DATA96 *)data)->data2);
			break;
		case 3:
			retVal = RegFieldWrite(REG_SPI_TX_(0+4*buff_num), 0, 32, ((DATA128 *)data)->data0);
			retVal = RegFieldWrite(REG_SPI_TX_(1+4*buff_num), 0, 32, ((DATA128 *)data)->data1);
			retVal = RegFieldWrite(REG_SPI_TX_(2+4*buff_num), 0, 32, ((DATA128 *)data)->data2);
			retVal = RegFieldWrite(REG_SPI_TX_(3+4*buff_num), 0, 32, ((DATA128 *)data)->data3);
			break;
		default:
			printf("invalid length...\n");
		}
#endif
	return retVal;
}
OPL_STATUS spiRcvDataGet( int buff_num, __u32 len,void *data )
{
    OPL_STATUS retVal;
#if 1
    if((buff_num!=0)&&(buff_num!=1)){
	     printf("invaild buffer number\n");
	     return OPL_ERROR;
    	}
	switch (((len>>8)-1)/32){
		case 0:
			retVal = RegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, (DATA32 *)data);
			break;
		case 1:
			retVal = RegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, &((DATA64 *)data)->data0);
			retVal = RegFieldRead(REG_SPI_RX_(1+4*buff_num), 0, 32, &((DATA64 *)data)->data1);
			break;
		case 2:
			retVal = RegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, &((DATA96 *)data)->data0);
			retVal = RegFieldRead(REG_SPI_RX_(1+4*buff_num), 0, 32, &((DATA96 *)data)->data1);
			retVal = RegFieldRead(REG_SPI_RX_(2+4*buff_num), 0, 32, &((DATA96 *)data)->data2);
			break;
		case 3:
			retVal = RegFieldRead(REG_SPI_RX_(0+4*buff_num), 0, 32, &((DATA128 *)data)->data0);
			retVal = RegFieldRead(REG_SPI_RX_(1+4*buff_num), 0, 32, &((DATA128 *)data)->data1);
			retVal = RegFieldRead(REG_SPI_RX_(2+4*buff_num), 0, 32, &((DATA128 *)data)->data2);
			retVal = RegFieldRead(REG_SPI_RX_(3+4*buff_num), 0, 32, &((DATA128 *)data)->data3);
			break;
		}
#endif
	return retVal;
}


OPL_STATUS spi_read_man(void)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
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
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));

	retVal=tmp_data.data3<<8;
//       printf("read spi flash manufacture ID = 0x%x\n",retVal);
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
	OPL_STATUS regVal = 0;
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
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));

	*status = (tmp_data.data0<<8)>>8 ;
//	OPL_PRINTF("read spi flash status  = 0x%x\n",tmp_data.data0);
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
	OPL_STATUS regVal = 0;
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
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));
//	spiStatusShow();

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
	OPL_STATUS regVal = 0;
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
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));
//    spiWriteStatus(0x9c);
//	spiStatusShow();

//    OPL_PRINTF("enable spi flash write...\n");
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
	OPL_STATUS regVal = 0;
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
//	OPL_PRINTF("debug :read back data3  = 0x%x\n",(tmp_data.data3));
//	OPL_PRINTF("debug :read back data2  = 0x%x\n",(tmp_data.data2));
//	OPL_PRINTF("debug :read back data1  = 0x%x\n",(tmp_data.data1));
//	OPL_PRINTF("debug :read back data0  = 0x%x\n",(tmp_data.data0));
//	spiStatusShow();

    printf("disable spi flash write...\n");
	return retVal;

}

/**
*@par Description
* This function erase a block of spi flash .
*
* @param [in] addr_ofs
*	   address offset of spi flash.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spi_erase_one_sector(__u32 addr_ofs)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ,tmp_data;
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
//	(&cmd)->data0 =(0x20000000 | (addr_ofs)) ;
     (&cmd)->data0 =(0xd8000000 | (addr_ofs*0x10000)) ;
	(&cmd)->data1 =0 ;
	(&cmd)->data2 =0 ;
	(&cmd)->data3 =(0xd8000000 | (addr_ofs*0x10000)) ;
//	(&cmd)->data3 =(0x20000000 | (addr_ofs)) ;
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

//	printf("start erase spi flash block %d...",addr_ofs);

    while (timeout--)
    {
        retVal = spi_read_status(&flash_status);
//		OPL_PRINTF("flash_status = 0x%x \n",flash_status);
		if(!(timeout%1000))
		{
			printf(".");
		}
        if (!(flash_status&0x3))
        {
           goto OUT;
        }
    }

	printf("\n chu  cuo  la......... : erase spi flash timeout\n");
	return OPL_ERROR;

OUT:
	printf("done\n");
//	retVal = spiStatusShow();
	return retVal;

}

/**
*@par Description
* This function program one word to spi flash .
*
* @param [in] *value
*	   pointer  of the word to be programed in flash.
* @param [in] addr_ofs
*	   address offset of spi flash.
 * @retval OPL_OK  This operation success.
*/

OPL_STATUS spi_write_one_word(__u32 *value,__u32 addr_ofs)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ,tmp_data;
	__u32 len;
	__u32 ready_buf_id = 0;
	__u32 flash_status;
	__u32 timeout=5000000;

	addr_ofs += SPI_AGENT_READ_OFS;
	/*enable spi flash write*/
	retVal = spiWriteEnable();

	retVal = spiTransLenSet(0x4040);

	/*get spi valid len */
	retVal = spiTransLenGet(&len);

	/*set cmd for spi flash*/
//printf("*value = 0x%x\n",*value);
//printf("addr_ofs = 0x%x\n",addr_ofs);
	(&cmd)->data0 = *value ;
	(&cmd)->data1 = (0x02000000 | (addr_ofs & 0x00ffffff)) ;
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
	printf("\n chu	cuo  la......... : erase spi flash timeout\n");
	return OPL_ERROR;
	OUT:
//		OPL_PRINTF("done\n");

	return retVal;

}
#if 1

OPL_STATUS sp_test(void)
{
	OPL_STATUS retVal;
	__u32 tmp_val;
	int i;
	for(i=0;i<0x40;i++)
	{
	  tmp_val = i;
        retVal = spiWordProgram(&tmp_val, 4*i+0x5fff0);
	}
    return retVal;
}


OPL_STATUS spiWordProgram(__u32 *value,__u32 addr_ofs)
{
	OPL_STATUS retVal;
	OPL_STATUS regVal = 0;
	DATA128 cmd ,tmp_data;
	__u32 len;
	__u32 ready_buf_id = 0;
	__u32 flash_status;
	__u32 timeout=5000000;

	/*enable spi flash write*/
	retVal = spiWriteEnable();

	retVal = spiTransLenSet(0x4040);

	/*get spi valid len */
	retVal = spiTransLenGet(&len);

	/*set cmd for spi flash*/
	(&cmd)->data0 = *value;
	(&cmd)->data1 = (0x02000000 | (addr_ofs & 0x00ffffff)) ;
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
//	printf("\n chu	cuo  la......... : erase spi flash timeout\n");
	return OPL_ERROR;
	OUT:
//		OPL_PRINTF("done\n");

	return retVal;

}
#endif
unsigned long spi_read_one_word(unsigned long addr_ofs)
{
	OPL_STATUS retVal;
//	OPL_STATUS regVal = 0;
	DATA128 cmd ,tmp_data;
	__u32 len;
	__u32 ready_buf_id = 0;
	__u32 flash_status;
	__u32 timeout=5000000;
       __u32 value=0;
       unsigned long ofs;

       ofs=addr_ofs;
#if 1
	retVal = spiTransLenSet(0x4848);
#else
	retVal = spiTransLenSet(0x4040);
#endif
	/*get spi valid len */
	retVal = spiTransLenGet(&len);
		
	/*set cmd for spi flash*/
    //printf("*value = 0x%x\n",*value);
    //printf("addr_ofs = 0x%x\n",addr_ofs);
    //fast read
#if 1
       (&cmd)->data0 =  0;
	(&cmd)->data1 =  ((unsigned int)(ofs<<8)) & 0xffffff00;
	(&cmd)->data2 = 0x0B ;
	(&cmd)->data3 = 0 ;
#else
       (&cmd)->data0 =  0;
	(&cmd)->data1 = (0x03000000 | (addr_ofs & 0x00ffffff)) ;
	(&cmd)->data2 = 0x0 ;
	(&cmd)->data3 = 0 ;
#endif

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
    //  oplRegRead(REG_SPI_RX_(0+4*ready_buf_id) ,&value);
	retVal = spiRcvDataGet(ready_buf_id,len,&value);
		
	return value;

}

/**
*@par Description
* This function program u-boot to spi flash .
*
 * @retval OPL_OK  This operation success.
*/
	/*-----------------------------------------------------------------------
	 */

	/*
	 * The following code cannot be run from FLASH!
	 */
	static void flash_get_offsets (flash_info_t *info)
	{
		int i;
		   unsigned int sect_size = 0x10000 ;	 /* 64k sectors*/

		   for( i = 0; i < info->sector_count; i++ )
				info->start[i] = PHYS_FLASH_1 + (i * sect_size);   /*each 64k*/

	}

	static flash_info_t *flash_get_info(ulong base)
	{
		int i;
		flash_info_t * info;

		for (i = 0; i < CFG_MAX_FLASH_BANKS; i ++) {
			info = & flash_info[i];
			if (info->start[0] <= base && base < info->start[0] + info->size)
				break;
		}

		return i == CFG_MAX_FLASH_BANKS ? 0 : info;
	}

	ulong flash_get_size (flash_info_t *info)
	{
		unsigned int id ;
		id = spi_read_man();
	    printf("========================x\n");
	    printf("flash id %x\n",id);
		switch (id >> 24 & 0xff) {

		case FLASH_MAN_ATMEL:
		    printf("ATMEL SPI flash detected.\n");
			info->flash_id = FLASH_MAN_ATMEL;
			break;
	   case FLASH_MAN_MX:
		    printf("MX SPI flash detected.\n");
			info->flash_id = FLASH_MAN_MX;
			break;
	   case FLASH_MAN_WB:
		    printf("WB SPI flash detected.\n");
			info->flash_id = FLASH_MAN_WB;
			break;
#if 0
		default:
			printf("UNKNOWN spi flash Factory 0x%X.\n",id >> 24 & 0xff);
			info->flash_id = FLASH_UNKNOWN;
			info->sector_count = 0;
			info->size = 0;
			break;
#endif
		default:
		    info->flash_id = (id >> 24 & 0xff);
		    break;
		}

		if (info->flash_id != FLASH_UNKNOWN) {
			switch (id >> 16 & 0xff) {

				case FLASH_AT26_ATMEL:
				info->sector_count = 64;
					info->size = 0x00400000;
					break;
				 case FLASH_MX_L320:
				info->sector_count = 64;
					info->size = 0x00400000;
					break;
			     case FLASH_WB_25X16:
				info->sector_count = 64;
					info->size = 0x00400000;
					break;
#if 0
				default:
					printf("--------- FLASH_UNKNOWN .\n");
					info->flash_id = FLASH_UNKNOWN;
					info->sector_count = 0;
				    info->size = 0;
					return (0); 		/* => no or unknown flash */
#endif
				default:
					printf("---- numoney flash .\n");
					info->flash_id = (id >> 16 & 0xff);
					info->sector_count = 128;
				    info->size = 0x00800000;
				    break;
					return (0); 		/* => no or unknown flash */
				}
			}
//		        printf("c\n");
//        run_command("md 0xb0000000 0x10",0);
		flash_get_offsets(info);
//		        printf("d\n");
//        run_command("md 0xb0000000 0x10",0);
		return (info->size);
	}

	unsigned long flash_init (void)
	{
		unsigned long size = 0;
		int i;

//		spiInit();
//    printf("=============1\n");
		for (i=0; i < CFG_MAX_FLASH_BANKS; ++i) {
//      printf("=============2\n");
		  memset(&flash_info[i], 0, sizeof(flash_info_t));
//			printf("111\n");
//			run_command("md 0xb0000000 0x10",0);


			size = flash_get_size(&flash_info[i]);
// run_command("flinfo",0);

//	        printf("222\n");
//            run_command("md 0xb0000000 0x10",0);
			flash_protect(FLAG_PROTECT_SET,
				  BOOTLOADER_ADDR,
				  BOOTLOADER_ADDR+SECTOR_SIZE*4-1,
				  &flash_info[i]);
//			 printf("333\n");
//           run_command("md 0xb0000000 0x10",0);

		}
//					run_command("flinfo",0);
	    cs0Cfgdefault();
//		printf("444\n");
//      run_command("md 0xb0000000 0x10",0);
		return size;

	}

	void flash_print_info (flash_info_t *info)
	{
		uchar *fmt;
		int i;
	 reCfgCs0(flash_info[0].size);
		if (info->flash_id == FLASH_UNKNOWN) {
			 printf ("missing or unknown FLASH type\n");
			return;
			}

		switch (info->flash_id & 0xff) {
			case FLASH_MAN_ATMEL:
				fmt = "AT26S321 SPI flash (32 Mbit)";
				break;
			default:
				fmt = "Unknown Chip Type\n";
				break;
		}

		 printf (fmt);

		 printf ("	Size: %ld MB in %d Sectors\n",
			 info->size >> 20,
			 info->sector_count);

		 printf ("	Sector Start Addresses:");

		 for (i=0; i<info->sector_count; ++i) {
			 if ((i % 5) == 0) {
				 printf ("\n   ");
			 }
		  printf (" %08lX%s", info->start[i],
				info->protect[i] ? " (RO)" : "	   ");

		 }
		   printf ("\n\n  flash space admesure:\n");
	  printf("\t\t\t\tuboot space  : 0x%x --- 0x%x\n",info->start[0],info->start[0]+envoffset-1);
	  printf("\t\t\t\tenv space    : 0x%x --- 0x%x\n",info->start[0]+envoffset,info->start[0]+cfg_offset-1);
	  printf("\t\t\t\tcfg space    : 0x%x --- 0x%x\n",info->start[0]+cfg_offset,info->start[0]+knl_1offset-1);
	  printf("\t\t\t\tos1 space    : 0x%x --- 0x%x\n",info->start[0]+knl_1offset,info->start[0]+knl_2offset-1);
	  printf("\t\t\t\tos2 space    : 0x%x --- 0x%x\n",info->start[0]+knl_2offset,info->start[0]+ flash_info[0].size-1);
		 cs0Cfgdefault();
		  printf ("\n");
	}



	int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
	{
		ulong data = 0; /* 16 or 32 bit word, matches flash bus width on MPC8XX */
		ulong ofs;
		int bytes;	  /* number of bytes to program in current word 	*/
		int left;	  /* number of bytes left to program			*/
		int i, res;

		 reCfgCs0(flash_info[0].size);
//		 printf("u-boot write flash...\n");
		for (left = cnt, res = 0;
			left > 0 && res == 0;
			addr += sizeof(data), left -= sizeof(data) - bytes) {

			bytes = addr & (sizeof(data) - 1);
			addr &= ~(sizeof(data) - 1);

		/* combine source and destination data so can program
		 * an entire word of 16 or 32 bits
		 */
		/*wangxin:big endian*/
#ifdef BIGENDIAN
			for (i = 0; i < sizeof(data); i++) {
				data <<= 8;
				if (i < bytes || i - bytes >= left )
				{
                                data += *((uchar *)addr + i);
                                udelay(5000);/*kxu add at 20100331 for spi host read done*/          
                            }
				else
				data += *src++;
			}

#else
		/*wangxin:little endian*/
			add=0;
			for (i = 0; i < sizeof(data); i++) {
				data <<= 8;
				if (i < bytes || i - bytes >= left )
				data += *((uchar *)addr +(sizeof(data)-i));
				else
				{
					data += *(src+sizeof(data)-1-i);
					add++;
				}
			}
			src += add;
#endif

		/* write one word to the flash */

			ofs = addr - PHYS_FLASH_1;

			if(ofs==0x40000){
//				printf("ofs=%x data=%x\n", ofs,data );


				spi_write_one_word(&data,ofs);
			}
//			printf("###ofs=%x data=%x\n", ofs,data );
			spi_write_one_word(&data,ofs);
		}
	cs0Cfgdefault();
		return (res);
	}

	int flash_erase (flash_info_t *info, int s_first, int s_last)
	{
		int num,i,prot;
		int sect;
		unsigned int ofs;

		int retVal;
//		__u32 header1 = 0x03030100 ;  /* test normal read */
//		__u32 header1 = 0x03bb0104 ;  /* test 2xIO read */
		__u32 header1 = 0x030b0108 ;  /**fast read**/
	    __u32 header2 = 0x10000000 ;
	    __u32 header3 = 0x00000000 ;
	    __u32 header4 = 0x00000000 ;

	 reCfgCs0(flash_info[0].size);
		if ((s_first < 0) || (s_first > s_last)) {
			if (info->flash_id == FLASH_UNKNOWN) {
						printf ("- missing\n");
				} else {
						printf ("- no sectors to erase\n");
				}
		    cs0Cfgdefault(); /*zw 922*/
			return 1;
			}

		prot = 0;
			for (sect=s_first; sect<=s_last; ++sect) {
				if (info->protect[sect]) {
						prot++;
				}
			}

			if (prot) {
				printf ("- Warning: %d protected sectors will not be erased!\n",
						prot);
			cs0Cfgdefault(); /*zw 922*/
			return 1;
			} else {
				printf ("\n");
			}

		num = s_last - s_first +1;
//		printf("s_last = %d    s_first = %d\n",s_last , s_first);
		for(i=0;i<num;i++){
		  //	ofs = 0x10000 * (s_first+i);
		        ofs =  (s_first+i);
//               	printf("ofs = %d  \n",ofs);
			spi_erase_one_sector(ofs);
			/*####################################*/
			if (ofs == 0){
				retVal = spiWordProgram(&header1,0x0);
		        retVal = spiWordProgram(&header2,0x4);
		        retVal = spiWordProgram(&header3,0x8);
		        retVal = spiWordProgram(&header4,0xc);
				}
			/*####################################*/
			printf(".");
		}
		cs0Cfgdefault();
		printf("\n");
	}

void spi_erase_chip()
{

}

void burn_spi_bootloader(unsigned long *src, unsigned int count)
{
}
void burn_spi_os(unsigned long *src, unsigned int count)
{
}

OPL_STATUS spiProgramUb(void)
{
	OPL_STATUS retVal;
	__u8 *tmp_buf = 0xa1300000;
	__u32 volatile count;

//       __u32 header[4] ={0x08030000 ,0x10000000,0x00000000,0x00000000};// {0x030b0108 ,0x10000000,0x00000000,0x00000000};
       __u32 header[4] ={0x030b0108 ,0x10000000,0x00000000,0x00000000};// {0x030b0108 ,0x10000000,0x00000000,0x00000000};

	for(count=0;count<4;count++)
	{
	    spi_erase_one_sector(count);
	}
	/*program 4 word head data to flash*/
	for(count=0;count<4;count++)
	{
	    retVal = spiWordProgram(&header[count],4*count);
	}
	/*program file to flash*/
	for(;count<(0x40000/4);count++)
	{
	    retVal = spiWordProgram(tmp_buf+4*(count-4),4*count);
	    if(!(count%656))
          {
              printf("\b\b\b");
              printf("%d%%",(count*100)/0x10000);
          }
	}

	printf("\nprigram file finished.\n");

	return retVal;

}



#if 0
OPL_STATUS spiProgramUb(void)
{
	OPL_STATUS retVal;
	__u32 *tmp_buf = NULL;
	int fd;
	int count;

	__u32 header1 = 0x030b0108 ;
	__u32 header2 = 0x10000000 ;
	__u32 header3 = 0x00000000 ;
	__u32 header4 = 0x00000000 ;

	/*open file*/
	fd = open("/nfs/home/wzhang/u-boot-onu2-rom.bin",O_RDONLY);

	if(fd < 0)
	{
	     printf("open file failed\n");
	     return OPL_ERROR;
	}else{
		  printf("open file ,fd = %d\n",fd);
		 }

	/*malloc a buffer */
	tmp_buf = malloc(0x40000);

	if(tmp_buf == NULL)
	{
	     printf("malloc buffer failed\n");
	     return OPL_ERROR;
	}

	/*read file to buffer*/
	read(fd,tmp_buf,0x40000);

	/*erase spi flash*/
	for(count=0;count<0x40;count++)
	{
	    spiEraseBlock(count);
	}

	/*program header to flash*/

	 retVal = spiWordProgram(&header1,0x0);
	 retVal = spiWordProgram(&header2,0x4);
	 retVal = spiWordProgram(&header3,0x8);
	 retVal = spiWordProgram(&header4,0xc);

	/*program file to flash*/
	for(count=0;count<(0x40000/4);count++)
	{
	    retVal = spiWordProgram((__u32*)(tmp_buf+4*count),4*count+0x10);
	}

	printf("prigram file finished.\n");

	close(fd);

	free(tmp_buf);

	return retVal;

}
#endif
OPL_STATUS RegFieldRead(__u32 regAddr,__u16 fieldOffset,__u16 fieldWidth,__u32 *data0)
{
	OPL_STATUS retVal;
	__u32 regVal;
	__u32 fieldMask;


	if(fieldWidth == 32)
	{
		fieldMask = 0xFFFFFFFF;
	}else
	{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}

	retVal = oplRegRead(regAddr,&regVal);

	if(OPL_OK != retVal)
	{
		return retVal;
	}

	regVal = regVal&fieldMask;

	*data0 = regVal>>fieldOffset;
	return OPL_OK;
}

OPL_STATUS RegFieldWrite(__u32 regAddr,__u16 fieldOffset,__u16 fieldWidth,__u32 data0)
{
	OPL_STATUS retVal;
	__u32 regVal;
	__u32 oldVal;
	__u32 fieldMask;



	if(fieldWidth == 32)
	{
		fieldMask = 0xFFFFFFFF;
	}else
	{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}

//       printf("regAddr = 0x%x\n",regAddr);
//	printf("1\n");
	retVal = oplRegRead(regAddr,&oldVal);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	oldVal = oldVal&(~fieldMask);

	regVal = (data0<<fieldOffset)&fieldMask;

//  printf("2\n");
	regVal = regVal|oldVal;
//  printf("regAddr = 0x%x\n",regAddr);
	retVal = oplRegWrite(regAddr,regVal);

	return retVal;
}

OPL_STATUS oplRegRead(__u32 regAddr,__u32 *regVal)
{
	if(OPL_NULL == regVal)
	{
		return -1;
	}

	*regVal = *(volatile __u32 *)((__u32)regBaseAddr + regAddr*4);
//	printf("a\n");

	return OPL_OK;
}

OPL_STATUS oplRegWrite(__u32 regAddr,__u32 regVal)
{
	OPL_STATUS retVal = OPL_OK;

	*(volatile __u32 *)((__u32)regBaseAddr + regAddr*4) = regVal;

	return retVal;
}
//#endif

#endif //#ifdef  CONFIG_SPI_NJZTE


