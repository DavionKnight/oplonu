/*************************************************************************
*
*  COPYRIGHT (C) 2003-2009 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  hal_interrupt.c
*
* DESCRIPTION: 
*	
*
* Date Created: Mar 18, 2009
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/src/hal_interrupt.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos.h>
#include <hal.h>
#include <hal_interrupt.h>
#include <interrupt.h>


extern OPL_INT_t oplIntInfo[];

UINT32 intEnable = 1;
static VOS_THREAD_t g_pstIntProcessThreadId = NULL;


/*******************************************************************************
*
* halInterruptLevel1Enable:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptLevel1Enable(VOID *pParam)
{
    HAL_API_INT_LEVEL1_ENABLE_t *pIntParam = (HAL_API_INT_LEVEL1_ENABLE_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = interruptLevel1Enable(pIntParam->ulModule, pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptLevel1EnableState:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptLevel1EnableState(VOID *pParam)
{
    HAL_API_INT_LEVEL1_ENABLE_STATE_t *pIntParam = (HAL_API_INT_LEVEL1_ENABLE_STATE_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = interruptLevel1EnableState(pIntParam->ulModule, &pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptLevel2Enable:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptLevel2Enable(VOID *pParam)
{
    HAL_API_INT_LEVEL2_ENABLE_t *pIntParam = (HAL_API_INT_LEVEL2_ENABLE_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = interruptLevel2Enable(pIntParam->ulModule, pIntParam->ulSubModule, pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptLevel2EnableState:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptLevel2EnableState(VOID *pParam)
{
    HAL_API_INT_LEVEL2_ENABLE_t *pIntParam = (HAL_API_INT_LEVEL2_ENABLE_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = interruptLevel2EnableState(pIntParam->ulModule, 
        pIntParam->ulSubModule, &pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptCounterEnable:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptCounterEnable(VOID *pParam)
{
    HAL_API_INT_COUNTER_EN_t *pIntParam = (HAL_API_INT_COUNTER_EN_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = intCountEnable(pIntParam->ulEnable);
    
}


/*******************************************************************************
*
* halInterruptCounterEnableState:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptCounterEnableState(VOID *pParam)
{
    HAL_API_INT_COUNTER_EN_t *pIntParam = (HAL_API_INT_COUNTER_EN_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    pIntParam->ulResult = intCountEnableState(&pIntParam->ulEnable);
}


/*******************************************************************************
*
* halInterruptCounterShow:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptCounterShow(VOID *pParam)
{
    HAL_API_INT_COUNTER_SHOW_t *pIntParam = (HAL_API_INT_COUNTER_SHOW_t *)pParam;

    if (NULL == pIntParam)
    {
        return;
    }

    if (0 == pIntParam->ulFd)
    {
        pIntParam->ulFd = OP_STDOUT_FILENO;
    }

	UINT32 firsIntIndex = OPL_ZERO;
	UINT32 secondIntIndex = OPL_ZERO;

	vosPrintf(pIntParam->ulFd, "%-20s %-12s %s\r\n", " Description", " Counter", " Error Counter");
	for(firsIntIndex = OPL_ZERO; (INT32)oplIntInfo[firsIntIndex].id != -1; firsIntIndex++)
	{
		vosPrintf(pIntParam->ulFd, "-------------------- ------------ ------------\r\n");
		vosPrintf(pIntParam->ulFd, "%-20s %-12d %-12d\r\n",
			oplIntInfo[firsIntIndex].name,
			oplIntInfo[firsIntIndex].counter,
			oplIntInfo[firsIntIndex].errCounter);
		
		for(secondIntIndex = OPL_ZERO; oplIntInfo[firsIntIndex].level2Int[secondIntIndex].id != -1; secondIntIndex++)
		{
			vosPrintf(pIntParam->ulFd, "\r\n%40s %-10d %-10d\r\n",
			oplIntInfo[firsIntIndex].level2Int[secondIntIndex].name,
			oplIntInfo[firsIntIndex].level2Int[secondIntIndex].counter,
			oplIntInfo[firsIntIndex].level2Int[secondIntIndex].ulErrCnt);
		}	
	}
}

#define waitForintrrupt 	wait_for_ipmux_intr
#define enableIntrrupt  	enable_ipmux_intr

static void halInterruptThread()
{
    UINT8 i;
    HAL_EVENT_INT_HANDLING_t stIntParam;
    OPL_EVENT_DATA_t stEventData;
    UINT32 ulParamLen = sizeof(HAL_EVENT_INT_HANDLING_t);

    stIntParam.ulModInt = OPL_ZERO;
    /* enable LOS_IND_INT_EN */
    oplRegFieldWrite(REG_PMAC_INT_EN, 31, 1, 1);
    oplRegFieldWrite(REG_PMAC_INT_EN, 29, 1, 1);

	while(1)
	{
		waitForintrrupt(&(stIntParam.ulModInt));

        for (i = OPL_ZERO; i<OPL_END_INTR; i++)
    	{ 
    	    if ((stIntParam.ulModInt & OPCONN_BIT(i)) &&
                (oplIntInfo[i].id < OPL_END_INTR) && 
                oplIntInfo[i].level2Int[OPL_ZERO].penAddr > OPL_ZERO)
    	    {
                oplRegRead(oplIntInfo[i].level2Int[OPL_ZERO].penAddr,
                    &(stIntParam.ulSubModInt[i]));
    	    }
        }

        if (NO_ERROR == halAppEventIsExisted(HAL_EVENT_INT_HANDLING) &&
            (OPCONN_BIT(OPL_PMAC_INTR) & stIntParam.ulModInt) && 
            (stIntParam.ulSubModInt[OPL_PMAC_INTR]&
            oplIntInfo[OPL_PMAC_INTR].level2Int[LOS_IND_INT_TYPE].penBit))
        {
            stEventData.eventId = HAL_EVENT_INT_HANDLING;
            stEventData.length = ulParamLen;
            stEventData.param = (VOID *)&stIntParam;
            halDrvEventRaise(&stEventData);
        }

	    if(intEnable)
	    {
		    intProcesstHandler(&stIntParam);
        }

		enableIntrrupt();
	}
}

