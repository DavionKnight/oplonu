/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	stats.h
*
* DESCRIPTION:
*	This file implements the structures and macros for stats module.
*
* Date Created:
*	2008-08-27
*
* Authors(optional):
*	Zhihua Lu
*
* Reviewed by (optional):
*
******************************************************************************/

#ifndef __STATS_H__
#define __STATS_H__

#include "cli.h"
#include "opl_driver.h"

#define POLL_GE_MASK		0x00000001
#define POLL_PON_MASK		0x00000002
#define POLL_FE_MASK		0x00000004
#define POLL_TM_MASK		0x00000008
#define POLL_CLE_MASK		0x00000010
#define POLL_BRG_MASK		0x00000020
#define POLL_MPCP_MASK		0x00000040
#define POLL_MARB_MASK		0x00000080
#define POLL_CPDMA_MASK		0x00000100
#define POLL_FEDMA_MASK		0x00000200
#define POLL_GEPARSER_MASK	0x00000400
#define POLL_CLEHIT_MASK	0x00000800
#define POLL_SWITCH_MASK	0x00001000

#define CNT_GE_NUM			38
#define CNT_PON_NUM			45
#define CNT_FE_NUM			9
#define CNT_TM_NUM			12
#define CNT_CLE_NUM			2
#define CNT_BRG_NUM			18
#define CNT_MPCP_NUM		17
#define CNT_MARB_NUM		6
#define CNT_CPDMA_NUM		11
#define CNT_FEDMA_NUM		7
#define CNT_GEPARSER_NUM	6
#define CNT_CLEHIT_NUM		128
#define CNT_SWH_PORT_NUM	30

#define SWITCH_PORT_NUM		7

#define DECIMAL_STR_LEN		21
#define LONG_HEX_NUM		8

typedef enum CNT_POLL_e
{
	CNT_GE=1,
	CNT_PON,
	CNT_FE,
	CNT_TM,
	CNT_CLE,
	CNT_BRG,
	CNT_MPCP,
	CNT_MARB,
	CNT_CPDMA,
	CNT_FEDMA,
	CNT_GEPARSER,
	CNT_CLEHIT,
	CNT_SWITCH
}CNT_POLL_t;

typedef struct OPL_CNT_ADD_s
{
	UINT32 uiHigh;
	UINT32 uiLow;
}OPL_CNT_ADD_t;

typedef struct OPL_CNT_s
{
	UINT16			usId;
	UINT8			*pucName;
	UINT32			uiCurrent;
	UINT32			uiTotal;
	OPL_CNT_ADD_t	stAdded;
	UINT8			*pucHelp;
}OPL_CNT_t;


/* PON Counter Special Treatment Index */
typedef enum OPL_PON_CNT_ST_IDX_e
{
    OPL_PON_CNT_ST_IDX_TxTotalPkts = 2,
    OPL_PON_CNT_ST_IDX_TxMACControlPkts = 6,
    OPL_PON_CNT_ST_IDX_TxOAMPkts = 15,
    OPL_PON_CNT_ST_IDX_RxTotalPkts = 22,
    OPL_PON_CNT_ST_IDX_RxMACControlPkts = 27,
    OPL_PON_CNT_ST_IDX_RxOAMPkts = 42
}OPL_PON_CNT_ST_IDX_t;


/* MPCP Counter Special Treatment Index */
typedef enum OPL_MPCP_CNT_ST_IDX_e
{
    OPL_MPCP_CNT_ST_IDX_REGREQ = 12,
    OPL_MPCP_CNT_ST_IDX_REGACK = 13
}OPL_MPCP_CNT_ST_IDX_t;

typedef struct ONU_PON_Port_Statistics_Get_1_s{
	UINT64 FramesTransmittedOK;			/* LLID�˿ڷ���֡����*/
	UINT64 OctetsTransmittedOK;			/* LLID�˿ڷ����ֽڼ���*/
	UINT64 MulticastFramesXmittedOK;	/* LLID�˿ڷ����鲥֡����*/
	UINT64 BroadcastFramesXmittedOK;	/* LLID�˿ڷ��͹㲥����� */
	UINT64 PONPauseFramesTransmittedOk;	/* PON�˿ڷ���PAUSE֡����*/
	UINT64 FramesReceivedOK;			/*LLID�˿ڽ���֡����*/
	UINT64 OctetsReceivedOK;			/* LLID�˿ڽ����ֽڼ��� */
	UINT64 MulticastFramesReceivedOK;	/* LLID�˿ڽ����鲥֡����*/
	UINT64 BroadcastFramesReceivedOK;	/* LLID�˿ڽ��չ㲥����� */
	UINT64 PONPauseFramesReceivedOk;	/* PON�˿ڽ���PAUSE֡����*/
	UINT64 SingleCollisionFrames;		/*����֡��ײ����*/
	UINT64 MultipleCollisionFrames;		/*���֡��ײ����*/ 	
	UINT64 AlignmentErrors;				/*����������*/
	UINT64 FrameTooLongErrors;			/*֡��������*/
}ONU_PON_Port_Statistics_Get_1_t;

