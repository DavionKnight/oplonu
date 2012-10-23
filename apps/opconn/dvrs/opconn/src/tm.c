/*
=============================================================================
     Header Name: tm.c

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/10/31 	Initial Version	
----------------   ------------  ----------------------------------------------
*/
#include <opl_driver.h>
#include <opl_errno.h>
#include <opl_debug.h>
#include <opl_utils.h>
#include <hw_interface.h>
#include <tm.h>

void tmWredC1ValGet(UINT32 max,UINT32 min,UINT8 *pc1)
{
	UINT8 c1 = OPL_ZERO;
	UINT8 index;

	for(index = OPL_ZERO; index < TM_WRED_C1_MAX_VAL - 1; index++)
	{
		if((2<<6) >= (TM_WRED_MAXP_FACTOR*(max - min)))
		{
			c1 = OPL_ZERO;
			break;
		}

		if((2<<(6 +15)) <= (TM_WRED_MAXP_FACTOR*(max - min)))
		{
			c1 = TM_WRED_C1_MAX_VAL - 1;
			break;
		}
		
		if(((2<<(index + 6)) <=(TM_WRED_MAXP_FACTOR*(max - min)))
			&&((2<<(index + 7)) >=(TM_WRED_MAXP_FACTOR*(max - min))))
		{
			if(((TM_WRED_MAXP_FACTOR*(max - min) )-( 2<<(index + 7))) <= ((2<<(index + 8)) - (TM_WRED_MAXP_FACTOR*(max - min))))
			{
				c1 = index;	
			}else
			{
				c1 = index+1;
			}
			break;
		}
	}
	*pc1 = c1;
}

OPL_STATUS tmUsWredC1ValGet(float p,UINT32 max,UINT32 min,UINT8 *pc1)
{
    UINT8 c1 = 0;
    UINT8 index;
    float maxp = p;
    float maxTh = max;
    float minTh = min;

    if(p >= 1 || NULL == pc1)
    {
        return OPL_ERROR;
    }

    if(max <= min)
    {
        return OPL_ERROR;
    }

    for(index = 0; index < (2^4-1);index++)
    {
        if((maxp*(2^7)*(2^index) - (maxTh-minTh))*(maxp*(2^7)*(2^(index+1)) - (maxTh-minTh)) < 0)
        {
            if((maxp*(2^7)*(2^index) - (maxTh-minTh))*(-1) <= (maxp*(2^7)*(2^(index+1)) - (maxTh-minTh)))
                c1 = index;
            else
                c1 = index+1;
            break;
        }else if((maxp*(2^7)*(2^index) - (maxTh-minTh))*(maxp*(2^7)*(2^(index+1)) - (maxTh-minTh)) == 0)
        {
            if((maxp*(2^7)*(2^index) - (maxTh-minTh))==0)
                c1 = index;
            else
                c1 = index+1;
            break;
        }
    }

    if(index == (2^4-1))
    {
        c1 = 15;
    }

    *pc1 = c1;

    return OPL_OK;
}

OPL_STATUS tmDsWredC1ValGet(float p,UINT32 max,UINT32 min,UINT8 *pc1)
{
    UINT8 c1 = 0;
    UINT8 index;
    float maxp = p;
    float maxTh = max;
    float minTh = min;

    if(p >= 1 || NULL == pc1)
    {
        return OPL_ERROR;
    }

    if(max <= min)
    {
        return OPL_ERROR;
    }

    for(index = 0; index < (2^8-1);index++)
    {
        if((maxp*(2^7)*(2^index) - (maxTh-minTh))*(maxp*(2^7)*(2^(index+1)) - (maxTh-minTh)) < 0)
        {
            if((maxp*(2^7)*(2^index) - (maxTh-minTh))*(-1) <= (maxp*(2^7)*(2^(index+1)) - (maxTh-minTh)))
                c1 = index;
            else
                c1 = index+1;
            break;
        }else if((maxp*(2^7)*(2^index) - (maxTh-minTh))*(maxp*(2^7)*(2^(index+1)) - (maxTh-minTh)) == 0)
        {
            if((maxp*(2^7)*(2^index) - (maxTh-minTh))==0)
                c1 = index;
            else
                c1 = index+1;
            break;
        }
    }

    if(index == (2^8-1))
    {
        c1 = (2^8-1);
    }

    *pc1 = c1;

    return OPL_OK;
}

OPL_STATUS tmInit(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 qId;
	UINT8 qPri;
	if (IS_ONU_RESPIN)
	{
		retVal = oplRegFieldWrite(REG_TM_CTRL, 0,9, 0x103);
	}
	else
	{
		retVal = oplRegFieldWrite(REG_TM_CTRL, 0,3, 0x03);
	}
    if(OPL_OK != retVal)
    {
        printf("set REG_TM_CTRL failed.\n");
        return retVal;
    }

#ifdef ONU_TM_MODE_1
    retVal = oplRegWrite(REG_GB_TM_MODE, 0x1);
#else
    retVal = oplRegWrite(REG_GB_TM_MODE, 0x0);
#endif
    if(OPL_OK != retVal)
    {
        printf("set REG_GB_TM_MODE failed.\n");
        return retVal;
    }
    
	/*set up stream qid pri*/
	for(qId = 0; qId <=7 ; qId++)
	{	
		qPri = qId;
		retVal = tmQueuePriHwWrite(UP_STREAM,qId,qPri);	
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}

	for(qId = 0; qId <=7 ; qId++)
	{	
		qPri = qId;
		retVal = tmQueuePriHwWrite(DOWN_STREAM,qId,qPri);	
		if(OPL_OK != retVal)
		{
			return retVal;
		}
	}

	for(qId = 0; qId <= 7; qId++)
	{
		tmQueueMaxCellNumHwWrite(UP_STREAM, qId, 0XFFFF);
		tmQueueMaxPktNumHwWrite(UP_STREAM, qId, 8000);
		tmQueueMaxCellNumHwWrite(DOWN_STREAM, qId, 0XFFFF);
		tmQueueMaxPktNumHwWrite(DOWN_STREAM, qId, 0x1FFF);
	}

	retVal = oplRegWrite(REG_TMDS_SHAPER_OFFSET, 4);
    if(OPL_OK != retVal)
    {
        printf("set REG_TMDS_SHAPER_OFFSET failed.\n");
    }

	return retVal;
}

OPL_STATUS tmGeShaperEnableHwWrite(UINT8 stream,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	regVal = enable?OPL_ENABLE:OPL_DISABLE;

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_GE_PARSER_SHAPER,31,1, regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite(REG_TMDS_SHAPER_EN,8,1, regVal);
			break;
		default:
			OPL_DRV_PRINTF(("no such stream.\n"));
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmGeShaperEnableHwRead(UINT8 stream,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(enable == OPL_NULL)
	{
		OPL_DRV_PRINTF(("in put null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_GE_PARSER_SHAPER,31,1, &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead(REG_TMDS_SHAPER_EN,8,1, &regVal);
			break;
		default:
			OPL_DRV_PRINTF(("no such stream.\n"));
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}

	if(OPL_OK == retVal)
	{
		*enable = regVal;
	}
	return retVal;
}
OPL_STATUS tmQueueShaperEnableHwWrite(UINT8 stream,UINT8 Qid,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	fieldOffset = Qid;
	fieldWidth = 1;
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_TMUS_SHAPER_EN,fieldOffset,fieldWidth, regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite(REG_TMDS_SHAPER_EN,fieldOffset,fieldWidth, regVal);
			break;
		default:
			break;
	}
	return retVal;
}

OPL_STATUS tmQueueShaperCirEnableHwWrite(UINT8 stream,UINT8 Qid,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	fieldOffset = Qid;
	fieldWidth = 1;
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_TMUS_SHAPER_EN,fieldOffset,fieldWidth, regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite(REG_TMDS_SHAPER_EN,fieldOffset,fieldWidth, regVal);
			break;
		default:
			break;
	}
	return retVal;
}

OPL_STATUS tmQueueShaperPirEnableHwWrite(UINT8 stream,UINT8 Qid,UINT8 enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	regVal = enable?OPL_ENABLE:OPL_DISABLE;
	fieldOffset = Qid;
	fieldWidth = 1;
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_TMUS_SHAPER_EN,fieldOffset+8,fieldWidth, regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite(REG_TMDS_SHAPER_EN,fieldOffset,fieldWidth, regVal);
			break;
		default:
			break;
	}
	return retVal;
}
OPL_STATUS tmQueueShaperCirEnableHwRead(UINT8 stream,UINT8 Qid,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	
	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	fieldOffset = Qid;
	fieldWidth = 1;
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_TMUS_SHAPER_EN,fieldOffset,fieldWidth, &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead(REG_TMDS_SHAPER_EN,fieldOffset,fieldWidth, &regVal);
			break;
		default:
			break;
	}
	*enable = regVal?OPL_ENABLE:OPL_DISABLE;
	return retVal;
}

OPL_STATUS tmQueueShaperPirEnableHwRead(UINT8 stream,UINT8 Qid,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	
	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	fieldOffset = Qid;
	fieldWidth = 1;
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_TMUS_SHAPER_EN,fieldOffset+8,fieldWidth, &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead(REG_TMDS_SHAPER_EN,fieldOffset,fieldWidth, &regVal);
			break;
		default:
			break;
	}
	*enable = regVal?OPL_ENABLE:OPL_DISABLE;
	return retVal;
}

OPL_STATUS tmQueueShaperEnableHwRead(UINT8 stream,UINT8 Qid,UINT8 *enable)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	
	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	if(OPL_NULL == enable)
	{
		return OPL_ERR_NULL_POINTER;
	}

	fieldOffset = Qid;
	fieldWidth = 1;
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_TMUS_SHAPER_EN,fieldOffset,fieldWidth, &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead(REG_TMDS_SHAPER_EN,fieldOffset,fieldWidth, &regVal);
			break;
		default:
			break;
	}
	*enable = regVal?OPL_ENABLE:OPL_DISABLE;
	return retVal;
}

