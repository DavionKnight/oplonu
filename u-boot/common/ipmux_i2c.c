/******************************************************************************/
/**  COPYRIGHT (C) 2003- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/
/*******************************************************************************
**    MODULE     : I2C Device driver source file specific to IPMux driver
**
**    FILE       : ipmux_i2c.c
**      
**    DESCRIPTION: This file contains IPMux I2C Controller device driver utility function.
**
**
**    NOTES      :
**
*******************************************************************************/
/* 
**    MODIFICATION HISTORY:
**    Author            		Date				Description of Changes
**    ----------------  	------------  	----------------------------------------------
**    yzhang            		07/04/2005      	Initial Version                               
**
*/

/* includes */ 

#include "ipmux_i2c.h"


/* imports */

/* Global */
I2C_SYS_CONFIG_t ipMuxI2CC;

	/* for debug system */
int nI2cDbgFd = OPL_ZERO; /* standard output device */
static int bNoStopIssue = OPL_FALSE;


int opwaycltIfI2cSTOStatusIssue ( void ) {return bNoStopIssue;};

/* defines */ 
#if 1
#define i2cInterruptClear()		{UINT32 u32tt; ipMuxRegRead/*BitClear*/ (REG_I2CCON, /*REG_BIT_I2CCON_SI*/(UINT32 *)&u32tt); }
#else
#define i2cInterruptClear()		ipMuxRegBitClear (REG_I2CCON, REG_BIT_I2CCON_SI)
#endif

	/* for debug system */
#ifdef opl_log
#undef opl_log
#endif
#define opl_log(fmt)		/*	printf(fmt)*//*printf fmt	opllog(&nzDbgFd, nI2cDbgFd, fmt) */

#define I2C_WAIT_NO_TRANSFER(ntimeout) 	{ \
												int iii; \
												for (iii = (ntimeout); iii; iii--) \
													{ \
													/* check TIP bit, if set, try again */ \
													if (ipMuxRegBitTest ((UINT32) REG_I2CSR, (UINT32) REG_BIT_I2CSR_TIP)) \
														{ \
														opl_log (("A transfer is in process, try again...%d.\n", iii)); \
														oplRtosTaskDelay (1);	/* give times to SIO to operate */ \
														ipMuxRegBitTest (REG_I2CCON,OPL_ZERO); \
														continue; \
													} \
													break; \
												} \
												if (0 == iii) \
													{ \
													opl_log (("wait SIO stopping tranfser time out.\n")); \
													break; \
												} \
											}


/* function declarations */

/*******************************************************************************
 **
 **			Chip configuration
 **
 *******************************************************************************/

static inline OPL_STATUS ipMuxRegRead(UINT32 regID, volatile UINT32 *pval)
{
	if (regID % 4 != 0)
		return OPL_ERROR;

        *(UINT32*)(pval) = *(volatile UINT32 *)(0xbf000000 + regID);
        return OPL_OK;
}

static inline OPL_STATUS ipMuxRegWrite(UINT32 regID, UINT32 val)
{
	if (regID % 4 != 0)
		return OPL_ERROR;

        *((volatile UINT32 *)(0xbf000000 + regID)) = (UINT32)(val);
        return OPL_OK;
}


static inline int ipMuxRegBitTest (UINT32 regId, UINT32 bits)
{
        UINT32 nTemp = 0;
	if (regId % 4 != 0)
		return OPL_ERROR;

        ipMuxRegRead (regId, &nTemp);
        return (nTemp & bits);
}

static inline void ipMuxRegBitSet(UINT32 regId, UINT32 bits)	
{
	UINT32 nTemp = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	ipMuxRegRead (regId, &nTemp);
	nTemp |= (bits);
	ipMuxRegWrite (regId, nTemp);
}

static inline void ipMuxRegBitClear(UINT32 regId, UINT32 bits)	
{
	UINT32 nTemp = OPL_ZERO;
	if (regId % 4 != 0)
		return;

	ipMuxRegRead (regId, &nTemp);
	nTemp &= (~(bits));
	ipMuxRegWrite (regId, nTemp);
}

void oplRtosTaskDelay (int tick)
{
	int j;
	while(tick > 0)
	{
		for(j = 0; j < 0x6000; j++)
		{
		}
		tick--;
	}
}

/*******************************************************************************
*
* i2cControllerEnable
*
* DESCRIPTION:
* 	this function enable the IPMux I2C Controller for transmission or receive.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	OPL_OK, the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cControllerEnable ( void )
	{
	/* enable interrupt to generate SI */
	ipMuxRegBitSet(REG_I2CEN, REG_BIT_I2CEN_INT);

	/* enable IPMux I2CC by settting ENSIO bit */
	ipMuxRegBitSet (REG_I2CCON, REG_BIT_I2CCON_ENSIO);

	/* Disable I2C Timeout */
	ipMuxRegWrite (REG_I2CTO, OPL_ZERO);
	opl_log (("enable IPMux I2C Controller ok.\n"));
	return OPL_OK;
}

/*******************************************************************************
*
* i2cControllerDisable
*
* DESCRIPTION:
* 	this function disable the IPMux I2C Controller from transmission or receive.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	OPL_OK, the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cControllerDisable ( void )
	{
	UINT32 regVal = 0;

	/* disable interrupt to generate SI */
	ipMuxRegBitClear (REG_I2CEN, REG_BIT_I2CEN_INT);

#if 0
	/* disable IPMux I2CC by clearing ENSIO bit */
	ipMuxRegBitClear (REG_I2CCON, REG_BIT_I2CCON_ENSIO);
#else
	ipMuxRegWrite (REG_I2CCON, regVal);

	/* stop IPMux I2CC by settting STO bit */
	ipMuxRegBitSet (REG_I2CCON, REG_BIT_I2CCON_STO);
#endif

	opl_log (("disable IPMux I2C Controller ok.\n"));
	return OPL_OK;
}

