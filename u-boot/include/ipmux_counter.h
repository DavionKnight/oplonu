#include "extra.h"

typedef enum IPMUX_COUNTER_KIND_E
{
		GE0_INTERFACE,
		GE1_INTERFACE,
		FE_INTERFACE,
		UTOPIA_INTERFACE,
		POSPHY_INTERFACE,
		MULTICAST,
		TRAFFIC_MANAGEMENT,
		PACKET_PROCESS_ENGINE,
		BRIDGE,
		IWF,
		ALL_COUNTER=-1
} IPMUX_COUNTER_KIND_e;

typedef enum IPMUX_COUNTER_TYPE_E
{
	RC,
		RW
} IPMUX_COUNTER_TYPE_e;


/* struct for access to IPMux Internal Table */
typedef struct IPMUX_COUNTER_DESC_TEST_s
{
	char name[50];
	UINT32 	regId;
	UINT32 	num;
	IPMUX_COUNTER_TYPE_e TYPE;		/*RC OR RW*/
	UINT32	bitWidth; 
	
	UINT32	nCur;
	UINT64	nSum;
	
	IPMUX_COUNTER_KIND_e kind;	    /*counter kind: */
} IPMUX_COUNTER_DESC_TEST_t;
