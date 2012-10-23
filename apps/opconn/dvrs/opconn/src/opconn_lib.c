/*
=============================================================================
     Header Name: opconn.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/

#include <opl_driver.h>
#include <opl_errno.h>
#include <opl_debug.h>
#include <opl_utils.h>
#include <opl_modif.h>
#include <opconn_lib.h>

#define OPL_MAP_LENGTH   (0x30000)   

UINT8 *regBaseAddr = OPL_NULL;
UINT32 gChipRevision = ONU_OPCONN;

OPL_STATUS oplDevInit(UINT8 *pMemBuff)
{
	regBaseAddr = pMemBuff;

	return OPL_OK;
}

OPL_STATUS oplDriverInit(void)
{
	OPL_STATUS retVal = OPL_OK;
	void *mmap_buff = OPL_NULL;

	retVal = opl_modules_init(&mmap_buff,OPL_MAP_LENGTH);

	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init module failed.\n"));
		return OPL_ERROR;
	}

	retVal = oplDevInit(mmap_buff);
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init memeory failed.\n"));
		return retVal;
	}

    retVal = oplHwAcessInit();
    if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init hardware access failed.\n"));
		return retVal;
	}

	retVal = chipInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("chipInit.\n"));
		return retVal;
	}

	retVal = linkListEnable();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("linkListEnable.\n"));
		return retVal;
	}

	retVal = clsInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init cls table failed.\n"));
		return retVal;
	}

	retVal = brgArlInit();
	if(retVal != OPL_OK)
	{
		OPL_DRV_PRINTF(("init arl table failed.\n"));
		return retVal;
	}

	retVal = brgVttInit();
	if(retVal != OPL_OK)
	{
		OPL_DRV_PRINTF(("init vtt table failed.\n"));
		return retVal;
	}

	retVal = brgPortInit();
	if(retVal != OPL_OK)
	{
		OPL_DRV_PRINTF(("init port table failed.\n"));
		return retVal;
	}

	retVal = tmInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init tm failed.\n"));
		return retVal;
	}
	
	retVal = eopl_dma_init();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init dma failed.\n"));
		return retVal;
	}

	retVal = gmacInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init gmac failed.\n"));
		return retVal;
	}
	
	retVal = pmacInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init pmac failed.\n"));
		return retVal;
	}
	/*init reserved mac control*/
	retVal  = brgRsvMacCtrlInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("rsvmac control init failed.\n"));
		return retVal;
	}
	
    retVal  = brgZeroMacDropInit();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("init zero mac packets dropping  failed.\n"));
		return retVal;
	}
	return retVal;
}

