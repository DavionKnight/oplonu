/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
*   This software is made available only to customers and prospective
*   customers of Opulan Technologies Corporation under license and may be
*   used only with Opulan semi-conductor products. 
*
* FILENAME:  cli_cmd_enable.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/cli/src/cli_cmd_enable.c#1 $
* $Log:$
*
*
**************************************************************************/
#include <stdlib.h>
#include "cli.h"
#include "cli_cmd_list.h"
#include <hw_interface.h>
#include <hal.h>
#include "hw_interface.h"
#include "tm.h"

#include <unistd.h>
#include <errno.h>
#include <sys/wait.h> 
#include <stats.h>
#include "run_led.h"
#include "uart_default.h"
//#include "serial_ser.h"
//#include "serial.h"
#include "terminal_server.h"
#include "terminal_server_api.h"
#include "cs_cmn.h"
#include "cs_types.h"
#include "product_info.h"
#include "ctc_2_1.h"
#include <math.h>

#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "sd.h"
#if defined(ONU_4PORT_AR8327)
#include "isis_reg_access.h"
#else
#include "shiva_reg_access.h"
#endif
#endif

STATUS cliCmdOrr(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_ORR_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    stRegParam.ulRegAddr = pstPt[0].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_ORR;
    stApiData.length = sizeof(HAL_API_GBL_ORR_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    return halAppApiCallSync(&stApiData);
}

STATUS cliCmdOwr(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_OWR_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    stRegParam.ulRegAddr = pstPt[0].u;
    stRegParam.ulRegVal = pstPt[1].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_OWR;
    stApiData.length = sizeof(HAL_API_GBL_OWR_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    return halAppApiCallSync(&stApiData);
}

STATUS cliCmdMrr(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_MRR_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stRegParam.ulRegAddr = pstPt[0].u;
    stRegParam.ulRegNum = pstPt[1].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_MRR;
    stApiData.length = sizeof(HAL_API_GBL_MRR_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdMwr(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_MWR_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stRegParam.ulRegAddr = pstPt[0].u;
    stRegParam.ulRegNum = pstPt[1].u;
    stRegParam.ulNewVal = pstPt[2].u;
    stRegParam.ulMode = pstPt[3].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_MWR;
    stApiData.length = sizeof(HAL_API_GBL_MWR_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);
    return retVal;
}

STATUS cliCmdOrt(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_ORT_t stRegParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stRegParam.ulRegion = pstPt[0].u;
    stRegParam.ulRecordStart = pstPt[1].u;
    stRegParam.ulRecordNum = pstPt[2].u;
    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_ORT;
    stApiData.length = sizeof(HAL_API_GBL_ORT_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);
    return retVal;
}

STATUS cliCmdOwt(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_OWT_t stRegParam;
	
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

	if (FALSE == utilIsInt(pstPt[4].p))
	{
		vosPrintf(pstEnv->nWriteFd, "%%Invalid input detected. Hex or deciamal format is expected.\r\n"); 
		return ERROR;
	}

    stRegParam.ulRegion = pstPt[0].u;
    stRegParam.ulRecordStart = pstPt[1].u;
    stRegParam.ulRecordNum = pstPt[2].u;
    stRegParam.ulOffset = pstPt[3].u;
    stRegParam.ulValue = vosStrToUInt(pstPt[4].p, NULL);

    stApiData.apiId = HAL_API_GBL_OWT;
    stApiData.length = sizeof(HAL_API_GBL_OWT_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    return halAppApiCallSync(&stApiData);
}

STATUS cliCmdOst(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_OST_t stRegParam;
	
    if (NULL == pstEnv)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stRegParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_GBL_OST;
    stApiData.length = sizeof(HAL_API_GBL_OST_t);
    stApiData.param = &stRegParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdBrgVttAdd(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
  
    if (pstPt[0].i >= 16)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if (pstPt[1].i >= 2)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if (pstPt[2].i >= 2)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if (pstPt[3].i >= 4096)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if (pstPt[4].i >= 4096)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    return brgVttDirectHwWrite(pstPt[0].i,1,pstPt[1].i, pstPt[2].i, pstPt[3].i, pstPt[4].i);
}
STATUS cliCmdBrgVttDel(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (pstPt[0].i >= 16)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    return brgVttDirectHwWrite(pstPt[0].i,0,0, 0,0, 0);
}
STATUS cliCmdBrgVttShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    return brgVttEntryShow(pstPt[0].i,pstPt[1].i);
}
#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)

STATUS cliCmdVttShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    return dalArVttShow(pstPt[0].i);
}

STATUS cliCmdVlanShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    return dalVlanShow(pstPt[0].i);
}
#endif

STATUS cliCmdTmUsShaperSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	int ret = OK;
  
	if (pstPt[0].i >= 8)
	{
		vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
		return ERROR;
	}
	if(pstPt[1].i == 1)//enable cir
	{
		ret += tmQueueShaperCirEnableHwWrite(UP_STREAM,pstPt[0].i,OPL_TRUE);
	}
	if(pstPt[1].i == 2)//disable cir
	{
		ret += tmQueueShaperCirEnableHwWrite(UP_STREAM,pstPt[0].i,OPL_FALSE);
	}
	if(pstPt[5].i == 1)//enable pir
	{
		ret += tmQueueShaperPirEnableHwWrite(UP_STREAM,pstPt[0].i,OPL_TRUE);
	}
	if(pstPt[5].i == 2)//disable pir
	{
		ret += tmQueueShaperPirEnableHwWrite(UP_STREAM,pstPt[0].i,OPL_FALSE);
	}
	if (pstPt[2].i == 1)//config cir
	{
		ret += tmUsShaperTabCirDirectHwWrite(pstPt[0].i,pstPt[3].i,pstPt[4].i);
	}
	if (pstPt[6].i == 1)// config pir
	{
		ret += tmUsShaperTabPirDirectHwWrite(pstPt[0].i,pstPt[7].i,pstPt[8].i);
	}

	return ret;    


    
}
STATUS cliCmdTmDsShaperSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	int ret = OK;

	if (pstPt[0].i > 8)
	{
		vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
		return ERROR;
	}
	if(pstPt[1].i == 1)
	{
		ret += tmQueueShaperEnableHwWrite(DOWN_STREAM,pstPt[0].i,OPL_TRUE);
	}
	if(pstPt[1].i == 2)
	{
		ret += tmQueueShaperEnableHwWrite(DOWN_STREAM,pstPt[0].i,OPL_FALSE);
	}
	if (pstPt[2].i == 1)
	{
		ret += tmDsShaperTabDirectHwWrite(pstPt[0].i,pstPt[3].i,pstPt[4].i);
	}
        

    return ret;
}

STATUS cliCmdPortQueuePriWeightSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int ret = OK;
	UINT8 stream;

	if (IS_ONU_RESPIN)
	{
		if(pstPt[0].i == 1)//upstream
			{
				stream=0;
			}
		else if(pstPt[0].i == 2)//downstream
			{
				stream=1;
			}else
			{
				return OPL_ERROR;
			}	
		/*configure queue priority*/
	    ret += tmQueuePriHwWrite(stream,pstPt[1].i,pstPt[2].i);
		/*configure queue weight*/
	 	ret += tmQueueWeightHwWrite(stream,pstPt[1].i,pstPt[3].i);
	}
	else
	{
		vosPrintf(pstEnv->nWriteFd, "%% Not support.\r\n");
	}
    return ret;
}

STATUS cliCmdTmQueueInfoSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	UINT8 stream = 0;

    if (pstPt[0].i >= 16)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }

	if(pstPt[0].i < 8)
	{
		stream = 0;	
	}else
	{
		stream = 1;
	}

	tmQueueMaxCellNumHwWrite(stream,(pstPt[0].i%8),pstPt[1].i);
	
    tmQueueMaxPktNumHwWrite(stream,(pstPt[0].i%8),pstPt[2].i);
}
STATUS cliCmdWredSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (pstPt[0].i >= 2)
    {
        vosPrintf(pstEnv->nWriteFd, "invalid input.\r\n");
        return ERROR;
    }
    if(pstPt[0].i == 0)
    {
        return tmUsWredTabDirectHwWrite(pstPt[1].i,pstPt[2].i,pstPt[3].i,pstPt[4].i);  
    }else
    {
        return tmDsWredTabDirectHwWrite(pstPt[1].i,pstPt[2].i,pstPt[3].i,pstPt[4].i);   
    }
    return OPL_ERROR;   
}


STATUS cliCmdTmTabShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    switch (pstPt[0].i)
    {
        case 1:
			cliTmCosIcosMapShow(pstEnv->nWriteFd);
			break;
		case 2:
			cliTmQidIcosMapShow(pstEnv->nWriteFd);
			break;
		case 3:	
			cliTmInQueueCounterShow(pstEnv->nWriteFd);
			break;
		case 4:	
			cliTmShaperConfigShow(pstEnv->nWriteFd);
			break;
		case 5:	
			cliTmInQueueConfigShow(pstEnv->nWriteFd);
			break;	
		case 6:
			cliTmWredConfigShow(pstEnv->nWriteFd);
			break;
		case 7:	
			cliTmQueueWeightShow(pstEnv->nWriteFd);
			break;
		case 8:	
			cliTmQueuePriorityShow(pstEnv->nWriteFd);
			break;
		case 9:	
			cliTmRptValueOfQueueShow(pstEnv->nWriteFd);
			break;
		case 10:	
			cliTmTotalPktsInQueueIncMpcpShow(pstEnv->nWriteFd);
			break;
        default:
            return ERROR;
    }
    return OK;
}

STATUS cliCmdCounterShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int portId;
    switch (pstPt[0].i)
    {
    	case 1:
			cliBrgGeParserCounterShow(pstEnv->nWriteFd);
			break;
		case 2:
			cliBrgPortCounterShow(pstEnv->nWriteFd);
			break;
		case 3:
			cliBrgCounterShow(pstEnv->nWriteFd);
			break;
		case 4:
			cliTmCounterShow(pstEnv->nWriteFd);
			break;
		case 5:
			cliPonMpcpCounterShow(pstEnv->nWriteFd);
			break;
		case 6:
			cliHostDmaCounterShow(pstEnv->nWriteFd);
			break;
		case 7: 	
			cliAllCounterShow(pstEnv->nWriteFd);
            vosPrintf(pstEnv->nWriteFd, "\r\n");
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
            sampleGetRMONCounter3(dev);
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
            extern sw_error_t atheros_show_cnt(fal_port_t port_id);
            for (portId = 0; portId < 7; portId++)
            {
                atheros_show_cnt(portId);
            }
#endif


			break;
        case 8:
			cliIntStatusShow(pstEnv->nWriteFd);
			break;
		default:
			break;
    }
    return OK;
}

STATUS cliCmdInterruptShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal;
    OPL_API_DATA_t stApiData;
    HAL_API_INT_COUNTER_SHOW_t stIntParam;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    stIntParam.ulFd = pstEnv->nWriteFd;

    stApiData.apiId = HAL_API_INT_COUNTER_SHOW;
    stApiData.length = sizeof(HAL_API_INT_COUNTER_SHOW_t);
    stApiData.param = &stIntParam;
    stApiData.eventFunc = NULL;
    
    retVal = halAppApiCallSync(&stApiData);

    cliShowEnd(pstEnv);

    return retVal;
}


STATUS cliCmdInterruptSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal = ERROR;
    OPL_API_DATA_t stApiData;
    HAL_API_INT_LEVEL1_ENABLE_t stIntLevel1Param;
    HAL_API_INT_LEVEL2_ENABLE_t stIntLevel2Param;


    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    stIntLevel1Param.ulModule = pstPt[0].u;
    stIntLevel1Param.ulEnable = pstPt[2].u - 1;

    stApiData.apiId = HAL_API_INT_LEVEL1_ENABLE;
    stApiData.length = sizeof(HAL_API_INT_LEVEL1_ENABLE_t);
    stApiData.param = &stIntLevel1Param;
    stApiData.eventFunc = NULL;

    retVal = halAppApiCallSync(&stApiData);
    retVal += stIntLevel1Param.ulResult;

    stIntLevel2Param.ulModule = pstPt[0].u;
    stIntLevel2Param.ulSubModule = pstPt[1].u;
    stIntLevel2Param.ulEnable = pstPt[2].u - 1;

    stApiData.apiId = HAL_API_INT_LEVEL2_ENABLE;
    stApiData.length = sizeof(HAL_API_INT_LEVEL2_ENABLE_t);
    stApiData.param = &stIntLevel2Param;
    stApiData.eventFunc = NULL;

    retVal += halAppApiCallSync(&stApiData);
    retVal += stIntLevel2Param.ulResult;

    return retVal;
}


STATUS cliCmdOnuLogicalIdSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	STATUS retVal;

	retVal = NO_ERROR;

	if (NULL == pstEnv || NULL == pstPt) 
	{
		retVal = ERROR;
		return retVal;
	}
	
    if (vosStrLen(pstPt[0].p) > 32)
    {
        vosPrintf(pstEnv->nWriteFd, "Logical onu id too long[%d]!\r\n", vosStrLen(pstPt[0].p));
        return ERROR;
    }

    if (pstPt[1].i == 1)
    {
        if (vosStrLen(pstPt[2].p) > 32)
        {
            vosPrintf(pstEnv->nWriteFd, "Password too long[%d]!\r\n", vosStrLen(pstPt[2].p));
            return ERROR;
        }
        odmPonLoidSet(pstPt[0].p);
        odmPonPasswordSet(pstPt[2].p);
    }
    else {
        odmPonLoidSet(pstPt[0].p);
	    odmPonPasswordSet(NULL);
    }	
	return retVal;
}


STATUS cliCmdMdioRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal = ERROR;
	UINT32 regId;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_MDIO_REG_t stRegParam;
    HAL_API_GBL_MDIO_REG_FIELDS_t stRegFieldsParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);

    if (pstPt[2].u == 1)
    {
    	for(regId = pstPt[1].u; regId < pstPt[1].u+pstPt[4].u;regId++)
    	{
            stRegParam.ucDevAddr = pstPt[0].u;
            stRegParam.ucRegAddr = regId;
            stRegParam.usData = 0;
            stRegParam.ulResult = 0;

            stApiData.apiId = HAL_API_GBL_MDIO_REG_READ;
            stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_t);
            stApiData.param = &stRegParam;
            stApiData.eventFunc = NULL;
            halAppApiCallSync(&stApiData);
    		vosPrintf(pstEnv->nWriteFd, "devAddr = %02u, regId = %02u val = 0x%04x\n", pstPt[0].u,regId,stRegParam.usData);
    	}
    }
    else if (pstPt[2].u == 2)
    {
        stRegFieldsParam.ucDevAddr = pstPt[0].u;
        stRegFieldsParam.ucRegAddr = pstPt[1].u;
        stRegFieldsParam.ucOffset = pstPt[6].u;
        stRegFieldsParam.ucWidth = pstPt[7].u;
        stRegFieldsParam.usData = 0;
        stRegFieldsParam.ulResult = 0;

        stApiData.apiId = HAL_API_GBL_MDIO_REG_FIELD_READ;
        stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_FIELDS_t);
        stApiData.param = &stRegFieldsParam;
        stApiData.eventFunc = NULL;
        retVal = halAppApiCallSync(&stApiData);
        vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d, offset = %d, width = %d, val = 0x%04x\n", pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u,stRegFieldsParam.usData);

    }
    else
    {
        stRegParam.ucDevAddr = pstPt[0].u;
        stRegParam.ucRegAddr = pstPt[1].u;
        stRegParam.usData = 0;
        stRegParam.ulResult = 0;

        stApiData.apiId = HAL_API_GBL_MDIO_REG_READ;
        stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_t);
        stApiData.param = &stRegParam;
        stApiData.eventFunc = NULL;
        halAppApiCallSync(&stApiData);
		vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d val = 0x%04x\n", pstPt[0].u, pstPt[1].u, stRegParam.usData);
    }

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdMdioWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    STATUS retVal = ERROR;
    OPL_API_DATA_t stApiData;
    HAL_API_GBL_MDIO_REG_t stRegParam;
    HAL_API_GBL_MDIO_REG_FIELDS_t stRegFieldsParam;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    if (pstPt[2].u == 1)
    {
        stRegParam.ucDevAddr = pstPt[0].u;
        stRegParam.ucRegAddr = pstPt[1].u;
        stRegParam.usData = pstPt[4].u;
        stRegParam.ulResult = 0;

        stApiData.apiId = HAL_API_GBL_MDIO_REG_WRITE;
        stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_t);
        stApiData.param = &stRegParam;
        stApiData.eventFunc = NULL;
        retVal =halAppApiCallSync(&stApiData);
		vosPrintf(pstEnv->nWriteFd, "devAddr = %d, regId = %d val = 0x%04x\n", pstPt[0].u,pstPt[1].u,stRegParam.usData);
    }
    else if (pstPt[2].u == 2)
    {
        stRegFieldsParam.ucDevAddr = pstPt[0].u;
        stRegFieldsParam.ucRegAddr = pstPt[1].u;
        stRegFieldsParam.ucOffset = pstPt[6].u;
        stRegFieldsParam.ucWidth = pstPt[7].u;
        stRegFieldsParam.usData = pstPt[8].u;
        stRegFieldsParam.ulResult = 0;

        stApiData.apiId = HAL_API_GBL_MDIO_REG_FIELD_WRITE;
        stApiData.length = sizeof(HAL_API_GBL_MDIO_REG_FIELDS_t);
        stApiData.param = &stRegFieldsParam;
        stApiData.eventFunc = NULL;
        retVal = halAppApiCallSync(&stApiData);
        if (retVal != OK || stRegFieldsParam.ulResult != OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "MDIO write (devAddr = %d, regId = %d, offset = %d, width = %d, val = 0x%04x) failed.\r\n", 
                pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u,pstPt[8].u);
        }
        else 
        {
            vosPrintf(pstEnv->nWriteFd, 
                "MDIO write (devAddr = %d, regId = %d, offset = %d, width = %d, val = 0x%04x) successfully.\r\n", 
                pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u,pstPt[8].u);
        }

    }

    return retVal;
}

#if defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
STATUS cliCmdAtherosPhyRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    sw_error_t retVal = ERROR;
	UINT32 regId;
    UINT16 regVal;
	UINT32 index;
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);
	regId = pstPt[2].u;

	for(index=0; index < pstPt[3].u;regId+=2,index++)
	{
	 #if defined(ONU_4PORT_AR8327)
        retVal = isis_phy_get(pstPt[0].u, pstPt[1].u, regId, &regVal);
	 #else
        retVal = shiva_phy_get(pstPt[0].u, pstPt[1].u, regId, &regVal);
	 #endif
        if (retVal != SW_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, PHY = %02u, regId = %02u, read failed.\n", 
                pstPt[0].u,pstPt[1].u,regId);
        }
        else {
		    vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, PHY = %02u, regId = %02u, val = 0x%04x\n", 
                pstPt[0].u,pstPt[1].u,regId,regVal);
        }
	}

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdAtherosPhyWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    sw_error_t retVal;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }
    #if defined(ONU_4PORT_AR8327)
    retVal = isis_phy_set(pstPt[0].u, pstPt[1].u, pstPt[2].u, (UINT16)pstPt[3].u);
    #else
    retVal = shiva_phy_set(pstPt[0].u, pstPt[1].u, pstPt[2].u, (UINT16)pstPt[3].u);
    #endif
    if (retVal != SW_OK)
    {
        vosPrintf(pstEnv->nWriteFd, 
            "Write devAddr = %02u, PHY = %02u, regId = %02u, val = 0x%04x failed.\n", 
            pstPt[0].u, pstPt[1].u, pstPt[2].u, (UINT16)pstPt[3].u);
    }
    else {
        vosPrintf(pstEnv->nWriteFd, 
            "Write devAddr = %02u, PHY = %02u, regId = %02u, val = 0x%04x succeeded.\n", 
            pstPt[0].u, pstPt[1].u, pstPt[2].u, (UINT16)pstPt[3].u);
    }

    return retVal;
}