OPL_STATUS tmQueuePriHwWrite(UINT8 stream,UINT8 Qid,UINT8 priority)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	UINT16 fieldOffset;
	UINT16 fieldWidth;

	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	fieldOffset = 0;
	fieldWidth = 3;
	regVal = priority&0x07;

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_TMUS_PRIORITY0 + Qid*4, fieldOffset, fieldWidth, regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite(REG_TMDS_PRIORITY0 + Qid*4, fieldOffset, fieldWidth, regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmQueuePriHwRead(UINT8 stream,UINT8 Qid,UINT8 *priority)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	UINT16 fieldOffset;
	UINT16 fieldWidth;

	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_NULL == priority)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	fieldOffset = 0;
	fieldWidth = 3;
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_TMUS_PRIORITY0 + Qid*4, fieldOffset, fieldWidth, &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead(REG_TMDS_PRIORITY0 + Qid*4, fieldOffset, fieldWidth, &regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	*priority = regVal;
	return retVal;
}

OPL_STATUS tmQueueWeightHwWrite(UINT8 stream,UINT8 Qid,UINT32 weight)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 regVal;

	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	fieldOffset = 0;
	fieldWidth = 8;
	regVal = weight;

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_TMUS_WEIGHT0 + Qid*4, fieldOffset, fieldWidth, regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite(REG_TMDS_WEIGHT0 + Qid*4, fieldOffset, fieldWidth, regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmQueueWeightHwRead(UINT8 stream,UINT8 Qid,UINT32 *weight)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 regVal;

	if(!((stream == UP_STREAM && Qid < TM_UP_STREAM_NUM) ||
		(stream == DOWN_STREAM && Qid < TM_DOWN_STREAM_NUM)))
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(weight == OPL_NULL)
	{
		return OPL_ERR_NULL_POINTER;
	}

	fieldOffset = 0;
	fieldWidth = 8;

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_TMUS_WEIGHT0 + Qid*4, fieldOffset, fieldWidth, &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead(REG_TMDS_WEIGHT0 + Qid*4, fieldOffset, fieldWidth, &regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	*weight = regVal;
	return retVal;
}

OPL_STATUS tmGeCirHwWrite(UINT8 stream,UINT32 geCir)
{
	OPL_STATUS retVal = OPL_OK;
	TM_DS_Q_SHAPER_PA_t dsQshaperPa;
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_GE_PARSER_SHAPER,  10,  15, geCir);
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, 8, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			
			dsQshaperPa.bfCir = geCir;
			
			retVal = oplTabWrite(OPCONN_TAB_DS_SHPBD, 8, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}	
			break;
		default:
			OPL_DRV_PRINTF(("no such stream.\n"));
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmGeCirHwRead(UINT8 stream,UINT32 *geCir)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	TM_DS_Q_SHAPER_PA_t dsQshaperPa;
		
	if(OPL_NULL == geCir)
	{
		return OPL_ERR_NULL_POINTER;
	}

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_GE_PARSER_SHAPER,  10,  15, &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, 8, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			regVal = dsQshaperPa.bfCir;
			break;
		default:
			OPL_DRV_PRINTF(("no such stream.\n"));
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	
	if(OPL_OK == retVal)
	{
		*geCir = regVal;
	}
	
	return retVal;
}

