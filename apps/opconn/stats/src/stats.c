/******************************************************************************
*    COPYRIGHT (C) 2003-2007 Opulan Technologies Corp. ALL RIGHTS RESERVED.
*
*                       Proprietary and Confidential
*  This software is made available only to customers and prospective
*  customers of Opulan Technologies Corporation under license and may be
*  used only with Opulan semi-conductor products.
*
* FILENAME:
*	stats.c
*
* DESCRIPTION:
*	This file implements the stats show and poll on ONU system.
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

#include "stats.h"
#include "hw_interface.h"
#include "oam.h"

#if defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6045) || defined(ONU_4PORT_88E6097)
#include "msApi.h"
#endif

OPL_CNT_t g_astCntGe[CNT_GE_NUM]={
{0x0920, "TxTotalPkt", 0, 0, {0, 0}, "Packets (good and error) transmitted."},
{0x0921, "TxBroad", 0, 0, {0, 0}, "Good broadcast packets transmitted."},
{0x0922, "TxMulti", 0, 0, {0, 0}, "Good multicast packets transmitted."},
{0x0923, "TxPause", 0, 0, {0, 0}, "PAUSE frames transmitted."},
{0x0924, "TxMacCtrl", 0, 0, {0, 0}, "MAC Control frames transmitted."},
{0x0925, "Tx64byte", 0, 0, {0, 0}, "64-byte packets (good and error) transmitted."},
{0x0926, "Tx65to127", 0, 0, {0, 0}, "65-127 bytes packets (good and error) transmitted."},
{0x0927, "Tx128to255", 0, 0, {0, 0}, "128-255 bytes packets (good and error) transmitted."},
{0x0928, "Tx256to511", 0, 0, {0, 0}, "256-511 bytes packets (good and error) transmitted."},
{0x0929, "Tx512to1023", 0, 0, {0, 0}, "512-1023 bytes packets (good and error) transmitted."},
{0x092A, "Tx1024to1518", 0, 0, {0, 0}, "1024-1518 bytes packets (good and error) transmitted."},
{0x092B, "TxByte", 0, 0, {0, 0}, "Good bytes transmitted."},
{0x092C, "Tx1519toMTU", 0, 0, {0, 0}, "1519-MTU bytes packets (good and error) transmitted."},
{0x0940, "RxGood", 0, 0, {0, 0}, "Good packets received."},
{0x0941, "RxDrop", 0, 0, {0, 0}, "Packets received and droped."},
{0x0942, "RxCRCAlignErr", 0, 0, {0, 0}, "Packets with crc or alignment error received."},
{0x0943, "RxLenErr", 0, 0, {0, 0}, "Packets with type/len field error received."},
{0x0944, "RxFragments", 0, 0, {0, 0}, "Packets less than 64 bytes and have crc error received."},
{0x0945, "RxJabbers", 0, 0, {0, 0}, "Packets longer than MTU and have crc error received."},
{0x0946, "RxTotalPkt", 0, 0, {0, 0}, "Packets (good and error) received."},
{0x0947, "RxBroad", 0, 0, {0, 0}, "Good broadcast packets received."},
{0x0948, "RxMulti", 0, 0, {0, 0}, "Good multicast packets received."},
{0x0949, "RxPause", 0, 0, {0, 0}, "PAUSE frames received."},
{0x094A, "RxErrMacCtrl", 0, 0, {0, 0}, "Unsupported MAC Control frames received."},
{0x094B, "RxMACCtrl", 0, 0, {0, 0}, "MAC Control frames received."},
{0x094C, "RxUndersize", 0, 0, {0, 0}, "Packets less than 64 bytes in length received."},
{0x094D, "RxOversize", 0, 0, {0, 0}, "Packets longer than MTU in length received."},
{0x094E, "Rx64byte", 0, 0, {0, 0}, "64-byte packets (good and error) received."},
{0x094F, "Rx65to127", 0, 0, {0, 0}, "65-127 bytes packets (good and error) received."},
{0x0950, "Rx128to255", 0, 0, {0, 0}, "128-255 bytes packets (good and error) received."},
{0x0951, "Rx256to511", 0, 0, {0, 0}, "256-511 bytes packets (good and error) received."},
{0x0952, "Rx512to1023", 0, 0, {0, 0}, "512-1023 bytes packets (good and error) received."},
{0x0953, "Rx1024to1518", 0, 0, {0, 0}, "1024-1518 bytes packets (good and error) received."},
{0x0954, "Rx1519toMTU", 0, 0, {0, 0}, "1519-MTU bytes packets (good and error) received."},
{0x0955, "RxByte", 0, 0, {0, 0}, "Good bytes received."},
{0x0959, "RxL2Multi", 0, 0, {0, 0}, "Good L2 multicast packets received."},
{0x0960, "RxRXERPkts", 0, 0, {0, 0}, "Packets received error with other reasons."},
{0x0961, "RxIPGErr", 0, 0, {0, 0}, "Packets received for which the gap is less than the minimum."},
};

OPL_CNT_t g_astCntPon[CNT_PON_NUM]={
{0x0A1C, "ErrBits", 0, 0, {0, 0}, "PMAC interface received error coded bits counter."},
{0x0A1D, "CorrectBits", 0, 0, {0, 0}, "PMAC interface received correct coded bits counter."},
{0x0A20, "TxTotalPkt", 0, 0, {0, 0}, "Packets (good and error) transmitted."},
{0x0A21, "TxBroad", 0, 0, {0, 0}, "Good broadcast packets transmitted."},
{0x0A22, "TxMulti", 0, 0, {0, 0}, "Good multicast packets transmitted."},
{0x0A23, "TxPause", 0, 0, {0, 0}, "PAUSE frames transmitted."},
{0x0A24, "TxMacCtrl", 0, 0, {0, 0}, "MAC Control frames transmitted."},
{0x0A25, "Tx64byte", 0, 0, {0, 0}, "64-byte packets (good and error) transmitted."},
{0x0A26, "Tx65to127", 0, 0, {0, 0}, "65-127 bytes packets (good and error) transmitted."},
{0x0A27, "Tx128to255", 0, 0, {0, 0}, "128-255 bytes packets (good and error) transmitted."},
{0x0A28, "Tx256to511", 0, 0, {0, 0}, "256-511 bytes packets (good and error) transmitted."},
{0x0A29, "Tx512to1023", 0, 0, {0, 0}, "512-1023 bytes packets (good and error) transmitted."},
{0x0A2A, "Tx1024to1518", 0, 0, {0, 0}, "1024-1518 bytes packets (good and error) transmitted."},
{0x0A2B, "TxByte", 0, 0, {0, 0}, "Good bytes transmitted."},
{0x0A2C, "Tx1519toMTU", 0, 0, {0, 0}, "1519-MTU bytes packets (good and error) transmitted."},
{0x0A2D, "TxOAM", 0, 0, {0, 0}, "OAM packets transmitted."},
{0x0A40, "RxGood", 0, 0, {0, 0}, "Good packets received."},
{0x0A41, "RxDrop", 0, 0, {0, 0}, "Packets received and droped."},
{0x0A42, "RxCRCAlignErr", 0, 0, {0, 0}, "Packets with crc or alignment error received."},
{0x0A43, "RxLenErr", 0, 0, {0, 0}, "Packets with type/len field error received."},
{0x0A44, "RxFragments", 0, 0, {0, 0}, "Packets less than 64 bytes and have crc error received."},
{0x0A45, "RxJabbers", 0, 0, {0, 0}, "Packets longer than MTU and have crc error received."},
{0x0A46, "RxTotalPkt", 0, 0, {0, 0}, "Packets (good and error) received."},
{0x0A47, "RxBroad", 0, 0, {0, 0}, "Good broadcast packets received."},
{0x0A48, "RxMulti", 0, 0, {0, 0}, "Good multicast packets received."},
{0x0A49, "RxPause", 0, 0, {0, 0}, "PAUSE frames received."},
{0x0A4A, "RxErrMacCtrl", 0, 0, {0, 0}, "Unsupported MAC Control frames received."},
{0x0A4B, "RxMACCtrl", 0, 0, {0, 0}, "MAC Control frames received."},
{0x0A4C, "RxUndersize", 0, 0, {0, 0}, "Packets less than 64 bytes in length received."},
{0x0A4D, "RxOversize", 0, 0, {0, 0}, "Packets longer than MTU in length received."},
{0x0A4E, "Rx64byte", 0, 0, {0, 0}, "64-byte packets (good and error) received."},
{0x0A4F, "Rx65to127", 0, 0, {0, 0}, "65-127 bytes packets (good and error) received."},
{0x0A50, "Rx128to255", 0, 0, {0, 0}, "128-255 bytes packets (good and error) received."},
{0x0A51, "Rx256to511", 0, 0, {0, 0}, "256-511 bytes packets (good and error) received."},
{0x0A52, "Rx512to1023", 0, 0, {0, 0}, "512-1023 bytes packets (good and error) received."},
{0x0A53, "Rx1024to1518", 0, 0, {0, 0}, "1024-1518 bytes packets (good and error) received."},
{0x0A54, "Rx1519toMTU", 0, 0, {0, 0}, "1519-MTU bytes packets (good and error) received."},
{0x0A55, "RxByte", 0, 0, {0, 0}, "Good bytes received."},
{0x0A56, "FECCORRECT", 0, 0, {0, 0}, "Packet that FEC has corrected."},
{0x0A57, "FECUNCOR", 0, 0, {0, 0}, "Packet that FEC has not corrected."},
{0x0A58, "FECNOERR", 0, 0, {0, 0}, "Packet without any error."},
{0x0A59, "RxL2Multi", 0, 0, {0, 0}, "Good L2 multicast packets received."},
{0x0A5a, "RxOAM", 0, 0, {0, 0}, "OAM packets received."},
{0x0A60, "RxRXERPkts", 0, 0, {0, 0}, "Packets received error with other reasons."},
{0x0A61, "RxIPGErr", 0, 0, {0, 0}, "Packets received for which the gap is less than the minimum."},
};

OPL_CNT_t g_astCntFe[CNT_FE_NUM]={
{0x0D10, "TxTotalPkt", 0, 0, {0, 0}, "Packets (good and error) transmitted."},
{0x0D11, "TxPause", 0, 0, {0, 0}, "PAUSE frames transmitted."},
{0x0D12, "RxGood", 0, 0, {0, 0}, "Good packets received."},
{0x0D13, "RxDrop", 0, 0, {0, 0}, "Packets received and droped."},
{0x0D14, "RxPause", 0, 0, {0, 0}, "PAUSE frames received."},
{0x0D15, "RxCRCAlignErr", 0, 0, {0, 0}, "Packets with crc or alignment error received."},
{0x0D16, "RxLenErr", 0, 0, {0, 0}, "Packets with type/len field error received by FE port."},
{0x0D17, "RxFragments", 0, 0, {0, 0}, "Packets less than 64 bytes and have crc error received."},
{0x0D18, "RxJabbers", 0, 0, {0, 0}, "Packets longer than MTU and have crc error received."},
};

OPL_CNT_t g_astCntTm[CNT_TM_NUM]={
{0x20AC, "UpWREDDrop", 0, 0, {0, 0}, "Upstream packets dropped due to WRED."},
{0x20AD, "UpEnQ", 0, 0, {0, 0}, "Upstream packets written into DDR queue."},
{0x20AE, "DnWREDDrop", 0, 0, {0, 0}, "Downstream packets dropped due to WRED."},
{0x20AF, "DnEnQ", 0, 0, {0, 0}, "Downstream packets written into DDR queue."},
{0x20B0, "UpInput", 0, 0, {0, 0}, "Upstream packets entered TM module."},
{0x20B1, "UpIndiDrop", 0, 0, {0, 0}, "Upstream packets dropped due to the indication from CLE module."},
{0x20B2, "UpQFullDrop", 0, 0, {0, 0}, "Upstream packets dropped due to DDR queue full."},
{0x20B3, "UpOutput", 0, 0, {0, 0}, "Upstream packets quitted TM module."},
{0x20B4, "DnInput", 0, 0, {0, 0}, "Downstream packets entered TM module."},
{0x20B5, "DnIndiDrop", 0, 0, {0, 0}, "Downstream packets dropped due to the indication from CLE module."},
{0x20B6, "DnQFullDrop", 0, 0, {0, 0}, "Downstream packets dropped due to DDR queue full."},
{0x20B7, "DnOutput", 0, 0, {0, 0}, "Downstream packets quitted TM module."},
};

OPL_CNT_t g_astCntCle[CNT_CLE_NUM]={
{0x3009, "RuleMiss", 0, 0, {0, 0}, "Packets which have rule mismatch for all srule tables."},
{0x300A, "RuleDrop", 0, 0, {0, 0}, "Packets which match the rule where the transfer action is drop."},
};

OPL_CNT_t g_astCntBrg[CNT_BRG_NUM]={
{0x3048, "GERxDrop", 0, 0, {0, 0}, "Received packets dropped due to Rx disabled at GE port."},
{0x3049, "GETxDrop", 0, 0, {0, 0}, "Transmited packets dropped due to Tx disabled at GE port."},
{0x304A, "UpVTTMis", 0, 0, {0, 0}, "Upstream packets dropped due to mismatch."},
{0x304B, "UpTagDrop", 0, 0, {0, 0}, "Upstream packets dropped not complying with the tag config."},
{0x304C, "PONRxDrop", 0, 0, {0, 0}, "Received packets dropped due to Rx disabled at PON port."},
{0x304D, "PONTxDrop", 0, 0, {0, 0}, "Transmited packets dropped due to Tx disabled at PON port."},
{0x304E, "DnVTTMis", 0, 0, {0, 0}, "Downstream packets dropped due to  mismatch."},
{0x304F, "DnTagDrop", 0, 0, {0, 0}, "Downstream packets dropped not complying with the tag config."},
{0x3050, "RsvMacDrop", 0, 0, {0, 0}, "Reserved mac packets dropped due to config in related registers."},
{0x3051, "NonUniDrop", 0, 0, {0, 0}, "Upstream packets dropped due to non-unicast SA ."},
{0x3052, "UserMACDrop", 0, 0, {0, 0}, "Upstream packets dropped due to DA+VID match in ARL table."},
{0x3053, "PonLearnDrop", 0, 0, {0, 0}, "Downstream packets dropped for ARL table is full or SA+VID mismatch."},
{0x3054, "GeLearnDrop", 0, 0, {0, 0}, "Upstream packets dropped for ARL table is full or SA+VID mismatch."},
{0x3055, "PonLernColDrop", 0, 0, {0, 0}, "Ds packets dropped for ARL learn conflict."},
{0x3056, "GeLernColDrop", 0, 0, {0, 0}, "Us packets dropped for ARL learn conflict."},
{0x3057, "DsNUniDrop", 0, 0, {0, 0}, "Ds packets dropped for non-unicast SA."},
{0x3058, "Ds0DADrop", 0, 0, {0, 0}, "Ds packets dropped caused by DA is 0 and learn conflict disabled."},
{0x3059, "Us0DADrop", 0, 0, {0, 0}, "Us packets dropped caused by DA is 0 and learn conflict disabled."},
};

OPL_CNT_t g_astCntMpcp[CNT_MPCP_NUM]={
{0x4012, "NormGate", 0, 0, {0, 0}, "Normal GATE packet counter rx."},
{0x4013, "DiscPkt", 0, 0, {0, 0}, "Register packet counter rx."},
{0x4014, "DROP", 0, 0, {0, 0}, "MPCP packet drop counter."},
{0x4015, "CLEPkt", 0, 0, {0, 0}, "CLE(traffic) packet counter."},
{0x4016, "CLEDrop", 0, 0, {0, 0}, "CLE(traffic) packet drop counter."},
{0x4017, "DiscGate", 0, 0, {0, 0}, "Discovery gate(broadcast and unicast) rx."},
{0x4018, "Grant", 0, 0, {0, 0}, "Grant rx."},
{0x4019, "GntDrop", 0, 0, {0, 0}, "Grant drop."},
{0x401A, "ForceRpt", 0, 0, {0, 0}, "Force report grant rx."},
{0x401B, "RptDrop", 0, 0, {0, 0}, "Force report grant dropped."},
{0x401C, "GntTotal", 0, 0, {0, 0}, "Total grant rx (tq)."},
{0x4020, "USNORPKT", 0, 0, {0, 0}, "Packet of upstream counter."},
{0x4021, "USREGREQ", 0, 0, {0, 0}, "Transmit register request message."},
{0x4022, "USREGACK", 0, 0, {0, 0}, "Transmit acknowledgement message."},
{0x4023, "USRPTPKT", 0, 0, {0, 0}, "Transmit report message."},
{0x4024, "SOPDROP", 0, 0, {0, 0}, "Packets dropped by MPCP_PARSER when packet haven't SOP flag."},
{0x4025, "CLEFullDrop", 0, 0, {0, 0}, "Packet dropped when traffic buffer is FULL."},
};

OPL_CNT_t g_astCntMarb[CNT_MARB_NUM]={
{0x5030, "USUniPage", 0, 0, {0, 0}, "Page number in DDR occupied by upstream unicast packets."},
{0x5031, "DSUniPage", 0, 0, {0, 0}, "Page number in DDR occupied by downstream unicast packets."},
{0x5032, "CPUTxPage", 0, 0, {0, 0}, "Page number in DDR occupied by unicast packets sending from CPU."},
{0x5034, "FETxPage", 0, 0, {0, 0}, "Page number in DDR occupied by the packets received on FE port."},
{0x5035, "MIPSWord", 0, 0, {0, 0}, "Words operated by MIPS Write on DDR interface."},
{0x5040, "FreePage", 0, 0, {0, 0}, "Free pages of data space in DDR RAM."},
};

OPL_CNT_t g_astCntCPDma[CNT_CPDMA_NUM]={
{0x6050, "Rx", 0, 0, {0, 0}, "DMA0 Packets rx."},
{0x6051, "Tx", 0, 0, {0, 0}, "DMA0 Packets tx."},
{0x6052, "GEtoCPU", 0, 0, {0, 0}, "Packets from GE port, rx."},
{0x6053, "GEPolDrop", 0, 0, {0, 0}, "Packets from GE port, dropped due to policy."},
{0x6054, "GEQFullDrop", 0, 0, {0, 0}, "Packets from GE port, dropped due to queue full."},
{0x6055, "PONtoCPU", 0, 0, {0, 0}, "Packets from PON port, rx."},
{0x6056, "PONPolDrop", 0, 0, {0, 0}, "Packets from PON port, dropped due to PON policing."},
{0x6057, "PONQFullDrop", 0, 0, {0, 0}, "Packets from PON port, dropped due to queue full."},
{0x6058, "Dequeue", 0, 0, {0, 0}, "Packets read out from queues."},
{0x6059, "GEFIFOFD", 0, 0, {0, 0}, "Packets dropped due to internal GE FIFO full."},
{0x605A, "PONFIFOFD", 0, 0, {0, 0}, "Packets dropped due to internal PON FIFO full."},
};

OPL_CNT_t g_astCntFEDma[CNT_FEDMA_NUM]={
{0x7008, "Tx", 0, 0, {0, 0}, "DMA1 Packets rx."},
{0x7009, "FEtoCPU", 0, 0, {0, 0}, "DMA1 Packets tx."},
{0x700A, "ErrDrop", 0, 0, {0, 0}, "Packets from FE port, dropped due to errors in the packet."},
{0x700B, "FIFOFD", 0, 0, {0, 0}, "Packets from FE port, dropped due to internal FIFO full."},
{0x700C, "QFullDrop", 0, 0, {0, 0}, "Packets from FE port, dropped due to queue full."},
{0x700D, "Dequeue", 0, 0, {0, 0}, "Packets read out from queue in DDR data space."},
{0x700E, "Rx", 0, 0, {0, 0}, "Packets written into (DMA1 Write) FEDMA_RXBD table."},
};

OPL_CNT_t g_astCntGeParser[CNT_GEPARSER_NUM]={
{0x8005, "RxDrop", 0, 0, {0, 0}, "Received packets dropped due to one or more reasons."},
{0x8006, "FIFOFD", 0, 0, {0, 0}, "Received packets dropped due to internal FIFO full."},
{0x8007, "ErrDrop", 0, 0, {0, 0}, "Received packets dropped due to errors in the packet."},
{0x8008, "TokenDrop", 0, 0, {0, 0}, "Received packets dropped due to token in GE_PARSER not enough."},
{0x800E, "Rx", 0, 0, {0, 0}, "All received packets including error packets."},
{0x800F, "MaxMinDrop ", 0, 0, {0, 0}, "Rreceived packets dropped due to too long or too short."},
};
#if defined(ONU_4PORT_88E6045) || defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097) 
OPL_CNT_t g_astCntSwhPort[SWITCH_PORT_NUM][CNT_SWH_PORT_NUM]={
{
{0, "RxGoodBytes", 0, 0, {0, 0}, "Good packets received."},
{0, "RxBadBytes", 0, 0, {0, 0}, "Packets with crc or alignment error received."},
{0, "TxFCSErr", 0, 0, {0, 0}, "Packets with crc or alignment error transmitted."},
{0, "RxUnicasts", 0, 0, {0, 0}, "Good unicast packets received."},
{0, "TxDeferred", 0, 0, {0, 0}, "Packets transmitted defered."},
{0, "RxBroadcasts", 0, 0, {0, 0}, "Good broadcast packets received."},
{0, "RxMulticasts", 0, 0, {0, 0}, "Good multicast packets received."},
{0, "64Octets", 0, 0, {0, 0}, "64-byte packets (received and transmitted)."},
{0, "127Octets", 0, 0, {0, 0}, "65-127 bytes packets (received and transmitted)."},
{0, "255Octets", 0, 0, {0, 0}, "128-255 bytes packets (received and transmitted)."},
{0, "511Octets", 0, 0, {0, 0}, "256-511 bytes packets (received and transmitted)."},
{0, "1023Octets", 0, 0, {0, 0}, "512-1023 bytes packets (received and transmitted)."},
{0, "maxOctets", 0, 0, {0, 0}, "1024-max bytes packets (received and transmitted)."},
{0, "TxGoodBytes", 0, 0, {0, 0}, "Good packets transmitted."},
{0, "TxUnicasts", 0, 0, {0, 0}, "Good unicast packets transmitted."},
{0, "TxExcessive", 0, 0, {0, 0}, "Excessive packets transmitted."},
{0, "TxMulticasts", 0, 0, {0, 0}, "Good multicast packets transmitted."},
{0, "TxBroadcasts", 0, 0, {0, 0}, "Good broadcast packets transmitted."},
{0, "TxSingle", 0, 0, {0, 0}, "Single packets transmitted."},
{0, "TxPause", 0, 0, {0, 0}, "PAUSE frames transmitted."},
{0, "RxPause", 0, 0, {0, 0}, "PAUSE frames received."},
{0, "TxMultiple", 0, 0, {0, 0}, "Multiple packets transmitted."},
{0, "RxUndersize", 0, 0, {0, 0}, "Packets less than 64 bytes but crc ok received."},
{0, "RxFragments", 0, 0, {0, 0}, "Packets less than 64 bytes and have crc error received."},
{0, "RxOversize", 0, 0, {0, 0}, "Packets longer than MTU bytes but crc ok received."},
{0, "RxJabber", 0, 0, {0, 0}, "Packets longer than MTU and have crc error received."},
{0, "RxMACRcvErr", 0, 0, {0, 0}, "Packets have mac crc error received."},
{0, "RxFCSErr", 0, 0, {0, 0}, "Packets have fcs error received."},
{0, "TxCollisions", 0, 0, {0, 0}, "Collision Packets have transmitted."},
{0, "TxLate", 0, 0, {0, 0}, "Late Packets have transmitted."},
},
};
#else
extern OPL_CNT_t g_astCntSwhPort[SWITCH_PORT_NUM][CNT_SWH_PORT_NUM];
#endif

OPL_CNT_t g_astCntCleHit[CNT_CLEHIT_NUM];

UINT32 g_uiCntPollFlag = 0xFFFFFFFF;

/*TIMER_UNIT_t g_stStatsTimer ={0,0,1};*/