/*******************************************************************************
*
* i2cControllerReset
*
* DESCRIPTION:
* 	this function reset the IPMux I2C Controller by sequence of disabling followed enabling.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	OPL_OK, the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cControllerReset ( void )
	{

	i2cControllerDisable ();
	i2cControllerEnable ();

	opl_log (("reset IPMux I2C Controller ok.\n"));
	return OPL_OK;
}


/*******************************************************************************
*
* i2cMasterModeSet
*
* DESCRIPTION:
* 	this function set the IPMux I2C Controller to work in master mode.
*
* INPUTS:
*	clockRate	 - SCF value, see I2C_SPEED_e. if the clockRate is invalid, default 400K used.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	OPL_OK, the operation is successful. 
*
* SEE ALSO: 
*/
OPL_STATUS i2cMasterModeSet (UINT32 clockRate )
	{
	UINT32 regVal = 0;

 	ipMuxRegRead (REG_I2CCON, &regVal);
 	switch (clockRate)
		{
		case	I2C_SCF_330:	/* 0 */
		case	I2C_SCF_288:	/* 1 */
		case	I2C_SCF_217:	/* 2 */
		case	I2C_SCF_146:	/* 3 */
		case	I2C_SCF_88:		/* 4 */
		case	I2C_SCF_59:		/* 5 */
		case	I2C_SCF_44:		/* 6 */
		case	I2C_SCF_36:		/* 7 */
			regVal &= (~REG_BIT_I2CCON_CR);
			regVal |= (clockRate & REG_BIT_I2CCON_CR);
			break;
		default:
			opl_log (("invalid I2C Controller Clock Rate value: %d, default speek 400K used.\n"));
	}

		/* clear AA bit */
	regVal &= (~REG_BIT_I2CCON_AA);
	ipMuxRegWrite (REG_I2CCON, regVal );

	/* yzhang, 20050919, add according to the registers 4-7 */
	ipMuxRegBitSet(REG_I2CEN, REG_BIT_I2CEN_MS);
	
	ipMuxI2CC.masterMode = 1;

	opl_log (("IPMux I2C Controller now works in master mode.\n"));
	return OPL_OK;
}

/*******************************************************************************
*
* i2cSlaveModeSet
*
* DESCRIPTION:
* 	this function set the IPMux I2C Controller to work in slave mode.
*
* INPUTS:
*	slvAddr	 - own slave address.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	OPL_OK, the operation is successful. 
*
* SEE ALSO: 
*/
OPL_STATUS i2cSlaveModeSet (UINT8 slvAddr )
	{
	UINT32 regVal = 0;
	UINT8 regAddr = slvAddr;

	/* set IPMux I2CC own slave address */

	regAddr <<= REG_BIT_I2CADR_ADR_OFF;
	regAddr &= REG_BIT_I2CADR_ADR;
	ipMuxRegWrite (REG_I2CADR, regAddr);

	/* set IPMux I2CC to slave mode by settting AA bit */
	
	ipMuxRegRead (REG_I2CCON, &regVal);
	
		/* set AA bit */
	ipMuxRegBitSet (REG_I2CCON, REG_BIT_I2CCON_AA);

	/* yzhang, 20050919, add according to the registers 4-7 */
	ipMuxRegBitClear(REG_I2CEN, REG_BIT_I2CEN_MS);

	ipMuxI2CC.masterMode = 0;

	opl_log (("IPMux I2C Controller now works in slave mode.\n"));
	return OPL_OK;
}

/*******************************************************************************
*
* i2cStatusGet
*
* DESCRIPTION:
* 	this function read the status code returned by the IPMux I2C Controller,
*	for interrupte mode, it just reads the status saved in ipMuxI2CC;
*	for polling mode, it reads the I2CSTA.
*
* INPUTS:
*	n/a
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	status code, or OPL_ERROR if the operation time out. 
*
* SEE ALSO: 
*/
int i2cStatusGet ( void )
	{
	#define I2C_SIO_INTERRUPT_TIME_OUT	3
	UINT32 regVal = 0xf8;
	UINT32 regTemp= 0xf8;
#if 0
	int i;

	for (i = I2C_SIO_INTERRUPT_TIME_OUT; i; i--)
		{
		/* check SI bit, if not set, must be something wrong, try again */
		if (ipMuxRegBitTest ((UINT32) REG_I2CCON, (UINT32) REG_BIT_I2CCON_SI))
			break;
		else
			{
			opl_log (("there is no SIO interrupt occurred, try again...%d.\n", i));
		}
		oplRtosTaskDelay (10);	/* give times to SIO to operate */
	}
	if (0 == i)
		{
		opl_log (("wait SIO interrupt Status Code time out.\n"));
		return OPL_ERROR;
	}
#endif	
	/* get status code accordingly */
	if (ipMuxI2CC.interrupt)
	{
		/* do nothing currently */
	}
	else 
		{
#if 1 /* just for debug, 20070213, yzhang */
	{
		//oplRtosTaskDelay (10);
	}
#endif

	
		//printf("regTemp = %x.\n", regTemp);
		int i = 0;
		for(i = 0 ; i<=10000; i++);
	

	
		ipMuxRegRead (REG_I2CSTA, &regVal);

		
		ipMuxI2CC.lastStatus = ipMuxI2CC.status;
		ipMuxI2CC.status = (UINT8)regVal;
	}

	opl_log (("get IPMux I2C Controller Status Code ok, %x.\n", ipMuxI2CC.status));
	return (int )ipMuxI2CC.status;
}

/*******************************************************************************
*
* i2cInterruptEnable
*
* DESCRIPTION:
* 	this function set the IPMux I2C Controller to work in interrupt or polling mode.
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	intMode	- 0: polling mode, non-zero: interrupt mode.
*
* RETURNS:
* 	OPL_OK, the operation is successful. 
*
* SEE ALSO: 
*/
OPL_STATUS i2cInterruptEnable ( int intMode )
	{
	char * str[] = {"polling", "interrupt"};

	if (intMode)
		{
		ipMuxI2CC.interrupt = 1;

			/* yzhang, 20050919, add according to the registers 4-7 */
		ipMuxRegBitSet(REG_I2CEN, REG_BIT_I2CEN_INT);
	}
	else
		{
		ipMuxI2CC.interrupt = 0;

			/* yzhang, 20050919, add according to the registers 4-7 */
		ipMuxRegBitClear(REG_I2CEN, REG_BIT_I2CEN_INT);
	}

	opl_log (("IPMux I2C Controller now works in %s mode.\n", str[ipMuxI2CC.interrupt]));
	return OPL_OK;
}

