/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  cli_cmd_pon.c
*
* DESCRIPTION: 
*	
*
* Date Created: Aug 19, 2008
*
* Authors(optional): 
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_pon.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <stdlib.h>
#include "cli.h"
#include "cli_cmd_list.h"

#include <dal_lib.h>
#include "product_info.h"
#include "odm_pon.h"
#include "oam.h"
#include "defs.h"
#include "dal_mac.h"

STATUS cliCmdOnuMpcpHoldoverContrlSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 enableStatus;

	if(NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}

	if(1 == psPara[0].i)
	{
		enableStatus = TRUE;
	}
	else if(2 == psPara[0].i)
	{
		enableStatus = FALSE;
	}
	else
	{
		vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return retVal;
	}

	retVal = odmPonMpcpFsmHoldoverControlSet(enableStatus);
	if(retVal != OK)
	{
		vosPrintf(pstEnv->nWriteFd,  "\r\ncommand execute error.\r\n");
		return retVal;
	}
	
	return retVal;
}

STATUS cliCmdOnuMpcpHoldoverTimeSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 holdTime;

	if(NULL == pstEnv || NULL == psPara)
	{
		return ERROR;
	}

	holdTime = (UINT32)psPara[0].u;

	retVal = odmPonMpcpFsmHoldoverTimeSet(holdTime);
	
	return retVal;
}

STATUS cliCmdPonLoopbackConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 enable;

	if (IS_ONU_RESPIN)
	{
		if (NULL == pstEnv || NULL == psPara) 
		{
			return ERROR;
		}

		retVal = odmPonLoopbackConfig(psPara[0].i-1, psPara[1].i-1);
		if(retVal != OK)
		{
			vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
			return retVal;
		}
	}
	else
	{
		vosPrintf(pstEnv->nWriteFd, "%%Not support.\r\n");
	}

	return OK;
}

STATUS cliCmdPonFecEnable(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT32 enable;
    
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    if(1 == psPara[0].i)//enable
    {
        enable = TRUE;
    }else if(2 == psPara[0].i)//disable
    {
        enable = FALSE;
    }else
    {
	    vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return retVal;
    }

	retVal = odmPonFecEnableSet(enable);
    if(retVal != OK)
    {
	    vosPrintf(pstEnv->nWriteFd, "\r\ninput error.\r\n");
		return retVal;
    }
	
	return OK;
}

STATUS cliCmdPonLaserCtrlSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    if (1 == psPara[0].u)
    {
        oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 5);
        vosPrintf(pstEnv->nWriteFd, "\r\nLaser on/off affected by grant and upstream data\r\n");
    }
    else if (2 ==  psPara[0].u)
    {
        oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 2);
        vosPrintf(pstEnv->nWriteFd, "\r\nLaser on\r\n");
    }
    else if (3 ==  psPara[0].u)
    {
        oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 8);
        vosPrintf(pstEnv->nWriteFd, "\r\nLaser off\r\n");
    }
	return OK;
}

STATUS cliCmdPonLaserOnSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    retVal = odmPonLaserOnSet(psPara[0].u);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
	return OK;
}

STATUS cliCmdPonTxPolarityConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;

	if (IS_ONU_RESPIN)
	{
		if (NULL == pstEnv || NULL == psPara) 
		{
			return ERROR;
		}

	    retVal = odmPonTxPolarityConfig(psPara[0].u-1);
	    if(OK != retVal)
	    {
	        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
	    }
	}
	else
	{
		vosPrintf(pstEnv->nWriteFd, "%%Not support.\r\n");
	}
	return OK;
}

STATUS cliCmdPonLaserOffSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    retVal = odmPonLaserOffSet(psPara[0].u);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
	return OK;
}

STATUS cliCmdPonSerdesDelaySet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    retVal = odmPonSerdesDelaySet(psPara[0].u);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
	return OK;
}