#if defined(ONU_4PORT_88E6045) || defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097) 
extern GT_QD_DEV *dev;
#endif

VOS_SEM_t g_pstStatsSem;

static VOS_THREAD_t g_pstStatsThreadId = NULL;


#define  ShowCntTitle( iWrFd,  uiMode)\
	if(CLI_MODE_SUPER == uiMode) \
	{ \
		vosPrintf(iWrFd, "\r\n%04s %-15s %-20s %s\r\n","ID","Name","Value","Help");\
	} \
	else \
	{ \
		vosPrintf(iWrFd, "\r\n%-15s %-20s %s\r\n","Name","Value","Help");\
	}


#define ShowCntValues( l_NameA, l_Cnt ) \
	for (ucCnt=0; ucCnt<l_Cnt; ucCnt++)	\
	{ \
		OPL_MEMSET(buffer, 0, 100); \
		sprintf(buffer,"%llu", l_NameA[ucCnt].stAdded); \
		if(CLI_MODE_SUPER == uiMode) \
		{	\
			vosPrintf(iWrFd, "0x%04x %-15s %-20s %s\r\n", \
							l_NameA[ucCnt].usId, l_NameA[ucCnt].pucName, \
							buffer, \
							l_NameA[ucCnt].pucHelp);\
		}\
		else\
		{\
			vosPrintf(iWrFd, "%-15s %-20s %s\r\n", \
							l_NameA[ucCnt].pucName, \
							buffer, \
							l_NameA[ucCnt].pucHelp); \
		}  \
	}


