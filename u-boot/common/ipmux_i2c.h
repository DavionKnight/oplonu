/*
=============================================================================
     Header Name: ipmux_i2c.h

     General Description:
general definition for the basic data type in Opulan driver.
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 yzhang            07/04/05      Initial Version
----------------   ------------  ----------------------------------------------
*/
#ifndef I2C_DRIVER_INCLUDE
#define	I2C_DRIVER_INCLUDE 

#define UINT32  unsigned int
#define INT32   int
#define UINT16  unsigned short
#define INT16   short
#define UINT    unsigned int
#define INT     int
#define UINT8   unsigned char
#define INT8    char

#define OPL_OK              0
#define OPL_ERROR           (-1)
#define OPL_FALSE           0
#define OPL_TRUE            1
#define OPL_STATUS          int
#define OPL_ZERO            0
#define OPL_NULL            ((void *) 0)    

#define IPMUX_REG_SIZE				32			/* 32 bits register for IPMux */
#define REG_ADDR_ALLIGNED		4				/* 32bits word */
#define FIELD_BITS(n)				(0xffffffff >> (IPMUX_REG_SIZE - (n)))

/* includes */


/* definition */
#define IPMUX_REG_BASE_I2C      		0x2000 						/* I2C interface                 */ 

	/* I2C registers */
#define REG_I2CSTA 						(IPMUX_REG_BASE_I2C + 0x0)
	
#define REG_I2CTO 						(IPMUX_REG_BASE_I2C + 0x0) 	/* share address with REG_I2CSTA  */
#define REG_BIT_I2CTOTE_OFF			7
#define REG_BIT_I2CTOTE					(0x1 << REG_BIT_I2CTOTE_OFF)
#define REG_BIT_I2CTOVAL				(0x7)

#define REG_I2CDAT 						(IPMUX_REG_BASE_I2C + 0x4)

#define REG_I2CADR 						(IPMUX_REG_BASE_I2C + 0x8)
#define REG_BIT_I2CADR_ADR_OFF		1
#define REG_BIT_I2CADR_ADR				(0x7f << REG_BIT_I2CADR_ADR_OFF)
#define REG_BIT_I2CADR_RW_OFF			0
#define REG_BIT_I2CADR_RW				(0x1 << REG_BIT_I2CADR_RW_OFF)

#define REG_I2CCON 						(IPMUX_REG_BASE_I2C + 0xC)
#define REG_BIT_I2CCON_AA_OFF			7
#define REG_BIT_I2CCON_AA				(0x1 << REG_BIT_I2CCON_AA_OFF)
#define REG_BIT_I2CCON_ENSIO_OFF		6
#define REG_BIT_I2CCON_ENSIO			(0x1 << REG_BIT_I2CCON_ENSIO_OFF)
#define REG_BIT_I2CCON_STA_OFF		5
#define REG_BIT_I2CCON_STA				(0x1 << REG_BIT_I2CCON_STA_OFF)
#define REG_BIT_I2CCON_STO_OFF		4
#define REG_BIT_I2CCON_STO				(0x1 << REG_BIT_I2CCON_STO_OFF)
#define REG_BIT_I2CCON_SI_OFF			3
#define REG_BIT_I2CCON_SI				(0x1 << REG_BIT_I2CCON_SI_OFF)
#define REG_BIT_I2CCON_CR_OFF			0
#define REG_BIT_I2CCON_CR				(0x7 << REG_BIT_I2CCON_CR_OFF)

#define REG_I2CEN 						(IPMUX_REG_BASE_I2C + 0x10)
#define REG_BIT_I2CEN_RD_OFF			3
#define REG_BIT_I2CEN_RD				(FIELD_BITS(1) << REG_BIT_I2CEN_RD_OFF)
#define REG_BIT_I2CEN_WR_OFF			2
#define REG_BIT_I2CEN_WR				(FIELD_BITS(1) << REG_BIT_I2CEN_WR_OFF)
#define REG_BIT_I2CEN_INT_OFF			1
#define REG_BIT_I2CEN_INT				(FIELD_BITS(1) << REG_BIT_I2CEN_INT_OFF)
#define REG_BIT_I2CEN_MS_OFF			0
#define REG_BIT_I2CEN_MS				(FIELD_BITS(1) << REG_BIT_I2CEN_MS_OFF)


#define REG_I2CSR 						(IPMUX_REG_BASE_I2C + 0x14)
#define REG_BIT_I2CSR_TIP_OFF			1
#define REG_BIT_I2CSR_TIP				(FIELD_BITS(1) << REG_BIT_I2CSR_TIP_OFF)
#define REG_BIT_I2CSR_BUSY_OFF			6
#define REG_BIT_I2CSR_BUSY				(FIELD_BITS(1) << REG_BIT_I2CSR_BUSY_OFF)



#define OPL_DRV_PRINTF(fmt) {\
								if(0)\
								{\
									printf fmt;\
								}\
								}

	/* only important in master mode, Clock Rate values: Serial Clock Frequncy(SCF) */
typedef enum
	{
	I2C_SCF_330,	/* 0 */
	I2C_SCF_288,	/* 1 */
	I2C_SCF_217,	/* 2 */
	I2C_SCF_146,	/* 3 */
	I2C_SCF_88,		/* 4 */
	I2C_SCF_59,		/* 5 */
	I2C_SCF_44,		/* 6 */
	I2C_SCF_36,		/* 7 */
	I2C_SCF_END
}I2C_SPEED_e;

	/* Status Errors */
#define OPL_I2C_BUSY					-1 /* transfer busy */
#define OPL_I2C_NO_DATA				-3 /* err: No data in block */
#define OPL_I2C_NACK_ON_DATA			-4 /* err: No ack on data */
#define OPL_I2C_NACK_ON_ADDRESS		-5 /* err: No ack on address */
#define OPL_I2C_DEVICE_NOT_PRESENT	-6 /* err: Device not present */

#define OPL_I2C_ARBITRATION_LOST		-7 /* err: Arbitration lost */
#define OPL_I2C_TIME_OUT				-8 /* err: Time out occurred */
#define OPL_I2C_SLV_WRITE_MORE_DATA_NEED	-9
#define OPL_I2C_SLV_READ_BUFFER_TOO_SMALL	-10

/* typedef  */
#define 	I2C_READ		1
#define 	I2C_WRITE		0

#define OPWAYCNU_REG_I2C_READ_FLAG_SET 		bNoStopIssue = OPL_TRUE
#define OPWAYCNU_REG_I2C_READ_FLAG_CLEAR	bNoStopIssue = OPL_FALSE

/* EPROM on CNU board' salve address, which is transfer by I2C*/
#define OPWAYEPROM_I2C_ADDR 0x50
#define OPWAYDEVICE_I2C_ADDR 0x00

	/* I2C device mode configuration */
typedef struct I2C_SYS_CONFIG_s
	{
	UINT8 masterMode	:1;		/* 1:master , 0:slave */
	UINT8 interrupt		:1;		/* 1:interrupt, 0:polling */
	UINT8 status;
	UINT8 lastStatus;
} I2C_SYS_CONFIG_t;


#endif /* I2C_DRIVER_INCLUDE */	