/*******************************************************************************
*
* i2cInterruptRoutine
*
* DESCRIPTION:
* 	this function can be called when SI is set in interrupt mode. It just records the SIO
*	status code. 
*
* INPUTS:
*	n/a.
*
* OUTPUT:
*	intMode	- 0: polling mode, non-zero: interrupt mode.
*
* RETURNS:
* 	OPL_OK, the operation is successful. 
* 	OPL_ERROR, for unexpected interrupt error. 
*
* SEE ALSO: 
*/
OPL_STATUS i2cInterruptRoutine ( void )
	{
	UINT32 regVal = 0;

	/* check SI bit, if not set, must be something wrong */
	if (0 == ipMuxRegBitTest ((UINT32) REG_I2CCON, (UINT32) REG_BIT_I2CCON_SI))
		{
		/* opl_log (("there is no SIO interrupt occurred.\n", i)); */
		return OPL_ERROR;
	}

	/* get status code accordingly */
	if (ipMuxI2CC.interrupt)
		{
		ipMuxRegRead (REG_I2CSTA, &regVal);
		ipMuxI2CC.lastStatus = ipMuxI2CC.status;
		ipMuxI2CC.status = (UINT8)regVal;
	}
	else 
		{
		/* do nothing currently except report a wrong call */
		return OPL_ERROR;
	}

	return OPL_OK;
}