STATUS cliCmdAtherosRegRead(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    sw_error_t retVal = ERROR;
	UINT32 regId;
    UINT32 regVal;
	UINT32 i=0;
	
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);
	
	regId=pstPt[1].u & 0xfffffffc;

    if (pstPt[2].u == 1)
    {
    	for(i=0; i < pstPt[4].u;regId+=4,i++)
    	{
    	     #if defined(ONU_4PORT_AR8327)
	     retVal = isis_reg_get(pstPt[0].u, regId, (a_uint8_t *) &regVal, sizeof(UINT32));
	     #else
            retVal = shiva_reg_get(pstPt[0].u, regId, (a_uint8_t *) &regVal, sizeof(UINT32));
	     #endif
            if (retVal != SW_OK)
            {
                vosPrintf(pstEnv->nWriteFd, 
                    "devAddr = %02u, regId = 0x%04x, read failed.\n", 
                    pstPt[0].u, regId);
            }
            else {
    		    vosPrintf(pstEnv->nWriteFd, 
                    "devAddr = %02u, regId = 0x%04x, val = 0x%08x\n", 
                    pstPt[0].u, regId, regVal);
            }
    	}
    }
    else if (pstPt[2].u == 2)
    {
        #if defined(ONU_4PORT_AR8327)
	 retVal = isis_reg_field_get(pstPt[0].u, regId, pstPt[6].u, pstPt[7].u, (a_uint8_t *) &regVal, sizeof(UINT32));
        #else
	 retVal = shiva_reg_field_get(pstPt[0].u, regId, pstPt[6].u, pstPt[7].u, (a_uint8_t *) &regVal, sizeof(UINT32));
	 #endif
        if (retVal != SW_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, offset = %d, width = %d, read failed.\n", 
                pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u);
        }
        else {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, offset = %d, width = %d, val = 0x%08x\n", 
                pstPt[0].u,pstPt[1].u,pstPt[6].u,pstPt[7].u, regVal);
        }
    }
    else if (pstPt[2].u == 0)
    {
        #if defined(ONU_4PORT_AR8327)
	 retVal = isis_reg_get(pstPt[0].u, regId, (a_uint8_t *) &regVal, sizeof(UINT32));
	 #else
        retVal = shiva_reg_get(pstPt[0].u, regId, (a_uint8_t *) &regVal, sizeof(UINT32));
	 #endif
        if (retVal != SW_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, read failed.\n", 
                pstPt[0].u, regId);
        }
        else {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, val = 0x%08x\n", 
                pstPt[0].u, regId, regVal);
        }
    }

    cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdAtherosRegWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    sw_error_t retVal = ERROR;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    if (pstPt[2].u == 1)
    {
        #if defined(ONU_4PORT_AR8327)
	 retVal = isis_reg_set(pstPt[0].u, pstPt[1].u, (a_uint8_t *) &pstPt[4].u, sizeof(UINT32));
	 #else
        retVal = shiva_reg_set(pstPt[0].u, pstPt[1].u, (a_uint8_t *) &pstPt[4].u, sizeof(UINT32));
	 #endif
        if (retVal != SW_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, write failed.\n", 
                pstPt[0].u, pstPt[1].u);
        }
        else {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, val = 0x%04x\n", 
                pstPt[0].u, pstPt[1].u, pstPt[4].u);
        }
    }
    else if (pstPt[2].u == 2)
    {
        #if defined(ONU_4PORT_AR8327)
	 retVal = isis_reg_field_set(pstPt[0].u, pstPt[1].u, pstPt[6].u, pstPt[7].u, (a_uint8_t *) &pstPt[8].u, sizeof(UINT32));
	 #else
        retVal = shiva_reg_field_set(pstPt[0].u, pstPt[1].u, pstPt[6].u, pstPt[7].u, (a_uint8_t *) &pstPt[8].u, sizeof(UINT32));
	 #endif
        if (retVal != SW_OK)
        {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, offset = %d, width = %d, write failed.\n", 
                pstPt[0].u, pstPt[1].u,pstPt[6].u,pstPt[7].u);
        }
        else {
            vosPrintf(pstEnv->nWriteFd, 
                "devAddr = %02u, regId = 0x%04x, offset = %d, width = %d, val = 0x%04x\n", 
                pstPt[0].u, pstPt[1].u,pstPt[6].u,pstPt[7].u,pstPt[8].u);
        }
    }

    return retVal;
}
STATUS cliCmdAtherosClassifyWrite(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    sw_error_t retVal = ERROR;

    if (NULL == pstEnv)
    {
        return ERROR;
    }
    cliShowStart(pstEnv);
	#if defined(ONU_4PORT_AR8327)
	retVal=isis_acl_rule_dump(0);
	retVal+=isis_acl_list_dump(0);
	#else
	retVal=shiva_acl_rule_dump(0);
	retVal+=shiva_acl_list_dump(0);
	#endif
	cliShowEnd(pstEnv);

    return retVal;
}

STATUS cliCmdAtherosMibShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    sw_error_t retVal = ERROR;
	UINT8 port=pstPt[0].u ;
    UINT32  atherosPort = dalToAtherosPortMap[port];

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    cliShowStart(pstEnv);
	retVal=atheros_show_cnt(atherosPort);
	cliShowEnd(pstEnv);

    return retVal;
}
extern UINT32 g_uiCntPollFlag;
STATUS cliCmdAtherosMibMode(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    OPL_BOOL bEnable;
	    
    if (NULL == pstEnv)
    {
        return ERROR;
    }
    if (1 == pstPt[0].i)
    {
    	g_uiCntPollFlag &= ~POLL_SWITCH_MASK;
    }
    else
    {
    	g_uiCntPollFlag=0xFFFFFFFF;
    }
    vosPrintf(pstEnv->nWriteFd,"\r\n mode %d  %x.\r\n",pstPt[0].i, g_uiCntPollFlag);

    return OK;

}
#endif