OPL_STATUS tmGeCbsHwWrite(UINT8 stream,UINT32 geCbs)
{
	OPL_STATUS retVal = OPL_OK;
	TM_DS_Q_SHAPER_PA_t dsQshaperPa;

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_GE_PARSER_SHAPER, 0, 10, geCbs);
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, 8, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			
			dsQshaperPa.bfCbs = geCbs;
			
			retVal = oplTabWrite(OPCONN_TAB_DS_SHPBD, 8, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			break;
		default:
			OPL_DRV_PRINTF(("no such stream.\n"));
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmGeCbsHwRead(UINT8 stream,UINT32 *geCbs)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;
	TM_DS_Q_SHAPER_PA_t dsQshaperPa;

	if(OPL_NULL == geCbs)
	{
		return OPL_ERR_NULL_POINTER;
	}

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_GE_PARSER_SHAPER,  0,  10, &regVal);
			if(OPL_OK == retVal)
			{
				*geCbs = regVal;
			}
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, 8, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			
			*geCbs = dsQshaperPa.bfCbs;
			break;
		default:
			OPL_DRV_PRINTF(("no such stream.\n"));
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmQueueIcosMapHwWrite(UINT8 stream,UINT8 cosVal,UINT8 icosVal)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(stream > DOWN_STREAM || cosVal >= TM_UP_STREAM_NUM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	regVal = icosVal;

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_TMUS_QID_ICOS0 + 4*cosVal,  0,  3,  regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite(REG_TMDS_QID_ICOS0 + 4*cosVal,  0,  3,  regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;	
	}
	return retVal;
}
OPL_STATUS tmQueueIcosMapHwRead(UINT8 stream,UINT8 cosVal,UINT8 *icosVal)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(stream > DOWN_STREAM || cosVal >= TM_UP_STREAM_NUM)
	{
		OPL_LOG_TRACE();
		OPL_DRV_PRINTF(("stream = %d cosval = %d\n",stream,cosVal));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_TMUS_QID_ICOS0 + 4*cosVal,  0,  3,  &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead(REG_TMDS_QID_ICOS0 + 4*cosVal,  0,  3,  &regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;	
	}
	*icosVal = regVal;
	return retVal;
}
OPL_STATUS tmCosIcosMapHwWrite(UINT8 stream,UINT8 cos,UINT8 icos)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(stream > DOWN_STREAM || cos >= TM_UP_STREAM_NUM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	regVal = icos;

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite(REG_BRG_I_COS_UP_MAP,  cos*3,  3,  regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite(REG_BRG_I_COS_DN_MAP,  cos*3,  3,  regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;	
	}
	return retVal;
}
OPL_STATUS tmCosIcosMapHwRead(UINT8 stream,UINT8 cos,UINT8 *icos)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 regVal;

	if(stream > DOWN_STREAM || cos >= TM_UP_STREAM_NUM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead(REG_BRG_I_COS_UP_MAP,  cos*3,  3, & regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead(REG_BRG_I_COS_DN_MAP,  cos*3,  3,  &regVal);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			break;	
	}
	*icos = regVal;
	return retVal;
}

/**************************************************************************************************
 *		The clk number should be added in every period, for cir and cbs of tmds shaper.
 *		e.g.  clk = 65MHz, the minimum of cir is 32kbps (4k byte/s), the max of cir is 1024Mbps, the inc steplength is 32 byte,
 *		then 4k/32 = 125 periods/s
 *		65M/125 = 520000 clks/period
 *		In every period, should be divided into 1024M/32k = 32768 parts
 *		In every part, the clk number should be: 520000/32768 = 15
 *		Then TMDS_CLKNUM_PERIOD = 520000 - 15*32768 = 8480 (0x2120)
 *
 *************************************************************************************************/
OPL_STATUS tmQueuePeriodsOfClkNumHwWrite(UINT8 stream,UINT32 clkNum)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 regVal;

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	fieldOffset = 0;
	fieldWidth = 16;	
	regVal = clkNum;

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite( REG_TMUS_CLKNUM_PERIOD,  fieldOffset,  fieldWidth,  regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite( REG_TMDS_CLKNUM_PERIOD,  fieldOffset,  fieldWidth,  regVal);
			break;
		default:
			retVal  = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

/**************************************************************************************************
 *		The clk number should be added in every period, for cir and cbs of tmds shaper.
 *		e.g.  clk = 65MHz, the minimum of cir is 32kbps (4k byte/s), the max of cir is 1024Mbps, the inc steplength is 32 byte,
 *		then 4k/32 = 125 periods/s
 *		65M/125 = 520000 clks/period
 *		In every period, should be divided into 1024M/32k = 32768 parts
 *		In every part, the clk number should be: 520000/32768 = 15
 *		Then TMDS_CLKNUM_PERIOD = 520000 - 15*32768 = 8480 (0x2120)
 *
 *************************************************************************************************/
OPL_STATUS tmQueuePeriodsOfClkNumHwRead(UINT8 stream,UINT32 *clkNum)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 regVal;

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_NULL == clkNum)
	{
		return OPL_ERR_NULL_POINTER;
	}
	
	fieldOffset = 0;
	fieldWidth = 16;	
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead( REG_TMUS_CLKNUM_PERIOD,  fieldOffset,  fieldWidth,  &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead( REG_TMDS_CLKNUM_PERIOD,  fieldOffset,  fieldWidth,  &regVal);
			break;
		default:
			retVal  = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	*clkNum = regVal;
	return retVal;
}


/**********************************************************************************************
 *		The clk number in every part, for cir and cbs of tmus shaper.
 *		e.g.  clk = 65MHz, the minimum of cir is 32kbps (4k byte/s), the max of cir is 1024Mbps, the inc steplength is 32 byte,
 *		then 4k/32 = 125 periods/s
 *		65M/125 = 520000 clks/period
 *		In every period, should be divided into 1024M/32k = 32768 parts
 *		In every part, the clk number should be: 520000/32768 = 15
 *		hen TMUS_STEP_CNT = 15-1
 *
 **********************************************************************************************/
OPL_STATUS tmQueueClkCountPerStepHwWrite(UINT8 stream,UINT32 clkCntPerStep)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 regVal;

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	fieldOffset = 0;
	fieldWidth = 8;	
	regVal = clkCntPerStep;
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldWrite( REG_TMUS_STEP_CNT,  fieldOffset,  fieldWidth,  regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldWrite( REG_TMDS_STEP_CNT,  fieldOffset,  fieldWidth,  regVal);
			break;
		default:
			retVal  = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
/**********************************************************************************************
 *		The clk number in every part, for cir and cbs of tmus shaper.
 *		e.g.  clk = 65MHz, the minimum of cir is 32kbps (4k byte/s), the max of cir is 1024Mbps, the inc steplength is 32 byte,
 *		then 4k/32 = 125 periods/s
 *		65M/125 = 520000 clks/period
 *		In every period, should be divided into 1024M/32k = 32768 parts
 *		In every part, the clk number should be: 520000/32768 = 15
 *		hen TMUS_STEP_CNT = 15-1
 *
 **********************************************************************************************/
OPL_STATUS tmQueueClkCountPerStepHwRead(UINT8 stream,UINT32 *clkCntPerStep)
{
	OPL_STATUS retVal = OPL_OK;
	UINT16 fieldOffset;
	UINT16 fieldWidth;
	UINT32 regVal;

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_NULL == clkCntPerStep)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	fieldOffset = 0;
	fieldWidth = 8;	
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplRegFieldRead( REG_TMUS_STEP_CNT,  fieldOffset,  fieldWidth, &regVal);
			break;
		case DOWN_STREAM:
			retVal = oplRegFieldRead( REG_TMDS_STEP_CNT,  fieldOffset,  fieldWidth, &regVal);
			break;
		default:
			retVal  = OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	*clkCntPerStep = regVal;
	return retVal;
}


OPL_STATUS tmQueueMaxCellNumHwWrite(UINT8 stream,UINT8 Qid,UINT16 maxCellNum)
{
	TM_US_Q_CONF_STA_t  upStreamQConfSta;
	TM_DS_Q_CONF_STA_t  dnStreamQConfSta;
	OPL_STATUS          retVal = OPL_OK;
	UINT8               tabIndex = OPL_ZERO;
    UINT32              tmMode = OPL_ZERO; 

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(UP_STREAM == stream && Qid < TM_UP_STREAM_NUM)
	{
		tabIndex = Qid;
	}else if(DOWN_STREAM == stream && Qid < TM_DOWN_STREAM_NUM)
	{
		tabIndex = Qid + TM_DOWN_STREAM_NUM;
	}else 
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

    /*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);
	
	switch(stream)
	{
		case UP_STREAM:
            if(tmMode)
            {
                retVal = oplRegFieldWrite(REG_TMUS_Q0_MAX_CELL_NUM+Qid*4,0,14,maxCellNum);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
            }else
            {
                retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
    			upStreamQConfSta.bfMaxNumOfCell = maxCellNum;
    			retVal = oplTabWrite(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}    
            }
			
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			dnStreamQConfSta.bfMaxNumOfCell = maxCellNum;
			retVal = oplTabWrite(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			break;
		default:
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}
OPL_STATUS tmQueueMaxCellNumHwRead(UINT8 stream,UINT8 Qid,UINT16 *maxCellNum)
{
	TM_US_Q_CONF_STA_t  upStreamQConfSta;
	TM_DS_Q_CONF_STA_t  dnStreamQConfSta;
	OPL_STATUS          retVal = OPL_OK;
	UINT8               tabIndex = OPL_ZERO;
    UINT32              tmMode = OPL_ZERO; 
    UINT32              data32 = 0;

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_NULL == maxCellNum)
	{
		return OPL_ERR_NULL_POINTER;
	}

	if(UP_STREAM == stream && Qid < TM_UP_STREAM_NUM)
	{
		tabIndex = Qid;
	}else if(DOWN_STREAM == stream && Qid < TM_DOWN_STREAM_NUM)
	{
		tabIndex = Qid + TM_DOWN_STREAM_NUM;
	}else 
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

     /*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);
     
	switch(stream)
	{
		case UP_STREAM:
            if(tmMode)
            {
                retVal = oplRegFieldRead(REG_TMUS_Q0_MAX_CELL_NUM+Qid*4,0,14,&data32);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                *maxCellNum = data32;
            }else
            {
                retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
    			*maxCellNum = upStreamQConfSta.bfMaxNumOfCell;	
            }
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			*maxCellNum = dnStreamQConfSta.bfMaxNumOfCell;
			break;
		default:
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmQueueMaxPktNumHwWrite(UINT8 stream,UINT8 Qid,UINT16 maxPktNum)
{
	TM_US_Q_CONF_STA_t  upStreamQConfSta;
	TM_DS_Q_CONF_STA_t  dnStreamQConfSta;
	OPL_STATUS          retVal = OPL_OK;
	UINT8               tabIndex = OPL_ZERO;
    UINT32              tmMode = OPL_ZERO; 

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(UP_STREAM == stream && Qid < TM_UP_STREAM_NUM)
	{
		tabIndex = Qid;
		if(TM_US_Q_MAX_PKT_NUM < maxPktNum)
		{
			OPL_DRV_PRINTF(("up stream max pkt num should be less than %d.\n",TM_US_Q_MAX_PKT_NUM));
			return OPL_ERR_INVALID_PARAMETERS;
		}
	}else if(DOWN_STREAM == stream && Qid < TM_DOWN_STREAM_NUM)
	{
		tabIndex = Qid + TM_DOWN_STREAM_NUM;
	}else 
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	  /*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);
      
	switch(stream)
	{
		case UP_STREAM:
            if(tmMode)
            {
                retVal = oplRegFieldWrite(REG_TMUS_Q0_MAX_PKT_NUM+Qid*4,0,13,maxPktNum);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
            }else
            {
                retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
    			upStreamQConfSta.bfMaxNumOfPkt = maxPktNum;
    			retVal = oplTabWrite(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}    
            }
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			dnStreamQConfSta.bfMaxNumOfPkt = maxPktNum;
			retVal = oplTabWrite(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			break;
		default:
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmQueueMaxPktNumHwRead(UINT8 stream,UINT8 Qid,UINT16 *maxPktNum)
{
	TM_US_Q_CONF_STA_t  upStreamQConfSta;
	TM_DS_Q_CONF_STA_t  dnStreamQConfSta;
	OPL_STATUS          retVal = OPL_OK;
	UINT8               tabIndex = OPL_ZERO;
    UINT32              tmMode = OPL_ZERO; 
    UINT32              data32 = 0;

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_NULL == maxPktNum)
	{
		return OPL_ERR_NULL_POINTER;
	}

	if(UP_STREAM == stream && Qid < TM_UP_STREAM_NUM)
	{
		tabIndex = Qid;
	}else if(DOWN_STREAM == stream && Qid < TM_DOWN_STREAM_NUM)
	{
		tabIndex = Qid + TM_DOWN_STREAM_NUM;
	}else 
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	/*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);

	switch(stream)
	{
		case UP_STREAM:
            if(tmMode)
            {
                retVal = oplRegFieldRead(REG_TMUS_Q0_MAX_PKT_NUM+Qid*4,0,14,&data32);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                *maxPktNum = data32;
            }else
            {
                retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
    			*maxPktNum = upStreamQConfSta.bfMaxNumOfPkt;    
            }
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			*maxPktNum = dnStreamQConfSta.bfMaxNumOfPkt;
			break;
		default:
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmPktNumInQueueHwRead(UINT8 stream,UINT8  Qid,UINT32 *numOfPkt)
{
	TM_US_Q_CONF_STA_t  upStreamQConfSta;
	TM_DS_Q_CONF_STA_t  dnStreamQConfSta;
	OPL_STATUS          retVal = OPL_OK;
	UINT8               tabIndex = OPL_ZERO;
    UINT32              tmMode = OPL_ZERO; 
    UINT32              data32 = 0;

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_NULL == numOfPkt)
	{
		return OPL_ERR_NULL_POINTER;
	}

	if(UP_STREAM == stream && Qid < TM_UP_STREAM_NUM)
	{
		tabIndex = Qid;
	}else if(DOWN_STREAM == stream && Qid < TM_DOWN_STREAM_NUM)
	{
		tabIndex = Qid + TM_DOWN_STREAM_NUM;
	}else 
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	/*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);
    
	switch(stream)
	{
		case UP_STREAM:
            if(tmMode)
            {
                retVal = oplRegFieldRead(REG_TMUS_PKT_INQ0_CNT+Qid*4,0,13,&data32);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                *numOfPkt = data32;
            }else
            {
                retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
    			*numOfPkt = upStreamQConfSta.bfNumOfPkt;    
            }
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			*numOfPkt = dnStreamQConfSta.bfNumOfPkt;
			break;
		default:
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmCellNumInQueueHwRead(UINT8 stream,UINT8  Qid,UINT32 *numOfCell)
{
	TM_US_Q_CONF_STA_t  upStreamQConfSta;
	TM_DS_Q_CONF_STA_t  dnStreamQConfSta;
	OPL_STATUS          retVal = OPL_OK;
	UINT8               tabIndex = OPL_ZERO;
    UINT32              tmMode = OPL_ZERO; 
    UINT32              data32 = 0;

	if(stream > DOWN_STREAM)
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(OPL_NULL == numOfCell)
	{
		return OPL_ERR_NULL_POINTER;
	}

	if(UP_STREAM == stream && Qid < TM_UP_STREAM_NUM)
	{
		tabIndex = Qid;
	}else if(DOWN_STREAM == stream && Qid < TM_DOWN_STREAM_NUM)
	{
		tabIndex = Qid + TM_DOWN_STREAM_NUM;
	}else 
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}

	/*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);
    
	switch(stream)
	{
		case UP_STREAM:
            if(tmMode)
            {
                retVal = oplRegFieldRead(REG_TMUS_CELL_INQ0_CNT+Qid*4,0,14,&data32);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                *numOfCell = data32;
            }else
            {
                retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
    			*numOfCell = upStreamQConfSta.bfNumOfCell;            
            }

			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, tabIndex, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			*numOfCell = dnStreamQConfSta.bfNumOfCell;
			break;
		default:
			return OPL_ERR_INVALID_PARAMETERS;
			break;
	}
	return retVal;
}

OPL_STATUS tmWredEntryHwWrite(UINT8 stream,UINT8 queueId,UINT32 *pEntry)
{
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == pEntry)
	{
		return OPL_ERR_NULL_POINTER;
	}

	if(queueId >= TM_DOWN_STREAM_NUM)
	{
		OPL_DRV_PRINTF(("can only support %d queue.\n",TM_DOWN_STREAM_NUM));
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplTabWrite(OPCONN_TAB_US_WRED,queueId, 1, pEntry);
			break;
		case DOWN_STREAM:
			retVal = oplTabWrite(OPCONN_TAB_DS_WRED,queueId, 1, pEntry);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			OPL_DRV_PRINTF(("no such  stream %d\n",stream));
			break;
	}

	return retVal;
}

OPL_STATUS tmWredEntryHwRead(UINT8 stream,UINT8 queueId,UINT32 *pEntry)
{
	OPL_STATUS retVal = OPL_OK;

	if(OPL_NULL == pEntry)
	{
		return OPL_ERR_NULL_POINTER;
	}

	if(queueId >= TM_DOWN_STREAM_NUM)
	{
		OPL_DRV_PRINTF(("can only support %d queue.\n",TM_DOWN_STREAM_NUM));
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplTabRead(OPCONN_TAB_US_WRED,queueId, 1, pEntry);
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_DS_WRED,queueId, 1, pEntry);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			OPL_DRV_PRINTF(("no such  stream %d\n",stream));
			break;
	}

	return retVal;
}

OPL_STATUS tmQueueCbsHwWrite(UINT8 stream,UINT8 Qid,UINT32 cbs)
{
	TM_DS_Q_SHAPER_PA_t dsQshaperPa;
	TM_US_Q_SHAPER_PA_t usQshaperPa;
	OPL_STATUS retVal = OPL_OK;

	if(stream == UP_STREAM && Qid > TM_UP_STREAM_NUM )
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}else if(stream == DOWN_STREAM && Qid > TM_UP_STREAM_NUM )
	{
	return OPL_ERR_INVALID_PARAMETERS;
	}

	if(cbs > 0x3ff)
	{
		OPL_DRV_PRINTF((" cbs should be less than 0x%d\n",0x3ff));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplTabRead(OPCONN_TAB_US_SHPBD, Qid, 1, (UINT32 *)&usQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			usQshaperPa.bfCbs = cbs;
			retVal = oplTabWrite(OPCONN_TAB_US_SHPBD, Qid, 1, (UINT32 *)&usQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, Qid, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			dsQshaperPa.bfCbs = cbs;
			retVal = oplTabWrite(OPCONN_TAB_DS_SHPBD, Qid, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			break;
		default:
			OPL_LOG_TRACE();
			break;
	}
	return retVal;
}

OPL_STATUS tmQueueCbsHwRead(UINT8 stream,UINT8 Qid,UINT32 *cbs)
{
	TM_DS_Q_SHAPER_PA_t dsQshaperPa;
	TM_US_Q_SHAPER_PA_t usQshaperPa;
	OPL_STATUS retVal = OPL_OK;

	if(cbs == OPL_NULL)
	{
		OPL_DRV_PRINTF(("in put null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(Qid > TM_UP_STREAM_NUM )
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplTabRead(OPCONN_TAB_US_SHPBD, Qid, 1, (UINT32 *)&usQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}

			*cbs = usQshaperPa.bfCbs;
			
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, Qid, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			*cbs = dsQshaperPa.bfCbs;
			break;
		default:
			OPL_LOG_TRACE();
			break;
	}
	return retVal;
}

OPL_STATUS tmQueueCirHwWrite(UINT8 stream,UINT8 Qid,UINT32 cir)
{
	TM_DS_Q_SHAPER_PA_t dsQshaperPa;
	TM_US_Q_SHAPER_PA_t usQshaperPa;
	OPL_STATUS retVal = OPL_OK;

	if(stream == UP_STREAM && Qid > TM_UP_STREAM_NUM )
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}else if(stream == DOWN_STREAM && Qid > TM_UP_STREAM_NUM )
	{
	return OPL_ERR_INVALID_PARAMETERS;
	}

	if(cir > 0x7fff)
	{
		OPL_DRV_PRINTF((" cbs should be less than 0x%d\n",0x7fff));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	switch(stream)
	{
		case UP_STREAM:
			retVal = oplTabRead(OPCONN_TAB_US_SHPBD, Qid, 1, (UINT32 *)&usQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			usQshaperPa.bfCir = cir;
			retVal = oplTabWrite(OPCONN_TAB_US_SHPBD, Qid, 1, (UINT32 *)&usQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, Qid, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			dsQshaperPa.bfCir = cir;
			retVal = oplTabWrite(OPCONN_TAB_DS_SHPBD, Qid, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			break;
		default:
			OPL_LOG_TRACE();
			break;
	}
	return retVal;
}

OPL_STATUS tmQueueCirHwRead(UINT8 stream,UINT8 Qid,UINT32 *cir)
{
	TM_DS_Q_SHAPER_PA_t dsQshaperPa;
	TM_US_Q_SHAPER_PA_t usQshaperPa;
	OPL_STATUS retVal = OPL_OK;

	if(cir == OPL_NULL)
	{
		OPL_DRV_PRINTF(("in put null pointer.\n"));
		return OPL_ERR_NULL_POINTER;
	}

	if(Qid > TM_UP_STREAM_NUM )
	{
		return OPL_ERR_INVALID_PARAMETERS;
	}
	
	switch(stream)
	{
		case UP_STREAM:
			retVal = oplTabRead(OPCONN_TAB_US_SHPBD, Qid, 1, (UINT32 *)&usQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}

			*cir = usQshaperPa.bfCir;
			
			break;
		case DOWN_STREAM:
			retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, Qid, 1, (UINT32 *)&dsQshaperPa);
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
			*cir = dsQshaperPa.bfCir;
			break;
		default:
			OPL_LOG_TRACE();
			break;
	}
	return retVal;
}

OPL_STATUS tmWredCfgSet(UINT8 stream,UINT8 queueId, UINT8 type,UINT32 value)
{
	OPL_STATUS retVal = OPL_OK;
	TM_DS_WRED_PA_t tmDsWredPa;
	TM_US_WRED_PA_t tmUsWredPa;
	UINT8 c1;

	if(queueId >= TM_DOWN_STREAM_NUM)
	{
		OPL_DRV_PRINTF(("can only support %d queue.\n",TM_DOWN_STREAM_NUM));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(stream == UP_STREAM)
	{
		retVal = tmWredEntryHwRead(stream,queueId,(UINT32 *)&tmUsWredPa);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
		switch(type)
		{
			case US_WRED_MINTH:
				tmUsWredPa.bfMinth = value;
				tmWredC1ValGet(tmUsWredPa.bfMaxthH<<12|tmUsWredPa.bfMaxthL,tmUsWredPa.bfMinth, &c1);
				tmUsWredPa.bfC1 = c1;
				retVal = tmWredEntryHwWrite(stream,queueId,(UINT32 *)&tmUsWredPa);
				break;
			case US_WRED_MAXTH:
				tmUsWredPa.bfMaxthL = value&0XFFF;
				tmUsWredPa.bfMaxthH= value>>12;
				tmWredC1ValGet(value,tmUsWredPa.bfMinth, &c1);
				tmUsWredPa.bfC1 = c1;
				retVal = tmWredEntryHwWrite(stream,queueId,(UINT32 *)&tmUsWredPa);
				break;
			case US_WRED_WEIGHT:
				tmUsWredPa.bfWeight= value;
				retVal = tmWredEntryHwWrite(stream,queueId,(UINT32 *)&tmUsWredPa);
				break;
			default:
				break;
		}
	}else if(stream == DOWN_STREAM)
	{
		retVal = tmWredEntryHwRead(stream,queueId,(UINT32 *)&tmDsWredPa);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
		switch(type)
		{
			case DS_WRED_MINTH:
				tmDsWredPa.bfMinth = value;
				tmWredC1ValGet(tmDsWredPa.bfMaxthH<<14|tmUsWredPa.bfMaxthL, tmDsWredPa.bfMinth, &c1);
				tmDsWredPa.bfC1 = c1;
				retVal = tmWredEntryHwWrite(stream,queueId,(UINT32 *)&tmDsWredPa);
				break;
			case DS_WRED_MAXTH:
				tmDsWredPa.bfMaxthL = value&0X3FFF;
				tmDsWredPa.bfMaxthH= value>>14;
				tmWredC1ValGet(tmDsWredPa.bfMaxthH<<14|tmUsWredPa.bfMaxthL, tmDsWredPa.bfMinth, &c1);
				tmDsWredPa.bfC1 = c1;
				retVal = tmWredEntryHwWrite(stream,queueId,(UINT32 *)&tmDsWredPa);
				break;
			case DS_WRED_WEIGHT:
				tmDsWredPa.bfWeight= value;
				retVal = tmWredEntryHwWrite(stream,queueId,(UINT32 *)&tmDsWredPa);
				break;
			default:
				OPL_DRV_PRINTF(("no such type in wred.\n"));
				retVal = OPL_ERR_INVALID_PARAMETERS;
				break;
		}
	}else 
	{
		OPL_DRV_PRINTF(("no such stream.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	return retVal;
}

OPL_STATUS tmWredCfgGet(UINT8 stream,UINT8 queueId,UINT8 type,UINT32 *value)
{
	OPL_STATUS retVal = OPL_OK;
	TM_DS_WRED_PA_t tmDsWredPa;
	TM_US_WRED_PA_t tmUsWredPa;

	if(OPL_NULL == value)
	{
		return OPL_ERR_NULL_POINTER;
	}

	if(queueId >= TM_DOWN_STREAM_NUM)
	{
		OPL_DRV_PRINTF(("can only support %d queue.\n",TM_DOWN_STREAM_NUM));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	if(stream == UP_STREAM)
	{
		retVal = tmWredEntryHwRead(stream,queueId,(UINT32 *)&tmUsWredPa);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
		switch(type)
		{
			case US_WRED_MINTH:
				*value = tmUsWredPa.bfMinth;
				break;
			case US_WRED_MAXTH:
				*value = (tmUsWredPa.bfMaxthL&0xFFF)|((tmUsWredPa.bfMaxthH&(0XFF))<<12);
				break;
			case US_WRED_WEIGHT:
				*value = tmUsWredPa.bfWeight;
				break;
			case US_WRED_C1:
				*value = tmUsWredPa.bfC1;
				break;
			default:
				break;
		}
	}else if(stream == DOWN_STREAM)
	{
		retVal = tmWredEntryHwRead(stream,queueId,(UINT32 *)&tmDsWredPa);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
		switch(type)
		{
			case DS_WRED_MINTH:
				*value = tmDsWredPa.bfMinth;
				break;
			case DS_WRED_MAXTH:
				*value = (tmDsWredPa.bfMaxthL&0x3FFF)|((tmDsWredPa.bfMaxthH&(0XF))<<12);
				break;
			case DS_WRED_WEIGHT:
				*value = tmDsWredPa.bfWeight;
				break;
			case DS_WRED_C1:
				*value = tmDsWredPa.bfC1 ;
				break;
			default:
				OPL_DRV_PRINTF(("no such type in wred.\n"));
				retVal = OPL_ERR_INVALID_PARAMETERS;
				break;
		}
	}else 
	{
		OPL_DRV_PRINTF(("no such stream.\n"));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	return retVal;
}
OPL_STATUS tmWredEnableHwWrite(UINT8 stream,UINT8 queueId,UINT32 enable)
{
    switch(stream)
    {
        case UP_STREAM:
            if(queueId < TM_UP_STREAM_NUM)
             {
                return oplRegFieldWrite(REG_TMUS_WRED_CTRL, queueId, 1, enable);
             }
            break;
        case DOWN_STREAM:
            if(queueId < TM_DOWN_STREAM_NUM)
             {
                return oplRegFieldWrite(REG_TMDS_WRED_CTRL, queueId, 1, enable);   
             }
            break;
        default:
            break;
    }
    return OPL_ERROR;
}

OPL_STATUS tmWredEnableHwRead(UINT8 stream,UINT8 queueId,UINT32 *enable)
{
    if(NULL == enable)
    {
        return OPL_ERR_NULL_POINTER;
    }
    switch(stream)
    {
        case UP_STREAM:
            if(queueId < TM_UP_STREAM_NUM)
             {
                return oplRegFieldRead(REG_TMUS_WRED_CTRL, queueId, 1, enable);
             }
            break;
        case DOWN_STREAM:
            if(queueId < TM_DOWN_STREAM_NUM)
             {
                return oplRegFieldRead(REG_TMDS_WRED_CTRL, queueId, 1, enable);   
             }
            break;
        default:
            break;
    }
    return OPL_ERROR;
}
OPL_STATUS tmWredConfigAll(UINT8 stream,UINT8 queueId,UINT8 weight,UINT32 maxTh,UINT32 minTh)
{
	OPL_STATUS retVal = OPL_OK;
	TM_DS_WRED_PA_t tmDsWredPa;
	TM_US_WRED_PA_t tmUsWredPa;
	UINT8 c1;
	
	if(queueId >= TM_DOWN_STREAM_NUM)
	{
		OPL_DRV_PRINTF(("can only support %d queue.\n",TM_DOWN_STREAM_NUM));
		return OPL_ERR_INVALID_PARAMETERS;
	}

	tmWredC1ValGet(maxTh, minTh, &c1);
	
	switch(stream)
	{
		case UP_STREAM:
			tmUsWredPa.bfC1 = c1;
			tmUsWredPa.bfWeight = weight;
			tmUsWredPa.bfMaxthL = maxTh&0XFFF;
			tmUsWredPa.bfMaxthH= maxTh>>12;
			tmUsWredPa.bfMinth = minTh;
			retVal = tmWredEntryHwWrite(stream,queueId,(UINT32 *)&tmUsWredPa);
			break;
		case DOWN_STREAM:
			tmDsWredPa.bfC1 = c1;
			tmDsWredPa.bfWeight = weight;
			tmDsWredPa.bfMaxthL = maxTh&0XFFF;
			tmDsWredPa.bfMaxthH= maxTh>>12;
			tmDsWredPa.bfMinth = minTh;
			retVal = tmWredEntryHwWrite(stream,queueId,(UINT32 *)&tmDsWredPa);
			break;
		default:
			retVal = OPL_ERR_INVALID_PARAMETERS;
			OPL_DRV_PRINTF(("no such  stream %d\n",stream));
			break;
	}
		
	return retVal;
}

OPL_STATUS tmCosIcosMapShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 upCosIcosMap[TM_UP_STREAM_NUM];
	UINT8 dnCosIcosMap[TM_DOWN_STREAM_NUM];
	UINT8 index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = tmCosIcosMapHwRead(UP_STREAM, index, &upCosIcosMap[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmCosIcosMapHwRead(DOWN_STREAM, index, &dnCosIcosMap[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	OPL_DRV_SHOW_PRINTF(("%-13s %-10s\n","cos","icos"));
	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("upstreamCos %d %-10d\n",index,upCosIcosMap[index]));
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("dnstreamCos%d %-10d\n",index,dnCosIcosMap[index]));
	}

	return OPL_OK;
}

OPL_STATUS tmQidIcosMapShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 upCosIcosMap[TM_UP_STREAM_NUM];
	UINT8 dnCosIcosMap[TM_DOWN_STREAM_NUM];
	UINT8 index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = tmQueueIcosMapHwRead(UP_STREAM, index, &upCosIcosMap[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmQueueIcosMapHwRead(DOWN_STREAM, index, &dnCosIcosMap[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	OPL_DRV_SHOW_PRINTF(("%-10s %-10s\n","Qid","icos"));
	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("upstreamQ%d %-10d\n",index,upCosIcosMap[index]));
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("dnstreamQ%d %-10d\n",index,dnCosIcosMap[index]));
	}

	return OPL_OK;
}

OPL_STATUS tmInQueueCounterShow(void)
{
	OPL_STATUS          retVal = OPL_OK;
	TM_US_Q_CONF_STA_t  upStreamQConfSta;
	TM_DS_Q_CONF_STA_t  dnStreamQConfSta;
	UINT8               index;
    UINT32              tmMode = OPL_ZERO; 
    UINT32              cellCounter[8] = {0};
    UINT32              pktCounter[8] = {0};

     /*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);

    OPL_DRV_SHOW_PRINTF(("TM mode:%d\n",tmMode));
    
	OPL_DRV_SHOW_PRINTF(("%-20s %-12s %-12s\n","QueueIndex","cellInQueue","pktInQueue"));
	for(index = OPL_ZERO; index < (TM_DOWN_STREAM_NUM + TM_UP_STREAM_NUM); index++)
	{
		if(index < TM_UP_STREAM_NUM)
		{
		    if(tmMode)
		    {
                retVal = oplRegFieldRead(REG_TMUS_CELL_INQ0_CNT+index*4,0,14,&cellCounter[index]);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                retVal = oplRegFieldRead(REG_TMUS_PKT_INQ0_CNT+index*4,0,13,&pktCounter[index]);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
            }else
            {
    			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, index, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
		    }
            
            if(tmMode)
            {
                OPL_DRV_SHOW_PRINTF(("usQueue%-13d %-12d %-12d\n",index,cellCounter[index],pktCounter[index]));
            }else
            {
                OPL_DRV_SHOW_PRINTF(("usQueue%-13d %-12d %-12d\n",index,upStreamQConfSta.bfNumOfCell,upStreamQConfSta.bfNumOfPkt));        
            }
		}else
		{
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, index, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			OPL_DRV_SHOW_PRINTF(("dsQueue%-13d %-12d %-12d\n",index - TM_UP_STREAM_NUM,dnStreamQConfSta.bfNumOfCell,dnStreamQConfSta.bfNumOfPkt));
		}
	}
	return retVal;
}

OPL_STATUS tmShaperConfigShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 *statusString[] = {"Disable","Enable",OPL_NULL};
	TM_DS_Q_SHAPER_PA_t dsQshaperPa[TM_UP_STREAM_NUM];
	TM_US_Q_SHAPER_PA_t usQshaperPa[TM_DOWN_STREAM_NUM];
	UINT32 geUpCir;
	UINT32 geUpCbs;
	UINT32 geDownCir;
	UINT32 geDownCbs;
	UINT8 upShaperEnable[TM_UP_STREAM_NUM] ;
	UINT8 dnShaperEnable[TM_DOWN_STREAM_NUM] ;
	UINT8 geUpShaperEnable;
	UINT8 geDownShaperEnable;
	UINT32 qid;

	/*get up stream shaper config info*/
	for(qid = OPL_ZERO; qid < TM_UP_STREAM_NUM;qid++)
	{
		retVal = oplTabRead(OPCONN_TAB_US_SHPBD, qid, 1, (UINT32 *)&(usQshaperPa[qid]));
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}

		retVal = tmQueueShaperEnableHwRead(UP_STREAM, qid, (UINT8 *)&(upShaperEnable[qid]));
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	/*get down stream shaper config info*/
	for(qid = OPL_ZERO; qid < TM_DOWN_STREAM_NUM;qid++)
	{
		retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, qid, 1, (UINT32 *)&(dsQshaperPa[qid]));
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}

		retVal = tmQueueShaperEnableHwRead(DOWN_STREAM, qid, (UINT8 *)&(dnShaperEnable[qid]));
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}
	/*get upstream total  ge shaper parameter*/
	retVal = tmGeCbsHwRead(UP_STREAM, &geUpCbs);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	retVal = tmGeCirHwRead(UP_STREAM, &geUpCir);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}
	
	retVal = tmGeShaperEnableHwRead(UP_STREAM, &geUpShaperEnable);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	/*get downstream total ge shaper parameter*/
	retVal = tmGeCbsHwRead(DOWN_STREAM, &geDownCbs);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	retVal = tmGeCirHwRead(DOWN_STREAM, &geDownCir);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}
	
	retVal = tmGeShaperEnableHwRead(DOWN_STREAM, &geDownShaperEnable);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	OPL_DRV_SHOW_PRINTF(("%s%-5s %-12s %-10s %-10s\n","stream","QID","shaperEnable","cir","cbs"));
	for(qid = OPL_ZERO; qid < TM_UP_STREAM_NUM; qid++)
	{
		OPL_DRV_SHOW_PRINTF(("%9s%-2d %-12s %-10d %-10d\n",
			"upstreamQ",
			qid,
			statusString[upShaperEnable[qid]],
			usQshaperPa[qid].bfCir,
			usQshaperPa[qid].bfCbs));		
	}
	
	OPL_DRV_SHOW_PRINTF(("%s%-5s %-12s %-10s %-10s\n","stream","QID","shaperEnable","cir","cbs"));
	for(qid = OPL_ZERO; qid < TM_UP_STREAM_NUM; qid++)
	{
		OPL_DRV_SHOW_PRINTF(("%9s%-2d %-12s %-10d %-10d\n",
			"dnstreamQ",
			qid,
			statusString[dnShaperEnable[qid]],
			dsQshaperPa[qid].bfCir,
			dsQshaperPa[qid].bfCbs));		
	}

	OPL_DRV_SHOW_PRINTF(("%-11s %-12s %-10d %-10d\n",
			"uptotal",
			statusString[geUpShaperEnable],
			geUpCir,
			geUpCbs));	

	OPL_DRV_SHOW_PRINTF(("%-11s %-12s %-10d %-10d\n",
			"dntotal",
			statusString[geDownShaperEnable],
			geDownCir,
			geDownCbs));	
	return OPL_OK;
}