STATUS cliCmdPonMacIdSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32  retVal = OK;
    UINT8   macId[MAC_LENGTH];

	char section[20];
	int ret=0;

	strcpy(section,"Product Information");


	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    retVal = string2mac(psPara[0].p,macId);
    if(OK != retVal)
    {
        return ERR_INVALID_PARAMETERS;
    }
	
    retVal = odmPonMacIdSet(macId);

    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
	if(vosConfigSectionIsExisted(PRODUCT_CFG_FILE,section))
	{
		ret = vosConfigSectionCreate(PRODUCT_CFG_FILE,section);
	}
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"PON MAC",psPara[0].p)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}

    cliTextConfigSave();
    vosConfigSave(NULL);
	
	return OK;
}

extern const char *g_acIfEthNameStr[INTERFACE_MAX*INTERFACE_MODE_MAX];
STATUS cliCmdLanMacIdSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	char section[20];
	int ret=0;
	char macStr[50]={0};
	char oldMacId[6] = {0};
	char   MacId[6] = {0};
	char ucIf = INTERFACE_LAN;
	char index = 0;
   char 	IfSectionBuff[32] = {0};
   char ucIfMode = 0;
	
	strcpy(section,"Product Information");

	if(strlen("00:00:00:00:00:00") !=strlen(psPara[0].p))
	{
		printf("The length of MAC is error\r\n");
		//return ERROR;
	}
	vosStrToMac(psPara[0].p,MacId);

   for (ucIfMode =0; ucIfMode < INTERFACE_MODE_MAX; ucIfMode++)
    {
		index = ucIf * INTERFACE_MODE_MAX + ucIfMode;
		vosHWAddrGet(g_acIfEthNameStr[index],&oldMacId[0]);
		printf("aaa\r\n");
		if((memcmp(MacId,oldMacId,MAC_LENGTH))&&(memcmp("00:00:00:00:00:00",psPara[0].p,17)))
		{
			vosSprintf(macStr,"ifconfig %s hw ether %s",g_acIfEthNameStr[index],psPara[0].p);
#if 0
			if(0 != vosSystem(macStr))
			{
				return ERROR;
			}
#else
			ret = vosSystem(macStr);
//			printf("ret is %d\r\n",ret);
#endif

			if(0 == ret)
			{
				/*Save Lan Mac in System Configuration File*/
				vosSprintf(IfSectionBuff, "If%d", index);
				vosConfigValueSet("/cfg/system.conf",IfSectionBuff,"MAC Address",psPara[0].p);
			}

		}
    }
   if(0 == ret)
	{
		/*Save Lan Mac in Product Configuration File*/
		if(vosConfigSectionIsExisted(PRODUCT_CFG_FILE,section))
		{
			ret = vosConfigSectionCreate(PRODUCT_CFG_FILE,section);
		}
		if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"LAN MAC",psPara[0].p)) != 0)
		{
				printf("save error\n");
				return ERROR;
		}
		printf("MacId is %x %x %x %x %x %x\r\n",MacId[0],MacId[1],MacId[2],MacId[3],MacId[4],MacId[5]);
	  dalArlMgmtMacRdt2Cpu(oldMacId, MacId);
	  vosConfigSave(NULL);
	}
   else
   {
	   printf("invalid hw-addr\r\n");
   }
 return OK;
}
STATUS cliCmdWanMacIdSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	char section[20];
	int ret=0;
	char macStr[50]={0};
	char ucIf = INTERFACE_WAN;
	char oldMacId[6] = {0};
	char index = 0;
   char 	IfSectionBuff[32] = {0};
	char   MacId[6] = {0};
	char ucIfMode = 0;

	strcpy(section,"Product Information");

	if(strlen("00:00:00:00:00:00") !=strlen(psPara[0].p))
	{
		printf("The length of MAC is error\r\n");
		//return ERROR;
	}
	vosStrToMac(psPara[0].p,MacId);

   for (ucIfMode =0; ucIfMode < INTERFACE_MODE_MAX; ucIfMode++)
	{
		index = ucIf * INTERFACE_MODE_MAX + ucIfMode;
		vosHWAddrGet(g_acIfEthNameStr[index],&oldMacId[0]);
		if((memcmp(MacId,oldMacId,MAC_LENGTH))&&(memcmp("00:00:00:00:00:00",psPara[0].p,17)))
		{
			/*We use eth3 for telnet pty eth4 for WAN */
			vosSprintf(macStr,"ifconfig %s hw ether %s",g_acIfEthNameStr[index],psPara[0].p);
			ret = vosSystem(macStr);
			if(0 == ret)
			{
				/*Save WAN Mac in System Configuration File*/
				vosSprintf(IfSectionBuff, "If%d", index);
			  vosConfigValueSet("/cfg/system.conf",IfSectionBuff,"MAC Address",psPara[0].p);
			}
		}
	}
   if(0 == ret)
	{
		if(vosConfigSectionIsExisted(PRODUCT_CFG_FILE,section))
		{
			ret = vosConfigSectionCreate(PRODUCT_CFG_FILE,section);
		}
		if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"WAN MAC",psPara[0].p)) != 0)
		{
				printf("save error\n");
				return ERROR;
		}

		vosConfigSave(NULL);
	}
   else
   {
	   printf("invalid hw-addr\r\n");
   }
	return OK;

}
STATUS cliCmdPonDbaAgentAllParaSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)