OPL_STATUS ShowCntGe(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nGe Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntGe, CNT_GE_NUM )


	return iStatus;
}

OPL_STATUS ShowCntPon(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nPon Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntPon, CNT_PON_NUM )


	return iStatus;



	return iStatus;
}
OPL_STATUS ShowCntFe(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nFe Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntFe, CNT_FE_NUM )

	return iStatus;
}

OPL_STATUS ShowCntTm(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nTm Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntTm, CNT_TM_NUM )

	return iStatus;
}

OPL_STATUS ShowCntCle(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nCle Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntCle, CNT_CLE_NUM )

	return iStatus;
}

OPL_STATUS ShowCntBrg(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nBrg Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntBrg, CNT_BRG_NUM )

	return iStatus;
}

OPL_STATUS ShowCntMpcp(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nMarb Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntMpcp, CNT_MPCP_NUM )

	return iStatus;
}

OPL_STATUS ShowCntMarb(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nMarb Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntMarb, CNT_MARB_NUM )

	return iStatus;
}

OPL_STATUS ShowCntCpdma(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nCpdma Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntCPDma, CNT_CPDMA_NUM )

	return iStatus;
}

OPL_STATUS ShowCntFedma(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nFedma Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntFEDma, CNT_FEDMA_NUM )

	return iStatus;
}