OPL_STATUS tmInQueueConfigShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	TM_US_Q_CONF_STA_t upStreamQConfSta;
	TM_DS_Q_CONF_STA_t dnStreamQConfSta;
	UINT8  index;
    UINT32 tmMode;
    UINT32 maxCellInQ[8] = {0};
    UINT32 maxPktInQ[8]  = {0};
    
     /*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);

	OPL_DRV_SHOW_PRINTF(("%-20s %-18s %-18s\n","QueueIndex","MaxCellNumInQueue","MaxPktNumInQueue"));
	for(index = OPL_ZERO; index < (TM_DOWN_STREAM_NUM + TM_UP_STREAM_NUM); index++)
	{
		if(index < TM_UP_STREAM_NUM)
		{
		    if(tmMode)
            {
                retVal = oplRegFieldRead(REG_TMUS_Q0_MAX_CELL_NUM+index*4,0,14,&maxCellInQ[index]);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                retVal = oplRegFieldRead(REG_TMUS_Q0_MAX_PKT_NUM+index*4,0,13,&maxPktInQ[index]);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                OPL_DRV_SHOW_PRINTF(("usQueue%-13d %-18d %-18d\n",index,maxCellInQ[index],maxPktInQ[index]));
            }else
            {
    			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, index, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
			    OPL_DRV_SHOW_PRINTF(("usQueue%-13d %-18d %-18d\n",index,upStreamQConfSta.bfMaxNumOfCell,upStreamQConfSta.bfMaxNumOfPkt));
            }    
		}else
		{
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, index, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			OPL_DRV_SHOW_PRINTF(("dsQueue%-13d %-18d %-18d\n",index - TM_UP_STREAM_NUM,dnStreamQConfSta.bfMaxNumOfCell,dnStreamQConfSta.bfMaxNumOfPkt));
		}
	}
	return retVal;
}

OPL_STATUS tmWredConfigShow(void)
{
	OPL_STATUS 		retVal = OPL_OK;
	TM_DS_WRED_PA_t tmDsWredPa;
	TM_US_WRED_PA_t tmUsWredPa;
	UINT32 			tmDsWredEnable;
	UINT32 			tmUsWredEnable;
	UINT8 			index;
	UINT8 			*statusString[3] = {"Disable","Enable",NULL};

	oplRegFieldRead(REG_TMDS_WRED_CTRL,0,1,&tmDsWredEnable);
	
	OPL_DRV_SHOW_PRINTF(("%-12s %-12s %-12s %-12s %-12s %-12s %-12s\n","stream","qid","status","minTh","maxTh","weight","c1"));

	for(index = 0; index <TM_UP_STREAM_NUM; index++)
	{
		retVal = tmWredEntryHwRead(UP_STREAM,index,(UINT32 *)&tmUsWredPa);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}

		oplRegFieldRead(REG_TMUS_WRED_CTRL,index,1,&tmUsWredEnable);
		
		OPL_DRV_SHOW_PRINTF(("%-12s %-12d %-12s %-12d %-12d %-12d %-12d\n",
				"upstream",
				index,
				statusString[tmUsWredEnable],
				tmUsWredPa.bfMinth,
				(tmUsWredPa.bfMaxthH&0xff)<<12|(tmUsWredPa.bfMaxthL&0xfff),
				tmUsWredPa.bfWeight,
				tmUsWredPa.bfC1));
		
	}

	for(index = 0; index <TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmWredEntryHwRead(DOWN_STREAM,index,(UINT32 *)&tmDsWredPa);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
		oplRegFieldRead(REG_TMDS_WRED_CTRL,index,1,&tmDsWredEnable);
		
		OPL_DRV_SHOW_PRINTF(("%-12s %-12d %-12s %-12d %-12d %-12d %-12d\n",
			"downstream",
			index,
			statusString[tmDsWredEnable],
			tmDsWredPa.bfMinth,
			(tmDsWredPa.bfMaxthH&0xff)<<12|(tmDsWredPa.bfMaxthL&0xfff),
			tmDsWredPa.bfWeight,
			tmDsWredPa.bfC1));
		
	}
	return retVal;
}
OPL_STATUS tmQueueWeightShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 upQueueWeight[TM_UP_STREAM_NUM];
	UINT32 dnQueueWeight[TM_DOWN_STREAM_NUM];
	UINT8 index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = tmQueueWeightHwRead(UP_STREAM, index, &upQueueWeight[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmQueueWeightHwRead(DOWN_STREAM, index, &dnQueueWeight[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	OPL_DRV_SHOW_PRINTF(("%-10s %-10s\n","Qid","weight"));
	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("upstreamQ%d %-10d\n",index,upQueueWeight[index]));
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("dnstreamQ%d %-10d\n",index,dnQueueWeight[index]));
	}

	return OPL_OK;
}

OPL_STATUS tmQueuePriorityShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 upQueuePri[TM_UP_STREAM_NUM];
	UINT8 dnQueuePri[TM_DOWN_STREAM_NUM];
	UINT8 index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = tmQueuePriHwRead(UP_STREAM, index, &upQueuePri[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmQueuePriHwRead(DOWN_STREAM, index, &dnQueuePri[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	OPL_DRV_PRINTF(("%-10s %-10s\n","Qid","priority"));
	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("upstreamQ%d %-10d\n",index,upQueuePri[index]));
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("dnstreamQ%d %-10d\n",index,dnQueuePri[index]));
	}

	return OPL_OK;
}

OPL_STATUS tmRptValueOfQueueShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32     rptValue[TM_UP_STREAM_NUM] = {0x0};
	UINT8      index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = oplRegFieldRead(REG_TM_RPT_LENGTH_OF_Q0 + 4*index, 0, 23,&rptValue[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

    OPL_DRV_PRINTF(("%-10s %s\n","Qid","rptValue(include overhead) in byte"));
    
	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("upstreamQ%d %-10d\n",index,rptValue[index]));
	}
    
	return OPL_OK;
}

OPL_STATUS tmTotalPktsInQueueIncMpcpShow(void)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32     pktsNum[TM_UP_STREAM_NUM] = {0x0};
	UINT8      index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = oplRegFieldRead(REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q0 + 4*index, 0, 14,&pktsNum[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

    OPL_DRV_PRINTF(("%-10s %s\n","Qid","totalPktsInqueue(include mpcp in fifo)"));
    
	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		OPL_DRV_SHOW_PRINTF(("upstreamQ%d %-10d\n",index,pktsNum[index]));
	}
    
	return OPL_OK;
}

OPL_STATUS tmUsShaperTabCirDirectHwWrite(UINT32 entryId,UINT32 cir,UINT32 cbs)
{
    TM_US_Q_SHAPER_PA_t  tmUsQShaperPa;
	OPL_STATUS iRet;
    tmUsQShaperPa.bfCbs = cbs;
    tmUsQShaperPa.bfCir = cir;
    tmUsQShaperPa.bfReserved0 = 0;
    iRet +=oplTabWrite(OPCONN_TAB_US_SHPBD, entryId, 1, (UINT32 *)&tmUsQShaperPa);
	return iRet;
}

OPL_STATUS tmUsShaperTabPirDirectHwWrite(UINT32 entryId,UINT32 pir,UINT32 pbs)
{
    TM_US_Q_SHAPER_PA_t  tmUsQShaperPa;
	OPL_STATUS iRet;
    tmUsQShaperPa.bfCbs = pbs;
    tmUsQShaperPa.bfCir = pir;
    tmUsQShaperPa.bfReserved0 = 0;
     iRet +=oplTabWrite(OPCONN_TAB_US_SHPBD, entryId+8, 1, (UINT32 *)&tmUsQShaperPa);
	return iRet;
}

OPL_STATUS tmUsShaperTabDirectHwWrite(UINT32 entryId,UINT32 cir,UINT32 cbs)
{
    TM_US_Q_SHAPER_PA_t  tmUsQShaperPa;

    tmUsQShaperPa.bfCbs = cbs;
    tmUsQShaperPa.bfCir = cir;
    tmUsQShaperPa.bfReserved0 = 0;
    return oplTabWrite(OPCONN_TAB_US_SHPBD, entryId, 1, (UINT32 *)&tmUsQShaperPa);
}
OPL_STATUS tmDsShaperTabDirectHwWrite(UINT32 entryId,UINT32 cir,UINT32 cbs)
{
    TM_DS_Q_SHAPER_PA_t  tmDsQShaperPa;
    tmDsQShaperPa.bfCbs = cbs;
    tmDsQShaperPa.bfCir = cir;
    tmDsQShaperPa.bfReserved0 = 0;
    return oplTabWrite(OPCONN_TAB_DS_SHPBD, entryId, 1, (UINT32 *)&tmDsQShaperPa);
}
OPL_STATUS tmQueueTabDirectHwWrite(UINT32 entryId,UINT32 maxCellNum,UINT32 maxPktNum)
{
    TM_DS_Q_CONF_STA_t tmDsQConfSta;
    TM_US_Q_CONF_STA_t tmUsQConfSta;
    INT32 iRet;
    if(entryId < 8)
    {
        tmUsQConfSta.bfMaxNumOfCell = maxCellNum;
        tmUsQConfSta.bfMaxNumOfPkt = maxPktNum;
        iRet = oplTabWrite(OPCONN_TAB_Q_CONF_STA, entryId, 1, (UINT32 *)&tmUsQConfSta);
    }else
    {
        tmDsQConfSta.bfMaxNumOfCell = maxCellNum;
        tmDsQConfSta.bfMaxNumOfPkt = maxPktNum;
        iRet = oplTabWrite(OPCONN_TAB_Q_CONF_STA, entryId, 1, (UINT32 *)&tmDsQConfSta);
    }
    return iRet;
}
OPL_STATUS tmUsWredTabDirectHwWrite(UINT32 entryId,UINT32 weight,UINT32 minth,UINT32 maxth)
{
	TM_US_WRED_PA_t tmUsWredPa;  
    UINT8 c1;
    if (minth > maxth) 
    {
        return OPL_ERR_INVALID_PARAMETERS;
    }
    tmWredC1ValGet(maxth, minth, &c1);
    tmUsWredPa.bfWeight = weight;
    tmUsWredPa.bfMinth = minth;
    tmUsWredPa.bfMaxthL = maxth;
    tmUsWredPa.bfMaxthH = maxth>>12;
    tmUsWredPa.bfC1 = c1;
    tmUsWredPa.bfReserved0 = 0;
    return oplTabWrite(OPCONN_TAB_US_WRED, entryId, 1, (UINT32 *)&tmUsWredPa);
}
OPL_STATUS tmDsWredTabDirectHwWrite(UINT32 entryId,UINT32 weight,UINT32 minth,UINT32 maxth)
{
	TM_DS_WRED_PA_t tmDsWredPa;  
    UINT8 c1;
    if (minth > maxth) 
    {
        return OPL_ERR_INVALID_PARAMETERS;
    }
    tmWredC1ValGet(maxth, minth, &c1);
    tmDsWredPa.bfWeight = weight;
    tmDsWredPa.bfMinth = minth;
    tmDsWredPa.bfMaxthL = maxth;
    tmDsWredPa.bfMaxthH = maxth>>12;
    tmDsWredPa.bfC1 = c1;
    tmDsWredPa.bfReserved0 = 0;
    return oplTabWrite(OPCONN_TAB_DS_WRED, entryId, 1, (UINT32 *)&tmDsWredPa);
}

OPL_STATUS tmMaxpUsWredTabDirectHwWrite(UINT32 entryId,float p,UINT32 weight,UINT32 minth,UINT32 maxth)
{
	TM_US_WRED_PA_t tmUsWredPa;  
    UINT8 c1;
    tmUsWredC1ValGet(p,maxth, minth, &c1);
    tmUsWredPa.bfWeight = weight;
    tmUsWredPa.bfMinth = minth;
    tmUsWredPa.bfMaxthL = maxth;
    tmUsWredPa.bfMaxthH = maxth>>12;
    tmUsWredPa.bfC1 = c1;
    tmUsWredPa.bfReserved0 = 0;
    return oplTabWrite(OPCONN_TAB_US_WRED, entryId, 1, (UINT32 *)&tmUsWredPa);
}
OPL_STATUS tmMaxpDsWredTabDirectHwWrite(UINT32 entryId,float p,UINT32 weight,UINT32 minth,UINT32 maxth)
{
	TM_DS_WRED_PA_t tmDsWredPa;  
    UINT8 c1;
    tmDsWredC1ValGet(p,maxth, minth, &c1);
    tmDsWredPa.bfWeight = weight;
    tmDsWredPa.bfMinth = minth;
    tmDsWredPa.bfMaxthL = maxth;
    tmDsWredPa.bfMaxthH = maxth>>12;
    tmDsWredPa.bfC1 = c1;
    tmDsWredPa.bfReserved0 = 0;
    return oplTabWrite(OPCONN_TAB_DS_WRED, entryId, 1, (UINT32 *)&tmDsWredPa);
}

OPL_STATUS cliTmCosIcosMapShow(int fd)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 upCosIcosMap[TM_UP_STREAM_NUM];
	UINT8 dnCosIcosMap[TM_DOWN_STREAM_NUM];
	UINT8 index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = tmCosIcosMapHwRead(UP_STREAM, index, &upCosIcosMap[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmCosIcosMapHwRead(DOWN_STREAM, index, &dnCosIcosMap[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	cliVosShowPrintf((fd,"%-13s %-10s\r\n","cos","icos"));
	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"upstreamCos%d %-10d\r\n",index,upCosIcosMap[index]));
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"dnstreamCos%d %-10d\r\n",index,dnCosIcosMap[index]));
	}

	return OPL_OK;
}

OPL_STATUS cliTmQidIcosMapShow(int fd)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 upCosIcosMap[TM_UP_STREAM_NUM];
	UINT8 dnCosIcosMap[TM_DOWN_STREAM_NUM];
	UINT8 index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = tmQueueIcosMapHwRead(UP_STREAM, index, &upCosIcosMap[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmQueueIcosMapHwRead(DOWN_STREAM, index, &dnCosIcosMap[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	cliVosShowPrintf((fd,"%-10s %-10s\r\n","Qid","icos"));
	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"upstreamQ%d %-10d\r\n",index,upCosIcosMap[index]));
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"dnstreamQ%d %-10d\r\n",index,dnCosIcosMap[index]));
	}

	return OPL_OK;
}

OPL_STATUS cliTmInQueueCounterShow(int fd)
{
	OPL_STATUS          retVal = OPL_OK;
	TM_US_Q_CONF_STA_t  upStreamQConfSta;
	TM_DS_Q_CONF_STA_t  dnStreamQConfSta;
	UINT8               index;
    UINT32              tmMode = OPL_ZERO; 
    UINT32              cellCounter[8] = {0};
    UINT32              pktCounter[8] = {0};

     /*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);

    cliVosShowPrintf((fd,"TM mode:%d\r\n",tmMode));
    
	cliVosShowPrintf((fd,"%-20s %-12s %-12s\r\n","QueueIndex","cellInQueue","pktInQueue"));
	/* Bug3191 o1_respin ,modified by cxji,2010/09/09 16:00:00 .begin*/
	for(index = OPL_ZERO; index < (TM_DOWN_STREAM_NUM + TM_UP_STREAM_NUM0); index++)
	{
		if(index < TM_UP_STREAM_NUM0)
		{
		    if(tmMode)
		    {
                retVal = oplRegFieldRead(REG_TMUS_CELL_INQ0_CNT+index*4,0,14,&cellCounter[index]);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                retVal = oplRegFieldRead(REG_TMUS_PKT_INQ0_CNT+index*4,0,13,&pktCounter[index]);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
            }else
            {
    			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, index, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
		    }
            
            if(tmMode)
            {
                cliVosShowPrintf((fd,"usQueue%-13d %-12d %-12d\r\n",index,cellCounter[index],pktCounter[index]));
            }else
            {
                cliVosShowPrintf((fd,"usQueue%-13d %-12d %-12d\r\n",index,upStreamQConfSta.bfNumOfCell,upStreamQConfSta.bfNumOfPkt));        
            }
		}else
		{
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, index, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			cliVosShowPrintf((fd,"dsQueue%-13d %-12d %-12d\r\n",index - TM_UP_STREAM_NUM0,dnStreamQConfSta.bfNumOfCell,dnStreamQConfSta.bfNumOfPkt));
		}
	}
	return retVal;
}