/*******************************************************************************
*
* i2cWrite
*
* DESCRIPTION:
* 	this function transmits data to I2C bus via the IPMux I2C Controller. the function first detect
*	I2CC work mode, and initialize the transfer data accordingly.
*
*	Note: for a special case, it is needed that a read operation is followed the write, the MSB of
*	parameter slcAddr is used to be such indicator:
*	slvAddr & 0x80 == 0x80		no stop is issued after data is written;
*	slvAddr & 0x80 == 0x00		normal write operation, stop is issued;
*
* INPUTS:
*	slvAddr	 - slave device address, only valid for master.
*	nByte	 - biffer size in bytes.
*	pDataBuf	 - pointer to data byte array.
*
* OUTPUT:
*	n/a.
*
* RETURNS:
* 	# of data byte written successfully, or ERROR CODE:
*	OPL_ERROR
*	OPL_I2C_TIME_OUT
*	OPL_I2C_ARBITRATION_LOST
*	OPL_I2C_SLV_WRITE_MORE_DATA_NEED
*
* SEE ALSO: 
*/
int i2cWrite (UINT8 slvAddr, UINT32 nByte, UINT8 *pDataBuf )
	{
	UINT8 regAddr = (slvAddr << REG_BIT_I2CADR_ADR_OFF) | I2C_WRITE;
	UINT8 statusCode;
	int i = OPL_ZERO;
	UINT32 regVal = OPL_ZERO;
	int bErrorState = OPL_ZERO;

	/* check master or slave mode */
	if (ipMuxI2CC.masterMode)
		{
		/* for master mode */
		if (ipMuxRegBitTest((UINT32) REG_I2CSR, (UINT32) REG_BIT_I2CSR_BUSY))
			{
			opl_log (("Ieee, I2C data bus is busy, you cannot Tx data before it is normal.\n"));
			return OPL_ERROR;
		}

		/* step1: set STA */
		ipMuxRegBitSet (REG_I2CCON, REG_BIT_I2CCON_STA);
		
		/* wait SIO status */
		statusCode = i2cStatusGet();
		
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("wait SIO Interrupt time out after STA, write to I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;
			case 0x08:	/* A START condition has been transmitted */
			case 0x10:	/* A repeated START condition has been transmitted */
					/* step2: write SLA+W */
					/* ipMuxRegWrite (REG_I2CADR, regAddr); */
				ipMuxRegWrite (REG_I2CDAT, regAddr);

					/* yzhang, 20050919, add according to the registers 4-7 */
				ipMuxRegRead(REG_I2CEN, &regVal);
				regVal &= ~(REG_BIT_I2CEN_RD);	/* clear RD bit */
				regVal |= REG_BIT_I2CEN_WR;		/* set WR bit */
				ipMuxRegWrite(REG_I2CEN, regVal);

				i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */
				break;
			case 0x00:	/* Bus error due to illegal START or STOP condition */
			case 0x70:	/* Bus error SDA stuck LOW */
			case 0x90:	/* Bus error SCL stuck LOW */
			default:
				opl_log (("1inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
				return OPL_ERROR;
		}

		/* wait SIO status */
		statusCode = i2cStatusGet();
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("wait SIO Interrupt time out after SLV+W, write to I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;
			case 0x18:	/* SLA+W has been transmitted; ACK has been received */
			case 0x20:	/* SLA+W has been transmitted; NO ACK has been received */
					/* step3: write data byte by byte */
				for (i = 0; !bErrorState && (i < nByte); i++)
					{
					/*I2C_WAIT_NO_TRANSFER(5);*/
					ipMuxRegWrite (REG_I2CDAT, pDataBuf[i]);

						/* yzhang, 20050919, add according to the registers 4-7 */
					ipMuxRegRead(REG_I2CEN, &regVal);
					regVal &= ~(REG_BIT_I2CEN_RD);	/* clear RD bit */
					regVal |= REG_BIT_I2CEN_WR;		/* set WR bit */
					ipMuxRegWrite(REG_I2CEN, regVal);

					i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */

						/* wait SIO status */
					statusCode = i2cStatusGet();
					switch (statusCode)
						{
						case OPL_ERROR:
							opl_log (("wait SIO Interrupt time out after DATA[%d], write to I2C controller failed.\n", i));
							return OPL_I2C_TIME_OUT;
						case 0x28:	/* Data byte in I2CDAT has been transmitted;ACK has been received */
						case 0x30:	/* Data byte in I2CDAT has been transmitted;NO ACK has been received */
							break;
						case 0x38:	/* Arbitration lost in NOT ACK bit */
							opl_log (("arbitration lost in data byte[%d].\n", i));
							/*return OPL_I2C_ARBITRATION_LOST;*/
							bErrorState = statusCode; 
						default:
							opl_log (("Bad Status Code received %x when in Master Tx mode, data[%d], stopped.\n", statusCode, i));
							bErrorState = statusCode; 

					}
				}
				break;
			default:
				opl_log (("2inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
				/*return OPL_ERROR;*/
				bErrorState = statusCode; 
		}

		if (opwaycltIfI2cSTOStatusIssue() /* OPWAYCNU_REG_I2C_READ_FLAG */)/*(0x80 & slvAddr)*/ /* yzhang added, 20070213 */
		{
			opl_log (("No STO issued after %d data transmitted, and returned immediately.\n", i));
			i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */
			return i;
			}
		else
		{
			/* step4: write STO */
	                ipMuxRegRead(REG_I2CCON, &regVal);
	                regVal &= ~(REG_BIT_I2CCON_STA);  	/* clear STA bit */
	                regVal |= REG_BIT_I2CCON_STO;           /* set STO bit, STO bit is read-cleard */
	                ipMuxRegWrite(REG_I2CCON, regVal);
		}
		i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */

		/* wait SIO status */
		statusCode = i2cStatusGet();
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("wait SIO Interrupt time out after STO, write to I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;
			case 0xF8:	/* A STOP condition has been transmitted */
					/* write finished */
				opl_log (("write to slave device %x# ok, %d of %d bytes transmitted.\n", slvAddr, i, nByte));
				return i;
			default:
				opl_log (("3inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
				return OPL_ERROR;
		}

	}
	else
		{
		/* for slave mode */

		/* step1: wait SIO status */
		statusCode = i2cStatusGet();
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("get SIO Status for slave transmition time out, write to I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;
			case 0xA8:	/* Own SLA+R has been received; ACK has been returned */
			case 0xB0:	/* Own SLA+R has been received; ACK has been returned */
			case 0xb8:  /* add by xwang : Data byte in I2CDAT has been transmitted; ACK has been received */
					/* step2: write data byte by byte */
				for (i = 0; i < nByte; i++)
					{
					ipMuxRegWrite (REG_I2CDAT, pDataBuf[i]);
					opl_log (("I2C Controller sent DATA[%d]=0x%x, \'%c\'.\n", i, pDataBuf[i], pDataBuf[i]));

			                      /* yzhang, 20050919, add according to the registers 4-7 */
			                ipMuxRegRead(REG_I2CEN, &regVal);
		                	regVal &= ~(REG_BIT_I2CEN_RD);  /* clear RD bit */
	                		regVal |= REG_BIT_I2CEN_WR;             /* set WR bit */
			                ipMuxRegWrite(REG_I2CEN, regVal);

					i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */

						/* wait SIO status */
					statusCode = i2cStatusGet();
					switch (statusCode)
						{
						case OPL_ERROR:
							opl_log (("wait SIO Interrupt time out after DATA[%d], write to I2C controller failed.\n", i));
							return OPL_I2C_TIME_OUT;
						case 0xB8:	/* Data byte in I2CDAT has been transmitted;ACK has been received */
						case 0xC0:	/* Data byte in I2CDAT has been transmitted;NO ACK has been received */
							break;
						case 0xC8:	/* Last data byte in I2CDAT has been transmitted (AA = 0); ACK has been received */
						case 0xF8:	/* Last data byte in I2CDAT has been transmitted (AA = 0); ACK has been received */
							opl_log (("remote master stop data receive after DATA[%d].\n", i));

							/* write finished, yet not completedly */
							opl_log (("slave write ok, %d of %d bytes transmitted.\n", i + 1, nByte));
							return i + 1;
					}
				}

				/* check if the more data still in want by remote master */
				/*if ((0xB8 == statusCode) || (0xC0 == statusCode))*/
				if (0xC0 == statusCode)
					{
					/* write finished */
					opl_log (("slave write stopped while more data are still in want, %d of %d bytes transmitted.\n", i, nByte));
					return OPL_I2C_SLV_WRITE_MORE_DATA_NEED;
				}

				/* write finished */
				opl_log (("slave write ok, %d of %d bytes transmitted.\n", i, nByte));
				return i;
			default:
				opl_log (("4inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
				return OPL_ERROR;
		}
	}
}

/*******************************************************************************
*
* i2cWriteRepeat
*
* DESCRIPTION:
* 	this function transmit data to I2C bus via the IPMux I2C Controller. It should be called
*	only when OPL_I2C_SLV_WRITE_MORE_DATA_NEED returned in slave mode.
*
* INPUTS:
*	nByte	 - biffer size in bytes.
*	pDataBuf	 - pointer to data byte array.
*
* OUTPUT:
*	pDataBuf	 - pointer to data byte received.
*
* RETURNS:
* 	# of data byte read successfully, or ERROR CODE:
*	OPL_ERROR
*	OPL_I2C_TIME_OUT
*	OPL_I2C_SLV_WRITE_MORE_DATA_NEED
*
* SEE ALSO: 
*/
int i2cWriteRepeat (UINT32 nByte, UINT8 *pDataBuf )
	{
	UINT8 statusCode;
	int i;
	UINT32 regVal = OPL_ZERO;

	/* check master or slave mode */
	if (ipMuxI2CC.masterMode)
		{
		opl_log (("this function should be called in master mode.\n"));
		return OPL_ERROR;
	}
	else
		{
		/* for slave mode */

		/* step1: wait SIO status */
		statusCode = i2cStatusGet();
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("get SIO Status for slave transmition time out, repeat write to I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;
			case 0xB8:	/* Data byte in I2CDAT has been transmitted;ACK has been received */
			case 0xC0:	/* Data byte in I2CDAT has been transmitted;NO ACK has been received */
					/* step2: write data byte by byte */
				for (i = 0; i < nByte; i++)
					{
					ipMuxRegWrite (REG_I2CDAT, pDataBuf[i]);
                                        opl_log (("I2C Controller sent DATA[%d]=0x%x, \'%c\'.\n", i, pDataBuf[i], pDataBuf[i]));
                                                                                                                            
                                              /* yzhang, 20050919, add according to the registers 4-7 */
                                        ipMuxRegRead(REG_I2CEN, &regVal);
                                        regVal &= ~(REG_BIT_I2CEN_RD);  /* clear RD bit */
                                        regVal |= REG_BIT_I2CEN_WR;             /* set WR bit */
                                        ipMuxRegWrite(REG_I2CEN, regVal);

					i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */

						/* wait SIO status */
					statusCode = i2cStatusGet();
					switch (statusCode)
						{
						case OPL_ERROR:
							opl_log (("wait SIO Interrupt time out after DATA[%d], write to I2C controller failed.\n", i));
							return OPL_I2C_TIME_OUT;
						case 0xB8:	/* Data byte in I2CDAT has been transmitted;ACK has been received */
						case 0xC0:	/* Data byte in I2CDAT has been transmitted;NO ACK has been received */
							break;
						case 0xC8:	/* Last data byte in I2CDAT has been transmitted (AA = 0); ACK has been received */
						case 0xF8:	/* Last data byte in I2CDAT has been transmitted (AA = 0); ACK has been received */
							opl_log (("remote master stop data receive after DATA[%d].\n", i));

							/* write finished, yet not completedly */
							opl_log (("slave write ok, %d of %d bytes transmitted.\n", i + 1, nByte));
							return i + 1;
					}
				}

				/* check if the more data still in want by remote master */
				/*if ((0xB8 == statusCode) || (0xC0 == statusCode))*/
				if (0xC0 == statusCode)
					{
					/* write finished */
					opl_log (("slave repeat write stopped while more data are still in want, %d of %d bytes transmitted.\n", i, nByte));
					return OPL_I2C_SLV_WRITE_MORE_DATA_NEED;
				}
				
				/* repeat write finished, might never come here. */
				opl_log (("slave write ok, %d of %d bytes transmitted.\n", i, nByte));
				return i;
			default:
				opl_log (("inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
				return OPL_ERROR;
		}
	}
}


/*******************************************************************************
*
* i2cRead
*
* DESCRIPTION:
* 	this function receives data from I2C bus via the IPMux I2C Controller. the function first detect
*	I2CC work mode, and initialize the receiver accordingly.
*
* INPUTS:
*	slvAddr	 - slave device address, only valid for master.
*	nByte	 - biffer size in bytes.
*	pDataBuf	 - pointer to data byte array.
*
* OUTPUT:
*	pDataBuf	 - pointer to data byte received.
*
* RETURNS:
* 	# of data byte read successfully, or ERROR CODE:
*	OPL_ERROR
*	OPL_I2C_TIME_OUT
*	OPL_I2C_ARBITRATION_LOST
*	OPL_I2C_SLV_READ_BUFFER_TOO_SMALL
*
* SEE ALSO: 
*/
int i2cRead (UINT8 slvAddr, UINT32 nByte, UINT8 *pDataBuf )
	{
	UINT8 regAddr = (slvAddr << REG_BIT_I2CADR_ADR_OFF) | I2C_READ;
	UINT8 statusCode;
	UINT32 regData;
	int i;
	UINT32 regVal = OPL_ZERO;
	int bErrorState = OPL_ZERO;

	/* check master or slave mode */
	if (ipMuxI2CC.masterMode)
		{
		/* for master mode */
	
		/* step1: set STA */
		ipMuxRegBitSet (REG_I2CCON, REG_BIT_I2CCON_STA | REG_BIT_I2CCON_AA);
		
		/* wait SIO status */
		statusCode = i2cStatusGet();
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("wait SIO Interrupt time out after STA, read from I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;
			case 0x08:	/* A START condition has been transmitted */
			case 0x10:	/* A repeated START condition has been transmitted */
					/* step2: write SLA+R */
				ipMuxRegWrite (REG_I2CDAT, regAddr);

			                /* yzhang, 20050919, add according to the registers 4-7 */
			        ipMuxRegRead(REG_I2CEN, &regVal);
			        regVal &= ~(REG_BIT_I2CEN_RD);  	/* clear RD bit */
			        regVal |= REG_BIT_I2CEN_WR;             /* set WR bit */
			        ipMuxRegWrite(REG_I2CEN, regVal);
				i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */
				break;
			default:
				opl_log (("inaptitude Status Code received: %x#, read from I2C controller failed.\n", statusCode));
				return OPL_ERROR;
		}

		/* wait SIO status */
		statusCode = i2cStatusGet();
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("wait SIO Interrupt time out after SLV+R, read from I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;

			case 0x40:	/* SLA+R has been transmitted; ACK has been received */
			case 0x48:	/* SLA+R has been transmitted; NO ACK has been received */
					/* step3: read data byte by byte */

				for (i = 0; !bErrorState && (i < nByte); i++)
					{
					#define ACCESS_DATA_PRIOR_STATUS
					/*
					 *	Note: this MACRO is defined to switch between two method
					 *	CNU version 20070213 dose not support ACCESS_DATA_PRIOR_STATUS
					 *	please undefined the MACRO
					 */
					#undef ACCESS_DATA_PRIOR_STATUS

					/* the last byte does not need ACK, 20070531 */
					if (i == (nByte - 1))
					{
						ipMuxRegBitClear (REG_I2CCON, REG_BIT_I2CCON_AA);
					}
					
                                        /* yzhang, 20050919, add according to the registers 4-7 */
                                   ipMuxRegRead(REG_I2CEN, &regVal);
       	                     regVal &= ~(REG_BIT_I2CEN_WR);          /* clear WR bit */
                	              regVal |= REG_BIT_I2CEN_RD;             /* set RD bit */
                        	       ipMuxRegWrite(REG_I2CEN, regVal);
#ifdef ACCESS_DATA_PRIOR_STATUS
					ipMuxRegRead (REG_I2CDAT, &regData);
					pDataBuf[i] = (UINT8)regData;
					opl_log (("Get byte[%d]=0x%x,\'%c\'.\n", i, pDataBuf[i], (char)pDataBuf[i]));
#endif
					i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */

						/* wait SIO status */
					statusCode = i2cStatusGet();
					switch (statusCode)
						{
						case OPL_ERROR:
							opl_log (("wait SIO Interrupt time out after DATA[%d], read from I2C controller failed.\n", i));
							return OPL_I2C_TIME_OUT;
						case 0x50:	/* Data byte in I2CDAT has been received;ACK has been received */
						case 0x58:	/* Data byte in I2CDAT has been received;NO ACK has been received */
#ifndef ACCESS_DATA_PRIOR_STATUS
							ipMuxRegRead (REG_I2CDAT, &regData);
							pDataBuf[i] = (UINT8)regData;
							opl_log (("Get byte[%d]=0x%x,\'%c\'.\n", i, pDataBuf[i], (char)pDataBuf[i]));
#endif
							break;
						case 0x38:	/* Arbitration lost in NOT ACK bit  */
							opl_log (("arbitration lost after data byte[%d] read.\n", i));
							return OPL_I2C_ARBITRATION_LOST;
						default:
							opl_log (("Bad status code %x got while Master Rx, data byte[%d], stopped.\n", statusCode, i));
							bErrorState = statusCode;
					}
				}
				break;
			case 0x38:	/* Arbitration lost in SLA+R */
				opl_log (("arbitration lost in SLA+R.\n"));
				return OPL_I2C_ARBITRATION_LOST;
			default:
				opl_log (("inaptitude Status Code received: %x#, write to I2C controller failed.\n", statusCode));
				return OPL_ERROR;
		}

		/* step4: write STO */
                ipMuxRegRead(REG_I2CCON, &regVal);
                regVal &= ~(REG_BIT_I2CCON_STA | REG_BIT_I2CCON_AA);        /* clear STA bit */
                regVal |= REG_BIT_I2CCON_STO;           /* set STO bit, STO bit is read-cleard */
                ipMuxRegWrite(REG_I2CCON, regVal);
		i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */

		/* wait SIO status */
		statusCode = i2cStatusGet();
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("wait SIO Interrupt time out after STO, read from I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;
			case 0xF8:	/* A STOP condition has been transmitted */
					/* write finished */
				opl_log (("read from slave device %x# ok, %d bytes received.\n", slvAddr, i));
				return i;
			default:
				opl_log (("inaptitude Status Code received: %x#, read from I2C controller failed.\n", statusCode));
				return OPL_ERROR;
		}

	}
	else
		{
		/* for slave mode */

       			/* yzhang, 20050919, add according to the registers 4-7 */
	        ipMuxRegRead(REG_I2CEN, &regVal);
	        regVal &= ~(REG_BIT_I2CEN_WR);  	/* clear WR bit */
	        regVal |= REG_BIT_I2CEN_RD;             /* set RD bit */
	        ipMuxRegWrite(REG_I2CEN, regVal);

		/* step1: wait SIO status */
		statusCode = i2cStatusGet();
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("get SIO Status for slave transmition time out, read from I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;
			case 0x68:	/* Own SLA+W has been received; ACK has been returned */
			case 0x60:	/* Own SLA+W has been received; ACK has been returned */
					/* step2: read data byte by byte */
				for (i = 0; i < (nByte + 1); i++)
					{
					ipMuxRegRead (REG_I2CDAT, &regData);
					pDataBuf[i] = (UINT8)regData;
					opl_log (("Get byte=0x%x,\'%c\'.\n", pDataBuf[i], (char)pDataBuf[i]));
					i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */

						/* wait SIO status */
					statusCode = i2cStatusGet();
					switch (statusCode)
						{
						case OPL_ERROR:
							opl_log (("wait SIO Interrupt time out after DATA[%d], read from I2C controller failed.\n", i));
							return OPL_I2C_TIME_OUT;
						case 0x88:	/* Data byte in I2CDAT has been received;NO ACK has been received */
						case 0x80:	/* Data byte in I2CDAT has been received;ACK has been received */
							break;
						case 0xA0:	/* A STOP condition or repeated START condition has been received while still addressed as SLV/REC */
						case 0xF8:	/* A STOP condition or reset condition has been received*/
							i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */
							opl_log (("remote master stop data transfer after DATA[%d].\n", i));

							/* read finished */
							opl_log (("slave read ok, %d bytes received.\n", i + 1));
							return i;
					}
				}

				/* check if the buffer overflow */
				if ((0x80 == statusCode) || (0x88 == statusCode))
					{
					/* read finished */
					opl_log (("slave read stopped for overflow of receive buffer, %d of %d bytes received.\n", i, nByte));
					return OPL_I2C_SLV_READ_BUFFER_TOO_SMALL;
				}
				
				/* read finished, might never come here. */
				opl_log (("slave read ok, %d bytes received.\n", i));
				return i;
			/* add by xwang : in polling mode, just one byte can be received because their is no fifo in IPMux I2C */
			case 0x80:	/* Data byte in I2CDAT has been received;ACK has been received */

				ipMuxRegRead (REG_I2CDAT, &regData);
				pDataBuf[0] = (UINT8)regData;
				i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */
				return 1;
				break;
			default:
				opl_log (("inaptitude Status Code received: %x#, read from I2C controller failed.\n", statusCode));
				return OPL_ERROR;
		}
	}
}

/*******************************************************************************
*
* i2cReadRepeat
*
* DESCRIPTION:
* 	this function receives data from I2C bus via the IPMux I2C Controller. It should be called
*	only when OPL_I2C_SLV_READ_BUFFER_TOO_SMALL returned in slave mode.
*
* INPUTS:
*	nByte	 - biffer size in bytes.
*	pDataBuf	 - pointer to data byte array.
*
* OUTPUT:
*	pDataBuf	 - pointer to data byte received.
*
* RETURNS:
* 	# of data byte read successfully, or ERROR CODE:
*	OPL_ERROR
*	OPL_I2C_TIME_OUT
*	OPL_I2C_SLV_READ_BUFFER_TOO_SMALL
*
* SEE ALSO: 
*/
int i2cReadRepeat (UINT32 nByte, UINT8 *pDataBuf )
	{
	UINT8 statusCode;
	UINT32 regData;
	int i;

	/* check master or slave mode */
	if (ipMuxI2CC.masterMode)
		{
		opl_log (("this function should be called in master mode.\n"));
		return OPL_ERROR;
	}
	else
		{
		/* for slave mode */

		/* step1: wait SIO status */
		statusCode = i2cStatusGet();
		switch (statusCode)
			{
			case OPL_ERROR:
				opl_log (("get SIO Status for slave transmition time out, repeat read from I2C controller failed.\n"));
				return OPL_I2C_TIME_OUT;
			case 0x88:	/* Data byte in I2CDAT has been received;NO ACK has been received */
			case 0x80:	/* Data byte in I2CDAT has been received;ACK has been received */
					/* step2: read data byte by byte */
				for (i = 0; i < nByte; i++)
					{
					ipMuxRegRead (REG_I2CDAT, &regData);
					pDataBuf[i] = (UINT8)regData;
					i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */

						/* wait SIO status */
					statusCode = i2cStatusGet();
					switch (statusCode)
						{
						case OPL_ERROR:
							opl_log (("wait SIO Interrupt time out after DATA[%d], read from I2C controller failed.\n", i));
							return OPL_I2C_TIME_OUT;
						case 0x88:	/* Data byte in I2CDAT has been received;NO ACK has been received */
						case 0x80:	/* Data byte in I2CDAT has been received;ACK has been received */
							break;
						case 0xA0:	/* A STOP condition or repeated START condition has been received while still addressed as SLV/REC */
						case 0xF8:	/* A STOP condition or reset condition has been received*/
							i2cInterruptClear();	/* clear SIO interrupt and let SIO continue */
							opl_log (("remote master stop data transfer after DATA[%d].\n", i));

							/* read finished */
							opl_log (("slave read ok, %d bytes received.\n", i + 1));
							return i;
					}
				}

				/* check if the buffer overflow */
				if ((0x80 == statusCode) || (0x88 == statusCode))
					{
					/* read finished */
					opl_log (("slave repeat read stopped for overflow of receive buffer, %d of %d bytes received.\n", i, nByte));
					return OPL_I2C_SLV_READ_BUFFER_TOO_SMALL;
				}
				
				/* repeat read finished, might never come here. */
				opl_log (("slave repeat read ok, %d bytes received.\n", i));
				return i;
			default:
				opl_log (("inaptitude Status Code received: %x#, repeat read from I2C controller failed.\n", statusCode));
				return OPL_ERROR;
		}
	}
}

/*******************************************************************************
*
* ipmuxI2CInit	-initial routine for IPMux I2C
*
* DESCRIPTION:
* 	Call this function to start I2C device on IPMux.
*	in this function, 
*
* INPUTS:
*	i2cMode	- zero: slave mode; non-zero: master mode. 
*	intMode	- 0: polling mode; 1: interrupt mode.
*	slvAddr	- if slave mode, this is the slave address.
*	nRate	- see below:
*		I2C_SCF_330:	 0
*		I2C_SCF_288:	 1
*		I2C_SCF_217:	 2
*		I2C_SCF_146:	 3
*		I2C_SCF_88:		 4
*		I2C_SCF_59:		 5
*		I2C_SCF_44:		 6
*		I2C_SCF_36:		 7
*
*
* OUTPUT:
*	n/a.
*
* RETURNS:
*	OPL_OK.
*
* SEE ALSO: 
*/
int ipmuxI2CInit (int i2cMode, int intMode, UINT8 slvAddr, int nRate)
	{

	/* reset the I2C first */
	i2cControllerReset ();
	i2cControllerReset ();

	/* interrupt Mode */
	i2cInterruptEnable(intMode);

	/* master or slave */
	if (i2cMode)
		{
		opl_log (("IPMux I2C is in master mode\n"));
		i2cMasterModeSet(nRate /* I2C_SCF_330 */); 
	}
	else
	{
		opl_log (("IPMux I2C is in slave mode\n"));
		i2cSlaveModeSet(slvAddr);
	}
	i2cControllerEnable ();

	opl_log (("IPMux I2C initialized ok.\n"));
	return OPL_OK;
}

/*******************************************************************************
*
* i2cpr         -Multiple Read I2C device
*
* DESCRIPTION:
*        read registers and print the value of specified register to STANDARD OUTPUT device.
*
* INPUTS:
*        regid         - register ID.
*        num          - number of registers.
*
* OUTPUT:
*        n/a.
*
* RETURNS:
*       OPL_OK, if the read successful.
*
* SEE ALSO: 
*/
UINT8 i2cpr16 (UINT8 slave,UINT16 regisd)
{
	UINT8 pVal;
	int nRet;
	UINT32 i = 2000;
	UINT8 u8Buf[3];
	u8Buf[0]= (UINT8)(regisd/0x100);
	u8Buf[1]= (UINT8)(regisd%0x100);
	
	OPWAYCNU_REG_I2C_READ_FLAG_SET;
	nRet = i2cWrite( (OPWAYEPROM_I2C_ADDR | slave), (UINT32) 2, (UINT8 *) u8Buf);
	OPWAYCNU_REG_I2C_READ_FLAG_CLEAR;
	
	if (OPL_ZERO >= nRet)
		{
		opl_log (("IPMux I2C write failed, %d.\n",  nRet));
		return OPL_ERROR;
	}
	while(i-->0);
	nRet = i2cRead( (OPWAYEPROM_I2C_ADDR | slave), (UINT32) 1, (UINT8 *) &pVal);
	if (OPL_ZERO >= nRet)
		{
		printf (("IPMux I2C read failed, %d.\n",  nRet));
		return OPL_ERROR;
	}
	opl_log (("Read [0x%02x]EPROM address at 0x%02x%02x, value= 0x%02x.\n",(OPWAYEPROM_I2C_ADDR | slave), u8Buf[0],u8Buf[1], pVal));
	return pVal;
}
 
/*******************************************************************************
*
* i2cpw         -Multiple Read I2C device
*
* DESCRIPTION:
*        read registers and print the value of specified register to STANDARD OUTPUT device.
*
* INPUTS:
*        regid         - register ID.
*        num          - number of registers.
*
* OUTPUT:
*        n/a.
*
* RETURNS:
*       OPL_OK, if the read successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cpw16 (UINT8 slave,UINT16 regisd, UINT8 val)
{
	int nRet;
	UINT8 u8Buf[3];

	/* put data to buffer */
	u8Buf[0] = (UINT8)(regisd/0x100);
	u8Buf[1] = (UINT8)(regisd%0x100);
	u8Buf[2] = val;

	/* write the buffer to I2C device */
	nRet = i2cWrite( (OPWAYEPROM_I2C_ADDR | slave), 3 , (UINT8 *) u8Buf);
	if (OPL_ZERO >= nRet)
		{
		opl_log (("IPMux I2C write failed, %d.\n",  nRet));
		return OPL_ERROR;
	}

	opl_log (("Write [0x%02x]EPROM address at 0x%02x%02x, value= 0x%02x.\n",(OPWAYEPROM_I2C_ADDR | slave), u8Buf[0],u8Buf[1], (UINT32)val));
	return OPL_OK;
}

/*******************************************************************************
*
* i2cpr         -Multiple Read I2C device
*
* DESCRIPTION:
*        read registers and print the value of specified register to STANDARD OUTPUT device.
*
* INPUTS:
*        regid         - register ID.
*        num          - number of registers.
*
* OUTPUT:
*        n/a.
*
* RETURNS:
*       OPL_OK, if the read successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cpr8 (UINT8 slave,UINT16 regisd)
{
	UINT8 pVal;
	int nRet;
	UINT32 i = 2000;
	UINT8 u8Buf[3];
	u8Buf[0]= (UINT8)regisd;

	printf ("Read [0x%02x]EPROM address at 0x%02x\n",slave, u8Buf[0]);
	
	OPWAYCNU_REG_I2C_READ_FLAG_SET;
	nRet = i2cWrite( slave, (UINT32) 1, (UINT8 *) u8Buf);
	OPWAYCNU_REG_I2C_READ_FLAG_CLEAR;
	
	if (OPL_ZERO >= nRet)
		{
		printf ("IPMux I2C write failed, %d.\n",  nRet);
		return OPL_ERROR;
	}
	while(i-->0);
	nRet = i2cRead( slave, (UINT32) 1, (UINT8 *) &pVal);
	if (OPL_ZERO >= nRet)
		{
		printf ("IPMux I2C read failed, %d.\n",  nRet);
		return OPL_ERROR;
	}
	printf ("Read [0x%02x]EPROM address at 0x%02x, value= 0x%02x.\n",slave, u8Buf[0], pVal);
	return OPL_OK;
}
 
/*******************************************************************************
*
* i2cpw         -Multiple Read I2C device
*
* DESCRIPTION:
*        read registers and print the value of specified register to STANDARD OUTPUT device.
*
* INPUTS:
*        regid         - register ID.
*        num          - number of registers.
*
* OUTPUT:
*        n/a.
*
* RETURNS:
*       OPL_OK, if the read successful.
*
* SEE ALSO: 
*/
OPL_STATUS i2cpw8 (UINT8 slave,UINT16 regisd, UINT8 val)
{
	int nRet;
	UINT8 u8Buf[3];

	/* put data to buffer */
	u8Buf[0] = (UINT8)regisd;
	u8Buf[1] = val;

	/* write the buffer to I2C device */
	nRet = i2cWrite( slave, 2 , (UINT8 *) u8Buf);
	if (OPL_ZERO >= nRet)
		{
		printf ("IPMux I2C write failed, %d.\n",  nRet);
		return OPL_ERROR;
	}

	printf ("Write [0x%02x]EPROM address at 0x%02x, value= 0x%02x.\n", slave, u8Buf[0], (UINT32)val);
	return OPL_OK;
}
/*******************************************************************************
*
* i2cReadRandom - i2c Random Read
*
* DESCRIPTION:
*   this function receives data from I2C bus via the I2C Controller. the function first detect
*   I2CC work mode, and initialize the receiver accordingly.
*
* INPUTS:
*   slvAddr  - slave device address, only valid for master.
*   staRegAddr  - start register address, only valid for master.
*   nByte    - biffer size in bytes.
*   pDataBuf     - pointer to data byte array.
*
* OUTPUT:
*   pDataBuf     - pointer to data byte received.
*
* RETURNS:
*   # of data byte read successfully, or ERROR CODE:
*   OPL_ERROR
*   OPL_I2C_TIME_OUT
*   OPL_I2C_ARBITRATION_LOST
*   OPL_I2C_SLV_READ_BUFFER_TOO_SMALL
*
* SEE ALSO: 
*/
int i2cReadRandom (UINT8 slvAddr, UINT8 staRegAddr, UINT32 nByte, UINT8 *pDataBuf )
{
    int retVal = 0;
	/* check master or slave mode */
	if (ipMuxI2CC.masterMode)
	{
		/* for master mode */
        retVal = i2cWrite(slvAddr, 1, &staRegAddr);
        if (0 > retVal)
        {
            OPL_DRV_PRINTF (("I2C write start register address failed. (%d)\n", retVal));
            return retVal;
        }
	}
    return i2cRead(slvAddr, nByte, pDataBuf);
}

OPL_STATUS drvI2cDevWr (int nBlock, UINT8 nOffset, int nByte, char * pDataBuf)
{
    /*UINT8 slvAddr = (0xa << 3) | ((UINT8)nBlock & 0x7);*/
    UINT8 slvAddr = OPWAYEPROM_I2C_ADDR | nBlock ;
    UINT8 pDataOffBuf[64];

    if(nByte>32)
    {
        nByte = 32;
        printf ("nByte must be less than 32Bytes\n\r");
    }
    pDataOffBuf[0] = nOffset;
    memcpy ((void *)((unsigned int)pDataOffBuf + 1), (const void *)pDataBuf, (unsigned int)nByte);
    return  i2cWrite(slvAddr, 1 + nByte, (UINT8 *) &pDataOffBuf);

}

OPL_STATUS drvI2cDevRd (UINT8 slvAddr, UINT8 staRegAddr, UINT32 nByte, UINT8 *pDataBuf)
{
    return i2cReadRandom ( (OPWAYEPROM_I2C_ADDR | slvAddr),  staRegAddr,  nByte,  pDataBuf );
}

void RdEpromTest()
{
    UINT8 p[16];
    volatile UINT32 i=0;
    UINT8 tmp[16]={0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};
    UINT8 tmp1[16]={0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    ipmuxI2CInit (1, 0, 0, 0);

#if 0
        printf("\n\nclk 0x40\n");
    oplRegWrite(OPL_REG_I2C_CTRL, 0x40);
    drvI2cDevWr(0,0,8,&tmp[0]);
usleep(10000);
drvI2cDevRd(0,0,16,p);
    for(i=0;i<16;i++)
    {
        printf("p[%d]=0x%x\n",i,p[i]);
    }
    
        printf("\n\nclk 0x41\n");
    oplRegWrite(OPL_REG_I2C_CTRL, 0x41);
    drvI2cDevWr(0,0,8,&tmp1[0]);
usleep(10000);
drvI2cDevRd(0,0,16,p);
    for(i=0;i<16;i++)
    {
        printf("p[%d]=0x%x\n",i,p[i]);
    }

        printf("\n\nclk 0x42\n");
    oplRegWrite(OPL_REG_I2C_CTRL, 0x42);
    drvI2cDevWr(0,0,8,&tmp[0]);
usleep(10000);
drvI2cDevRd(0,0,16,p);
    for(i=0;i<16;i++)
    {
        printf("p[%d]=0x%x\n",i,p[i]);
    }

        printf("\n\nclk 0x43\n");
    oplRegWrite(OPL_REG_I2C_CTRL, 0x43);
    drvI2cDevWr(0,0,8,&tmp1[0]);
usleep(10000);
drvI2cDevRd(0,0,16,p);
    for(i=0;i<16;i++)
    {
        printf("p[%d]=0x%x\n",i,p[i]);
    }
#endif

}