OPL_STATUS ShowCntGeParser(int iWrFd, unsigned int uiMode)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nGeParser Counter");


	ShowCntTitle( iWrFd, uiMode )
	ShowCntValues( g_astCntGeParser, CNT_GEPARSER_NUM )

	return iStatus;
}

OPL_STATUS ShowCntCleHit(int iWrFd, UINT8 ucStart, UINT8 ucNum)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
	UINT32 uiRegVal;
 	unsigned char buffer[100];	

	vosPrintf(iWrFd, "\r\nCle hit Counter");
	vosPrintf(iWrFd, "\r\n%-6s %-20s\r\n","RuleID","Value");

	for (ucCnt=0; ucCnt<ucNum; ucCnt++)
	{
		OPL_MEMSET(buffer, 0, 100); 
		sprintf(buffer,"%llu", g_astCntCleHit[ucCnt+ucStart].stAdded);
		vosPrintf(iWrFd, "%-6d %-20s \r\n",
				ucCnt+ucStart, buffer); 

	}

	return iStatus;
}

OPL_STATUS ShowCntSwhPort(int iWrFd, UINT8 ucStart, UINT8 ucNum, int iType)
{
	OPL_STATUS iStatus=OPL_OK;
 	unsigned char buffer[100];	

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097)
	UINT8	ucCnt, ucPortId, ucCntShow;
	GT_STATS_COUNTER_SET3	stPortCounter;
	UINT32	*puiCounter=(UINT32 *)&stPortCounter;

	vosPrintf(iWrFd, "\r\nSwitch Port Counter");
	vosPrintf(iWrFd, "\r\n%-15s","Name");
	for(ucPortId=ucStart; ucPortId<ucStart+ucNum; ucPortId++)
	{
			OPL_MEMSET(buffer, 0, 100);
			sprintf(buffer,"(%1d)Value", ucPortId );
			vosPrintf(iWrFd, "%-20s", buffer);
	}
	for(ucCnt=0; ucCnt<CNT_SWH_PORT_NUM; ucCnt++)
	{
			vosPrintf(iWrFd, "\r\n%-15s ", g_astCntSwhPort[0][ucCnt].pucName);
			for(ucPortId=ucStart-1; ucPortId<ucStart-1+ucNum; ucPortId++)
			{
				OPL_MEMSET(buffer, 0, 100);
				sprintf(buffer,"%llu", g_astCntSwhPort[ucPortId][ucCnt].stAdded);
				vosPrintf(iWrFd, "%-20s", buffer);
			}
	}
	vosPrintf(iWrFd, "\r\n");