OPL_STATUS cliTmShaperConfigShow(int fd)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 *statusString[] = {"Disable","Enable",OPL_NULL};
	TM_DS_Q_SHAPER_PA_t dsQshaperPa[TM_DOWN_STREAM_NUM];/*bug|3224*/
	TM_US_Q_SHAPER_PA_t usQshaperPa[TM_UP_STREAM_NUM];/*bug|3224*/
	TM_US_Q_SHAPER_PA1_t usQshaperPa1[TM_UP_STREAM_NUM];
	UINT32 geUpCir;
	UINT32 geUpCbs;
	UINT32 geDownCir;
	UINT32 geDownCbs;
	UINT8 upShaperCirEnable[TM_UP_STREAM_NUM] ;
	UINT8 upShaperPirEnable[TM_UP_STREAM_NUM] ;	
	UINT8 dnShaperEnable[TM_DOWN_STREAM_NUM] ;
	UINT8 geUpShaperEnable;
	UINT8 geDownShaperEnable;
	UINT32 qid;

	/*get up stream shaper config info*/
	for(qid = OPL_ZERO; qid < TM_UP_STREAM_NUM;qid++)
	{
		retVal = oplTabRead(OPCONN_TAB_US_SHPBD, qid, 1, (UINT32 *)&(usQshaperPa[qid]));
		if(OPL_OK != retVal)
		{	
			OPL_LOG_TRACE();
			return retVal;
		}
		if (IS_ONU_RESPIN)
		{
			retVal = oplTabRead(OPCONN_TAB_US_SHPBD, qid+8, 1, (UINT32 *)&(usQshaperPa1[qid]));
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
		}

		retVal = tmQueueShaperCirEnableHwRead(UP_STREAM, qid, (UINT8 *)&(upShaperCirEnable[qid]));
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}

		if (IS_ONU_RESPIN)
		{
			retVal = tmQueueShaperPirEnableHwRead(UP_STREAM, qid, (UINT8 *)&(upShaperPirEnable[qid]));
			if(OPL_OK != retVal)
			{
				OPL_LOG_TRACE();
				return retVal;
			}
		}
	}

	/*get down stream shaper config info*/
	for(qid = OPL_ZERO; qid < TM_DOWN_STREAM_NUM;qid++)
	{
		retVal = oplTabRead(OPCONN_TAB_DS_SHPBD, qid, 1, (UINT32 *)&(dsQshaperPa[qid]));
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}

		retVal = tmQueueShaperEnableHwRead(DOWN_STREAM, qid, (UINT8 *)&(dnShaperEnable[qid]));
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}
	/*get upstream total  ge shaper parameter*/
	retVal = tmGeCbsHwRead(UP_STREAM, &geUpCbs);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	retVal = tmGeCirHwRead(UP_STREAM, &geUpCir);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}
	
	retVal = tmGeShaperEnableHwRead(UP_STREAM, &geUpShaperEnable);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	/*get downstream total ge shaper parameter*/
	retVal = tmGeCbsHwRead(DOWN_STREAM, &geDownCbs);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}

	retVal = tmGeCirHwRead(DOWN_STREAM, &geDownCir);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}
	
	retVal = tmGeShaperEnableHwRead(DOWN_STREAM, &geDownShaperEnable);
	if(OPL_OK != retVal)
	{
		OPL_LOG_TRACE();
		return retVal;
	}
	if (IS_ONU_RESPIN)
	{
		cliVosShowPrintf((fd,"%s%-5s %-12s %-10s %-10s %-12s %-10s %-10s\r\n","stream","QID","CirEnable","cir","cbs","PirEnable","pir","pbs"));
		for(qid = OPL_ZERO; qid < TM_UP_STREAM_NUM; qid++)
		{
			cliVosShowPrintf((fd,"%9s%-2d %-12s %-10d %-10d %-12s %-10d %-10d \r\n",
				"upstreamQ",
				qid,
				statusString[upShaperCirEnable[qid]],
				usQshaperPa[qid].bfCir,
				usQshaperPa[qid].bfCbs,
				statusString[upShaperPirEnable[qid]],
				usQshaperPa1[qid].bfPir,
				usQshaperPa1[qid].bfPbs));		
		}
	}
	else
	{
		cliVosShowPrintf((fd,"%s%-5s %-12s %-10s %-10s\r\n","stream","QID","shaperEnable","cir","cbs"));
		for(qid = OPL_ZERO; qid < TM_UP_STREAM_NUM; qid++)
		{
			cliVosShowPrintf((fd,"%9s%-2d %-12s %-10d %-10d\r\n",
				"upstreamQ",
				qid,
				statusString[upShaperCirEnable[qid]],
				usQshaperPa[qid].bfCir,
				usQshaperPa[qid].bfCbs));		
		}
	}
	cliVosShowPrintf((fd,"%s%-5s %-12s %-10s %-10s\r\n","stream","QID","shaperEnable","cir","cbs"));
	for(qid = OPL_ZERO; qid < TM_DOWN_STREAM_NUM; qid++)
	{
		cliVosShowPrintf((fd,"%9s%-2d %-12s %-10d %-10d\r\n",
			"dnstreamQ",
			qid,
			statusString[dnShaperEnable[qid]],
			dsQshaperPa[qid].bfCir,
			dsQshaperPa[qid].bfCbs));		
	}

	cliVosShowPrintf((fd,"%-11s %-12s %-10d %-10d\r\n",
			"uptotal",
			statusString[geUpShaperEnable],
			geUpCir,
			geUpCbs));	

	cliVosShowPrintf((fd,"%-11s %-12s %-10d %-10d\r\n",
			"dntotal",
			statusString[geDownShaperEnable],
			geDownCir,
			geDownCbs));	
	return OPL_OK;
}

