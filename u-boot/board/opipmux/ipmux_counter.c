#include <configs/opipmux.h>

#ifdef INCLUDE_COUNTER 
	#ifndef INCLUDE_FUNC_64BIT
		#define INCLUDE_FUNC_64BIT 1
	#endif
#endif
#ifdef INCLUDE_SDRAM_COUNTER 
	#ifndef INCLUDE_FUNC_64BIT
		#define INCLUDE_FUNC_64BIT 1
	#endif
#endif

#ifdef INCLUDE_FUNC_64BIT
#include <common.h>
#include "ipmux_counter.h"
#define atoi(x) simple_strtoul(x,OPL_NULL,0)
#endif

#ifdef INCLUDE_COUNTER

//#define IPMUX_COUNTER_OFFSET -0x24200+0xc420
#define IPMUX_COUNTER_OFFSET 0
char IPMUX_COUNTER_KIND_NAME[][30]={
		"GE0_INTERFACE",
		"GE1_INTERFACE",
		"FE_INTERFACE",
		"UTOPIA_INTERFACE",
		"POSPHY_INTERFACE",
		"MULTICAST",
		"TRAFFIC_MANAGEMENT",
		"PACKET_PROCESS_ENGINE",
		"BRIDGE",
		"IWF"
} ;

IPMUX_COUNTER_DESC_TEST_t ipmuxCounters[]={
	{"UTP0 RX Cell Count"                                , 0x10320, 1, RC, 26, 0, 0, 0, UTOPIA_INTERFACE},
	{"UTP0 RX HEC Error Cell Count"                      , 0x10324, 1, RC, 16, 0, 0, 0, UTOPIA_INTERFACE},
	{"UTP0 RX Parity Error Cell Count"                   , 0x10328, 1, RC, 16, 0, 0, 0, UTOPIA_INTERFACE},
	{"UTP0 RX OAM Cell Count"                            , 0x1032C, 1, RC, 16, 0, 0, 0, UTOPIA_INTERFACE},
	{"UTP0 TX Cell Count"                                , 0x10330, 1, RC, 26, 0, 0, 0, UTOPIA_INTERFACE},
	{"UTP1 RX Cell Count"                                , 0x10334, 1, RC, 26, 0, 0, 0, UTOPIA_INTERFACE},
	{"UTP1 RX HEC Error Cell Count"                      , 0x10338, 1, RC, 16, 0, 0, 0, UTOPIA_INTERFACE},
	{"UTP1 RX Parity Error Cell Count"                   , 0x1033C, 1, RC, 16, 0, 0, 0, UTOPIA_INTERFACE},
	{"UTP1 RX OAM Cell Count"                            , 0x10340, 1, RC, 16, 0, 0, 0, UTOPIA_INTERFACE},
	{"UTP1 TX Cell Count"                                , 0x10344, 1, RC, 26, 0, 0, 0, UTOPIA_INTERFACE},
	
	{"RX UPPT Cell Lookup Error Count"                   , 0x1031C , 1, RC, 16, 0, 0, 0, UTOPIA_INTERFACE},
	{"POS0 Rx Packet Count"                              , 0x10348, 1, RC, 26, 0, 0, 0, POSPHY_INTERFACE},      
	
	{"POS0 RX Parity Error Word Count"                   , 0x1034C, 1, RC, 16, 0, 0, 0, POSPHY_INTERFACE},
	{"POS0 RX Pin Error Packet Count"                    , 0x10350, 1, RC, 16, 0, 0, 0, POSPHY_INTERFACE},
	{"POS0 Tx Packet Count"                              , 0x10354, 1, RC, 26, 0, 0, 0, POSPHY_INTERFACE},
	{"POS1 Rx Packet Count"                              , 0x10358, 1, RC, 26, 0, 0, 0, POSPHY_INTERFACE},
	{"POS1 RX Parity Error Word Count"                   , 0x1035C, 1, RC, 16, 0, 0, 0, POSPHY_INTERFACE},
	{"POS1 RX Pin Error Packet Count"                    , 0x10360, 1, RC, 16, 0, 0, 0, POSPHY_INTERFACE},
	{"POS1 Tx Packet Count"                              , 0x10364, 1, RC, 26, 0, 0, 0, POSPHY_INTERFACE},
	{"RX UPPT Packet Lookup Error Count"                 , 0x13E40, 1, RC, 16, 0, 0, 0, IWF},
	{"RX EPPT Lookup Error Count"                        , 0x13E44, 1, RC, 16, 0, 0, 0, IWF},
	{"TX UPPT Lookup Error Count"                        , 0x13E48, 1, RC, 16, 0, 0, 0, IWF},
	{"RX OAM Packet Count"                               , 0x13E4C, 1, RC, 16, 0, 0, 0, IWF},
	{"RX AAL0 Cell Count"                                , 0x13E50, 1, RC, 26, 0, 0, 0, IWF},
	{"RX AAL0 Packet Count"                              , 0x13E54, 1, RC, 26, 0, 0, 0, IWF},
	{"TX OAM Drop Packet Count"                          , 0x13E58, 1, RC, 16, 0, 0, 0, IWF},
	{"TX AAL0 Drop Packet Count"                         , 0x13E5C, 1, RC, 16, 0, 0, 0, IWF},
	/*     {""                                                  , 0x13E60, 1, RC, , 0, 0, 0, IWF},      _  Error define in doc find!  */
	{"RX AAL5 Cell Count"                                , 0x13E64, 1, RC, 26, 0, 0, 0, IWF},
	{"RX AAL5 Drop Cell Count"                           , 0x13E68, 1, RC, 16, 0, 0, 0, IWF},
	{"RX AAL5 Assembly Cell Count"                       , 0x13E6C, 1, RC, 26, 0, 0, 0, IWF},
	{"RX Undersize Packet Count"                         , 0x13E70, 1, RC, 16, 0, 0, 0, IWF},
	{"RX Oversize Packet Count"                          , 0x13E74, 1, RC, 16, 0, 0, 0, IWF},
	{"RX POSPHY Packet CRC Error Count"                  , 0x13E78, 1, RC, 16, 0, 0, 0, IWF},
	{"Host Buffer Overflow Drop Packet Count"            , 0x14408, 1, RC, 16, 0, 0, 0, TRAFFIC_MANAGEMENT},
	{"Host Policing Drop Packet Count"                   , 0x1440C, 1, RC, 16, 0, 0, 0, TRAFFIC_MANAGEMENT},
	{"Up_Flow WRED Drop Packet Count"                    , 0x14410, 1, RC, 16, 0, 0, 0, TRAFFIC_MANAGEMENT},
	{"Up_Flow Queue Length Full Drop Packet Count"       , 0x14418, 1, RC, 16, 0, 0, 0, TRAFFIC_MANAGEMENT},
	{"Dn_Flow WRED Drop Packet Count"                    , 0x14F08, 1, RC, 16, 0, 0, 0, TRAFFIC_MANAGEMENT},
	{"Dn_Flow Queue Length Full Drop Packet Count"       , 0x14F10, 1, RC, 16, 0, 0, 0, TRAFFIC_MANAGEMENT},
	{"Classification Miss Event Count"                   , 0x18190, 1, RC, 16, 0, 0, 0, PACKET_PROCESS_ENGINE},
	{"Flow Search Miss Event Count"                      , 0x18194, 1, RC, 16, 0, 0, 0, PACKET_PROCESS_ENGINE},
	{"Up VLAN Replacement Error Count"                   , 0x21200, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"Dn VLAN Replacement Error Count"                   , 0x21204, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"Port No Active Drop Packet Count"                  , 0x21208, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"Reserved MAC Address Packet Drop Packet Count"     , 0x2120C, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"VLAN Error Drop Packet Count"                      , 0x21210, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"SA Check Error Drop Packet Count"                  , 0x21214, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"MC Drop Packet Count"                              , 0x21218, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"MAC Number Limit Drop Packet Count"                , 0x2121C, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"Forward Disable Drop Packet Count"                 , 0x21220, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"VLAN CC Drop Packet Count"                         , 0x21224, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"Flooding Disable Drop Packet Count"                , 0x21228, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"PVC Isolation Drop Unicast Packet Count"           , 0x2122C, 1, RC, 16, 0, 0, 0, BRIDGE},
	{"Fld_Bc Policing Drop Packet Count"                 , 0x21450, 1, RC, 16, 0, 0, 0, MULTICAST},
	{"Multicast Queue Overflow Drop Packet Count"        , 0x21454, 1, RC, 16, 0, 0, 0, MULTICAST},
	{"Egress Port Based Drop Flooding Packet Count"      , 0x21458, 1, RC, 16, 0, 0, 0, MULTICAST},
	{"Egress Port Based Drop Broadcast Packet Count"     , 0x2145C, 1, RC, 16, 0, 0, 0, MULTICAST},
	{"PVC Isolation Drop Non-Unicast Packet Count"       , 0x21460, 1, RC, 16, 0, 0, 0, MULTICAST},
	{"Entering Multicast Queue Packet Count"             , 0x21464, 1, RC, 26, 0, 0, 0, MULTICAST},
	{"Replicated Packet Count"                           , 0x21468, 1, RC, 26, 0, 0, 0, MULTICAST},
	{"Rx etherStatsDropEvents Count"                     , 0x8100, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsOctets Count"                         , 0x8104, 1, RC, 32, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsPkts Count"                           , 0x8108, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsBroadcastPkts Count"                  , 0x810C, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsMulticastPkts Count"                  , 0x8110, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsCRCAlignErrors Count"                 , 0x8114, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsUndersizePkts Count"                  , 0x8118, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsOversizePkts Count"                   , 0x811C, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsJabbers Count"                        , 0x8120, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsPkts64Octets Count"                   , 0x8124, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsPkts65to127Octets Count"              , 0x8128, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsPkts128to255Octets Count"             , 0x812C, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Rx therStatsPkts256to511Octets Count"              , 0x8130, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsPkts512to1023Octets Count"            , 0x8134, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Rx etherStatsPkts1024to1518Octets Count"           , 0x8138, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Rx aMACControlFrames Count"                        , 0x813C, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	{"Rx aPAUSEMACCtrlFrames Count"                      , 0x8140, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	{"Rx aUnsupportedOpcodes Count"                      , 0x8144, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	{"Tx etherStatsOctets Count"                         , 0x8148, 1, RC, 32, 0, 0, 0, GE0_INTERFACE},
	{"Tx etherStatsPkts Count"                           , 0x814C, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Tx etherStatsBroadcastPkts Count"                  , 0x8150, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Tx etherStatsMulticastPkts Count"                  , 0x8154, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Tx etherStatsPkts64Octets Count"                   , 0x8158, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Tx etherStatsPkts65to127Octets Count"              , 0x815C, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Tx etherStatsPkts128to255Octets Count"             , 0x8160, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Tx therStatsPkts256to511Octets Count"              , 0x8164, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Tx etherStatsPkts512to1023Octets Count"            , 0x8168, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Tx etherStatsPkts1024to1518Octets Count"           , 0x816C, 1, RC, 26, 0, 0, 0, GE0_INTERFACE},
	{"Tx aPAUSEMACCtrlFrames Count"                      , 0x8170, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	{"Tx aMACControlFrames Count"                        , 0x8174, 1, RC, 16, 0, 0, 0, GE0_INTERFACE},
	
	{"Rx etherStatsDropEvents Count"                     , 0xa100, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsOctets Count"                         , 0xa104, 1, RC, 32, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsPkts Count"                           , 0xa108, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsBroadcastPkts Count"                  , 0xa10C, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsMulticastPkts Count"                  , 0xa110, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsCRCAlignErrors Count"                 , 0xa114, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsUndersizePkts Count"                  , 0xa118, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsOversizePkts Count"                   , 0xa11C, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsJabbers Count"                        , 0xa120, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsPkts64Octets Count"                   , 0xa124, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsPkts65to127Octets Count"              , 0xa128, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsPkts128to255Octets Count"             , 0xa12C, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Rx therStatsPkts256to511Octets Count"              , 0xa130, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsPkts512to1023Octets Count"            , 0xa134, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Rx etherStatsPkts1024to1518Octets Count"           , 0xa138, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Rx aMACControlFrames Count"                        , 0xa13C, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	{"Rx aPAUSEMACCtrlFrames Count"                      , 0xa140, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	{"Rx aUnsupportedOpcodes Count"                      , 0xa144, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	{"Tx etherStatsOctets Count"                         , 0xa148, 1, RC, 32, 0, 0, 0, GE1_INTERFACE},
	{"Tx etherStatsPkts Count"                           , 0xa14C, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Tx etherStatsBroadcastPkts Count"                  , 0xa150, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Tx etherStatsMulticastPkts Count"                  , 0xa154, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Tx etherStatsPkts64Octets Count"                   , 0xa158, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Tx etherStatsPkts65to127Octets Count"              , 0xa15C, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Tx etherStatsPkts128to255Octets Count"             , 0xa160, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Tx therStatsPkts256to511Octets Count"              , 0xa164, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Tx etherStatsPkts512to1023Octets Count"            , 0xa168, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Tx etherStatsPkts1024to1518Octets Count"           , 0xa16C, 1, RC, 26, 0, 0, 0, GE1_INTERFACE},
	{"Tx aPAUSEMACCtrlFrames Count"                      , 0xa170, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	{"Tx aMACControlFrames Count"                        , 0xa174, 1, RC, 16, 0, 0, 0, GE1_INTERFACE},
	
	{"Rx etherStatsDropEvents Count"                     , 0xC100, 1, RC, 16, 0, 0, 0, FE_INTERFACE},
	{"Rx etherStatsPkts Count"                           , 0xC104, 1, RC, 26, 0, 0, 0, FE_INTERFACE},
	{"Tx etherStatsPkts Count"                           , 0xC108, 1, RC, 26, 0, 0, 0, FE_INTERFACE},

	/*     {"RX UPPT Cell error                                ", , 1, RC, , 0, 0, 0, IWF},      _  Error define in doc find!  */
	{""     ,     0,0, 0,     0,0,0,0}
};


/*******************************************************************************
*
* CounterUpdate
*
* DESCRIPTION:
* 	 update current value and accumulative value of the indicated kind counter.
*
* INPUTS:
*	kind 		- the counter kind.
*
* OUTPUT:
*	The counters current value and accumulative value.
*
* RETURNS:
*	OPL_OK,                       If the operation is successful.
*	OPL_ERROR,                    If the operation is failed.
*
* SEE ALSO: 
*
*/
int CounterUpdate (IPMUX_COUNTER_KIND_e kind) 
{
	return  CounterProcess(kind,0,0,1);
}

/*******************************************************************************
*
* CounterClear
*
* DESCRIPTION:
* 	 clear current value and accumulative value of the indicated kind counter.
*
* INPUTS:
*	kind 		- the counter kind.
*
* OUTPUT:
*	The counters current value and accumulative value.
*
* RETURNS:
*	OPL_OK,                       If the operation is successful.
*	OPL_ERROR,                    If the operation is failed.
*
* SEE ALSO: 
*
*/
int CounterClear (IPMUX_COUNTER_KIND_e kind)
{
	return  CounterProcess(kind,0,1,0);
}


/*******************************************************************************
*
* CounterShow
*
* DESCRIPTION:
* 	 show current value and accumulative value of the indicated kind counter.
*
* INPUTS:
*	kind 		- the counter kind.
*
* OUTPUT:
*	The counters current value and accumulative value.
*
* RETURNS:
*	OPL_OK,                       If the operation is successful.
*	OPL_ERROR,                    If the operation is failed.
*
* SEE ALSO: 
*
*/
int CounterShow (IPMUX_COUNTER_KIND_e kind)
{
	return  CounterProcess(kind,1,0,0);
}

/*******************************************************************************
*
* generatMaskBits
*
* DESCRIPTION:
* 	 generate the mask according to bitwidth.
*
* INPUTS:
*	kind 		- the counter kind.
*
* OUTPUT:
*	The counters current value and accumulative value.
*
* RETURNS:
*	OPL_OK,                       If the operation is successful.
*	OPL_ERROR,                    If the operation is failed.
*
* SEE ALSO: 
*
*/
static int generatMaskBits(UINT32 Arr[],int iLenMask,int iLenArr)
{
	int i;
	int j=0;
	for(i=0; i<iLenArr  ; i++)
	{
		Arr[i]=0;
	}
	for(i=0; i< iLenMask; i++)
	{
		if ((i!=0) && (i %32==0) )
		{
			j++;
		}
		Arr[j]=Arr[j]<<1 | 1;
	}
	
	return j+1; 
	
}

/*******************************************************************************
*
* CounterProcess
*
* DESCRIPTION:
* 	Do the IPMUX counters clear, update and show.
*
* INPUTS:
*	kind 		- the counter kind.
*	show		- if print the counters .
*	clear		- if clear the counters statistic.
*   update		- if update the counters from rigisters.
*
* OUTPUT:
*	The counters current value and accumulative value.
*
* RETURNS:
*	OPL_OK,                       If the operation is successful.
*	OPL_ERROR,                    If the operation is failed.
*
* SEE ALSO: 
*
*/
int CounterProcess(IPMUX_COUNTER_KIND_e kind,int show,int clear,int update, int hexDisplay) 
{
	
	UINT32                       u1Next;
	UINT32                       u4BaseAddr;
	UINT32                       u4Offset;
	UINT32                       u1WriteVal;
	UINT32                       u1ReadVal;
	int                          i, j,iNo;
	UINT32 *pu1Wval;
	UINT32 *pu1Rval;
	int regNum, regNumBad;
	UINT32 regAddr;
	#define CounterMaxWidth 1
	UINT32 DataMask[CounterMaxWidth];
	unsigned long long  ssss=0x1FFFFFFFF;

	printf("ssss=%x",ssss);
	u4BaseAddr = (UINT32)IPMUX_REG_BASE;
	/* make sure the IPMMUX Chip is out of the RESET state */
	/*-----------------------------------------------------*/
	if (show) printf("\nIPMUX %s couters process : %s %s %s, display mode : %s...\n",kind==-1?"all" : IPMUX_COUNTER_KIND_NAME[kind],show?"show":"",update?"update":"",clear?"clear":"",hexDisplay?"hex":"decimal");

	j=0;

	i=0;
	regNumBad= 0;
	regNum = 0;
	iNo=0;
	
	if (show) printf ("%3s %-5s %-8s %-16s %-8s\n","#","regid", "Current","Total","Name");
	if (show) printf ("%3s %5s %8s %16s %8s\n","---","-----" ,"--------","----------------","---------------------------------------------");

	while(ipmuxCounters[i].regId != 0) 
	{
		if ((kind==-1) || (ipmuxCounters[i].kind==kind))
		{
			regAddr =IPMUX_COUNTER_OFFSET+ ipmuxCounters[i].regId + j * REG_ADDR_ALLIGNED;
			
			/*clear*/
			if (clear)	
			{
				ipmuxCounters[i].nCur=0;
				ipmuxCounters[i].nSum.high =0;
				ipmuxCounters[i].nSum.low =0;
			}

			/*read*/
			if (update) 
			{
				printfCounter("\n------i=%d,ipmuxCounters[i].regId=0x%x,regAddr=0x%x\n",i,ipmuxCounters[i].regId,regAddr);

				ipMuxRegRead(regAddr, &u1ReadVal);
			

				generatMaskBits(DataMask,ipmuxCounters[i].bitWidth,CounterMaxWidth);
				
				if (u1ReadVal>DataMask[0]) 
				{
					printf("\n*** error!u1ReadVal=0x%x, dataMask=0x%x\n",u1ReadVal,DataMask[0]);
				}
				
				u1ReadVal &= DataMask[0];

				ipmuxCounters[i].nCur=u1ReadVal;
				oplUINT64Add(&ipmuxCounters[i].nSum,ipmuxCounters[i].nCur);

			}
			/* show  */ 
			
				
			if (show) 
			{
				if (0==hexDisplay) 
					printf ("%3d %5x %8u %16s %s\n", \
							      iNo++,				 regAddr,				ipmuxCounters[i].nCur , 
								  oplAsciiUint64(&ipmuxCounters[i].nSum,1), ipmuxCounters[i].name             );
				else
					printf ("%3d %5x %08x %08x-%08x %s\n", \
							      iNo++,				 regAddr,				ipmuxCounters[i].nCur , 
								  ipmuxCounters[i].nSum.high , ipmuxCounters[i].nSum.low , ipmuxCounters[i].name             );
			}

			regNum ++;

		}
		i++;
	}
	
	if (show) printf("\n	%d Counters displayed.\n", regNum);

	//if (clear) printf("\n	%d Counters cleared.\n", regNum);

	return	(OPL_OK);
}




#endif

#ifdef INCLUDE_FUNC_64BIT
/*******************************************************************************
*
* oplUINT64Add
*
* DESCRIPTION:
* 	 convert UINT64 var to chars for display
*
* INPUTS:
*	ptm 		- the UINT64 data pointer.
*	nRemoveLeadingSpace	- remove leading space counter.
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO: 
*
*/
char * oplAsciiUint64 (char *ptm, int nRemoveLeadingSpace)
{
	static char n64bit[20 + 1]; /* hold the max 20 decimal digit
	string for a 64-bit value  */
	UINT64 u64Tmp;
	UINT64 u64Tmp1;
	char *pstr;
	char cTmp;
int i;
UINT32 nTmp;

	pstr=(char *)&u64Tmp1;
	bzero ((char *)n64bit, sizeof(n64bit));

	u64Tmp.high = ((UINT64*)ptm)->low;
	u64Tmp.low = ((UINT64*)ptm)->high;

	u64Tmp1.high = ((UINT64*)ptm)->low;
	u64Tmp1.low = ((UINT64*)ptm)->high;
	
//	((UINT64*)ptm)->low=ntohl(((UINT64*)ptm)->low);
//	((UINT64*)ptm)->high=ntohl(((UINT64*)ptm)->high);
	
	for (i=0; i <4; i++)
	{
		((char*)&(((UINT64*)&u64Tmp1)->high))[i] = 	((char*)&u64Tmp.high)[3-i];
	}
	for (i=0; i <4; i++)
	{
		((char*)&(((UINT64*)&u64Tmp1)->low))[i] = 	((char*)&u64Tmp.low)[3-i];
	}

	sprintf ((char *)&n64bit[0], "%20u", (UINT32)pstr[0]*7936 +
			 (UINT32)pstr[1]*656 + (UINT32)pstr[2]*7776 + (UINT32)pstr[3]*7296 +
			 (UINT32)pstr[4]*7216 + (UINT32)pstr[5]*65536 + (UINT32)pstr[6]*256 +
			 (UINT32)pstr[7]);

	cTmp = n64bit[16];

	n64bit[16] = '\0';

	nTmp = atoi ((char*)&n64bit[0]);
	
	sprintf ((char *)&n64bit[0], "%16u", (UINT32)pstr[0]*3792 +
			 (UINT32)pstr[1]*7671 + (UINT32)pstr[2]*1162 + (UINT32)pstr[3]*9496 +
			 (UINT32)pstr[4]*1677 + nTmp);

	n64bit[16] = cTmp;

	cTmp = n64bit[12];

	n64bit[12] = '\0';

	nTmp = atoi ((char*)&n64bit[0]);

	sprintf ((char *)&n64bit[0], "%12u", (UINT32)pstr[0]*5940 +
			 (UINT32)pstr[1]*4749 + (UINT32)pstr[2]*995 + (UINT32)pstr[3]*42 + nTmp);

	n64bit[12] = cTmp;

	cTmp = n64bit[8];

	n64bit[8] = '\0';

	nTmp = atoi ((char*)&n64bit[0]);

	sprintf ((char *)&n64bit[0], "%8u", (UINT32)pstr[0]*2057 +
			 (UINT32)pstr[1]*281 + (UINT32)pstr[2] + nTmp);

	n64bit[8] = cTmp;

	cTmp = n64bit[4];

	n64bit[4] = '\0';

	nTmp = atoi ((char*)&n64bit[0]);

	sprintf ((char *)&n64bit[0], "%4u", (UINT32)pstr[0]*7 + nTmp);

	n64bit[4] = cTmp;



	for (nTmp = 0; nTmp < sizeof(n64bit); nTmp++)

	{
		
		if ('0' == n64bit[nTmp])
			
			n64bit[nTmp] = ' ';
		
		if (' ' != n64bit[nTmp])
			
			break;
		
	}

	if ('\0' == n64bit[nTmp])

	{
		
		n64bit[nTmp - 1] = '0';
		
		/*printf ("timestamp   :0x%08x%08x,%s\n",
		(UINT32)*((UINT32*)&pstr[0]), (UINT32)*((UINT32*)&pstr[4]), (char
		*)&n64bit[nTmp - 1]);*/
		
		if (nRemoveLeadingSpace)
			
			return (char *) &n64bit[nTmp - 1];
		
	}

	else

	{
		
	/*printf ("timestamp   :0x%08x%08x,%s\n",
	(UINT32)*((UINT32*)&pstr[0]), (UINT32)*((UINT32*)&pstr[4]), (char
		*)&n64bit[nTmp]);*/
		
		if (nRemoveLeadingSpace)
			
			return (char *) &n64bit[nTmp];
		
	}

	return (char *)&n64bit[0];

}


/*******************************************************************************
*
* oplUINT64Add
*
* DESCRIPTION:
* 	 add UINT32 value to UINT64 var.
*
* INPUTS:
*	p64 		- the value added to.
*	uAugant		- the value added.  
*
* OUTPUT:
*	N/A.
*
* RETURNS:
*	N/A.
*
* SEE ALSO: 
*
*/
void oplUINT64Add (UINT64 *p64, UINT32 uAugant)
{
	
	UINT32 nTmpLow;
	
	if (p64)
	{
		
		nTmpLow = p64->low;
		
		p64->low += uAugant;
		
		if ((UINT32)p64->low < (UINT32)nTmpLow)
		{
			p64->high++;
		}
	}
	
}

#endif
