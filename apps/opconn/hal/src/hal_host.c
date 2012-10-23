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
* FILENAME:  hal_host.c
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
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/hal/src/hal_host.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <vos.h>
#include <hal.h>
#include <opl_host.h>

#ifdef OPL_DMA0_ETH
    #define PKT_BUFF_MODE 
#endif

#define HOST_DMA_COUNTER_NUM    11

static VOS_THREAD_t g_pstHostThreadId = NULL;

typedef struct HOST_COUNTER_s
{
	INT32 	id;
	UINT8	*name;
	UINT64 	counter;
} HOST_COUNTER_t;

enum {
    HAL_API_HOST_CNT_CPDMA_RX_INT, 
    HAL_API_HOST_CNT_CPDMA_TX_INT, 
    HAL_API_HOST_CNT_RXBD_FULL_INT,
    HAL_API_HOST_CNT_QUEUE_FULL_INT, 
    HAL_API_HOST_CNT_PLC_DROP_INT, 
    HAL_API_HOST_CNT_CPTM_RD_ERR_INT, 
    HAL_API_HOST_CNT_FIFO_EPD_DROP_INT,
    HAL_API_HOST_CNT_TX_LONG_PKT_INT, 
    HAL_API_HOST_CNT_LL_FULL_INT,
    HAL_API_HOST_CNT_FIFO_PPD_ERR_INT, 
    HAL_API_HOST_CNT_ENQ_LEN_ERR_INT,
    HAL_API_HOST_CNT_NUM
};