OPL_STATUS cliTmInQueueConfigShow(int fd)
{
	OPL_STATUS retVal = OPL_OK;
	TM_US_Q_CONF_STA_t upStreamQConfSta;
	TM_DS_Q_CONF_STA_t dnStreamQConfSta;
	UINT8  index;
    UINT32 tmMode;
    UINT32 maxCellInQ[8] = {0};
    UINT32 maxPktInQ[8]  = {0};
    
     /*read tm mode*/
    oplRegFieldRead(REG_GB_TM_MODE,0,1,&tmMode);

	cliVosShowPrintf((fd,"%-20s %-18s %-18s\r\n","QueueIndex","MaxCellNumInQueue","MaxPktNumInQueue"));
	/* Bug3191 o1_respin ,modified by cxji,2010/09/09 16:00:00 .begin*/
	for(index = OPL_ZERO; index < (TM_DOWN_STREAM_NUM + TM_UP_STREAM_NUM0); index++)
	{
		if(index < TM_UP_STREAM_NUM0)
		{
		    if(tmMode)
            {
                retVal = oplRegFieldRead(REG_TMUS_Q0_MAX_CELL_NUM+index*4,0,14,&maxCellInQ[index]);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                retVal = oplRegFieldRead(REG_TMUS_Q0_MAX_PKT_NUM+index*4,0,13,&maxPktInQ[index]);
                if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
                cliVosShowPrintf((fd,"usQueue%-13d %-18d %-18d\r\n",index,maxCellInQ[index],maxPktInQ[index]));
            }else
            {
    			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, index, 1, (UINT32 *)&upStreamQConfSta);
    			if(OPL_OK != retVal)
    			{
    				return retVal;
    			}
			    cliVosShowPrintf((fd,"usQueue%-13d %-18d %-18d\r\n",index,upStreamQConfSta.bfMaxNumOfCell,upStreamQConfSta.bfMaxNumOfPkt));
            }    
		}else
		{
			retVal = oplTabRead(OPCONN_TAB_Q_CONF_STA, index, 1, (UINT32 *)&dnStreamQConfSta);
			if(OPL_OK != retVal)
			{
				return retVal;
			}
			cliVosShowPrintf((fd,"dsQueue%-13d %-18d %-18d\r\n",index - TM_UP_STREAM_NUM0,dnStreamQConfSta.bfMaxNumOfCell,dnStreamQConfSta.bfMaxNumOfPkt));
		}
	}
	return retVal;
}

