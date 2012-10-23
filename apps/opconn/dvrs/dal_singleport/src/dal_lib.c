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

extern UINT8 oam_src_mac[6];

#define POLICING_LINE_RATE_ENABLE 1
#define SYSTEM_CLOCK_125M 1

OPL_STATUS dalChipReset()
{
    return chipReset();
}

OPL_STATUS dalSwitchReset()
{
    return OPL_OK;
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

	retVal = dalPonInit();
	if(OPL_OK != retVal)
	{
		return retVal;
	}

#if POLICING_LINE_RATE_ENABLE
	/* Modified shaper offset:20=8premble+12IPG */
    retVal = oplRegWrite(REG_TMDS_SHAPER_OFFSET, 0x14);
    if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_SHAPER_OFFSET, 0x14);
    if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegFieldWrite(REG_GE_PARSER_STEP_INC, 8, 8, 0x14);
	if(OPL_OK != retVal){
		return retVal;
	}
#endif

#if SYSTEM_CLOCK_125M	/*for 125MHz system clock */
	retVal = oplRegFieldWrite(REG_GE_PARSER_STEP_INC, 0, 8, 0x32);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_CLKNUM_PERIOD, 0x0480);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMDS_CLKNUM_PERIOD, 0x0480);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_STEP_CNT, 0x3C);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMDS_STEP_CNT, 0x3C);
	if(OPL_OK != retVal){
		return retVal;
	}
#else
	retVal = oplRegFieldWrite(REG_GE_PARSER_STEP_INC, 0, 8, 0x30);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_CLKNUM_PERIOD, 0x7880);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMDS_CLKNUM_PERIOD, 0x7880);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMUS_STEP_CNT, 0x3F);
	if(OPL_OK != retVal){
		return retVal;
	}
	retVal = oplRegWrite(REG_TMDS_STEP_CNT, 0x3F);
	if(OPL_OK != retVal){
		return retVal;
	}
#endif


	return OPL_OK;
}

