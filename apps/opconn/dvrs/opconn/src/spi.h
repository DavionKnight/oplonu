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

/***** STRUCTURES and UNIONS *****/

/***** TYPEDEFS *****/
#define OPL_UINT32_t UINT32
#define OPL_UINT8_t UINT8
#define OPL_UINT16_t UINT16
#define DATA32 UINT32

typedef struct data64
{
	OPL_UINT32_t data0;	
    OPL_UINT32_t data1;
}DATA64;

typedef struct data96
{
	OPL_UINT32_t data0;	
    OPL_UINT32_t data1;
	OPL_UINT32_t data2;
}DATA96;

typedef struct data128
{
	OPL_UINT32_t data0;	
    OPL_UINT32_t data1;
	OPL_UINT32_t data2;
	OPL_UINT32_t data3;
}DATA128;

typedef struct spi_int_status{
  OPL_UINT8_t ready_buff_id ;
	OPL_UINT8_t spi_trans_error_int ;
	OPL_UINT8_t spi_trans_done_int ;
	OPL_UINT8_t spi_config_error_int ;
	OPL_UINT8_t spi_complete_int ;
}SPI_INT_STATUS;

/***** EXTERNS *****/
#if 0
OPL_STATUS spiInit(int div);
OPL_STATUS spiFreqDivSet(OPL_UINT32_t div);
OPL_STATUS spiTransLenSet (OPL_UINT32_t len) ;
OPL_STATUS  spiTransLenGet (OPL_UINT32_t *len) ;
OPL_STATUS spiIntEnable(void);
OPL_STATUS spiIntDisable(void);
OPL_STATUS  spiBufReadySet (OPL_UINT32_t buf_num) ;
OPL_STATUS spiAutoSelectChose(OPL_UINT32_t option);
OPL_STATUS spiSlaveSelect(OPL_UINT32_t cs);
OPL_STATUS waitTransDone( void );
OPL_STATUS  waitBusy( void );
OPL_STATUS spiStartTrans(void);
OPL_STATUS spiTransDataFill( int buff_num, OPL_UINT32_t len, void *data );
OPL_STATUS spiRcvDataGet( int buff_num, OPL_UINT32_t len,void *data );
void spiRegShow(void);
OPL_STATUS spiReadMan(void);
OPL_STATUS spiReadStatus(OPL_UINT32_t *status);
OPL_STATUS spiWriteStatus(OPL_UINT32_t status);
OPL_STATUS spiWriteEnable(void);
OPL_STATUS spiWriteDisable(void);
OPL_STATUS spiEraseBlock(OPL_UINT32_t addr_ofs);
OPL_STATUS spiWordProgram(OPL_UINT32_t *value,OPL_UINT32_t addr_ofs);
OPL_STATUS spiProgramUb(char * filename);
OPL_STATUS spiStatusShow();
#endif
/***** PROTOTYPES *****/

#endif /* SPI_H */
/** 
 *@} 
 */
/* END FILE */