OPL_STATUS cliTmWredConfigShow(int fd)
{
	OPL_STATUS 		retVal = OPL_OK;
	TM_DS_WRED_PA_t tmDsWredPa;
	TM_US_WRED_PA_t tmUsWredPa;
	UINT32 			tmDsWredEnable;
	UINT32 			tmUsWredEnable;
	UINT8 			index;
	UINT8 			*statusString[3] = {"Disable","Enable",NULL};

	oplRegFieldRead(REG_TMDS_WRED_CTRL,0,1,&tmDsWredEnable);
	
	cliVosShowPrintf((fd,"%-12s %-12s %-12s %-12s %-12s %-12s %-12s\r\n","stream","qid","status","minTh","maxTh","weight","c1"));

	for(index = 0; index <TM_UP_STREAM_NUM; index++)
	{
		retVal = tmWredEntryHwRead(UP_STREAM,index,(UINT32 *)&tmUsWredPa);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}

		oplRegFieldRead(REG_TMUS_WRED_CTRL,index,1,&tmUsWredEnable);
		
		cliVosShowPrintf((fd,"%-12s %-12d %-12s %-12d %-12d %-12d %-12d\r\n",
				"upstream",
				index,
				statusString[tmUsWredEnable],
				tmUsWredPa.bfMinth,
				(tmUsWredPa.bfMaxthH&0xff)<<12|(tmUsWredPa.bfMaxthL&0xfff),
				tmUsWredPa.bfWeight,
				tmUsWredPa.bfC1));
		
	}

	for(index = 0; index <TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmWredEntryHwRead(DOWN_STREAM,index,(UINT32 *)&tmDsWredPa);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
		oplRegFieldRead(REG_TMDS_WRED_CTRL,index,1,&tmDsWredEnable);
		
		cliVosShowPrintf((fd,"%-12s %-12d %-12s %-12d %-12d %-12d %-12d\r\n",
			"downstream",
			index,
			statusString[tmDsWredEnable],
			tmDsWredPa.bfMinth,
			(tmDsWredPa.bfMaxthH&0xff)<<12|(tmDsWredPa.bfMaxthL&0xfff),
			tmDsWredPa.bfWeight,
			tmDsWredPa.bfC1));
		
	}
	return retVal;
}
OPL_STATUS cliTmQueueWeightShow(int fd)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32 upQueueWeight[TM_UP_STREAM_NUM];
	UINT32 dnQueueWeight[TM_DOWN_STREAM_NUM];
	UINT8 index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = tmQueueWeightHwRead(UP_STREAM, index, &upQueueWeight[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmQueueWeightHwRead(DOWN_STREAM, index, &dnQueueWeight[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	cliVosShowPrintf((fd,"%-10s %-10s\r\n","Qid","weight"));
	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"upstreamQ%d %-10d\r\n",index,upQueueWeight[index]));
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"dnstreamQ%d %-10d\r\n",index,dnQueueWeight[index]));
	}

	return OPL_OK;
}

