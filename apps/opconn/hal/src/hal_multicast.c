/*
=============================================================================
     File Name: hal_multicast.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 xfan    2009/03/17   Initial Version
----------------   ------------  ----------------------------------------------
*/

#include "opl_driver.h"
#include "opconn_api.h"
#include <vos.h>
#include "errors.h"
#include "hal.h"
#include "dal_lib.h"

#include "hal_multicast.h"

OPL_RESULT halMulticastInit(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastInit();
}

OPL_RESULT halMulticastEnable(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastEnable();
}

OPL_RESULT halMulticastDisable(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastDisable();
}

OPL_RESULT halMulticastArlAddPort(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastArlAddPort(p->port, p->mac);
}

OPL_RESULT halMulticastArlPortDel(void *pInput)
{
    HAL_MC_CFG_INFO_t *p = (HAL_MC_CFG_INFO_t *)pInput;

    if(NULL == p)
    {
        HAL_MC_PRINTF(("input null pointer.\n"));
        return OPL_ERROR;
    }

    return dalMulticastArlPortDel(p->port, p->mac);
}

OPL_RESULT halMcRegister(void)
{
    OPL_API_UNIT_t stApiUnit;
    OPL_RESULT ulRet = 0;

    stApiUnit.apiId = HAL_API_MCAST_INIT ;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastInit;
    ulRet = halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_MCAST_ENABLE;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastEnable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_MCAST_DISABLE;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastDisable;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_MCAST_ARL_ADD;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastArlAddPort;
    ulRet += halDrvApiRegister(&stApiUnit);

    stApiUnit.apiId = HAL_API_MCAST_ARL_DEL;
    stApiUnit.apiFunc = (OPL_API_FUNC)halMulticastArlPortDel;
    ulRet += halDrvApiRegister(&stApiUnit);

    return ulRet;
}