HOST_COUNTER_t g_stHostDmaCounter[HAL_API_HOST_CNT_NUM+1] = 
{
    { HAL_API_HOST_CNT_CPDMA_RX_INT, "CPDMA_RX_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_CPDMA_TX_INT, "CPDMA_TX_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_RXBD_FULL_INT, "RXBD_FULL_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_QUEUE_FULL_INT, "QUEUE_FULL_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_PLC_DROP_INT, "PLC_DROP_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_CPTM_RD_ERR_INT, "CPTM_RD_ERR_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_FIFO_EPD_DROP_INT, "FIFO_EPD_DROP_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_TX_LONG_PKT_INT, "TX_LONG_PKT_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_LL_FULL_INT, "LL_FULL_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_FIFO_PPD_ERR_INT, "FIFO_PPD_ERR_INT", OPL_ZERO },
    { HAL_API_HOST_CNT_ENQ_LEN_ERR_INT, "ENQ_LEN_ERR_INT", OPL_ZERO },
    { -1 , NULL, OPL_ZERO }
};

static void halHostThread(void)
{
	UINT32 intStatus = OPL_ZERO;
    UINT32 index = 0;

    enum { DMA0_RX_MASK = 0x0001 };

#ifndef PKT_BUFF_MODE
    oplRegWrite(REG_DMA0_INT_EN,0x7ff);
    enable_host_dma0_interrupt();
#endif

	while (1)
	{
        wait_for_host_dma0_interrupt();
#ifndef PKT_BUFF_MODE
		oplRegRead(REG_DMA0_INT, &intStatus);
        
        for (index = 0; index < HOST_DMA_COUNTER_NUM; index++)
        {
            if (intStatus&(1<<index))
            {
                g_stHostDmaCounter[index].counter++;
            }
        }
#endif

#ifndef OPL_DMA0_ETH
        if (DMA0_RX_MASK & intStatus)
#endif
		{
			eopl_host_recv();
		}

#ifndef PKT_BUFF_MODE
        enable_host_dma0_interrupt();
#endif
	}

	vosThreadExit(0);
}

void halHostReceive()
{
    eopl_host_recv();
}

void halHostSend(void *pParam)
{
    HAL_API_HOST_SEND_t *pHostParam = (HAL_API_HOST_SEND_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    if (NULL == pHostParam->pvPacket)
    {
        pHostParam->ulResult = OPL_ERR_NULL_POINTER;
        return;
    }

    pHostParam->ulResult = eopl_host_send(pHostParam->pvPacket, pHostParam->usPacketLen);
}


void halHostSendToUplink(void *pParam)
{
    HAL_API_HOST_SEND_TO_UPLINK_t *pHostParam = (HAL_API_HOST_SEND_TO_UPLINK_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    if (NULL == pHostParam->pvPacket)
    {
        pHostParam->ulResult = OPL_ERR_NULL_POINTER;
        return;
    }

    pHostParam->ulResult = eopl_send_to_uplink(
        pHostParam->usPortNum, 
        pHostParam->pvPacket, 
        pHostParam->usPacketLen);
}


void halHostSendToDownlink(void *pParam)
{
    HAL_API_HOST_SEND_TO_DOWNLINK_t *pHostParam = (HAL_API_HOST_SEND_TO_DOWNLINK_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    if (NULL == pHostParam->pvPacket)
    {
        pHostParam->ulResult = OPL_ERR_NULL_POINTER;
        return;
    }

    pHostParam->ulResult = eopl_send_to_down_link(
        pHostParam->usPortNum, 
        pHostParam->usWithTag, 
        (UINT8)pHostParam->stPacketType, 
        pHostParam->pvPacket, 
        pHostParam->usPacketLen);
}

void halHostDumpEnable(void *pParam)
{
    HAL_API_HOST_DUMP_EN_t *pHostParam = (HAL_API_HOST_DUMP_EN_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    eopl_host_dump_enable(pHostParam->ulEnable);
}


void halHostCounterClear(void)
{
    UINT32 index = 0;
    for (index = 0; index < HOST_DMA_COUNTER_NUM; index++)
    {
        g_stHostDmaCounter[index].counter = OPL_ZERO;
    }
}

void halHostCounterShow(void *pParam)
{
    HAL_API_HOST_COUNTER_SHOW_t *pHostParam = (HAL_API_HOST_COUNTER_SHOW_t *)pParam;

    if (NULL == pHostParam)
    {
        return;
    }

    if (0 == pHostParam->ulFd)
    {
        pHostParam->ulFd = OP_STDOUT_FILENO;
    }

	INT32 index = OPL_ZERO;

	vosPrintf(pHostParam->ulFd, "%-32s %-12s\r\n", " Description", " Counter");
	for(index = OPL_ZERO; g_stHostDmaCounter[index].id != -1; index++)
	{
		vosPrintf(pHostParam->ulFd, "-------------------------------- ------------\r\n");
		vosPrintf(pHostParam->ulFd, "%-32s %-12d\r\n",
			g_stHostDmaCounter[index].name,
			g_stHostDmaCounter[index].counter);
    }
}


STATUS halHostThreadInit(void)
{
    int ret;

    if (g_pstHostThreadId) {
        return ERROR;
    }

    g_pstHostThreadId = vosThreadCreate("tHost", OP_VOS_THREAD_STKSZ, 20,
                        (void *)halHostThread, (void *)NULL);
    if (g_pstHostThreadId == NULL) {
        return ERROR;
    }

    return OK;
}

STATUS halHostThreadShutdown(void)
{
    if (!g_pstHostThreadId) {
        return ERROR;
    }

    if (g_pstHostThreadId != NULL) {
        vosThreadDestroy(g_pstHostThreadId);
    }

    return OK;
}



/*******************************************************************************
*
* halHostInit:	 to initialize host hal module
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
*   other   - initialize failed.
*
* SEE ALSO: 
*/
OPL_RESULT halHostInit(void)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    /* register host API */

    stApiUnit.apiId = HAL_API_HOST_RECEIVE;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostReceive;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostSend;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND_TO_UPLINK;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostSendToUplink;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND_TO_DOWNLINK;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostSendToDownlink;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_DUMP_EN;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostDumpEnable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_COUNTER_CLEAR;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostCounterClear;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_COUNTER_SHOW;
    stApiUnit.apiFunc = (OPL_API_FUNC)halHostCounterShow;
    ulRet += halDrvApiRegister(&stApiUnit);

    halHostThreadInit();

    return ulRet;
}

/*******************************************************************************
*
* halHostDestroy:	 
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
OPL_RESULT halHostDestroy()
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet;

    halHostThreadShutdown();

#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    halDma1ThreadShutdown();
#endif

    stApiUnit.apiFunc = NULL;

    /* deregister host API */

    stApiUnit.apiId = HAL_API_HOST_RECEIVE;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND_TO_UPLINK;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_SEND_TO_DOWNLINK;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_DUMP_EN;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_COUNTER_CLEAR;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_HOST_COUNTER_SHOW;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ulRet;
}