static VOS_THREAD_t g_pstI2cTestThreadId = NULL;
struct I2C_TEST_ARG_s
{
    int i2cMode;
    int intMode;
    int slaveAddr;
    int frequency;
    int fd;
};

struct I2C_TEST_ARG_s g_stI2CTestArg;

UINT32 g_ulI2CTestTotal = 0;
UINT32 g_ulI2CTestReadCnt = 0;
UINT32 g_ulI2CTestErrCnt = 0;

static void i2cTestingThread(void *pArg)
{
    int i; 
    STATUS retVal = ERROR;
    enum { BUF_SIZE = 256 };
    UINT8 aucOriginBuf[BUF_SIZE+1] = {0};
    UINT8 aucBuf[BUF_SIZE+1] = {0};
    char acTime[TIME_STR_LEN];
    time_t now;
    struct tm *pstLogTime;

    struct I2C_TEST_ARG_s *pstI2CTestArg = (struct I2C_TEST_ARG_s *)pArg;

    if (NULL == pstI2CTestArg) 
    {
        vosThreadExit(0);
        return;
    }

    g_ulI2CTestTotal = 0;
    g_ulI2CTestReadCnt = 0;
    g_ulI2CTestErrCnt = 0;

    time(&now);
    pstLogTime = gmtime(&now);
    strftime(acTime, TIME_STR_LEN, "%Y-%m-%d %H:%M:%S", pstLogTime);
    vosPrintf(pstI2CTestArg->fd, "\r\n\r\nI2C testing task started, at %s.\r\n", acTime);

    i2cInit(pstI2CTestArg->i2cMode, pstI2CTestArg->intMode, pstI2CTestArg->slaveAddr, pstI2CTestArg->frequency);

    while(1)
    {
        time(&now);
        pstLogTime = gmtime(&now);
        strftime(acTime, TIME_STR_LEN, "%Y-%m-%d %H:%M:%S", pstLogTime);
        retVal = i2cReadRandom(OT_EPROM_I2C_ADDR, 0, BUF_SIZE, aucBuf);
        g_ulI2CTestTotal++;
        if (retVal < 0) /* check status */
        {
            vosPrintf(pstI2CTestArg->fd, "%s: i2cRead got error(%ld). Reset I2C.\r\n", acTime, retVal);
            i2cInit(pstI2CTestArg->i2cMode, pstI2CTestArg->intMode, pstI2CTestArg->slaveAddr, pstI2CTestArg->frequency);
            vosSleep(1);
        }
        else {
            g_ulI2CTestReadCnt++;
            if (BUF_SIZE == retVal && g_ulI2CTestReadCnt == 3)
            {
                memcpy(aucOriginBuf, aucBuf, BUF_SIZE);
            }

            if (g_ulI2CTestReadCnt < 4)
            {
                vosPrintf(pstI2CTestArg->fd, "%s: i2cRead got data[%d]: \r\n", acTime, retVal);
                for (i = 0; i < retVal; i++)
                {
                    vosPrintf(pstI2CTestArg->fd, "%02X ", aucBuf[i]);
                }
                vosPrintf(pstI2CTestArg->fd, "\r\n\r\n");
            }
            else {
                if (0 != memcmp(aucOriginBuf, aucBuf, BUF_SIZE))
                {
                    g_ulI2CTestErrCnt++;
                    vosPrintf(pstI2CTestArg->fd, "%s: i2cRead got error data[%d]: \r\n", acTime, retVal);
                    for (i = 0; i < retVal; i++)
                    {
                        vosPrintf(pstI2CTestArg->fd, "%02X ", aucBuf[i]);
                    }
                    vosPrintf(pstI2CTestArg->fd, "\r\n\r\n");
                }
            }
            
            vosPrintf(pstI2CTestArg->fd, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            vosPrintf(pstI2CTestArg->fd, "%lu/%lu/%lu",g_ulI2CTestTotal,g_ulI2CTestReadCnt,g_ulI2CTestErrCnt);
        }
        /*vosUSleep(10);*/
    }
    vosThreadExit(0);
}

STATUS i2cTestingThreadInit(int fd, int intMode, int frequency)
{
    int ret;

    if (g_pstI2cTestThreadId) {
        return ERROR;
    }

    g_stI2CTestArg.i2cMode = 1;
    g_stI2CTestArg.intMode = intMode;
    g_stI2CTestArg.slaveAddr = 0;
    g_stI2CTestArg.frequency = frequency;
    g_stI2CTestArg.fd = fd;

    g_pstI2cTestThreadId = vosThreadCreate("tI2CTest", OP_VOS_THREAD_STKSZ, 90,
                        (void *)i2cTestingThread, (void *)&g_stI2CTestArg);
    if (g_pstI2cTestThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS i2cTestingThreadShutdown(int fd)
{
    char acTime[TIME_STR_LEN];
    time_t now;
    struct tm *pstLogTime;

    if (!g_pstI2cTestThreadId) {
        return ERROR;
    }

    if (g_pstI2cTestThreadId != NULL) {
        vosThreadDestroy(g_pstI2cTestThreadId);
        g_pstI2cTestThreadId = NULL;
    }

    time(&now);
    pstLogTime = gmtime(&now);
    strftime(acTime, TIME_STR_LEN, "%Y-%m-%d %H:%M:%S", pstLogTime);
    vosPrintf(fd, "\r\n\r\nI2C testing task stoped, at %s.\r\n", acTime);
    vosPrintf(fd, "total: %lu, read: %lu, err: %lu.\r\n\r\n",g_ulI2CTestTotal,g_ulI2CTestReadCnt,g_ulI2CTestErrCnt);
    
    return OK;
}


STATUS cliCmdI2CTestStart(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv || NULL == pstPt) 
	{
		return ERROR;
	}
	
    if (NULL != g_pstI2cTestThreadId)
    {
        vosPrintf(pstEnv->nWriteFd, "I2C testing task has already been started.\r\n");
        return OK;
    }
    
    return i2cTestingThreadInit(pstEnv->nWriteFd, pstPt[0].u-1, pstPt[1].u-1);
}

STATUS cliCmdI2CTestStop(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{

    if (NULL == pstEnv) 
	{
		return ERROR;
	}
	
    if (NULL == g_pstI2cTestThreadId)
    {
        vosPrintf(pstEnv->nWriteFd, "I2C testing task is not started.\r\n");
        return OK;
    }
    
    return i2cTestingThreadShutdown(pstEnv->nWriteFd);
}

STATUS cliCmdLinuxShell(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    int oldPhase, pid, stat;

    if (NULL == pstEnv)
    {
        return ERROR;
    }

    oldPhase = pstEnv->phase;
    pstEnv->phase = CLI_CMD_PHASE_SYS_SHELL;
    pstEnv->used = TRUE;
    if (pstEnv->ucIsConsole)
    {
        vosTermRestore(pstEnv->nReadFd);
        setenv("PS1", "OPULAN / #", 1);
        vosSystem("/bin/sh");
	    vosTermConfig(pstEnv->nReadFd);
    }
    else {
	    vosPrintf(pstEnv->nWriteFd, "%% Does not support, please telnet to 23 port.\r\n");
    }

    pstEnv->phase = oldPhase;

    return OK;
}


STATUS  cliCmdBoardReset(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	char uData;
	
	cpldRead(CS1_RESET_REG,&uData);
	cpldWrite(CS1_RESET_REG, (uData & 0xfe));
      vosSleep(1);;
	cpldWrite(CS1_RESET_REG,(uData | 0x01));

	return OK;
}

STATUS cliCmdPonCpld(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	char uData;
	unsigned char pon_num=0;

#if 0
	pon_num = pstPt[1].u - 1;
	pon_num = 0x1 & (!pon_num);
	pon_num = pon_num << 1;
	pon_num_and = pon_num |0xfd;
	cpldRead(0x0b,&uData);
	cpldWrite(0x0b,(uData & 0xfe));
	cpldWrite(0x0b,((uData & 0xfe) & pon_num_and | pon_num));
#else
	pon_num = pstPt[1].u - 1;
	pon_num = 0x1 & (!pon_num);	
	pon_num = pon_num << 1;
	cpldRead(CS1_PON_CONTROL,&uData);
	uData &= 0xfc;
	uData |= pon_num;
	cpldWrite(CS1_PON_CONTROL,uData);	
#endif
printf("set cpld pon%d over ..\n",pstPt[1].u);
	return OK;
}

STATUS cliCmdPonCpu(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	char uData;
	unsigned char pon_num=0;
#if 0
	pon_num = pstPt[1].u - 1;
	pon_num = 0x1 & (!pon_num);	
	pon_num = pon_num << 1;
	pon_num_and = pon_num |0xfd;
	cpldRead(0x0b,&uData);
	cpldWrite(0x0b,(uData | 0x01));
	cpldWrite(0x0b,((uData | 0x01) & pon_num_and | pon_num));
#else
	pon_num = pstPt[1].u - 1;
	pon_num = 0x1 & (!pon_num);	
	pon_num = pon_num << 1;
	pon_num |= 0x1;
	cpldRead(CS1_PON_CONTROL,&uData);
	uData &= 0xfc;
	uData |= pon_num;
	cpldWrite(CS1_PON_CONTROL,uData);	
#endif
printf("set cpu pon%d over ..\n",pstPt[1].u);
	return OK;
}
STATUS cliCmdSerialSet(ENV_t *pstEnv,PARA_TABLE_t *pstPt)
{

	unsigned int  baud_rate,data_size,stopbit,parity_size;
	unsigned char port_num;
	int value;
	char FCR_reg = 0;
	char MCR_reg;

	
    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

printf("port_num is %d,baud_rate is %d, data_size is %d ,stopbit is %d ,parity_size is %d\n",pstPt[1].u,pstPt[3].u,pstPt[5].u,pstPt[7].u,pstPt[9].u);
	port_num  = pstPt[1].u-1;
	baud_rate = pstPt[3].u;
	data_size = pstPt[5].u;
	stopbit =pstPt[7].u;
	parity_size =  pstPt[9].u;

 #ifndef SERIAL_8250
    Serial_Cs_Set(port_num);

   	UARTClearAllReg();
 
	UARTEFREnable(FALSE);
 
	S2E_SET_info(port_num, SERIAL_BAUD, &baud_rate);
 
	S2E_SET_info(port_num ,SERIAL_DATA_SIZE, &data_size);
 
	S2E_SET_info(port_num, SERIAL_PARITY_CTRL, &parity_size);
 
	S2E_SET_info(port_num, SERIAL_STOP_BIT, &stopbit);
 
	value = SERIAL_DEFAULT_FC;
	S2E_SET_info(port_num, SERIAL_FC_MODE_CTRL, &value);
 
    Serial_Cs_Set(port_num);
    UARTFIFOEnable(TRUE, &FCR_reg);
    UARTDMAEnable(TRUE, &FCR_reg);
    UART_FIRO_Rx_Trigger_Level(SERIAL_DEFAULT_TRIGGER_RX, &FCR_reg);
//	printf("FCR_reg is 0x%x\n",FCR_reg);
    UARTSet_FCR(FCR_reg);

 
    UARTIntEnable(TRUE);
    //
    // Start the UART.
    //
 
    SerialPurgeData(port_num);
    UARTFIRO_RDY_Enable(TRUE);
    //
    // (Re)enable the UART transmit and receive interrupts.
    //
    UARTIntType(SERIAL_INT_TYPE_RX, TRUE);
    UARTIntType(SERIAL_INT_TYPE_TX, TRUE);

    UARTRxTimeOutEn(TRUE);
 //UARTEFREnable(TRUE);
	if((SERIAL_TYPE_485 == S2E_Control.serial_type)&&(SERIAL_DUPLEX_HALF ==serial_duplex))
	{
		Serial_Uart_485_half(port_num, TRUE);
	}
	else
	{
    		Serial_Uart_485_half(port_num, FALSE);
	}
	cpldcs2Read(0xc, &MCR_reg);
	MCR_reg&=0xef;
	cpldcs2Write(0xc,MCR_reg);    //stop loopback  add by zhangjj   2013.1.7
#else

	port_num  = pstPt[1].u-1;
	baud_rate = pstPt[3].u;
	data_size = pstPt[5].u;
	stopbit =pstPt[7].u;
	parity_size =  pstPt[9].u;
ts_serial_open_single(port_num, baud_rate, data_size, stopbit, parity_size);
#endif


	int ret;
	term_server_config_t Term_serv_cfg;

	ret = ts_get_config(pstPt[1].u, &Term_serv_cfg);
	if(ERROR == ret)
	{
		printf("get  config error\n");
		return OK;
	}	
	Term_serv_cfg.uart_cfg.baud_rate = baud_rate;
	Term_serv_cfg.uart_cfg.data_bits = data_size;
	Term_serv_cfg.uart_cfg.stop_bits = stopbit;
	Term_serv_cfg.uart_cfg.parity= parity_size;

	ts_save_config(pstPt[1].u, &Term_serv_cfg);
	
	printf("serial set successfully...!\n");
	
#if 0

	for(i=0;i<20;i++)
	{
		cpldcs2Read(i, &setData);
		printf("set Data  %d  is 0x%x\n",i,setData);
	}
#endif
//}
  //  cliShowEnd(pstEnv);
	
	return OK;
}

STATUS cliCmdMessageSend(ENV_t *pstEnv,PARA_TABLE_t *pstPt)
{
	unsigned int portnum;
	unsigned int para=0;

	portnum = pstPt[1].u-1;

	printf("port  is %d\n",pstPt[1].u);
	printf("message is %s\n",pstPt[3].p);

//	SerialSend(portnum, pstPt[3].p, strlen(pstPt[3].p));

	portComSend(serial_fds[portnum],pstPt[3].p, strlen(pstPt[3].p));

	return OK;
}

STATUS cliCmdModeType(ENV_t *pstEnv,PARA_TABLE_t *pstPt)
{
#ifndef SERIAL_8250
	unsigned int ulPort ;
	unsigned int serial_type;

	ulPort = pstPt[1].u-1;
	serial_type = pstPt[3].u;
if(serial_type == 232)
	 SerialSetUartInterface(ulPort, SERIAL_TYPE_232);
else
	SerialSetUartInterface(ulPort, SERIAL_TYPE_485);

	 printf("set port %d mode %d successfully\n",ulPort,serial_type);
	 return OK;
#endif
}

STATUS cliCmdReceive(ENV_t *pstEnv,PARA_TABLE_t *pstPt)
{
	if(!strcmp(pstPt[1].p,"on"))
	{
		receive_flag = 1;
		printf("receive task on...\n");
	}
	else if(!strcmp(pstPt[1].p,"off"))
	{
		receive_flag = 0;
		printf("receive task off...\n");
	}
	else
	{
		printf("error command please check the last parameter!\n");
		return ERROR;
	}
	return OK;
}

STATUS cliCmdTsConfigure(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	term_server_config_t Term_serv_cfg;
	int ret;

	printf("uart is %d ,proto is %d,port is %d\n",pstPt[1].u,pstPt[3].u,pstPt[5].u);

            
                memset(&Term_serv_cfg,0,sizeof(term_server_config_t));

		ret = ts_get_config(pstPt[1].u, &Term_serv_cfg);
		if(ERROR == ret)
		{
                Term_serv_cfg.uart_cfg.baud_rate = SERIAL_DEFAULT_BAUD_RATE;
                Term_serv_cfg.uart_cfg.data_bits = SERIAL_DEFAULT_DATA_SIZE;
                Term_serv_cfg.uart_cfg.parity = SERIAL_DEFAULT_PARITY;
                Term_serv_cfg.uart_cfg.stop_bits = 1;
		}
			
                Term_serv_cfg.client_fd = -1;
//			if(old_port == 0)			//if have not bind before then initialize, else  do not
                Term_serv_cfg.server_fd = -1;
                Term_serv_cfg.init_flag = VALID_CONFIG_FLAG;
                Term_serv_cfg.uart_cfg.mode = UART_MODE_INTR;
                Term_serv_cfg.uart_cfg.duplex = UART_DUPLEX_HALF;

                Term_serv_cfg.client_closed_flag = 1;
                Term_serv_cfg.uart_cfg.uart_id = pstPt[1].u;
                Term_serv_cfg.proto_type = pstPt[3].u;
                Term_serv_cfg.proto_port = pstPt[5].u;
                Term_serv_cfg.client_timeout = 0;
                Term_serv_cfg.min_payload = 40;
                Term_serv_cfg.max_payload = 1024;
                Term_serv_cfg.max_res_time = 1000;
                if(CS_E_OK == ts_uart_id_check(pstPt[1].u))
                {
                    if(epon_request_onu_ts_enable(0,0,
                        Term_serv_cfg.uart_cfg.uart_id,
                        (term_server_config_t *)&Term_serv_cfg))
                        printf("Terminal Server %d is created failed\n\r",
                                            UART_TO_TERM_SERV(pstPt[1].u));
                    else
                        printf("Terminal Server %d is created successfully\n\r",
                                            UART_TO_TERM_SERV(pstPt[1].u));
//					printf("Term serv enable is %d\n",Term_serv_cfg.enable_flag);
				ts_save_config(pstPt[1].u, &Term_serv_cfg);
                }
                else
                    printf("Invalid UART ID [1~4]\n\r");

 return OK;
}

STATUS cliCmdTsDisable(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	if(CS_E_OK == ts_uart_id_check(pstPt[1].u))
	{
		epon_request_onu_ts_disable(0,0,pstPt[1].u);
		
	}
	else
	{
		printf("Invalid UART ID [1~4]\n\r");
	}
	return OK;
}

STATUS cliCmdTsShow(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
#if 0
	char buf[20]={0};
	printf("\nThe Config Terminal Server Have Saved:\n\n");
	vosSprintf(buf,"cat %s",TERM_SERV_CFG_FILE);
	system(buf);
#else
//	char section[20];
	int ret=0;

//	strcpy(section,"ts.conf");
//printf("section is %s ...\r\n",section);
	vosConfigShowByModule(TERM_SERV_CFG_FILE,pstEnv->nWriteFd);
#endif
	return OK;
}
STATUS cliCmdTsSetip(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	char ip_set[30]={0};

	if(vosStrCmp(pstPt[1].p,"lan")==0)
		vosSprintf(ip_set, "ifconfig eth0 %s",pstPt[3].p);
	else if(vosStrCmp(pstPt[1].p,"wan")==0)
		vosSprintf(ip_set, "ifconfig eth4 %s",pstPt[3].p);
	else
	{
		printf("Please input correct command :\nLike   ts ip set wan 192.168.0.1\n");
		return OK;
	}
	printf("New IP is %s\n",pstPt[3].p);
	printf("\nAll of Terminal Servers Will Restart ...\n\n");

	ts_disable(1);
	ts_disable(2);
	ts_disable(3);
	ts_disable(4);
	system(ip_set);
//	system("ifconfig eth0:0");
vosSleep(1);
	ts_auto_create();
	printf("\nSet New IP Successfully!\n\n");
	return OK;
}
STATUS cliCmdTelnetSetip(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	char ip_set[30]={0};

	printf("New IP is %s\n",pstPt[1].p);
	vosSprintf(ip_set, "ifconfig eth3 %s",pstPt[1].p);
	system(ip_set);
	telnet_ip_save(pstPt[1].p);
	return OK;
}
STATUS cliCmdProductDateSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{

	char section[20];
	char date_value[15];
	int ret=0;

	strcpy(section,PRODCUT_INFO_SECTION);
	vosSprintf(date_value, "%d-%d-%d",pstPt[1].u,pstPt[3].u,pstPt[5].u);

	if(vosConfigSectionIsExisted(PRODUCT_CFG_FILE,section))
	{
		ret = vosConfigSectionCreate(PRODUCT_CFG_FILE,section);
	}
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"Manufature date",date_value)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}
	    cliTextConfigSave();
    vosConfigSave(NULL);
	return OK;
}

STATUS cliCmdProductSerialSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	char section[20];
	int ret=0;

	strcpy(section,PRODCUT_INFO_SECTION);

	if(vosConfigSectionIsExisted(PRODUCT_CFG_FILE,section))
	{
		ret = vosConfigSectionCreate(PRODUCT_CFG_FILE,section);
	}
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"Serial number",pstPt[1].p)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}

    cliTextConfigSave();
    vosConfigSave(NULL);
	return OK;
}
STATUS cliCmdProductHwVerSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	char section[20];
	char date_value[10];
	int ret=0;

	strcpy(section,PRODCUT_INFO_SECTION);
	vosSprintf(date_value, "V%d.%dB%d",pstPt[1].u,pstPt[3].u,pstPt[5].u);

	if(vosConfigSectionIsExisted(PRODUCT_CFG_FILE,section))
	{
		ret = vosConfigSectionCreate(PRODUCT_CFG_FILE,section);
	}
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"Hardware version",date_value)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}
	   cliTextConfigSave();
    vosConfigSave(NULL);
	return OK;
}
/*
STATUS cliCmdProductMACSet(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	char section[20];
	int ret=0;

	strcpy(section,PRODCUT_INFO_SECTION);

	if((ret =strlen(pstPt[1].p))!=17)
	{
		printf("Length of MAC is wrong!\n");
		return ERROR;
	}

	if(vosConfigSectionIsExisted(PRODUCT_CFG_FILE,section))
	{
		ret = vosConfigSectionCreate(PRODUCT_CFG_FILE,section);
	}
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"MAC",pstPt[1].p)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}
	
	return OK;
}*/
STATUS cliCmdProductShowInfo(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	char section[20];
	int ret=0;

	strcpy(section,PRODCUT_INFO_SECTION);

	vosConfigShowBySection(PRODUCT_CFG_FILE,section,pstEnv->nWriteFd);

	return OK;
}

STATUS cliCmdRelayAlarmEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	if(0 == pstPt[1].u)
		SetAlarmEnable(1);
	else if(1 == pstPt[1].u)
		SetAlarmEnable(0);
	else
		printf("error\r\n");
	return OK;
}

STATUS cliCmdLedAlarmEnable(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	test_led_flag =1;	
	return OK;
}
STATUS cliCmdLedAlarmDisable(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	test_led_flag =2;
	return OK;
}
STATUS cliCmdDisplayOpm(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
	short int temperature;
	u16_t vcc;
	u16_t current;
	u16_t txPower;
	u16_t rxPower;
	double txdbm =0.9,rxdbm = 0.0;

	stub_get_onu_optical_transiver_temperature(&temperature);
	stub_get_onu_optical_transiver_supply_vcc(&vcc);
	stub_get_onu_optical_transiver_tx_bias_current(&current);
	stub_get_onu_optical_transiver_tx_power(&txPower);
	stub_get_onu_optical_transiver_rx_power(&rxPower);

	temperature /=256;
	vcc = (u16_t)(vcc*0.1);
	current *=0.002;
	txdbm = txPower;
	rxdbm = rxPower;

	txdbm = 10*log10(txdbm*0.0001);
	rxdbm = 10*log10(rxdbm*0.0001);

	printf("temperature			:%hd C\n",temperature);
	printf("working-voltage			:%hd mV\n",vcc);
	printf("bias-current			:%hd mA\n",current);
	printf("txPower				:%4.1f dbm\n",txdbm);
	printf("rxPower				:%4.1f dbm\n",rxdbm);
	return OK;
}
STATUS cliCmdShowThreadInfo(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    if (NULL == pstEnv)
    {
        return ERROR;
    }

	vosThreadShow(pstEnv->nWriteFd);

    return OK;
}