#elif defined(ONU_4PORT_AR8306) \
	|| defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
   stats_showArx8306( iWrFd, ucStart, ucNum );	
#else
	vosPrintf(iWrFd, "\r\nTHIS COMMAND SUPPORT ONLY MULTIPORT PRODUCT!\r\n");
#endif

	return iStatus;
}

void statsLock()
{
	
    vosSemTake(&g_pstStatsSem);
}

void statsUnLock()
{
    vosSemGive(&g_pstStatsSem);
}


OPL_STATUS CntClr(int iModule, UINT8 ucStart, UINT8 ucNum )
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt, ucPortId;

    statsLock();

	switch(iModule)
	{
		case CNT_GE:
			for (ucCnt=0; ucCnt<CNT_GE_NUM; ucCnt++)
			{
				g_astCntGe[ucCnt].uiTotal = 0;
				g_astCntGe[ucCnt].uiCurrent = 0;
				g_astCntGe[ucCnt].stAdded.uiLow = 0;
				g_astCntGe[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_PON:
			for (ucCnt=0; ucCnt<CNT_PON_NUM; ucCnt++)
			{
				g_astCntPon[ucCnt].uiTotal = 0;
				g_astCntPon[ucCnt].uiCurrent = 0;
				g_astCntPon[ucCnt].stAdded.uiLow = 0;
				g_astCntPon[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_FE:
			for (ucCnt=0; ucCnt<CNT_FE_NUM; ucCnt++)
			{
				g_astCntFe[ucCnt].uiTotal = 0;
				g_astCntFe[ucCnt].uiCurrent = 0;
				g_astCntFe[ucCnt].stAdded.uiLow = 0;
				g_astCntFe[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_TM:
			for (ucCnt=0; ucCnt<CNT_TM_NUM; ucCnt++)
			{
				g_astCntTm[ucCnt].uiTotal = 0;
				g_astCntTm[ucCnt].uiCurrent = 0;
				g_astCntTm[ucCnt].stAdded.uiLow = 0;
				g_astCntTm[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_CLE:
			for (ucCnt=0; ucCnt<CNT_CLE_NUM; ucCnt++)
			{
				g_astCntCle[ucCnt].uiTotal = 0;
				g_astCntCle[ucCnt].uiCurrent = 0;
				g_astCntCle[ucCnt].stAdded.uiLow = 0;
				g_astCntCle[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_BRG:
			for (ucCnt=0; ucCnt<CNT_BRG_NUM; ucCnt++)
			{
				g_astCntBrg[ucCnt].uiTotal = 0;
				g_astCntBrg[ucCnt].uiCurrent = 0;
				g_astCntBrg[ucCnt].stAdded.uiLow = 0;
				g_astCntBrg[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_MPCP:
			for (ucCnt=0; ucCnt<CNT_MPCP_NUM; ucCnt++)
			{
				g_astCntMpcp[ucCnt].uiTotal = 0;
				g_astCntMpcp[ucCnt].uiCurrent = 0;
				g_astCntMpcp[ucCnt].stAdded.uiLow = 0;
				g_astCntMpcp[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_MARB:
			for (ucCnt=0; ucCnt<CNT_MARB_NUM; ucCnt++)
			{
				g_astCntMarb[ucCnt].uiTotal = 0;
				g_astCntMarb[ucCnt].uiCurrent = 0;
				g_astCntMarb[ucCnt].stAdded.uiLow = 0;
				g_astCntMarb[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_CPDMA:
			for (ucCnt=0; ucCnt<CNT_CPDMA_NUM; ucCnt++)
			{
				g_astCntCPDma[ucCnt].uiTotal = 0;
				g_astCntCPDma[ucCnt].uiCurrent = 0;
				g_astCntCPDma[ucCnt].stAdded.uiLow = 0;
				g_astCntCPDma[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_FEDMA:
			for (ucCnt=0; ucCnt<CNT_FEDMA_NUM; ucCnt++)
			{
				g_astCntFEDma[ucCnt].uiTotal = 0;
				g_astCntFEDma[ucCnt].uiCurrent = 0;
				g_astCntFEDma[ucCnt].stAdded.uiLow = 0;
				g_astCntFEDma[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_GEPARSER:
			for (ucCnt=0; ucCnt<CNT_GEPARSER_NUM; ucCnt++)
			{
				g_astCntGeParser[ucCnt].uiTotal = 0;
				g_astCntGeParser[ucCnt].uiCurrent = 0;
				g_astCntGeParser[ucCnt].stAdded.uiLow = 0;
				g_astCntGeParser[ucCnt].stAdded.uiHigh = 0;
			}
			break;
		case CNT_CLEHIT:
			for (ucCnt=0; ucCnt<CNT_CLEHIT_NUM; ucCnt++)
			{
				g_astCntCleHit[ucCnt].uiTotal = 0;
				g_astCntCleHit[ucCnt].uiCurrent = 0;
				g_astCntCleHit[ucCnt].stAdded.uiLow = 0;
				g_astCntCleHit[ucCnt].stAdded.uiHigh = 0;
			}
			break;
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097)
		case CNT_SWITCH:
			for(ucPortId= ucStart; ucPortId < ucStart + ucNum; ucPortId++)
			{
				if (ucPortId > SWITCH_PORT_NUM)
				{
					break;
				}
				
		#if defined(ONU_4PORT_88E6097)		
				gstatsFlushPort(dev,ucPortId - 1);
		#endif
				for (ucCnt=0; ucCnt<CNT_SWH_PORT_NUM; ucCnt++)
				{
					g_astCntSwhPort[ucPortId][ucCnt].uiTotal = 0;
					g_astCntSwhPort[ucPortId][ucCnt].uiCurrent = 0;
					g_astCntSwhPort[ucPortId][ucCnt].stAdded.uiLow = 0;
					g_astCntSwhPort[ucPortId][ucCnt].stAdded.uiHigh = 0;
				}
			}
			break;
#else
#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
		case CNT_SWITCH:
			for(ucPortId=ucStart; ucPortId<ucStart+ucNum; ucPortId++)
            {         
                if(ucPortId>SWITCH_PORT_NUM)
                    break;
				stats_clearArx8306( ucPortId );
            }

#endif
#endif
		default:
			break;
	}

    statsUnLock();

	return iStatus;
}

void PollingHandler(void *pvInParams)
{
	OPL_STATUS iStatus=OPL_OK;
	UINT8 ucCnt;
	UINT32 uiRegVal;

#if defined(ONU_4PORT_88E6045) || defined(ONU_4PORT_88E6046) || defined(ONU_4PORT_88E6097)
	UINT8	ucPortId, ucCntShow;
	GT_STATS_COUNTER_SET3	stPortCounter;
	UINT32	*puiCounter=(UINT32 *)&stPortCounter;
#endif

    while (1)
    {
        statsLock();

    	if (g_uiCntPollFlag & POLL_GE_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_GE_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntGe[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntGe[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntGe[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntGe[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntGe[ucCnt].stAdded.uiHigh += 1;
    				g_astCntGe[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntGe[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntGe[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_PON_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_PON_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntPon[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntPon[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntPon[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntPon[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntPon[ucCnt].stAdded.uiHigh += 1;
    				g_astCntPon[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntPon[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntPon[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_FE_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_FE_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntFe[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntFe[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntFe[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntFe[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntFe[ucCnt].stAdded.uiHigh += 1;
    				g_astCntFe[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntFe[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntFe[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_TM_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_TM_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntTm[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntTm[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntTm[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntTm[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntTm[ucCnt].stAdded.uiHigh += 1;
    				g_astCntTm[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntTm[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntTm[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_CLE_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_CLE_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntCle[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntCle[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntCle[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntCle[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntCle[ucCnt].stAdded.uiHigh += 1;
    				g_astCntCle[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntCle[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntCle[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_BRG_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_BRG_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntBrg[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntBrg[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntBrg[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntBrg[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntBrg[ucCnt].stAdded.uiHigh += 1;
    				g_astCntBrg[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntBrg[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntBrg[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_MPCP_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_MPCP_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntMpcp[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntMpcp[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntMpcp[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntMpcp[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntMpcp[ucCnt].stAdded.uiHigh += 1;
    				g_astCntMpcp[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntMpcp[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntMpcp[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_MARB_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_MARB_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntMarb[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntMarb[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntMarb[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntMarb[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntMarb[ucCnt].stAdded.uiHigh += 1;
    				g_astCntMarb[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntMarb[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntMarb[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_CPDMA_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_CPDMA_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntCPDma[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntCPDma[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntCPDma[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntCPDma[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntCPDma[ucCnt].stAdded.uiHigh += 1;
    				g_astCntCPDma[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntCPDma[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntCPDma[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_FEDMA_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_FEDMA_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntFEDma[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntFEDma[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntFEDma[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntFEDma[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntFEDma[ucCnt].stAdded.uiHigh += 1;
    				g_astCntFEDma[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntFEDma[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntFEDma[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_GEPARSER_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_GEPARSER_NUM; ucCnt++)
    		{
    			iStatus = oplRegRead(g_astCntGeParser[ucCnt].usId*4, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntGeParser[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntGeParser[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntGeParser[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntGeParser[ucCnt].stAdded.uiHigh += 1;
    				g_astCntGeParser[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntGeParser[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntGeParser[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
    	if (g_uiCntPollFlag & POLL_CLEHIT_MASK)
    	{
    		for (ucCnt=0; ucCnt<CNT_CLEHIT_NUM; ucCnt++)
    		{
    			iStatus = oplTabRead(OPCONN_TAB_CLS_COUNTER, ucCnt, 1, &uiRegVal);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			if ((0xFFFFFFFF-g_astCntCleHit[ucCnt].stAdded.uiLow)>=uiRegVal)
    			{
    				g_astCntCleHit[ucCnt].stAdded.uiLow += uiRegVal;
    			}
    			else if (0xFFFFFFFF != g_astCntCleHit[ucCnt].stAdded.uiHigh)
    			{
    				g_astCntCleHit[ucCnt].stAdded.uiHigh += 1;
    				g_astCntCleHit[ucCnt].stAdded.uiLow = uiRegVal - (0xFFFFFFFF-g_astCntCleHit[ucCnt].stAdded.uiLow) - 1;
    			}
    			else
    			{
    				g_astCntCleHit[ucCnt].stAdded.uiLow = 0xFFFFFFFF;
    			}
    		}
    	}
#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)
    	if (g_uiCntPollFlag & POLL_SWITCH_MASK)
    	{
    		for(ucPortId=0; ucPortId<SWITCH_PORT_NUM; ucPortId++)
    		{
    			iStatus = gstatsGetPortAllCounters3(dev, ucPortId, &stPortCounter);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			for(ucCnt=0, ucCntShow=0; ucCnt<CNT_SWH_PORT_NUM+2; ucCnt++, ucCntShow++)
    			{
    				if ((1==ucCnt)||(15==ucCnt))
    				{
    					ucCntShow--;
    					continue;
    				}
    				if ((0xFFFFFFFF-g_astCntSwhPort[ucPortId][ucCntShow].stAdded.uiLow)>=puiCounter[ucCnt])
    				{
    					g_astCntSwhPort[ucPortId][ucCntShow].stAdded.uiLow += puiCounter[ucCnt];
    				}
    				else if (0xFFFFFFFF != g_astCntSwhPort[ucPortId][ucCntShow].stAdded.uiHigh)
    				{
    					g_astCntSwhPort[ucPortId][ucCntShow].stAdded.uiHigh += 1;
    					g_astCntSwhPort[ucPortId][ucCntShow].stAdded.uiLow = 
    						puiCounter[ucCnt] - (0xFFFFFFFF-g_astCntSwhPort[ucPortId][ucCntShow].stAdded.uiLow) - 1;
    				}
    				else
    				{
    					g_astCntSwhPort[ucPortId][ucCntShow].stAdded.uiLow = 0xFFFFFFFF;
    				}
    			}
    		}
    	}
#endif

#if  defined(ONU_4PORT_88E6097)
    	if (g_uiCntPollFlag & POLL_SWITCH_MASK)
    	{
    		for(ucPortId=0; ucPortId<SWITCH_PORT_NUM; ucPortId++)
    		{
    			iStatus = gstatsGetPortAllCounters3(dev, ucPortId, &stPortCounter);
    			if (OPL_OK != iStatus)
    			{
    				continue;
    			}
    			for(ucCnt=0, ucCntShow=0; ucCnt<CNT_SWH_PORT_NUM+2; ucCnt++, ucCntShow++)
    			{
    				if ((1==ucCnt)||(15==ucCnt))
    				{
    					ucCntShow--;
    					g_astCntSwhPort[ucPortId][ucCntShow].stAdded.uiHigh = puiCounter[ucCnt];
    				}
    				else
    				{
    					g_astCntSwhPort[ucPortId][ucCntShow].stAdded.uiLow = puiCounter[ucCnt];
    				}
    			}
    		}
    	}
#endif

#if !defined(SWITCH_TEST)
        /* added by zgan - 2009/10/22, test AR8228 reset issue  */
#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
    		UINT8	ucPortId;

    		if (g_uiCntPollFlag & POLL_SWITCH_MASK)
    			for(ucPortId=0; ucPortId<SWITCH_PORT_NUM; ucPortId++)		
    				stats_getArx8306( ucPortId );
#endif
#endif

        statsUnLock();
        vosSleep(1);
    }
    vosThreadExit(0);
	return;
}

OPL_STATUS odmStatsThreadInit(void)
{
    if (g_pstStatsThreadId) {
        return OPL_ERROR;
    }

	if (VOS_SEM_OK != vosSemCreate(&g_pstStatsSem, 0, 1))
	{
	    printf("Statistics lock initialize failed.\n");
	    return;
	}
    g_pstStatsThreadId = vosThreadCreate("tStats", OP_VOS_THREAD_STKSZ, 70,
                        (void *)PollingHandler, (void *)NULL);
    if (g_pstStatsThreadId == NULL) {
        return OPL_ERROR;
    }

    return OPL_OK;
}

OPL_STATUS odmStatsThreadShutdown(void)
{
    if (!g_pstStatsThreadId) {
        return OPL_ERROR;
    }

    if (g_pstStatsThreadId != NULL) {
        vosThreadDestroy(g_pstStatsThreadId);
        g_pstStatsThreadId = NULL;
    }

    return OPL_OK;
}

UINT32 odmPONPortStatisticsGet1(ONU_PON_Port_Statistics_Get_1_t * ponStats)
{
	statsLock();

	ponStats->FramesTransmittedOK = (UINT64)g_astCntPon[2].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[2].stAdded.uiHigh << 32);
	ponStats->OctetsTransmittedOK = (UINT64)g_astCntPon[13].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[13].stAdded.uiHigh << 32);
	ponStats->MulticastFramesXmittedOK = (UINT64)g_astCntPon[4].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[4].stAdded.uiHigh << 32);
	ponStats->BroadcastFramesXmittedOK = (UINT64)g_astCntPon[3].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[3].stAdded.uiHigh << 32);
	ponStats->PONPauseFramesTransmittedOk = (UINT64)g_astCntPon[5].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[5].stAdded.uiHigh << 32);

	ponStats->FramesReceivedOK = (UINT64)g_astCntPon[16].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[16].stAdded.uiHigh << 32);
	ponStats->OctetsReceivedOK = (UINT64)g_astCntPon[37].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[37].stAdded.uiHigh << 32);
	ponStats->MulticastFramesReceivedOK = (UINT64)g_astCntPon[24].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[24].stAdded.uiHigh << 32);
	ponStats->BroadcastFramesReceivedOK = (UINT64)g_astCntPon[23].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[23].stAdded.uiHigh << 32);
	ponStats->PONPauseFramesReceivedOk = (UINT64)g_astCntPon[25].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[25].stAdded.uiHigh << 32);
	ponStats->SingleCollisionFrames = 0;
	ponStats->MultipleCollisionFrames = 0;
	ponStats->AlignmentErrors = (UINT64)g_astCntPon[18].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[18].stAdded.uiHigh << 32);
	ponStats->FrameTooLongErrors = (UINT64)g_astCntPon[21].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[21].stAdded.uiHigh << 32);

	statsUnLock();

	return OK;
}

UINT32 odmPONPortStatisticsGet2(ONU_PON_Port_Statistics_Get_2_t * ponStats)
{
	statsLock();

	ponStats->CRC8Errors = (UINT64)g_astCntPon[18].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[18].stAdded.uiHigh << 32);
	ponStats->FECCorrectedBlocks = (UINT64)g_astCntPon[38].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[38].stAdded.uiHigh << 32);
	ponStats->FECUncorrectableBlocks = (UINT64)g_astCntPon[39].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[39].stAdded.uiHigh << 32);
	ponStats->MPCPMACCtrlFramesTransmitted = (UINT64)g_astCntPon[6].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[6].stAdded.uiHigh << 32);
	ponStats->MPCPMACCtrlFramesReceived = (UINT64)g_astCntPon[27].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntPon[27].stAdded.uiHigh << 32);

	ponStats->MPCPTxGate = 0;
	ponStats->MPCPTxRegAck = (UINT64)g_astCntMpcp[13].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[13].stAdded.uiHigh << 32);
	ponStats->MPCPTxRegister = 0; /* OLT·¢µÄ */
	ponStats->MPCPTxRegRequest = (UINT64)g_astCntMpcp[12].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[12].stAdded.uiHigh << 32);
	ponStats->MPCPTxReport = (UINT64)g_astCntMpcp[14].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[14].stAdded.uiHigh << 32);
	ponStats->MPCPRxGate = (UINT64)g_astCntMpcp[0].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[0].stAdded.uiHigh << 32);
	ponStats->MPCPRxRegAck = 0;
	ponStats->MPCPRxRegister = (UINT64)g_astCntMpcp[1].stAdded.uiLow + 
		(UINT64)((UINT64)g_astCntMpcp[1].stAdded.uiHigh << 32);
	ponStats->MPCPRxRegRequest = 0;
	ponStats->MPCPRxReport = 0;

	statsUnLock();

	return OK;
}

UINT32 odmPortFluxStatisticsCounterGet(ONU_Port_Flux_Statistics_Counter_t *portStats)
{
	statsLock();

	portStats->ifInOctets =
	    (UINT64)g_astCntGe[34].stAdded.uiLow +
	    (UINT64)((UINT64)g_astCntGe[34].stAdded.uiHigh << 32);
	portStats->ifInUcastPkts =
        (UINT64)g_astCntGe[13].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[13].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[20].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[20].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[21].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[21].stAdded.uiHigh << 32);
	portStats->ifInNUcastPkts =
        (UINT64)g_astCntGe[20].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[20].stAdded.uiHigh << 32) +
        (UINT64)g_astCntGe[21].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[21].stAdded.uiHigh << 32);
	portStats->ifInDiscards =
        (UINT64)g_astCntGe[14].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[14].stAdded.uiHigh << 32);
	portStats->ifInErrors =
        (UINT64)g_astCntGe[19].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[19].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[13].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[13].stAdded.uiHigh << 32);

    portStats->ifOutOctets =
        (UINT64)g_astCntGe[11].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[11].stAdded.uiHigh << 32);
	portStats->ifOutUcastPkts =
        (UINT64)g_astCntGe[0].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[0].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[1].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[1].stAdded.uiHigh << 32) -
        (UINT64)g_astCntGe[2].stAdded.uiLow -
        (UINT64)((UINT64)g_astCntGe[2].stAdded.uiHigh << 32);
	portStats->ifOutNUcastPkts =
        (UINT64)g_astCntGe[1].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[1].stAdded.uiHigh << 32) +
        (UINT64)g_astCntGe[2].stAdded.uiLow +
        (UINT64)((UINT64)g_astCntGe[2].stAdded.uiHigh << 32);
	portStats->ifOutDiscards = 0;
	portStats->ifOutErrors = 0;

	statsUnLock();

	return OK;
}



OPL_STATUS odmPonSinglePortStatsGet(UINT8 port_id, port_statistics_t *portStats)
{

#if defined(ONU_4PORT_88E6045)||defined(ONU_4PORT_88E6046)||defined(ONU_4PORT_88E6097)
	portStats->OctetsTx = 105;
	portStats->OctetsTxRate = 0;
	portStats->OctetsTxTopRate = 0;
	portStats->UnicastFramesTx = 106;
	portStats->NonUnicastFramesTx = 107;
	portStats->FrameTxRate = 0;
	portStats->TxPacketsError = 109;

	portStats->OctetsRx = 100;
	portStats->OctetsRxRate = 0;
	portStats->OctetsRxTopRate = 0;
	portStats->UnicastFramesRx = 101;
	portStats->NonUnicastFramesRx = 102;
	portStats->FrameRxRate = 0;
	portStats->RxPacketsError = 104;
#elif defined(ONU_4PORT_AR8306)||defined(ONU_4PORT_AR8228)||defined(ONU_4PORT_AR8327)
	portStats->OctetsTx = (UINT64)((UINT64)g_astCntSwhPort[port_id][31].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][31].stAdded.uiLow;

	/* Total good frames tranmitted with a broadcast Destination address  */
	portStats->OctetsTxRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][19].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][19].stAdded.uiLow;

	/* Total good frames transmitted with a multicast Destination address */
	portStats->OctetsTxTopRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][21].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][21].stAdded.uiLow;

	portStats->UnicastFramesTx = (UINT64)((UINT64)g_astCntSwhPort[port_id][23].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][23].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][24].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][24].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][25].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][25].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][26].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][26].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][27].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][27].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][28].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][28].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][29].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][29].stAdded.uiLow -
					(
						(UINT64)((UINT64)g_astCntSwhPort[port_id][19].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][19].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][21].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][21].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][30].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][30].stAdded.uiLow
					);

	portStats->NonUnicastFramesTx = (UINT64)((UINT64)g_astCntSwhPort[port_id][19].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][19].stAdded.uiLow +
					(UINT64)((UINT64)g_astCntSwhPort[port_id][21].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][21].stAdded.uiLow;

	/* Total good PAUSE frames transmitted */
	portStats->FrameTxRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][20].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][20].stAdded.uiLow;

	portStats->TxPacketsError = (UINT64)((UINT64)g_astCntSwhPort[port_id][30].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][30].stAdded.uiLow;

	portStats->OctetsRx = (UINT64)((UINT64)g_astCntSwhPort[port_id][15].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][15].stAdded.uiLow
					+ (UINT64)((UINT64)g_astCntSwhPort[port_id][16].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][16].stAdded.uiLow;

	/* The number of good broadcast frames received */
	portStats->OctetsRxRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][0].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][0].stAdded.uiLow;

	/* The number of good multicast frames received. */
	portStats->OctetsRxTopRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][2].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][2].stAdded.uiLow;

	portStats->UnicastFramesRx = (UINT64)((UINT64)g_astCntSwhPort[port_id][5].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][5].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][6].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][6].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][7].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][7].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][8].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][8].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][9].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][9].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][10].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][10].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][11].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][11].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][12].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][12].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][13].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][13].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][14].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][14].stAdded.uiLow -
					(
						(UINT64)((UINT64)g_astCntSwhPort[port_id][0].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][0].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][2].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][2].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][3].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][3].stAdded.uiLow +
						(UINT64)((UINT64)g_astCntSwhPort[port_id][4].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][4].stAdded.uiLow
					);

	portStats->NonUnicastFramesRx = (UINT64)((UINT64)g_astCntSwhPort[port_id][0].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][0].stAdded.uiLow
							+ (UINT64)((UINT64)g_astCntSwhPort[port_id][2].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][2].stAdded.uiLow;

	/* The number of PAUSE frames received */
	portStats->FrameRxRate = (UINT64)((UINT64)g_astCntSwhPort[port_id][1].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][1].stAdded.uiLow;

	portStats->RxPacketsError = (UINT64)((UINT64)g_astCntSwhPort[port_id][3].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][3].stAdded.uiLow
					+ (UINT64)((UINT64)g_astCntSwhPort[port_id][4].stAdded.uiHigh << 32) + (UINT64)g_astCntSwhPort[port_id][4].stAdded.uiLow;
#else
	portStats->OctetsTx = (UINT64)((UINT64)g_astCntGe[11].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[11].stAdded.uiLow;
	portStats->OctetsTxRate = 0;
	portStats->OctetsTxTopRate = 0;
	portStats->UnicastFramesTx = (UINT64)((UINT64)g_astCntGe[0].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[0].stAdded.uiLow
						- (UINT64)((UINT64)g_astCntGe[1].stAdded.uiHigh << 32) - (UINT64)g_astCntGe[1].stAdded.uiLow
						- (UINT64)((UINT64)g_astCntGe[2].stAdded.uiHigh << 32) - (UINT64)g_astCntGe[2].stAdded.uiLow;
	portStats->NonUnicastFramesTx = (UINT64)((UINT64)g_astCntGe[1].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[1].stAdded.uiLow
						+ (UINT64)((UINT64)g_astCntGe[2].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[2].stAdded.uiLow;
	portStats->FrameTxRate = 0;
	portStats->TxPacketsError = 0;

	portStats->OctetsRx = (UINT64)((UINT64)g_astCntGe[34].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[34].stAdded.uiLow;
	portStats->OctetsRxRate = 0;
	portStats->OctetsRxTopRate = 0;
	portStats->UnicastFramesRx = (UINT64)((UINT64)g_astCntGe[13].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[13].stAdded.uiLow
		- (
		(UINT64)((UINT64)g_astCntGe[20].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[20].stAdded.uiLow +
		(UINT64)((UINT64)g_astCntGe[21].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[21].stAdded.uiLow
		);
	portStats->NonUnicastFramesRx = (UINT64)((UINT64)g_astCntGe[20].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[20].stAdded.uiLow +
		(UINT64)((UINT64)g_astCntGe[21].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[21].stAdded.uiLow;
	portStats->FrameRxRate = 0;
	portStats->RxPacketsError = (UINT64)((UINT64)g_astCntGe[19].stAdded.uiHigh << 32) + (UINT64)g_astCntGe[19].stAdded.uiLow -
		(UINT64)((UINT64)g_astCntGe[13].stAdded.uiHigh << 32) - (UINT64)g_astCntGe[13].stAdded.uiLow;
#endif

	return OK;
}

OPL_STATUS odmPonStatsGet(pon_statistics_t * ponStats)
{
	UINT8 Index;

	if (ponStats == NULL){
		return ERROR;
	}

    statsLock();

	/* get pon statistics information */
	ponStats->OctetsTx = g_astCntPon[13].stAdded.uiLow;

	/* Good broadcast packets transmitted.*/
	ponStats->OctetsTxRate = g_astCntPon[3].stAdded.uiLow;

	/* Good multicast packets transmitted */
	ponStats->OctetsTxTopRate = g_astCntPon[4].stAdded.uiLow;

	ponStats->FrameTx = g_astCntPon[2].stAdded.uiLow;

	/* OAM packets transmitted */
	ponStats->FrameTxRate = g_astCntPon[15].stAdded.uiLow;

	ponStats->TxPacketsError = 0;

	ponStats->OctetsRx = g_astCntPon[37].stAdded.uiLow;

	/* Good broadcast packets received */
	ponStats->OctetsRxRate = g_astCntPon[23].stAdded.uiLow;

	/* Good multicast packets received */
	ponStats->OctetsRxTopRate = g_astCntPon[24].stAdded.uiLow;

	ponStats->FrameRx = g_astCntPon[22].stAdded.uiLow;

	/* OAM packets received */
	ponStats->FrameRxRate = g_astCntPon[42].stAdded.uiLow;

	ponStats->RxLosePackets = g_astCntPon[17].stAdded.uiLow;

	if (0 != g_astCntPon[22].stAdded.uiLow)
	{
		ponStats->RxLosePacketsRate = g_astCntPon[17].stAdded.uiLow / g_astCntPon[22].stAdded.uiLow;
	}
	else
	{
		ponStats->RxLosePacketsRate = 0;
	}

    statsUnLock();
	return OK;
}


