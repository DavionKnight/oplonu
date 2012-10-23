/******************************************************************************/
/**  COPYRIGHT (C) 2003- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/
/*******************************************************************************
 **    MODULE     : IPMux mii, including MII read, write function.
 **
 **    FILE       : ipmux_mdio.c
 **
 **    DESCRIPTION: This file contains IPMux MII configuration driver utility function.
 **
 **
 **    NOTES      :
 **
 *******************************************************************************/
/*
 **    MODIFICATION HISTORY:
 **    Author            			Date				Description of Changes
 **    ----------------  	------------  	----------------------------------------------
 **    Jiang Mingli            	07/06/09      	                Initial Version
 **
 */

/* includes */
#include <common.h>
#include "opl_driver.h"
#include "ipmux_misc.h"
#include "opl_debug.h"
#include "ipmux_mdio.h"


OPL_STATUS oplRegFieldRead(UINT32 regAddr,UINT16 fieldOffset,UINT16 fieldWidth,UINT32 *data0)
{
	OPL_STATUS retVal;
	UINT32 regVal;
	UINT32 fieldMask;
	
	if(OPL_NULL == data0)
	{
		opl_log(("oplRegFieldRead: data0 NULL\n"));
		return OPL_ERROR;
	}

	if((fieldOffset>31)||(fieldWidth<1)||(fieldWidth>32)||((fieldOffset + fieldWidth)>32))
	{
		opl_log(("error offset %d,error width %d\n",fieldOffset,fieldWidth));
		return OPL_ERROR;
	}

	if(fieldWidth == 32)
	{
		fieldMask = 0xFFFFFFFF;
	}else
	{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}
	
#ifdef HW_DEBUG
	printf("fieldMask = %08x\n",fieldMask);
#endif

	
	ipMuxRegRead(regAddr,&regVal); 
//	printf("ipMuxRegRead: regAddr = %08x, regVal = %08x\n",regAddr, regVal);

#ifdef HW_DEBUG
	printf("regVal = %08x\n",regVal);
#endif

	regVal = regVal&fieldMask;

#ifdef HW_DEBUG
	printf("regVal = %08x\n",regVal);
#endif

	*data0 = regVal>>fieldOffset;
#ifdef HW_DEBUG
	printf("data0 = %08x\n",*data0);
#endif
	return OPL_OK;	
}

OPL_STATUS oplRegFieldWrite(UINT32 regAddr,UINT16 fieldOffset,UINT16 fieldWidth,UINT32 data0)
{
	OPL_STATUS retVal;
	UINT32 regVal;
	UINT32 oldVal;
	UINT32 fieldMask;

	if((fieldOffset>31)||(fieldWidth<1)||(fieldWidth>32)||((fieldOffset + fieldWidth)>32))
	{
		opl_log(("error offset %d,error width %d\n",fieldOffset,fieldWidth));
		return OPL_ERROR;
	}

	if(fieldWidth == 32)
	{
		fieldMask = 0xFFFFFFFF;
	}else
	{
		fieldMask = (~(0XFFFFFFFF<<fieldWidth))<<fieldOffset;
	}
	
#ifdef HW_DEBUG
	printf("fieldMask = %08x\n",fieldMask);
#endif

	ipMuxRegRead(regAddr,&oldVal);
//	printf("ipMuxRegRead: regAddr = %08x, regVal = %08x\n",regAddr, regVal);

#ifdef HW_DEBUG
	printf("oldVal = %08x\n",oldVal);
#endif


	oldVal = oldVal&(~fieldMask);

#ifdef HW_DEBUG
	printf("oldVal = %08x\n",oldVal);
#endif


	regVal = (data0<<fieldOffset)&fieldMask;

#ifdef HW_DEBUG
	printf("regVal = %08x\n",regVal);
#endif


	regVal = regVal|oldVal;
#ifdef HW_DEBUG
	printf("regVal = %08x\n",regVal);
#endif

//	printf("ipMuxRegWrite: regAddr = %08x, regVal = %08x\n",regAddr, regVal);
	ipMuxRegWrite(regAddr,regVal);
	
	return OPL_OK;	
}	


