#include <configs/opipmux.h>
#ifdef INCLUDE_SDRAM_COUNTER
#include "opl_driver.h"

#include "commonforAPI.h"
#include "commoninfo.h"


OPL_STATUS TableAddrCal16(UINT8 *pucValue,UINT32 ulValueLen
                               ,UINT16 usAddrMask,UINT16 *pusAddress)
{
    UINT16     usCrcValue = 0;
    OPL_STATUS iStatus;
    

    if(NULL == pucValue)
    {
        return PARAMETER_INPUT_POINTER_NULL;
    }

    iStatus = CRC16BitShift(pucValue,ulValueLen,&usCrcValue);
    if(OPL_OK != iStatus)
    {
        return iStatus ;
    }

    *pusAddress = usAddrMask & usCrcValue;
    return OPL_OK;
}

OPL_STATUS TableAddrCal32(UINT8 *pucValue,UINT32 ulValueLegnth
                                   ,UINT32 ulAddrMask,UINT32 *pulAddress)
{
    UINT32 ulCrcValue = 0;
    OPL_STATUS iStatus;


    if(NULL == pucValue)
    {
        return PARAMETER_INPUT_POINTER_NULL;
    }

    iStatus = CRC32BitShift(pucValue,ulValueLegnth,&ulCrcValue);
    if(OPL_OK != iStatus)
    {
        return iStatus;
    }

    *pulAddress = ulAddrMask & ulCrcValue;
    return OPL_OK;
}

OPL_STATUS CRC16BitShift(UINT8 *pucValue,UINT32 ulValueLegnth
                                                       ,UINT16 *pusCrcValue)
{
    UINT8  ucCharBit  = 0;
	UINT16 usCrc     = 0xFFFF;

	if(NULL == pucValue)
	{
	    return PARAMETER_INPUT_POINTER_NULL;
	}

	while(0 != ulValueLegnth--)
	{
		for(ucCharBit = CRC_BYTE_BIT_COUNT; 0 != ucCharBit; ucCharBit /= 2)
		{
			if(0 != (usCrc & CRC16_MOST_SIGNIFICIANT_BIT))
			{
				usCrc *= 2;
				usCrc ^= CRC16_POLYNOMIAL_CCITT;
			}
			else
			{
				usCrc *= 2;
			}

			if(((*pucValue) & ucCharBit) != 0)
			{
				usCrc ^= CRC16_POLYNOMIAL_CCITT;
			}
		}

		pucValue++;
	}

    *pusCrcValue = usCrc;
    return(OPL_OK);
}

OPL_STATUS CRC32BitShift(UINT8 *pucValue,UINT32 ulValueLegnth
                                                  ,UINT32 *pulCrcValue)
{
    UINT8  ucCharBit = 0;
	UINT32 ulCrc     = 0;

	if(NULL == pucValue)
	{
	    return PARAMETER_INPUT_POINTER_NULL;
	}

	while(0 != ulValueLegnth--)
	{
		for(ucCharBit = CRC_BYTE_BIT_COUNT; 0 != ucCharBit; ucCharBit /= 2)
		{
			if(0 != (ulCrc&CRC32_MOST_SIGNIFICIANT_BIT))
			{
				ulCrc *= 2;
				ulCrc ^= CRC32_POLYNOMIAL;
			}
			else
			{
				ulCrc *= 2;
			}

			if(((*pucValue)&ucCharBit) != 0)
			{
				ulCrc ^= CRC32_POLYNOMIAL;
			}
		}

		pucValue++;
	}

    *pulCrcValue = ulCrc;
	return(OPL_OK);
}

#endif