OPL_STATUS cliTmQueuePriorityShow(int fd)
{
	OPL_STATUS retVal = OPL_OK;
	UINT8 upQueuePri[TM_UP_STREAM_NUM];
	UINT8 dnQueuePri[TM_DOWN_STREAM_NUM];
	UINT8 index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = tmQueuePriHwRead(UP_STREAM, index, &upQueuePri[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		retVal = tmQueuePriHwRead(DOWN_STREAM, index, &dnQueuePri[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

	cliVosShowPrintf(("%-10s %-10s\r\n","Qid","priority"));
	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"upstreamQ%d %-10d\r\n",index,upQueuePri[index]));
	}

	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"dnstreamQ%d %-10d\r\n",index,dnQueuePri[index]));
	}

	return OPL_OK;
}

OPL_STATUS cliTmRptValueOfQueueShow(int fd)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32     rptValue[TM_UP_STREAM_NUM] = {0x0};
	UINT8      index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = oplRegFieldRead(REG_TM_RPT_LENGTH_OF_Q0 + 4*index, 0, 23,&rptValue[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

    cliVosShowPrintf(("%-10s %s\r\n","Qid","rptValue(include overhead) in byte"));
    
	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"upstreamQ%d %-10d\r\n",index,rptValue[index]));
	}
    
	return OPL_OK;
}

OPL_STATUS cliTmTotalPktsInQueueIncMpcpShow(int fd)
{
	OPL_STATUS retVal = OPL_OK;
	UINT32     pktsNum[TM_UP_STREAM_NUM] = {0x0};
	UINT8      index;

	for(index = OPL_ZERO; index < TM_UP_STREAM_NUM; index++)
	{
		retVal = oplRegFieldRead(REG_TM_TOTAL_PKTS_INC_MPCP_IN_FIFO_Q0 + 4*index, 0, 14,&pktsNum[index]);
		if(OPL_OK != retVal)
		{
			OPL_LOG_TRACE();
			return retVal;
		}
	}

    cliVosShowPrintf(("%-10s %s\r\n","Qid","totalPktsInqueue(include mpcp in fifo)"));
    
	for(index = OPL_ZERO; index < TM_DOWN_STREAM_NUM; index++)
	{
		cliVosShowPrintf((fd,"upstreamQ%d %-10d\r\n",index,pktsNum[index]));
	}
    
	return OPL_OK;
}

