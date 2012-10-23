/*
=============================================================================
     File Name: dal_lib.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/20		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <dal_lib.h>
#include <pthread.h>

#include <ssdk_init.h>

/* compatible to TW_F420 board */
UINT8 dalToAtherosPortMap[] = {0,1,2,3,4,5,6,0,0,0,0};
UINT8 atherosToDalPortMap[] ={0,1,2,3,4,5,6,0,0,0,0,0};

UINT8 logicToPhyPortMap[] = {0,0,1,2,3,9,5,0,0,0,0};
UINT8 phyToLogicPortMap[] ={4,3,2,1,0,0,0,0,0,5,0,0};

extern UINT8 oam_src_mac[6];


OPL_STATUS dalChipReset()
{
    return chipReset();
}

OPL_STATUS dalSwitchReset()
{
    a_uint32_t regVal;
    regVal = 0x80000000;

	printf("Going to reset switch\n");
    return isis_reg_set(0, 0, (a_uint8_t *)&regVal, sizeof(a_uint32_t));
}

OPL_STATUS dalDemonInit(void)
{
	OPL_STATUS retVal = OPL_OK;

	retVal = oplDriverInit();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init oplDriverInit failed.\n"));
		return retVal;
	}

	retVal = dalPonInit();
	if(OPL_OK != retVal)
	{
		return retVal;
	}

	retVal = atheros_init();
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("Atheros ssdk init failed.\n"));
		return retVal;
	}

    /* hack for downstream 400M */
    tmDsShaperTabDirectHwWrite(8, 12500, 1023);

	retVal = dalAclInit();	
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("Atheros Acl failed.\n"));
		return retVal;
	}

	retVal = dalArlInit();	
	if(OPL_OK != retVal)
	{
		OPL_DRV_PRINTF(("Atheros Arl failed.\n"));
		return retVal;
	}

	retVal = dalClsInit();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init dalClsInit failed.\n"));
		return retVal;
	}

	retVal = dalPortInit();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init dalPort failed.\n"));
		return retVal;
	}
	
	retVal = dalVttInit();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init dalVttInit failed.\n"));
		return retVal;
	}

	retVal = dalArlFlushAll();
	if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("init dalArlFlushAll failed.\n"));
		return retVal;
	}

    /* disable UART1 */
    retVal = oplRegFieldWrite(REG_GPIO_MUX, 1, 1, 0);
    if(OPL_OK != retVal)
	{
		OPL_DAL_PRINTF(("Disable UART1 failed.\n"));
		return retVal;
	}
 
	return OPL_OK;
}