STATUS cliCmdOpticalModuleOnOff(ENV_t *pstEnv, PARA_TABLE_t *pstPt)
{
    UINT32 retVal;
    int i;

    if (NULL == pstEnv || NULL == pstPt)
    {
        return ERROR;
    }

    if (1 == pstPt->u)
    {
        vosPrintf(pstEnv->nWriteFd, "Turn on optical module ... ");
        odmPonTxPowerDown(FALSE);
        vosPrintf(pstEnv->nWriteFd, "done\r\n");
    }
    else if (2 == pstPt->u)
    {
        vosPrintf(pstEnv->nWriteFd, "Turn off optical module ... ");
        odmPonTxPowerDown(TRUE);
        vosPrintf(pstEnv->nWriteFd, "done\r\n");
    }

    return OK;
}


extern unsigned long g_ulVosAllocBytes;
extern unsigned long g_ulVosAllocCalls;
extern unsigned long g_ulVosFreeBytes;
extern unsigned long g_ulVosFreeCalls;

STATUS cliCmdShowMemInfo(ENV_t *pstEnv, PARA_TABLE_t *psPara)
{
    char *header  = "  status    calls       bytes\r\n";
    char *divider = " --------- ----------- ----------- \r\n";
    char *format = " %-9s %11u %11u\r\n";
    
    vosPrintf(pstEnv->nWriteFd, header);
    vosPrintf(pstEnv->nWriteFd, divider);
    vosPrintf(pstEnv->nWriteFd, format, "alloc", g_ulVosAllocCalls, g_ulVosAllocBytes);
    vosPrintf(pstEnv->nWriteFd, format, "free", g_ulVosFreeCalls, g_ulVosFreeBytes);

}