{
    UINT32 retVal = OK;
    UINT32 cpRptSendMode;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    if(psPara[0].i == 1)
    {
        cpRptSendMode = 0;  /*last mode*/
    }else
    {
        cpRptSendMode = 1; /*first mode*/
    }
#if 0
    vosPrintf(pstEnv->nWriteFd, "\r\%d %d %d %d %d\r\n",
        psPara[0].i,
        psPara[1].u,
        psPara[2].u,
        psPara[3].u,
        psPara[4].u);
#endif
    retVal = odmPonDbaAgentAllParaSet(cpRptSendMode, 
        psPara[1].u,
        psPara[2].u,
        psPara[3].u,
        psPara[4].u);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
	return OK;
}

STATUS cliCmdPonTxDelaySet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
    UINT32 cpRttCompenEn;
    UINT32 cpRttCompenSyncEn;
    UINT32 cpRttCorrectEn;
    UINT32 cpRttCorrectSyncEn;
    UINT32 cpTxDlyEn;
    UINT32 cpSyncEn;
    UINT32 cpRptsentDelt;
    UINT32 cpRttOffset;
    UINT32 cpTxDly;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    cpRttCompenEn       = psPara[0].i -1;
    cpRttCompenSyncEn   = psPara[1].i -1;
    cpRttCorrectEn      = psPara[2].i -1;
    cpRttCorrectSyncEn  = psPara[3].i -1;
    cpTxDlyEn           = psPara[4].i -1;
    cpSyncEn            = psPara[5].i -1;
    cpRptsentDelt       = psPara[6].u;
    cpRttOffset         = psPara[7].u;
    cpTxDly             = psPara[8].u;
#if 0
    vosPrintf(pstEnv->nWriteFd, "\r\n%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
        cpRttCompenEn,
        cpRttCompenSyncEn,
        cpRttCorrectEn,
        cpRttCorrectSyncEn,
        cpTxDlyEn,
        cpSyncEn,
        cpRptsentDelt,
        cpRttOffset,
        cpTxDly);
#endif
    retVal = odmPonTxDelaySet(cpRttCompenEn,cpRttCompenSyncEn,cpRttCorrectEn,
        cpRttCorrectSyncEn,cpTxDlyEn,cpSyncEn,cpRptsentDelt,cpRttOffset,cpTxDly);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
	return OK;
}

STATUS cliCmdPonReportModeSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
    
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}
   
    retVal = odmPonReportModeSet(psPara[0].i - 1);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
	return OK;
}

STATUS cliCmdPonQsetThresholdSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
    
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}
   
    retVal = odmPonQsetThresholdSet(psPara[0].u,psPara[1].u,psPara[2].u);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
	return OK;
}

STATUS cliCmdPonRegSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
    UINT32 regId;
    UINT32 regVal;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    regId   = string2uint(psPara[0].p);
    regVal  = string2uint(psPara[1].p);
    retVal = odmPonRegSet(regId,regVal);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
	return OK;
}

STATUS cliCmdPonRegGet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
    UINT32 regId;
    UINT32 regVal;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    regId   = string2uint(psPara[0].p);
    retVal = odmPonRegGet(regId,&regVal);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
    }
    vosPrintf(pstEnv->nWriteFd, "reg(0x%04x) = 0x%08x\r\n",regId,regVal);
	return OK;
}

