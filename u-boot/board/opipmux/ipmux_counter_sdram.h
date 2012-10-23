#include "opl_driver.h"

#ifndef IPMUX_COUNTER_SDRAM_H
#define IPMUX_COUNTER_SDRAM_H

typedef struct OUTPUT_MEMBER_S
{
	int	iNo;//=-1;
	char headerIndexStr[20];//="";
} OUTPUT_MEMBER_T;


typedef enum IPMUX_COUNTER_SDRAM_KIND_E
{
		COUNTER_SDRAM_KIND_FLOW=0,
		COUNTER_SDRAM_KIND_CLASSIFY=1,
		COUNTER_SDRAM_KIND_PORT=2,
		COUNTER_SDRAM_KIND_PORT_VLAN=3,
		COUNTER_SDRAM_KIND_LINK=4,
		COUNTER_SDRAM_KIND_GROUP=5,
		COUNTER_SDRAM_KIND_ALL=-1

} IPMUX_COUNTER_SDRAM_KIND_e;
/*
typedef enum IPMUX_COUNTER_SDRAM_E
{
		COUNTER_SDRAM_FLOW_UP=0,
		COUNTER_SDRAM_FLOW_DOWN,
		COUNTER_SDRAM_CLASSIFY,
		COUNTER_SDRAM_PORT,
		COUNTER_SDRAM_PORT_VLAN_UP,
		COUNTER_SDRAM_PORT_VLAN_DOWN,
		// link & bonding  
		COUNTER_SDRAM_RXCL,
		COUNTER_SDRAM_RXCG,
		COUNTER_SDRAM_RXAL,
		COUNTER_SDRAM_TXCL,
		COUNTER_SDRAM_TXCG,
		COUNTER_SDRAM_TXAL,
		
		COUNTER_SDRAM_ALL=-1
} IPMUX_COUNTER_SDRAM_e;
*/
/* struct for access to IPMux Internal Table */
typedef struct IPMUX_COUNTER_SDRAM_s
{
	UINT32	nCur;
	UINT64	nSum;
} IPMUX_COUNTER_SDRAM_t;

typedef struct IPMUX_STATISTC_GEW_PV_CTL_TBL_s
{
#ifdef IPMUX_BIG_ENDIAN
    UINT32 :20;
    UINT32 bfSvid:12;

    UINT32 bfCvid:12;
    UINT32 bfCntGpId:10;
    UINT32 bfRhptr:10;
#else
    UINT32 bfSvid:12;
    UINT32 :20;

    UINT32 bfRhptr:10;
    UINT32 bfCntGpId:10;
    UINT32 bfCvid:12;
#endif
}IPMUX_STATISTC_GEW_PV_CTL_TBL_T;

typedef struct IPMUX_BIT_FIELD_DEF_s 
{
	char name[20];
	int bitS;
	int bitE;
} IPMUX_BIT_FIELD_DEF_t ;

typedef struct IPMUX_BIT_FIELD_VAL_s
{
	UINT32 cur;
	UINT64 sum;
} IPMUX_BIT_FIELD_VAL_t;

typedef struct IPMUX_STATISTC_TBL_DESC_s
{
	IPMUX_TAB_e tabId;		
	IPMUX_COUNTER_SDRAM_KIND_e kind;

	char valid;
	char useHugeBuf;		/* if malloc numEntry*numFields size or only numFields for the fields buffer */ 

	int numField;			/* number of field count */ 
	int numEntry;		/* number of table entry */ 

	char *header;		/* header to print */ 
	
	IPMUX_BIT_FIELD_VAL_t *fields;
	IPMUX_BIT_FIELD_DEF_t *fieldsDef;

}IPMUX_STATISTC_TBL_DESC_t;

typedef struct IPMUX_STATISTC_ACCESS_PV_CTL_TBL_s
{
#ifdef IPMUX_BIG_ENDIAN
    UINT32 :22;
    UINT32 bfPortId:10;

    UINT32 bfCvid:12;
    UINT32 bfCntGpId:10;
    UINT32 bfRhptr:10;
#else
    UINT32 bfPortId:10;
    UINT32 :22;

    UINT32 bfRhptr:10;
    UINT32 bfCntGpId:10;
    UINT32 bfCvid:12;
#endif
	
}IPMUX_STATISTC_ACCESS_PV_CTL_TBL_t;




#endif






