/*
=============================================================================
     File Name: hal_rstp.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 jiangmingli	   2009/03/17		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <vos.h>
#include "hal.h"
#include <dal_rstp.h>

#include "hal_rstp.h"

/*******************************************************************************
* halRstpModeSet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halRstpModeSet(IN VOID *pPara)
{
    OPL_BOOL bEnable;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    bEnable = *(OPL_BOOL *)pPara;

    dalRstpModeSet(bEnable);

    return;
}

/*******************************************************************************
* halRstpPortStateSet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halRstpPortStateSet(IN VOID *pPara)
{
    HAL_API_RSTP_PORT_STATE_SET_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_RSTP_PORT_STATE_SET_S *)pPara;

    dalRstpPortStateSet(pstPara->ulPortId, pstPara->ucPortState);

    return;
}

/*******************************************************************************
* halRstpPortStateGet
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halRstpPortStateGet(IN VOID *pPara)
{
    HAL_API_RSTP_PORT_STATE_GET_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_RSTP_PORT_STATE_GET_S *)pPara;

    dalRstpPortStateGet(pstPara->ulPortId, pstPara->pucPortState);

    return;
}

/*******************************************************************************
* halRstpPortMacFlush
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
VOID halRstpPortMacFlush(IN VOID *pPara)
{
    HAL_API_RSTP_PORT_MAC_FLUSH_S *pstPara;

    if (NULL == pPara)
    {
        OPL_TRACE();
        return;
    }

    pstPara = (HAL_API_RSTP_PORT_MAC_FLUSH_S *)pPara;

    dalRstpPortMacFlush(pstPara->ulPortid, pstPara->bOnlyThisPort);

    return;
}


/*******************************************************************************
* halRstpInit
*
* DESCRIPTION:
*
*	INPUTS:
*
*	OUTPUTS:
*		
* 	RETURNS:
*
* 	SEE ALSO: 
*/
OPL_RESULT halRstpInit(VOID)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    /* register rstp api */
    stApiUnit.apiId = HAL_API_RSTP_MODE_SET;
    stApiUnit.apiFunc = halRstpModeSet;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_RSTP_PORT_STATE_SET;
    stApiUnit.apiFunc = halRstpPortStateSet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_RSTP_PORT_STATE_GET;
    stApiUnit.apiFunc = halRstpPortStateGet;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_RSTP_PORT_MAC_FLUSH;
    stApiUnit.apiFunc = halRstpPortMacFlush;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ulRet;
}