STATUS cliCmdPonMutiRegGet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
    UINT32 startRegId;
    UINT32 numOfRegs;
    UINT32 regVal;
    UINT32 index;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    startRegId   = string2uint(psPara[0].p);
    numOfRegs = psPara[1].u;
    for(index = startRegId; index < startRegId + numOfRegs; index++)
    {
        retVal = odmPonRegGet(index,&regVal);
        if(OK != retVal)
        {
            vosPrintf(pstEnv->nWriteFd, "\r\nerror code %u\r\n",retVal);
        }
        vosPrintf(pstEnv->nWriteFd, "reg(0x%04x) = 0x%08x\r\n",index,regVal);
    }
	return OK;
}

STATUS cliCmdPonMutiTabGet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
    UINT32 tabId;
    UINT32 startRecId;
    UINT32 numOfRec;
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

    tabId       = psPara[0].u;
    startRecId  = psPara[1].u;
    numOfRec    = psPara[2].u;
    odmPonMultiTabGet(pstEnv->nWriteFd,tabId,startRecId,numOfRec);
	return OK;
}

STATUS cliCmdPonTabRecSet(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

	return OK;
}

STATUS cliCmdPonShowTabInfo(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
	if (NULL == pstEnv) 
	{
		return ERROR;
	}

    odmPonTabInfoGet(pstEnv->nWriteFd);
	return OK;
}

extern UINT8 *mpcpEventDisc[EVENT_END + 1];
extern UINT8 *mpcpStateDisc[STATE_END + 1];