typedef struct ONU_PON_Port_Statistics_Get_2_s{
	UINT64 CRC8Errors;						/* CRC������� */
	UINT64 FECCorrectedBlocks;				/* FEC��ȷ����� */
	UINT64 FECUncorrectableBlocks;			/* FEC�������� */
	UINT64 MPCPMACCtrlFramesTransmitted;	/* MPCP����֡���ͼ���*/
	UINT64 MPCPMACCtrlFramesReceived;		/* MPCP����֡���ռ���*/
	UINT64 MPCPTxGate;						/* MPCP Tx Gate����*/
	UINT64 MPCPTxRegAck;					/* MPCP Tx Reg Ack����*/
	UINT64 MPCPTxRegister;					/* MPCP Tx Register����*/
	UINT64 MPCPTxRegRequest;				/* MPCP Tx Registe Requestr����  */
	UINT64 MPCPTxReport;					/* MPCP Tx Report���� */
	UINT64 MPCPRxGate;						/*MPCP Rx Gate����*/
	UINT64 MPCPRxRegAck;					/*MPCP Rx Reg Ack����*/
	UINT64 MPCPRxRegister;					/*MPCP Rx Register����*/
	UINT64 MPCPRxRegRequest;				/*MPCP Rx RegRequest����*/
	UINT64 MPCPRxReport;					/*MPCP Rx Report����*/	
}ONU_PON_Port_Statistics_Get_2_t;

typedef struct ONU_Port_Flux_Statistics_Counter_s{
	UINT64 ifInOctets;       /* �յ������а��ֽ��� */
	UINT64 ifInUcastPkts;    /* �յ������е�������Ŀ */
	UINT64 ifInNUcastPkts;   /* �յ��������鲥����Ŀ */
	UINT64 ifInDiscards;     /* ��������а���Ŀ */
	UINT64 ifInErrors;       /* �յ������д������Ŀ  */
	UINT64 ifOutOctets;      /* �յ������а��ֽ���  */
	UINT64 ifOutUcastPkts;   /* �յ������е�������Ŀ  */
	UINT64 ifOutNUcastPkts;  /* �յ��������鲥����Ŀ  */
	UINT64 ifOutDiscards;    /* ��������а���Ŀ  */
	UINT64 ifOutErrors;      /* �յ������а���Ŀ  */
}ONU_Port_Flux_Statistics_Counter_t;


typedef struct pon_statistic_s{
    UINT64	OctetsTx;
    UINT64	OctetsTxRate;
    UINT64	OctetsTxTopRate;
    UINT64	FrameTx;
    UINT64	FrameTxRate;
    UINT64	TxPacketsError;
    UINT64	OctetsRx;
    UINT64	OctetsRxRate;
    UINT64	OctetsRxTopRate;
    UINT64	FrameRx;
    UINT64	FrameRxRate;
    UINT64	RxLosePackets;
    UINT64	RxLosePacketsRate;
}pon_statistics_t;

typedef struct port_statistic_s{
    UINT64	OctetsTx;
    UINT64	OctetsTxRate;
    UINT64	OctetsTxTopRate;
    UINT64	UnicastFramesTx;
    UINT64	NonUnicastFramesTx;
    UINT64	FrameTxRate;
    UINT64	TxPacketsError;
    UINT64	OctetsRx;
    UINT64	OctetsRxRate;
    UINT64	OctetsRxTopRate;
    UINT64	UnicastFramesRx;
    UINT64	NonUnicastFramesRx;
    UINT64	FrameRxRate;
    UINT64	RxPacketsError;
}port_statistics_t;

extern OPL_STATUS odmPonSinglePortStatsGet(UINT8 port_id, port_statistics_t *portStats);
extern OPL_STATUS odmPonStatsGet(pon_statistics_t * ponStats);

extern OPL_STATUS ShowCntGe(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntPon(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntFe(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntTm(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntCle(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntBrg(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntMpcp(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntMarb(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntCpdma(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntFedma(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntGeParser(int iWrFd, unsigned int uiMode);
extern OPL_STATUS ShowCntCleHit(int iWrFd, UINT8 ucStart, UINT8 ucNum);
extern void statsLock();
extern void statsUnLock();
extern void PollingHandler(void *);
extern OPL_STATUS CntClr(int iModule,UINT8 ucStart,UINT8 ucNum);
extern OPL_STATUS ShowCntSwhPort(int iWrFd, UINT8 ucStart, UINT8 ucNum, int iType);

extern OPL_STATUS odmStatsThreadInit(void);
extern OPL_STATUS odmStatsThreadShutdown(void);

#endif