/*******************************************************************************
* mdioRegisterRead
*
* DESCRIPTION:
*  		this function is usedt to read mdio data from a register.
*
*	INPUTS:
*			deviceAddr	: phy address (0-31)
*			regAddr		: reg address  (0-31)
*			
*	OUTPUTS:
*		   data0			: data buffer for read out data
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS mdioRegisterRead(UINT32 deviceAddr,UINT32 regAddr,UINT16 *data0)
{
	UINT32 mdioBusy;
	UINT32 timeOut = MDIO_TIMEOUT;
	UINT32 regVal;
	OPL_STATUS retVal;
		
/* first check that it is not busy */

	retVal = oplRegFieldRead(REG_MDIO_BUSY, 0, 1, &mdioBusy);
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	while(mdioBusy&MDIO_BUSY)
	{
		if(!timeOut--)
		{
			opl_log(("mdioRegisterRead: timeout 0\n"));
		}
		retVal = oplRegFieldRead(REG_MDIO_BUSY, 0, 1, &mdioBusy);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}
	
	retVal = oplRegFieldWrite(REG_MDIO_PHYAD,0,5,deviceAddr&0x1f);

	if(OPL_OK != retVal)
	{
		opl_log(("mdioRegisterRead: oplRegFieldWrite REG_MDIO_PHYAD error 0\n"));
		return retVal;
	}

	retVal = oplRegFieldWrite(REG_MDIO_REGAD, 0, 5, regAddr&0x1f);
	if(OPL_OK != retVal)
	{
		opl_log(("mdioRegisterRead: oplRegFieldWrite REG_MDIO_REGAD error 0\n"));
		return retVal;
	}
	timeOut = MDIO_TIMEOUT;

	retVal = oplRegFieldWrite(REG_MDIO_OP_PULSE, 0, 2, MDIO_READ);
	if(OPL_OK != retVal)
	{
		opl_log(("mdioRegisterRead: oplRegFieldWrite REG_MDIO_OP_PULSE error 0\n"));
		return OPL_OK;
	}
	
	retVal = oplRegFieldRead(REG_MDIO_BUSY, 0, 1, &mdioBusy);
	if(OPL_OK != retVal)
	{
		opl_log(("mdioRegisterRead: oplRegFieldRead REG_MDIO_BUSY error 0\n"));
		return retVal;
	}

	while(mdioBusy&MDIO_BUSY)
	{
		if(!timeOut--)
		{
			opl_log(("mdioRegisterRead: timeout 0\n"));
			return OPL_ERROR;
		}
		retVal = oplRegFieldRead(REG_MDIO_BUSY, 0, 1, &mdioBusy);
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}

	retVal = oplRegFieldRead(REG_MDIO_READ_DATA, 0, 16, &regVal);
	
	*data0 = regVal;
	return retVal;
}
/*******************************************************************************
* mdioRegisterWrite
*
* DESCRIPTION:
*  		this function is usedt to write mdio data to a register.
*
*	INPUTS:
*			deviceAddr	: phy address (0-31)
*			regAddr		: reg address  (0-31)
*			data0			: value to be write
*	OUTPUTS:
*		   
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS mdioRegisterWrite(UINT32 deviceAddr,UINT32 regAddr,UINT16 data0)
{
	UINT32 mdioBusy;
	UINT32 timeOut = MDIO_TIMEOUT;
	OPL_STATUS retVal;

	retVal = oplRegFieldRead(REG_MDIO_BUSY,0, 1, &mdioBusy);
	if(OPL_OK != retVal)
	{
		opl_log(("mdioRegisterWrite: oplRegFieldRead REG_MDIO_BUSY error\n"));
		return retVal;
	}
	while(MDIO_BUSY&mdioBusy)
	{
		if(!timeOut--)
		{
			opl_log(("mdioRegisterWrite: timeout 0\n"));
			return OPL_ERROR;
		}
		retVal = oplRegFieldRead(REG_MDIO_BUSY,0, 1, &mdioBusy);
		if(OPL_OK != retVal)
		{
			opl_log(("mdioRegisterWrite: oplRegFieldRead REG_MDIO_BUSY error\n"));
			return retVal;
		}
	}

	retVal = oplRegFieldWrite(REG_MDIO_PHYAD, 0, 5, deviceAddr&0x1f);
	if(OPL_OK != retVal)
	{
		opl_log(("mdioRegisterWrite: oplRegFieldWrite REG_MDIO_PHYAD error\n"));
		return OPL_ERROR;
	}
	retVal = oplRegFieldWrite(REG_MDIO_REGAD, 0, 5, regAddr&0x1f);
	if(OPL_OK != retVal)
	{
		opl_log(("mdioRegisterWrite: oplRegFieldWrite REG_MDIO_REGAD error\n"));
		return OPL_ERROR;
	}

	retVal = oplRegFieldWrite(REG_MDIO_WRITE_DATA, 0, 16, data0);
	if(OPL_OK != retVal)
	{
		opl_log(("mdioRegisterWrite: oplRegFieldWrite REG_MDIO_WRITE_DATA error\n"));
		return OPL_ERROR;
	}

	retVal = oplRegFieldWrite(REG_MDIO_OP_PULSE, 0, 2, MDIO_WRITE);
	if(OPL_OK != retVal)
	{
		opl_log(("mdioRegisterWrite: oplRegFieldWrite REG_MDIO_WRITE_DATA error\n"));
		return OPL_ERROR;
	}
	
	return retVal;	
}

/*******************************************************************************
* mdioRegisterFieldRead
*
* DESCRIPTION:
*  		this function is usedt to read a bitfied from specified offset of a register.
*
*	INPUTS:
*			deviceAddr	: phy address (0-31)
*			regAddr		: reg address  (0-31)
*			offset			: the offset of the start bit of the bitfield (0-31).
*			width			: the width of data (0-31)
*	OUTPUTS:
*		   data0			: the data read out.
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS mdioRegisterFieldRead(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 *data0)
{
	UINT16 fieldMask;
	UINT16 regVal;
	OPL_STATUS retVal;

	if(offset > 15 || width < 1 || width > 16||((offset + width)>16))
	{
		return OPL_ERROR;
	}

	if(OPL_NULL == data0)
	{
		return OPL_ERROR;
	}

	if(width == 16)
	{
		fieldMask = 0xffff;
	}else
	{
		fieldMask = (~(0xffff<<width))<<offset;		
	}

	retVal = mdioRegisterRead( deviceAddr,  regAddr, &regVal);
	if(OPL_OK != retVal)
	{
		return retVal;
	}
	regVal = regVal&fieldMask;
	*data0 = regVal>>offset;
	return retVal;
}
/*******************************************************************************
* mdioRegisterFieldWrite
*
* DESCRIPTION:
*  		this function is usedt to write a bitfied to specified offset of a register.
*
*	INPUTS:
*			deviceAddr	: phy address (0-31)
*			regAddr		: reg address  (0-31)
*			offset			: the offset of the start bit of the bitfield (0-31).
*			width			: the width of data (0-31)
*			 data0			: the data to be writed.
*	OUTPUTS:
*		  
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS mdioRegisterFieldWrite(UINT8 deviceAddr,UINT8 regAddr,UINT8 offset,UINT8 width,UINT16 data0)
{
	UINT16 fieldMask;
	UINT16 regVal;
	OPL_STATUS retVal;

	if(offset > 15 || width < 1 || width > 16||((offset + width)>16))
	{
		return OPL_ERROR;
	}

	if(width == 16)
	{
		fieldMask = 0xffff;
	}else
	{
		fieldMask = (~(0xffff<<width))<<offset;		
	}

	retVal = mdioRegisterRead( deviceAddr,  regAddr, &regVal);

	if(OPL_OK != retVal)
	{
		return retVal;
	}
	regVal = regVal&~fieldMask;
	regVal = regVal|((data0<<offset)&fieldMask);
	retVal = mdioRegisterWrite( deviceAddr,  regAddr,  regVal);
	return retVal;
}