STATUS cliCmdPonShowConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32 retVal = OK;
    UINT32 cpRptSendMode;
    UINT32 cpDeltaTxNormalFecTime;
    UINT32 cpDeltaTxNormalNoFecTime;
    UINT32 cpTxMpcpFecTime;
    UINT32 cpTxMpcpNoFecTime;
    UINT8  *rptSendModeString[] = {"Last","First"};
	UINT8  *silenceStateString[] = {"Disable", "Enable"};
	
	UINT32  silenceEn;
	UINT32  silenceTime;
    UINT32  laserOn;
    UINT32  laserOff;
    UINT32  serdesDelay;
    UINT32  llid = 1;
    UINT32  syncTime = 32;
    UINT8   macId[6];
    UINT32  currentRegStatus = 0;
    UINT32  cpRttCompenEn;
    UINT32  cpRttCompenSyncEn;
    UINT32  cpRttCorrectEn;
    UINT32  cpRttCorrectSyncEn;
	UINT32  cpTxDlyEn;
    UINT32  cpSyncEn;
    UINT32  cpRptsentDelt;
    UINT32  cpRttOffset;
    UINT32  cpTxDly;
    UINT8   *statusString[] = {"Disable","Enable"};
	UINT32  holdoverStatus;
	UINT32  holdoverTime;

    ODM_PON_STATUS_t odmPonStatus;
  
    if(1 == psPara[0].i)
    {
        cliShowStart(pstEnv);

		retVal = odmPonSilenceEnableGet(&silenceEn);
		if(OK != retVal)
		{
			vosPrintf(pstEnv->nWriteFd, "get onu silence enable status failed.\r\n");
		}
		
		retVal = odmPonSilenceTimeGet(&silenceTime);
		if(OK != retVal)
		{
			vosPrintf(pstEnv->nWriteFd, "get onu silence time failed.\r\n");
		}
		
        retVal = odmPonLaserOnGet(&laserOn);
        if(OK != retVal)
        {
            vosPrintf(pstEnv->nWriteFd, "get laser on time failed.\r\n");
        }
        retVal = odmPonLaserOffGet(&laserOff);
        if(OK != retVal)
        {
            vosPrintf(pstEnv->nWriteFd, "get laser off time failed.\r\n");
        }
        retVal = odmPonSerdesDelayGet(&serdesDelay);
        if(OK != retVal)
        {
            vosPrintf(pstEnv->nWriteFd, "get serdesdelay failed.\r\n");
        }
        
        retVal = odmPonStatusGet(&odmPonStatus);
        if(OK != retVal)
        {
            vosPrintf(pstEnv->nWriteFd, "get pon status failed.\r\n");
        }

		retVal = odmPonMpcpFsmHoldoverControlGet(&holdoverStatus);
		if(OK != retVal)
		{
			vosPrintf(pstEnv->nWriteFd, "get pon holdover status failed.\r\n");
		}

		retVal = odmPonMpcpFsmHoldoverTimeGet(&holdoverTime);
		if(OK != retVal)
		{
			vosPrintf(pstEnv->nWriteFd, "get pon holdover time failed.\r\n");
		}

        if(STATE_REGISTERD == odmPonStatus.bfDiscCState)
        {
            currentRegStatus = 1;
            retVal = dalPonLlidGet(&llid);
                if(OK != retVal)
            {
                vosPrintf(pstEnv->nWriteFd, "get llid failed.\r\n");
            }
            retVal = dalPonSyncTimeGet(&syncTime);
            if(OK != retVal)
            {
                vosPrintf(pstEnv->nWriteFd, "get synctime failed.\r\n");
            }    
        }
        
        retVal = odmPonMacIdGet(&macId[0]);
        if(OK != retVal)
        {
            vosPrintf(pstEnv->nWriteFd, "get macid failed.\r\n");
        }

		vosPrintf(pstEnv->nWriteFd,"%15s	%s\r\n",
			"Onu Silence State :", silenceStateString[silenceEn]);

		
		vosPrintf(pstEnv->nWriteFd,"%15s	%d(s)\r\n",
			"Onu Silence Time :", silenceTime);

        vosPrintf(pstEnv->nWriteFd,"\r\n%15s    %d(tq)\r\n",
            "laserOn:",laserOn);
        vosPrintf(pstEnv->nWriteFd,"%15s    %d(tq)\r\n",
            "laserOff:",laserOff);
        vosPrintf(pstEnv->nWriteFd,"%15s    %d(byte)\r\n",
            "serdesdelay:",serdesDelay);
        vosPrintf(pstEnv->nWriteFd,"%15s    %02x:%02x:%02x:%02x:%02x:%02x\r\n",
            "macid:",
            macId[0],
            macId[1],
            macId[2],
            macId[3],
            macId[4],
            macId[5]);

		{
			UINT8 oam_status;
			OamStatusGet(&oam_status);
			vosPrintf(pstEnv->nWriteFd,"%15s    %s\r\n", "OAM Status:",(oam_status == EX_DISCOVERY_SEND_ANY?"registered":"not registered"));
		}	
        if(currentRegStatus)
        {
            vosPrintf(pstEnv->nWriteFd,"%15s    %s\r\n","MPCP Status:","registered");  
            vosPrintf(pstEnv->nWriteFd,"%15s    0x%08x\r\n","llid:",llid);  
            vosPrintf(pstEnv->nWriteFd,"%15s    0x%08x\r\n","synctime:",syncTime);  
        }else
        {
            vosPrintf(pstEnv->nWriteFd,"%15s    %s\r\n",
            "MPCP status:","not registered");   
        } 

		if(holdoverStatus)
		{
			vosPrintf(pstEnv->nWriteFd, "%15s    %s\r\n", "holdover:", "enabled");
			vosPrintf(pstEnv->nWriteFd, "%15s    %08d(ms)\r\n", "holdover time:", holdoverTime);		
		}
		else
		{
			vosPrintf(pstEnv->nWriteFd, "%15s    %s\r\n", "holdover:", "disabled");
		}
		

        vosPrintf(pstEnv->nWriteFd,"-------pon statu info----------------\r\n");
        
        vosPrintf(pstEnv->nWriteFd,"%-12s %-5s %s\r\n","type", "value", "desc");

    	vosPrintf(pstEnv->nWriteFd,"%-12s %d\r\n","gatePendNum",odmPonStatus.bfGatePendingNum);
    	
    	if(odmPonStatus.bfDiscLEvent <= EVENT_WATCHDOG_TIMEOUT)
    	{
    		vosPrintf(pstEnv->nWriteFd,"%-12s %-5d %s\r\n","lastEvent",odmPonStatus.bfDiscLEvent,mpcpEventDisc[odmPonStatus.bfDiscLEvent]);
    	}
    	
    	if(odmPonStatus.bfDiscCEvent <= EVENT_WATCHDOG_TIMEOUT)
    	{
    		vosPrintf(pstEnv->nWriteFd,"%-12s %-5d %s\r\n","currEvent",odmPonStatus.bfDiscCEvent,mpcpEventDisc[odmPonStatus.bfDiscCEvent]);
    	}

    	if(odmPonStatus.bfDiscLState <= STATE_WATCHDOG_TIMEOUT)
    	{
    		vosPrintf(pstEnv->nWriteFd,"%-12s %-5d %s\r\n","lastState",odmPonStatus.bfDiscLState,mpcpStateDisc[odmPonStatus.bfDiscLState]);
    	}
    	
    	if(odmPonStatus.bfDiscCState <= STATE_WATCHDOG_TIMEOUT)
    	{
    		vosPrintf(pstEnv->nWriteFd,"%-12s %-5d %s\r\n","currState",odmPonStatus.bfDiscCState,mpcpStateDisc[odmPonStatus.bfDiscCState]);
    	}
        cliShowEnd(pstEnv);
    }else if(2 == psPara[0].i)
    {
        retVal = odmPonDbaAgentAllParaGet(&cpRptSendMode,&cpDeltaTxNormalFecTime,&cpDeltaTxNormalNoFecTime,
                    &cpTxMpcpFecTime,&cpTxMpcpNoFecTime);
        if(OK != retVal)
        {
            return retVal;
        }
        vosPrintf(pstEnv->nWriteFd,"%30s:    %s\r\n","reportSendMode",rptSendModeString[cpRptSendMode]);
        vosPrintf(pstEnv->nWriteFd,"%30s:    %d\r\n","DeltaTxNormalFecTime",cpDeltaTxNormalFecTime);
        vosPrintf(pstEnv->nWriteFd,"%30s:    %d\r\n","DeltaTxNormalNoFecTime",cpDeltaTxNormalNoFecTime);
        vosPrintf(pstEnv->nWriteFd,"%30s:    %d\r\n","TxMpcpFecTime",cpTxMpcpFecTime);
        vosPrintf(pstEnv->nWriteFd,"%30s:    %d\r\n","TxMpcpNoFecTime",cpTxMpcpNoFecTime);  
    }else if(3 == psPara[0].i)
    {
        retVal = odmPonTxDelayGet(&cpRttCompenEn,&cpRttCompenSyncEn,&cpRttCorrectEn,
            &cpRttCorrectSyncEn,&cpTxDlyEn,&cpSyncEn,&cpRptsentDelt,&cpRttOffset,&cpTxDly);
        if(OK != retVal)
        {
            return retVal;
        }
        vosPrintf(pstEnv->nWriteFd,"%20s:    %s\r\n","RttCompenEn",statusString[cpRttCompenEn&0x01]);
        vosPrintf(pstEnv->nWriteFd,"%20s:    %s\r\n","RttCompenSyncEn",statusString[cpRttCompenSyncEn&0x01]);
        vosPrintf(pstEnv->nWriteFd,"%20s:    %s\r\n","RttCorrectEn",statusString[cpRttCorrectEn&0x01]);
        vosPrintf(pstEnv->nWriteFd,"%20s:    %s\r\n","RttCorrectSyncEn",statusString[cpRttCorrectSyncEn&0x01]);
        vosPrintf(pstEnv->nWriteFd,"%20s:    %s\r\n","TxDlyEn",statusString[cpTxDlyEn&0x01]);  
        vosPrintf(pstEnv->nWriteFd,"%20s:    %s\r\n","SyncEn",statusString[cpSyncEn&0x01]);
        vosPrintf(pstEnv->nWriteFd,"%20s:    %d\r\n","RptsentDelt",cpRptsentDelt);
        vosPrintf(pstEnv->nWriteFd,"%20s:    %d\r\n","RttOffset",cpRttOffset);
        vosPrintf(pstEnv->nWriteFd,"%20s:    %d\r\n","TxDly",cpTxDly);       
    }
    return OK;
}


