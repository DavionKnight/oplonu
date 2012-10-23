/*=============================================================================
     Header Name: commoninfo.h

     General Description:
general definition for the basic data type in Opulan driver.
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
yangcheng          07/05/05           Initial Version
----------------   ------------  ----------------------------------------------*/



#ifndef IPMUX_COMMONINFO_H
#define IPMUX_COMMONINFO_H



#define CRC_BYTE_BIT_COUNT           0x80
#define CRC16_POLYNOMIAL_AMERICAN    0x8005
#define CRC16_POLYNOMIAL_CCITT       0x1021
#define CRC16_RESULT_LENGTH          0x0000FFFF
#define CRC16_MOST_SIGNIFICIANT_BIT  0x00008000
#define CRC32_POLYNOMIAL             0x04C11DB7
#define CRC32_RESULT_LENGTH          0xFFFFFFFF
#define CRC32_MOST_SIGNIFICIANT_BIT  0x80000000





extern OPL_STATUS CRC16BitShift(UINT8 *pucValue,UINT32 ulValueLegnth,UINT16 *pusCrcValue);
extern OPL_STATUS CRC16ByteLookup(UINT8 *pucValue,UINT32 ulValueLegnth,UINT16 *pusCrcValue);
extern OPL_STATUS CRC16HfByteLookup(UINT8 *pucValue,UINT32 ulValueLegnth,UINT16 *pusCrcValue);
extern OPL_STATUS CRC32BitShift(UINT8 *pucValue,UINT32 ulValueLegnth,UINT32 *pulCrcValue);
extern OPL_STATUS CRC32ByteLookup(UINT8 *pucValue,UINT32 ulValueLegnth,UINT32 *pulCrcValue);
extern OPL_STATUS CRC32HfByteLookup(UINT8 *pucValue,UINT32 ulValueLegnth,UINT32 *pulCrcValue);
#endif