STATUS halInterruptThreadInit(void)
{
    int ret;

    if (g_pstIntProcessThreadId) {
        return ERROR;
    }

    g_pstIntProcessThreadId = vosThreadCreate("tIntProc", OP_VOS_THREAD_STKSZ, 80,
                        (void *)halInterruptThread, (void *)NULL);
    if (g_pstIntProcessThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS halInterruptThreadShutdown(void)
{
    if (!g_pstIntProcessThreadId) {
        return ERROR;
    }

    if (g_pstIntProcessThreadId != NULL) {
        vosThreadDestroy(g_pstIntProcessThreadId);
    }

    return OK;
}




/*******************************************************************************
*
* halInterruptInit:	 to initialize interrupt hal module
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	n/a.
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	OK      - initialize successfully.
*   ERROR   - initialize failed.
*
* SEE ALSO: 
*/
OPL_RESULT halInterruptInit(void)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    /* register interrupt API */

    stApiUnit.apiId = HAL_API_INT_LEVEL1_ENABLE;
    stApiUnit.apiFunc = halInterruptLevel1Enable;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL1_ENABLE_STATE;
    stApiUnit.apiFunc = halInterruptLevel1EnableState;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL2_ENABLE;
    stApiUnit.apiFunc = halInterruptLevel2Enable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL2_ENABLE_STATE;
    stApiUnit.apiFunc = halInterruptLevel2EnableState;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_ENABLE;
    stApiUnit.apiFunc = halInterruptCounterEnable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_ENABLE_STATE;
    stApiUnit.apiFunc = halInterruptCounterEnableState;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_SHOW;
    stApiUnit.apiFunc = halInterruptCounterShow;
    ulRet += halDrvApiRegister(&stApiUnit);

    if(intEnable)
	{
        /* enable global interrupt */
		enableIntrrupt();
        
        /* enable specific interrupt here */
#if 0  /* deleted by Gan Zhiheng - 2009/07/15, only use for MPW */
        oplRegFieldWrite(REG_PMAC_INT_EN, 31, 1, 1);
#endif /* #if 0 */

	}

    halInterruptThreadInit();

    return ulRet;
}

/*******************************************************************************
*
* halInterruptDestroy:	 
*
* DESCRIPTION:
* 	
*
* INPUTS:
*	
*
* OUTPUTS:
*	n/a.
*
* RETURN:
* 	n/a
*
* SEE ALSO: 
*/
VOID halInterruptDestroy()
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    halInterruptThreadShutdown();

    stApiUnit.apiFunc = NULL;

    /* deregister interrupt API */

    stApiUnit.apiId = HAL_API_INT_LEVEL1_ENABLE;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL1_ENABLE_STATE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL2_ENABLE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_LEVEL2_ENABLE_STATE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_ENABLE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_ENABLE_STATE;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_INT_COUNTER_SHOW;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ;
}