STATUS cliCmdPonParameterShow(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    UINT32  retVal = OK;
	UINT32  silenceEn;
	UINT32  silenceTime;
    UINT32  laserOn;
    UINT32  laserOff;
    UINT32  serdesDelay;
    UINT32  llid = 1;
    UINT32  syncTime = 32;
    UINT8   macId[6];
    UINT32  currentRegStatus = 0;
    ODM_PON_STATUS_t odmPonStatus;
    
	if (NULL == pstEnv) 
	{
		return ERROR;
	}
    cliShowStart(pstEnv);

    retVal = odmPonSilenceEnableGet(&silenceEn);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "get onu silence enable status failed.\r\n");
    }

    retVal = odmPonSilenceTimeGet(&silenceTime);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "get onu silence time failed.\r\n");
    }
	
    retVal = odmPonLaserOnGet(&laserOn);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "get laser on time failed.\r\n");
    }
    retVal = odmPonLaserOffGet(&laserOff);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "get laser off time failed.\r\n");
    }
    retVal = odmPonSerdesDelayGet(&serdesDelay);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "get serdesdelay failed.\r\n");
    }

    retVal = odmPonStatusGet(&odmPonStatus);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "get pon status failed.\r\n");
    }

    if(STATE_REGISTERD == odmPonStatus.bfDiscCState)
    {
        currentRegStatus = 1;
        retVal = dalPonLlidGet(&llid);
        if(OK != retVal)
        {
            vosPrintf(pstEnv->nWriteFd, "get llid failed.\r\n");
        }
        retVal = dalPonSyncTimeGet(&syncTime);
        if(OK != retVal)
        {
            vosPrintf(pstEnv->nWriteFd, "get synctime failed.\r\n");
        }    
    }

     vosPrintf(pstEnv->nWriteFd,"-------pon statu info----------------\r\n");
    retVal = odmPonMacIdGet(&macId[0]);
    if(OK != retVal)
    {
        vosPrintf(pstEnv->nWriteFd, "get macid failed.\r\n");
    }

	if(silenceEn = 1)
	{
    	vosPrintf(pstEnv->nWriteFd,"\r\n%15s    %s\r\n",
        "Onu Silence State:","enabled");
	}
	else
	{
    	vosPrintf(pstEnv->nWriteFd,"\r\n%15s    %s\r\n",
        "Onu Silence State:","disabled");	
	}
	
    vosPrintf(pstEnv->nWriteFd,"%15s    %d(s)\r\n",
        "Onu Silence Time :", silenceTime);
	
    vosPrintf(pstEnv->nWriteFd,"\r\n%15s    %d(tq)\r\n",
		
        "laserOn:",laserOn);
    vosPrintf(pstEnv->nWriteFd,"%15s    %d(tq)\r\n",
        "laserOff:",laserOff);
    vosPrintf(pstEnv->nWriteFd,"%15s    %d(byte)\r\n",
        "serdesdelay:",serdesDelay);
    vosPrintf(pstEnv->nWriteFd,"%15s    %02x:%02x:%02x:%02x:%02x:%02x\r\n",
        "macid:",
        macId[0],
        macId[1],
        macId[2],
        macId[3],
        macId[4],
        macId[5]);
	{
		UINT8 oam_status;
		OamStatusGet(&oam_status);
		vosPrintf(pstEnv->nWriteFd,"%15s	%s\r\n", "OAM Status:",(oam_status == EX_DISCOVERY_SEND_ANY?"registered":"not registered"));
	}	
	
    if(currentRegStatus)
    {
        vosPrintf(pstEnv->nWriteFd,"%15s    %s\r\n","MPCP Status:","registered");  
        vosPrintf(pstEnv->nWriteFd,"%15s    0x%08x\r\n","llid:",llid);  
        vosPrintf(pstEnv->nWriteFd,"%15s    0x%08x\r\n","synctime:",syncTime);  
    }else
    {
        vosPrintf(pstEnv->nWriteFd,"%15s    %s\r\n",
        "MPCP Status:","not registered");   
    }

    vosPrintf(pstEnv->nWriteFd,"-------pon statu info----------------\r\n");

    

    vosPrintf(pstEnv->nWriteFd,"%-12s %-5s %s\r\n","type", "value", "desc");

	vosPrintf(pstEnv->nWriteFd,"%-12s %d\r\n","gatePendNum",odmPonStatus.bfGatePendingNum);
	
	if(odmPonStatus.bfDiscLEvent <= EVENT_WATCHDOG_TIMEOUT)
	{
		vosPrintf(pstEnv->nWriteFd,"%-12s %-5d %s\r\n","lastEvent",odmPonStatus.bfDiscLEvent,mpcpEventDisc[odmPonStatus.bfDiscLEvent]);
	}
	
	if(odmPonStatus.bfDiscCEvent <= EVENT_WATCHDOG_TIMEOUT)
	{
		vosPrintf(pstEnv->nWriteFd,"%-12s %-5d %s\r\n","currEvent",odmPonStatus.bfDiscCEvent,mpcpEventDisc[odmPonStatus.bfDiscCEvent]);
	}

	if(odmPonStatus.bfDiscLState <= STATE_WATCHDOG_TIMEOUT)
	{
		vosPrintf(pstEnv->nWriteFd,"%-12s %-5d %s\r\n","lastState",odmPonStatus.bfDiscLState,mpcpStateDisc[odmPonStatus.bfDiscLState]);
	}
	
	if(odmPonStatus.bfDiscCState <= STATE_WATCHDOG_TIMEOUT)
	{
		vosPrintf(pstEnv->nWriteFd,"%-12s %-5d %s\r\n","currState",odmPonStatus.bfDiscCState,mpcpStateDisc[odmPonStatus.bfDiscCState]);
	}
	
    cliShowEnd(pstEnv);
	return OK;
}

STATUS cliCmdOnuSilenceEnSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_BOOL bEnable;
    
    if (NULL == pstEnv)
    {
        return ERROR;
    }

    if (1 == pstPt[0].i)
    {
        odmPonSilenceEnableSet(1);
    }
    else
    {
       	odmPonSilenceEnableSet(0);
    }

    return OK;
}

STATUS cliCmdOnuSilenceTimeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_BOOL bEnable;
	UINT32 silenceTime;
	
    if (NULL == pstEnv)
    {
        return ERROR;
    }

	silenceTime = (UINT32)(pstPt[0].u);

    odmPonSilenceTimeSet(silenceTime);

    return OK;
}


STATUS cliCmdDataPathMtuSizeGet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 ret = NO_ERROR;
    UINT32 mtuSize = 0;

    ret = odmOnuMtuGet(&mtuSize);

    if (NO_ERROR != ret)
    {
        vosPrintf(pstEnv->nWriteFd, "Show DataPath Mtu failed\r\n"); 
        return ERROR;
    }
    vosPrintf(pstEnv->nWriteFd,"DataPath maximum transmission unit size is %d\r\n",mtuSize);
    return NO_ERROR;	
}

STATUS cliCmdDataPathMtuSizeSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 ret = NO_ERROR;
    UINT32 mtuSize = 0;

    if ((NULL == pstEnv) || (NULL == pstPt))
    {
        return ERROR;
    }

    mtuSize = pstPt->u;
    if(64 > mtuSize)
        mtuSize = 64; 

    if(PON_MAC_MTU < mtuSize)
        mtuSize = PON_MAC_MTU;

    ret = odmOnuMtuSet(mtuSize);

    if (NO_ERROR != ret)
    {
        vosPrintf(pstEnv->nWriteFd, "Set data path MTU failed\r\n"); 
        return ERROR;
    }

	return NO_ERROR;
}

STATUS cliCmdPonBufferResetConfig(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
	STATUS retVal = OK;
	UINT8 old, enable;

	if (NULL == pstEnv || NULL == psPara) 
	{
		return ERROR;
	}

	if (1 != psPara[0].i){
		enable = 0;
	}
	else {
		enable = 1;
	}

	ponBufferResetConfigGet(&old);
	ponBufferResetConfigSet(enable);
	vosPrintf(pstEnv->nWriteFd, "\r\npon buffer reset enable %d->%d.\r\n", old, enable);

	return OK;
}

