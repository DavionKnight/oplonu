#include <configs/opipmux.h>
#include "ipmux_counter_sdram.h"
/*
#ifndef DEBUG_IN_WIN32
    #ifndef INCLUDE_TABLE_RW_FUNC   
        #define INCLUDE_TABLE_RW_FUNC 1
    #endif
#endif
*/
#ifdef INCLUDE_SDRAM_COUNTER
    #ifndef INCLUDE_TABLE_RW_FUNC   
        #define INCLUDE_TABLE_RW_FUNC 1
    #endif
#endif

#ifdef INCLUDE_TAB_TEST
    #ifndef INCLUDE_TABLE_RW_FUNC   
        #define INCLUDE_TABLE_RW_FUNC 1
    #endif
#endif

#ifdef INCLUDE_TABLE_RW_FUNC
#define DBGTAB_UPDATE printf
#define IPMUX_ACCESS_PORT_NUM 576
#define DBGCFGTAB //printf("------DEBUG CFGTAB:");printf
#define DBGTAB //printf("______DEBUG TAB:");printf
IPMUX_MOD_TAB_t mod_tab[] = {
    /*  {modid, tab_num,width,ctrl,     data0,      desc } */   
    { 0,    16, 11,     0x20008,        0x2000C,    "Bridge related tables" },
    { 1,    9,  19,     0x180FC,        0x18100,    "Classification related table and Remark tables" },
    { 2,    6,  1,  0x14124,        0x14128,    "Qos and Traffic Management related table, upstream" },
    { 3,    11, 10,     0x14E08,        0x14E0C,    "Qos and Traffic Management related table, downstream" },
    { 4,    16, 8,  0x13C04,        0x13C08,    "AAL5 Assemble, UPPT,EPPT, OAM and SAR" },
    { 5,    2,  1,  0x2B44,         0x2B40,     "DMA0 BDs, Host CPU interface" },
    { 6,    2,  1,  0x2BC4,         0x2BC0,     "DMA1 BDs, Fast Ethernet interface" },
    { 7,    6,  1,  0x10100,        0x10104,    "ATM bonding" },
    /*  { 8,    1,  11,     0x0,        0x0,    "Queue Management" },
    */  { 0,    0,  0,  OPL_ZERO,   OPL_ZERO,   (char *)OPL_NULL }
};


IPMUX_TAB_DESC_t tab_desc[] = {
    { IPMUX_TAB_PORT,           IPMUX_TAB_PORT_LEN,         IPMUX_TAB_PORT_WIDTH,       "Bridge Port table (PTable)" },
    { IPMUX_TAB_ARL,            IPMUX_TAB_ARL_LEN,              IPMUX_TAB_ARL_WIDTH,        "Bridge Address Resolution logic table " },
    { IPMUX_TAB_VLAN_PTR,       IPMUX_TAB_VLAN_PTR_LEN,     IPMUX_TAB_VLAN_PTR_WIDTH,   "Bridge Vlan Pointer Table" },
    { IPMUX_TAB_VLAN_HASH_A,    IPMUX_TAB_VLAN_HASH_A_LEN,  IPMUX_TAB_VLAN_HASH_A_WIDTH,"Bridge Vlan Hash Table A" },
    { IPMUX_TAB_VLAN_HASH_B,    IPMUX_TAB_VLAN_HASH_B_LEN,  IPMUX_TAB_VLAN_HASH_B_WIDTH,"Bridge Vlan Hash Table B" },
    { IPMUX_TAB_VLAN,           IPMUX_TAB_VLAN_LEN,         IPMUX_TAB_VLAN_WIDTH,       "Bridge VLAN table (VTable)" },
    { IPMUX_TAB_VLAN_REP,       IPMUX_TAB_VLAN_REP_LEN,     IPMUX_TAB_VLAN_REP_WIDTH,   "Bridge CTAG replacement table" },
    { IPMUX_TAB_VLAN_MEM,       IPMUX_TAB_VLAN_MEM_LEN,     IPMUX_TAB_VLAN_MEM_WIDTH,   "Bridge VLAN Cross Connection Enable table" },
    { IPMUX_TAB_SACT  ,         IPMUX_TAB_SACT_LEN,         IPMUX_TAB_SACT_WIDTH,       "Bridge MAC Filter Table, also source address control talbe, SACT" },
    { IPMUX_TAB_REMARK,         IPMUX_TAB_REMARK_LEN,       IPMUX_TAB_REMARK_WIDTH,     "Bridge remark table" },
    { IPMUX_TAB_MC_PTR,         IPMUX_TAB_MC_PTR_LEN,       IPMUX_TAB_MC_PTR_WIDTH,     "Bridge MC pointer table, Multicast Group table" },
    { IPMUX_TAB_MC,             IPMUX_TAB_MC_LEN,           IPMUX_TAB_MC_WIDTH,         "Bridge MC Table, Multicast Group member table" },
    { IPMUX_TAB_EGRESS_PORT,    IPMUX_TAB_EGRESS_PORT_LEN,  IPMUX_TAB_EGRESS_PORT_WIDTH,"Bridge Port Egress Contrl table (ECT)" },
    { IPMUX_TAB_LIMIT,          IPMUX_TAB_LIMIT_LEN,        IPMUX_TAB_LIMIT_WIDTH,      "Bridge MAC limitation table" },
    { IPMUX_TAB_LIMIT_CNT,      IPMUX_TAB_LIMIT_CNT_LEN,    IPMUX_TAB_LIMIT_CNT_WIDTH,  "Bridge MAC limitation counter table" },
    { IPMUX_TAB_PRIORITY_TAG,   IPMUX_TAB_PRIORITY_TAG_LEN, IPMUX_TAB_PRIORITY_TAG_WIDTH, "Bridge VLAN priority tag table" },
    
    { IPMUX_TAB_RULE_MASK,      IPMUX_TAB_RULE_MASK_LEN,    IPMUX_TAB_RULE_MASK_WIDTH,  "Classify Rule Mask table" },
    { IPMUX_TAB_RULE_HASH,      IPMUX_TAB_RULE_HASH_LEN,    IPMUX_TAB_RULE_HASH_WIDTH,  "Classify rule hash address table" },
    { IPMUX_TAB_RULE,           IPMUX_TAB_RULE_LEN,             IPMUX_TAB_RULE_WIDTH,       "Classify rule table" },
    { IPMUX_TAB_FLOW_MASK,      IPMUX_TAB_FLOW_MASK_LEN,    IPMUX_TAB_FLOW_MASK_WIDTH,  "Classify Flow ID mask table" },
    { IPMUX_TAB_FLOW_HASH,      IPMUX_TAB_FLOW_HASH_LEN,    IPMUX_TAB_FLOW_HASH_WIDTH,  "Classify flow address pointer (hash) table" },
    { IPMUX_TAB_FLOW,           IPMUX_TAB_FLOW_LEN,         IPMUX_TAB_FLOW_WIDTH,       "Classify Flow table" },
    { IPMUX_TAB_FLOW_RMK,       IPMUX_TAB_FLOW_RMK_LEN,     IPMUX_TAB_FLOW_RMK_WIDTH,   "Classify Flow Remark rule table" },
    { IPMUX_TAB_CLS_CNT_HIT,    IPMUX_TAB_CLS_CNT_HIT_LEN,  IPMUX_TAB_CLS_CNT_HIT_WIDTH,    "Classify Hit Counter table" },
    
    { IPMUX_TAB_US_FSHP,        IPMUX_TAB_US_FSHP_LEN,      IPMUX_TAB_US_FSHP_WIDTH,    "Upstream Flow Shaper Table" },
    { IPMUX_TAB_US_FWRED,       IPMUX_TAB_US_FWRED_LEN,     IPMUX_TAB_US_FWRED_WIDTH,   "Upstream Flow WRED Table" },
    { IPMUX_TAB_US_FLOW_R,      IPMUX_TAB_US_FLOW_R_LEN,    IPMUX_TAB_US_FLOW_R_WIDTH,  "Upstream Flow Table (Read Only)" },
    { IPMUX_TAB_US_FLOW_W,      IPMUX_TAB_US_FLOW_W_LEN,    IPMUX_TAB_US_FLOW_W_WIDTH,  "Upstream Flow Table (Write Only)" },
    { IPMUX_TAB_US_FCNT,        IPMUX_TAB_US_FCNT_LEN,      IPMUX_TAB_US_FCNT_WIDTH,    "Upstream Flow Queue Counter table" },
    { IPMUX_TAB_US_FSTATUS,     IPMUX_TAB_US_FSTATUS_LEN,   IPMUX_TAB_US_FSTATUS_WIDTH, "Upstream Flow Queue Status Table" },
    { IPMUX_TAB_US_FCNT_DROP,   IPMUX_TAB_US_FCNT_DROP_LEN, IPMUX_TAB_US_FCNT_DROP_WIDTH,   "Upstream Flow Rx Packet Drop Counter Table" },
    
    { IPMUX_TAB_DS_FSHP,        IPMUX_TAB_DS_FSHP_LEN,      IPMUX_TAB_DS_FSHP_WIDTH,    "Downstream Flow Shaper Table" },
    { IPMUX_TAB_DS_FWRED,       IPMUX_TAB_DS_FWRED_LEN,     IPMUX_TAB_DS_FWRED_WIDTH,   "Downstream Flow WRED Table" },
    { IPMUX_TAB_DS_FPSEQ,       IPMUX_TAB_DS_FPSEQ_LEN,     IPMUX_TAB_DS_FPSEQ_WIDTH,   "Downstream Flow Port Sequence Table (Read Only)" },
    { IPMUX_TAB_DS_FPWPRI,      IPMUX_TAB_DS_FPWPRI_LEN,    IPMUX_TAB_DS_FPWPRI_WIDTH, "Downstream Port Weight and Priority Table (Read Only)" },
    { IPMUX_TAB_DS_FPWSUM,      IPMUX_TAB_DS_FPWSUM_LEN,    IPMUX_TAB_DS_FPWSUM_WIDTH, "Downstream Port Weight Sum Table" },
    { IPMUX_TAB_DS_FLOW_W,      IPMUX_TAB_DS_FLOW_W_LEN,    IPMUX_TAB_DS_FLOW_W_WIDTH, "Downstream Flow Table (Write Only)" },
    { IPMUX_TAB_DS_PSEQ_R,      IPMUX_TAB_DS_PSEQ_R_LEN,    IPMUX_TAB_DS_PSEQ_R_WIDTH, "Downstream Port Sequence Table (Read Only)" },
    { IPMUX_TAB_DS_FCNT,        IPMUX_TAB_DS_FCNT_LEN,      IPMUX_TAB_DS_FCNT_WIDTH,    "Downstream Flow Queue Counter table" },
    { IPMUX_TAB_DS_FSTATUS,     IPMUX_TAB_DS_FSTATUS_LEN,   IPMUX_TAB_DS_FSTATUS_WIDTH, "Downstream Flow Queue Status Table" },
    { IPMUX_TAB_DS_FCNT_DROP,   IPMUX_TAB_DS_FCNT_DROP_LEN,IPMUX_TAB_DS_FCNT_DROP_WIDTH, "Downstream Flow Rx Packet Drop Counter Table" },
    
    { IPMUX_TAB_UPPT,           IPMUX_TAB_UPPT_LEN,             IPMUX_TAB_UPPT_WIDTH,       "SAR UPPT" },
    { IPMUX_TAB_EPPT,           IPMUX_TAB_EPPT_LEN,             IPMUX_TAB_EPPT_WIDTH,       "SAR EPPT" },
    { IPMUX_TAB_SAR_QS,         IPMUX_TAB_SAR_QS_LEN,       IPMUX_TAB_SAR_QS_WIDTH, "SAR Queue Status Table" },
    { IPMUX_TAB_SAR_CNT_APCTRL,IPMUX_TAB_SAR_CNT_APCTRL_LEN,IPMUX_TAB_SAR_CNT_APCTRL_WIDTH, "SAR Access Port Tx/Rx Counter Control Table" },
    { IPMUX_TAB_SAR_CNT_APGRPRX,IPMUX_TAB_SAR_CNT_APGRPRX_LEN,IPMUX_TAB_SAR_CNT_APGRPRX_WIDTH, "SAR Access Port Rx Counter Group Table" },
    { IPMUX_TAB_SAR_CNT_APGRPTX,IPMUX_TAB_SAR_CNT_APGRPTX_LEN,IPMUX_TAB_SAR_CNT_APGRPTX_WIDTH, "SAR Access Port Tx Counter Group Table" },
    { IPMUX_TAB_SAR_CNT_PVPTR,  IPMUX_TAB_SAR_CNT_PVPTR_LEN,    IPMUX_TAB_SAR_CNT_PVPTR_WIDTH,  "SAR Port VLAN Counter Pointer Table" },
    { IPMUX_TAB_SAR_CNT_PVHASH,IPMUX_TAB_SAR_CNT_PVHASH_LEN,    IPMUX_TAB_SAR_CNT_PVHASH_WIDTH, "SAR Port VLAN Counter Hash Table" },
    { IPMUX_TAB_SAR_CNT_PVIN,   IPMUX_TAB_SAR_CNT_PVIN_LEN, IPMUX_TAB_SAR_CNT_PVIN_WIDTH,       "SAR Port VLAN In Frame Statistics Group Table" },
    { IPMUX_TAB_SAR_CNT_PVOUT,  IPMUX_TAB_SAR_CNT_PVOUT_LEN,    IPMUX_TAB_SAR_CNT_PVOUT_WIDTH,  "SAR Port VLAN Out Frame Statistics Group Table" },
    { IPMUX_TAB_SAR_CNT_VLANPTR,IPMUX_TAB_SAR_CNT_VLANPTR_LEN,IPMUX_TAB_SAR_CNT_VLANPTR_WIDTH,  "SAR GEW VLAN Counter Pointer Table" },
    { IPMUX_TAB_SAR_CNT_VLANHASH,IPMUX_TAB_SAR_CNT_VLANHASH_LEN,IPMUX_TAB_SAR_CNT_VLANHASH_WIDTH, "SAR GEW VLAN Counter Hash Table" },
    { IPMUX_TAB_SAR_CNT_VLANIN, IPMUX_TAB_SAR_CNT_VLANIN_LEN,   IPMUX_TAB_SAR_CNT_VLANIN_WIDTH, "SAR GEW VLAN In Frame Statistics Group Table" },
    { IPMUX_TAB_SAR_CNT_VLANOUT,IPMUX_TAB_SAR_CNT_VLANOUT_LEN,IPMUX_TAB_SAR_CNT_VLANOUT_WIDTH,  "SAR GEW VLAN Out Frame Statistics Group Table" },
    
    { IPMUX_TAB_HOST_RX,        IPMUX_TAB_HOST_RX_LEN,      IPMUX_TAB_HOST_RX_WIDTH,    "HOST Rx BD table, DMA0 BD table" },
    { IPMUX_TAB_HOST_TX,        IPMUX_TAB_HOST_TX_LEN,      IPMUX_TAB_HOST_TX_WIDTH,    "HOST Tx BD table, DMA0 BD table" },
    { IPMUX_TAB_FE_RX,          IPMUX_TAB_FE_RX_LEN,        IPMUX_TAB_FE_RX_WIDTH,      "Fast Ethernet Rx BD table, DMA1 BD table" },
    { IPMUX_TAB_FE_TX,          IPMUX_TAB_FE_TX_LEN,        IPMUX_TAB_FE_TX_WIDTH,      "Fast Ethernet Tx BD table, DMA1 BD table" },
    
    { IPMUX_TAB_ATB_CNT_RXCL,   IPMUX_TAB_ATB_CNT_RXCL_LEN, IPMUX_TAB_ATB_CNT_RXCL_WIDTH,   "Rx cells per Link,except asm" },
    { IPMUX_TAB_ATB_CNT_RXCG,   IPMUX_TAB_ATB_CNT_RXCG_LEN, IPMUX_TAB_ATB_CNT_RXCG_WIDTH,   "Rx cells per Group,except asm" },
    { IPMUX_TAB_ATB_CNT_RXAL,   IPMUX_TAB_ATB_CNT_RXAL_LEN, IPMUX_TAB_ATB_CNT_RXAL_WIDTH,   "Rx asm cells per Link" },
    { IPMUX_TAB_ATB_CNT_TXCL,   IPMUX_TAB_ATB_CNT_TXCL_LEN, IPMUX_TAB_ATB_CNT_TXCL_WIDTH,   "Tx cells per Link,except asm" },
    { IPMUX_TAB_ATB_CNT_TXCG,   IPMUX_TAB_ATB_CNT_TXCG_LEN, IPMUX_TAB_ATB_CNT_TXCG_WIDTH,   "Tx cells per Group,except asm" },
    { IPMUX_TAB_ATB_CNT_TXAL,   IPMUX_TAB_ATB_CNT_TXAL_LEN, IPMUX_TAB_ATB_CNT_TXAL_WIDTH,   "Tx asm cells per Link" },
    
    { IPMUX_TAB_END,            0,                          0,                          OPL_NULL }
    
};
#endif
#ifdef INCLUDE_SDRAM_COUNTER
#include <common.h>
#define LINE "--------------------------------------------------------"
#define globalHexDisplay 1

/* ------------------------ to remove in ipmux driver START -----------------------*/


/* ------------------------ to remove in ipmux driver END -----------------------*/
#define OPERATION_SHOW   0x001
#define OPERATION_UPDATE 0x010
#define OPERATION_CLEAR  0x100

#define OUTPUT_FMT_HEX                  0x0001
#define OUTPUT_FMT_PRINTHEADER          0x0010
/* 0x100 : text format ; otherwise table format */ 
#define OUTPUT_FMT_TEXT                 0x0100
/* 0=1;1=2  */ 
#define OUTPUT_FMT_TEXT_2FIELDS_PER_LINE    0x1000

IPMUX_BIT_FIELD_DEF_t fields_APGRPRX[]={
    //name               s,  e         
    {"RxCRCAlignErrors",    0,  11},
    {"RxUndersizePkts" ,   12,  23},
    {"RxOversizePkts",     24,  35},
    {"RxDropEvents",       36,  47},
    {"RxPkts64",           48,  67},
    {"RxPkts65to127",      68,  87},
    {"RxPkts128to255",     88, 107},
    {"RxPkts256to511",    108, 127},
    {"RxPkts512to1023" ,  128, 147},
    {"RxPkts1024to1518",  148, 167},
    {"RxOctets",          168, 193},
    {"RxPkts",            194, 213},
    {"RxBroadcastPkts",   214, 233},
    {"RxMulticastPkts",   234, 253} 
    
};

IPMUX_BIT_FIELD_DEF_t fields_ACCESS_PVHASH[]={
    //name               s,  e 
    {"Port ID",   0,   9 },
    {"VID",      10,  19 },
    {"CNT_GP_ID",20,  31 },
    {"RHP",      32,  41 }
};

IPMUX_BIT_FIELD_DEF_t fields_ACCESS_PVIN[]={
    //name               s,  e 
    //{"PVInFrames",   0,   11 },
    //{"PVInDiscards", 12,  31 }
    {"InFrames",   0,   11 },
    {"InDiscards", 12,  31 }
};
IPMUX_BIT_FIELD_DEF_t fields_ACCESS_PVOUT[]={
    //name               s,  e 
    {"OutFrames",   0,   19 }
};

IPMUX_BIT_FIELD_DEF_t fields_GEW_PVHASH[]={
    //name               s,  e 
    {"svid",      0,   9 },
    {"cvid",     10,  19 },
    {"CNT_GP_ID",20,  31 },
    {"RHP",      32,  41 }
};

IPMUX_BIT_FIELD_DEF_t fields_GEW_PVIN[]={
    //name               s,  e 
    {"InFrames",   0,   11 }
};
IPMUX_BIT_FIELD_DEF_t fields_GEW_PVOUT[]={
    //name               s,  e 
    {"OutFrames",   0,   19 }
};

IPMUX_BIT_FIELD_DEF_t fields_USDROP[]={
    //name               s,  e 
    {"USRxDrop",   0,   25 }
};

IPMUX_BIT_FIELD_DEF_t fields_US[]={
    //name               s,  e 
    {"USCounter",   0,   25 }
};

IPMUX_BIT_FIELD_DEF_t fields_DSDROP[]={
    //name               s,  e 
    {"DSRxDrop",   0,   25 }
};


IPMUX_BIT_FIELD_DEF_t fields_DS[]={
    //name               s,  e 
    {"DSCounter",   0,   25 }
};

IPMUX_BIT_FIELD_DEF_t fields_HIT[]={
    //name               s,  e 
    {"CLSHit ",   0,   25 }
};

IPMUX_BIT_FIELD_DEF_t fields_RXCL[]={
    //name               s,  e 
    {"RXCells",   0,   25 }
};

IPMUX_BIT_FIELD_DEF_t fields_TXCL[]={
    //name               s,  e 
    {"TXCells",   0,   25 }
};

IPMUX_BIT_FIELD_DEF_t fields_RXAL[]={
    //name               s,  e 
    {"RXAsmCells",   0,   15 }
};

IPMUX_BIT_FIELD_DEF_t fields_TXAL[]={
    //name               s,  e 
    {"TXAsmCells",   0,   15 }
};

IPMUX_BIT_FIELD_DEF_t fields_RXCG[]={
    //name               s,  e 
    {"RXCellsExAsm",   0,   31 }
};

IPMUX_BIT_FIELD_DEF_t fields_TXCG[]={
    //name               s,  e 
    {"TXCellsExAsm",   0,   31 }
};  
/*
IPMUX_BIT_FIELD_DEF_t fields_ACCESS_PVHASH[]={
name                 s,  e 
}
*/
IPMUX_BIT_FIELD_DEF_t fields_APGRPTX[]={
    //name          s , e                 
    {"TxPkts64",            0,  19},       
    {"TxPkts65to127",      20,  39},       
    {"TxPkts128to255",     40,  59},       
    {"TxPkts256to511",     60,  79},       
    {"TxPkts512to1023",    80,  99},       
    {"TxPkts1024to1518",  100, 119},       
    {"TxOctets",          120, 145},       
    {"TxPkts",            146, 165},       
    {"TxBroadcastPkts",   166, 185},       
    {"TxMulticastPkts",   186, 205}    
    
};
    /*tableid                                   kind                          v hugebuf field entry  header         fields  fieldsdef*/
IPMUX_STATISTC_TBL_DESC_t IPMUX_COUNTERS_SDRAM[]={
    {IPMUX_TAB_ATB_CNT_RXCL,        COUNTER_SDRAM_KIND_FLOW,     1,   1,  1,   144, OPL_NULL, OPL_NULL, fields_RXCL}, 
    {IPMUX_TAB_ATB_CNT_RXCG,        COUNTER_SDRAM_KIND_FLOW,     1,   1,  1,    36, OPL_NULL, OPL_NULL, fields_RXCG}, 
    {IPMUX_TAB_ATB_CNT_RXAL,        COUNTER_SDRAM_KIND_FLOW,     1,   1,  1,   144, OPL_NULL, OPL_NULL, fields_RXAL}, 
    {IPMUX_TAB_ATB_CNT_TXCL,        COUNTER_SDRAM_KIND_FLOW,     1,   1,  1,   144, OPL_NULL, OPL_NULL, fields_TXCL}, 
    {IPMUX_TAB_ATB_CNT_TXCG,        COUNTER_SDRAM_KIND_FLOW,     1,   1,  1,    36, OPL_NULL, OPL_NULL, fields_TXCG}, 
    {IPMUX_TAB_ATB_CNT_TXAL,        COUNTER_SDRAM_KIND_FLOW,     1,   1,  1,   144, OPL_NULL, OPL_NULL, fields_TXAL}, 
    
    {IPMUX_TAB_SAR_CNT_APGRPTX,     COUNTER_SDRAM_KIND_PORT,     1,   1,  10,  256, OPL_NULL, OPL_NULL, fields_APGRPTX}, 
    {IPMUX_TAB_SAR_CNT_APGRPRX,     COUNTER_SDRAM_KIND_PORT,     1,   1,  14,  256, OPL_NULL, OPL_NULL, fields_APGRPRX}, 
    
    {IPMUX_TAB_SAR_CNT_VLANHASH,    COUNTER_SDRAM_KIND_PORT_VLAN,1,   1,   4, 1024, OPL_NULL, OPL_NULL, fields_GEW_PVHASH },
    {IPMUX_TAB_SAR_CNT_VLANIN,      COUNTER_SDRAM_KIND_PORT_VLAN,1,   1,   1, 1024, OPL_NULL, OPL_NULL, fields_GEW_PVIN },
    {IPMUX_TAB_SAR_CNT_VLANOUT,     COUNTER_SDRAM_KIND_PORT_VLAN,1,   1,   1, 1024, OPL_NULL, OPL_NULL, fields_GEW_PVOUT},
    
    {IPMUX_TAB_SAR_CNT_PVHASH,      COUNTER_SDRAM_KIND_PORT_VLAN,1,   1,   4, 1024, OPL_NULL, OPL_NULL, fields_ACCESS_PVHASH },
    {IPMUX_TAB_SAR_CNT_PVIN,        COUNTER_SDRAM_KIND_PORT_VLAN,1,   1,   2, 1024, OPL_NULL, OPL_NULL, fields_ACCESS_PVIN  },
    {IPMUX_TAB_SAR_CNT_PVOUT,       COUNTER_SDRAM_KIND_PORT_VLAN,1,   1,   1, 1024, OPL_NULL, OPL_NULL, fields_ACCESS_PVOUT},
    
    {IPMUX_TAB_US_FCNT_DROP,        COUNTER_SDRAM_KIND_FLOW     ,1,   1,   1,  512, OPL_NULL, OPL_NULL, fields_USDROP},
    {IPMUX_TAB_US_FCNT,                  COUNTER_SDRAM_KIND_FLOW     ,1,   1,   1,  512, OPL_NULL, OPL_NULL, fields_US},
    
    {IPMUX_TAB_DS_FCNT_DROP,        COUNTER_SDRAM_KIND_FLOW     ,1,   1,   1, 1024, OPL_NULL, OPL_NULL, fields_DSDROP},
    {IPMUX_TAB_DS_FCNT,                 COUNTER_SDRAM_KIND_FLOW     ,1,   1,   1, 1024, OPL_NULL, OPL_NULL, fields_DS},

    {IPMUX_TAB_CLS_CNT_HIT,         COUNTER_SDRAM_KIND_CLASSIFY ,1,   1,   1,  512, OPL_NULL, OPL_NULL, fields_HIT},
    
    {OPL_ZERO,                      OPL_ZERO               , OPL_ZERO,  OPL_ZERO, OPL_ZERO,  OPL_ZERO, OPL_NULL, OPL_NULL}   
};
char IPMUX_COUNTER_SDRAM_KIND_NAME[][30]={
    "FLOW", 
        "CLASSIFY", 
        "PORT", 
        "PORT_VLAN", 
        "LINK_BONDING" 
} ;


/*******************************************************************************
*
* tableSearchEmptyEntry
*
* DESCRIPTION:
*   Search the IPMUX tables to find a enpty entry.
*
*
* INPUTS:
*   tabId   - the table to be searched.
*
* OUTPUT:
*   n/a
*
* RETURNS:
*    OPL_ERROR,                    Register not founded.
*    other,                        The first empty entry of the table.
*
* SEE ALSO: 
*/
static int tableSearchEmptyEntry(UINT32 tabId)
{
    int i=0;
    int iStatus;
    UINT32 data;
    IPMUX_STATISTC_TBL_DESC_t curTbl;
    /* lookup table id */
    while (1) {
        
        iStatus=sdramCounterTableRead(&curTbl,tabId,i,&data);

        if (iStatus==OPL_ERROR) 
        {
            return OPL_ERROR;
        }
        if (iStatus==OPL_OK) return i;  /* empty entry */ 
        i++;
    }
    
    printf("Find a enpty entry in table %d failed.\n", tabId);
    return OPL_ERROR;
    
}

/*******************************************************************************
*
* gewPortVlanSearchRhpEnd
*
* DESCRIPTION:
*   Search the IPMUX port vlan hash table to find the rehash end entry.
*
*
* INPUTS:
*   tabId   - the table to be searched.
*
* OUTPUT:
*   n/a
*
* RETURNS:
*    OPL_ERROR,                    Register not founded.
*    other,                        The first empty entry of the table.
*
* SEE ALSO: 
*/
static int gewPortVlanSearchRhpEnd(UINT32 index)
{
    int i=0;
    int iStatus;
    UINT32 data=0;
    IPMUX_STATISTC_TBL_DESC_t curTbl;   
    i=index;
    /* lookup table id */
    while (1) {
        iStatus=sdramCounterTableRead(&curTbl,IPMUX_TAB_SAR_CNT_VLANHASH ,i,&data);
        if (iStatus==OPL_ERROR) return OPL_ERROR;
        if (0==curTbl.fields[0].cur) 
        {
            /* RHP is empty */ 
            return i;
        }
        if (curTbl.fields[0].cur==i) 
        {
            printf("Gew port vlan hash table data wrong : the entry %d rehashed to itself.\n",i);
            return OPL_ERROR;
        };
        
        /* search for the next  */ 
        i=curTbl.fields[0].cur;
    }
    
    printf("Search the gew port vlan hash table to find the rehash end entry failed.\n");
    return OPL_ERROR;
}


/*******************************************************************************
*
* tableSearch
*
* DESCRIPTION:
*   Test the IPMUX registers.
*
*
* INPUTS:
*   pu4RegId    - the register offset.
*
* OUTPUT:
*   n/a
*
* RETURNS:
*    OPL_ERROR,                    Register not founded.
*    other,                        The index of the register in the array ipmuxReg.
*
* SEE ALSO: 
*/
static int tableSearch(UINT32 eTabId)
{
    int i;
    
    
    /* lookup table id */
    for (i = 0;  tab_desc[i].desc; i++)
        if (eTabId == tab_desc[i].id) break;
        
        if (OPL_NULL == tab_desc[i].desc) /* no found */
        {
            printf("Invlid Table Id %d passed.\n", eTabId);
            return OPL_ERR_NO(OPL_ERR_UNKNOWN_MEM_REGION);
        }
        return i;
}


/*******************************************************************************
*
* tableSearch
*
* DESCRIPTION:
*   Test the IPMUX registers.
*
*
* INPUTS:
*   pu4RegId    - the register offset.
*
* OUTPUT:
*   n/a
*
* RETURNS:
*    OPL_ERROR,                    Register not founded.
*    other,                        The index of the register in the array ipmuxReg.
*
* SEE ALSO: 
*/
static int sdramCounterTableSearch(UINT32 tabId)
{
    int i=0;
    
    /* lookup table id */
    while (IPMUX_COUNTERS_SDRAM[i].numEntry!=OPL_ZERO) {
        if ((IPMUX_COUNTERS_SDRAM[i].valid) && (tabId == (UINT32)IPMUX_COUNTERS_SDRAM[i].tabId))
        {
            return i;
        }
        i++;
    }
    
    printf("Invlid Table Id %d.\n", tabId);
    return OPL_ERROR;
    
}

OPL_STATUS BridgeStasticAddr(UINT16 usStag  ,UINT16 usCtag ,UINT16 *pusAddr)
{
    UINT8      aucTemp[BRIDGE_VLAN_INDEX_LENGTH];
    OPL_STATUS iStatus;
    
    if( OPL_NULL == pusAddr)
    {
        return PARAMETER_INPUT_POINTER_NULL;
    }
    
    /*Calculate address*/
    aucTemp[0]  = 0xFF & (usStag >> 4);
    aucTemp[1]  = 0xF0 & (usStag << 4);
    aucTemp[1] |= 0x0F & (usCtag >> 8);
    aucTemp[2]  = 0xFF & usCtag;
    
    iStatus = TableAddrCal16(aucTemp,BRIDGE_VLAN_INDEX_LENGTH
        ,BRIDGE_VLAN_ADDRESS_LENGTH,pusAddr);
    
    return  iStatus;
    
}
static OPL_STATUS sdramCounterInit()
{
    static int inited=0;
    IPMUX_STATISTC_TBL_DESC_t *curTbl;
    char *header;
    char strtmp[40];
    int i=0,j=0;
    
    if (inited!=0) 
    {
        return OPL_OK;
    }
    
    inited++;
    
    /* lookup table id */
    i=0;
        
    while (IPMUX_COUNTERS_SDRAM[i].numEntry!=OPL_ZERO) {
        if (IPMUX_COUNTERS_SDRAM[i].valid) 
        {
            curTbl=(&IPMUX_COUNTERS_SDRAM[i]);
            
            /* init header  */ 
            curTbl->header=(char *)malloc(24*curTbl->numField+1);
            if (OPL_NULL==curTbl->header) 
            {
                printf("Malloc memery for table header failed\n");
                return TABLE_INIT_MEMORY_NULL; 
            }
            
            if (curTbl->useHugeBuf) 
            {
                
                //printf("molloc size = %d\n",sizeof(IPMUX_BIT_FIELD_VAL_t)*curTbl->numField*curTbl->numEntry);
                curTbl->fields = (IPMUX_BIT_FIELD_VAL_t *)malloc(sizeof(IPMUX_BIT_FIELD_VAL_t)*curTbl->numField*curTbl->numEntry);
                //printf("curTbl->fields =%x\n",curTbl->fields );
                    
                bzero(curTbl->fields,sizeof(IPMUX_BIT_FIELD_VAL_t)*curTbl->numField*curTbl->numEntry);
            }
            else
            {
                curTbl->fields = (IPMUX_BIT_FIELD_VAL_t *)malloc(sizeof(IPMUX_BIT_FIELD_VAL_t)*curTbl->numField);
                bzero(curTbl->fields,sizeof(IPMUX_BIT_FIELD_VAL_t)*curTbl->numField);
            }
            
            if (OPL_NULL==curTbl->fields) 
            {
                printf("Malloc memery for table counters failed\n");
                return TABLE_INIT_MEMORY_NULL; 
            }
            
        }
        i++;
    }
    
    /* lookup table id */
    i=0;
    while (IPMUX_COUNTERS_SDRAM[i].numEntry!=OPL_ZERO) {
        if (IPMUX_COUNTERS_SDRAM[i].valid) 
        {
            header=IPMUX_COUNTERS_SDRAM[i].header;
            switch(IPMUX_COUNTERS_SDRAM[i].tabId) {
                
                
            case IPMUX_TAB_SAR_CNT_APGRPRX:  
                sprintf(header, "%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s", "RxEthCRCAlignErrors", "RxEthUndersizePkts", "RxEthOversizePkts", "RxEthDropEvents", "RxEthPkts64", "RxEthPkts65to127", "RxEthPkts128to255", "RxEthPkts256to511", "RxEthPkts512to1023", "RxEthPkts1024to1518", "RxEthOctets", "RxEthPkts", "RxEthBroadcastPkts", "RxEthMulticastPkts"); break;
            case IPMUX_TAB_SAR_CNT_APGRPTX:  
                sprintf(header, "%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s%-21.21s", "TxEthPkts64", "TxEthPkts65to127", "TxEthPkts128to255", "TxEthPkts256to511", "TxEthPkts512to1023", "TxEthPkts1024to1518", "TxEthOctets", "TxEthPkts", "TxEthBroadcastPkts", "TxEthMulticastPkts");break;
                
            default:
                strcpy(header,"");
                for(j=0; j<  IPMUX_COUNTERS_SDRAM[i].numField; j++)
                {
                    sprintf(strtmp, "%-21.21s%",IPMUX_COUNTERS_SDRAM[i].fieldsDef[j].name);
                    strcat(header,strtmp);
                }
                break;
            }   
            //printf(" %s\n",header);
        }
        i++;
    }
    
    return OPL_OK;
    
}
int ipmuxSDCounterShow (IPMUX_COUNTER_SDRAM_KIND_e kind,int arg1,int arg2,int arg3)
{
    int oper=OPERATION_SHOW;
    int hexDisplay=1;
    
    switch(kind) {
    case COUNTER_SDRAM_KIND_FLOW:   
        break;
    case COUNTER_SDRAM_KIND_CLASSIFY:
        break;
    case COUNTER_SDRAM_KIND_PORT:
        //accessPortCounterProcess(oper,hexDisplay,arg1,arg2,arg3);
        break;
    case COUNTER_SDRAM_KIND_PORT_VLAN:
        break;
    case COUNTER_SDRAM_KIND_LINK:
        break;
    case COUNTER_SDRAM_KIND_GROUP:
        break;
    case COUNTER_SDRAM_KIND_ALL:
        break;
        
    default:
        break;
    }
    return OPL_OK;
}


/*******************************************************************************
*
* accessPortCounterCfg
*
* DESCRIPTION:
*   Do the IPMUX Access Port Statistic counters configuration.
*
* INPUTS:
*   portId      - the access port ID.
*   groupId     - the group ID for the port.
*   show        - print the information.
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   OPL_OK,                       If the oper is successful.
*   OPL_ERROR,                    If the oper is failed.
*
* SEE ALSO: 
*
*/
OPL_STATUS accessPortCounterCfg(int portId,int groupId,int show)
{
    if (show) 
    {
        printf("autoconfig port %d to group %d.\n",portId,groupId);
    }

    if (OPL_ERROR==ipmuxTableWrite(IPMUX_TAB_SAR_CNT_APCTRL,portId,1,&groupId))
    {
        printf("Auto config port %d counter groupid to %d failed\n",portId,groupId);
        return OPL_ERROR;
    }       
}
/*******************************************************************************
*
* accessPortCounterProcess
*
* DESCRIPTION:
*   Do the IPMUX Access Port Statistic counters clear, update and show.
*
* INPUTS:
*   portId      - the access port ID.
*   portIdStart - the start port ID.
*   portIdEnd   - the end port ID.
*   offset      - the offset of the port entry.
*   num         - the number of the entry.
*   autoconfig  - aoto config APST table.
*
* OUTPUT:
*   The counters current value and accumulative value.
*
* RETURNS:
*   OPL_OK,                       If the oper is successful.
*   OPL_ERROR,                    If the oper is failed.
*
* SEE ALSO: 
*
*/
int accessPortCounterProcess(int oper, int hexDisplay,int portIdStart,int portIdEnd,int autoconfig)
{
    int i=0;
    int iGroup=0;
    int firstPort=1;
    int indexRx,indexTx;
    int outputFmt;
    OUTPUT_MEMBER_T tag;
    IPMUX_STATISTC_TBL_DESC_t *curTblTx;
    IPMUX_STATISTC_TBL_DESC_t *curTblRx;

#define IPMUX_GEW_ACCESS_PORT_NUM 576
    sdramCounterInit();
    
    if (oper & OPERATION_SHOW)
    {
        printf("\nAccess port statistic counters:\n");
    }
    
    indexRx=sdramCounterTableSearch(IPMUX_TAB_SAR_CNT_APGRPRX);
    
    if (indexRx==OPL_ERROR) 
    {
        return OPL_ERROR;
    }
    curTblRx=&(IPMUX_COUNTERS_SDRAM[indexRx]);
    
    indexTx=sdramCounterTableSearch(IPMUX_TAB_SAR_CNT_APGRPTX);
    
    if (indexRx==OPL_ERROR) 
    {
        return OPL_ERROR;
    }
    curTblTx=&(IPMUX_COUNTERS_SDRAM[indexTx]);
    
    /* statitic up  */ 
    printf("%s %d-%d statistic:\n",IPMUX_COUNTER_SDRAM_KIND_NAME[COUNTER_SDRAM_KIND_PORT],portIdStart,portIdEnd);
    for(i=portIdStart; ((i< IPMUX_ACCESS_PORT_NUM) && (i<=portIdEnd)); i++)
    {
        if (autoconfig) 
        {
            iGroup=i-portIdStart+1;
            if (OPL_ERROR==accessPortCounterCfg(i,iGroup,1))
            {
                return OPL_ERROR;
            }
        }

        if (OPL_ERROR!=ipmuxTableRead(IPMUX_TAB_SAR_CNT_APCTRL,i,1,&iGroup)) 
        {
            DBGTAB("read config table %d: rec=%d, group=%d\n",IPMUX_TAB_SAR_CNT_APCTRL,i,iGroup);
            if (iGroup) 
            {
                /* update data  */ 
                outputFmt=OUTPUT_FMT_PRINTHEADER | (hexDisplay?OUTPUT_FMT_HEX:0) | OUTPUT_FMT_TEXT ;//| OUTPUT_FMT_TEXT_2FIELDS_PER_LINE ; 
                sprintf(tag.headerIndexStr,"port %d:",i);
                sdramCounterProcess(IPMUX_TAB_SAR_CNT_APGRPRX,iGroup,1,!OPERATION_SHOW & oper,outputFmt,COUNTER_SDRAM_KIND_PORT, &tag,IPMUX_TAB_SAR_CNT_APGRPTX,-1,-1);

                firstPort=0;
            }   
            else
            {
                printf("  Port %d not configed in table APST, no counter data exist!\n",i);
            }
        }
        else
        {
            printf("  Read Port %d config in table APST failed!\n",i);
        }

    }
    
    return OPL_OK;
}


int ipmuxSDCounterClear (IPMUX_COUNTER_SDRAM_KIND_e kind,int arg1,int arg2,int arg3)
{
    int oper=OPERATION_CLEAR;
    int hexDisplay=1;
    
    switch(kind) {
    case COUNTER_SDRAM_KIND_FLOW:   
        break;
    case COUNTER_SDRAM_KIND_CLASSIFY:
        break;
    case COUNTER_SDRAM_KIND_PORT:
        accessPortCounterProcess(oper,hexDisplay,arg1,arg2,arg3);
        break;
    case COUNTER_SDRAM_KIND_PORT_VLAN:
        break;
    case COUNTER_SDRAM_KIND_LINK:
        break;
    case COUNTER_SDRAM_KIND_GROUP:
        break;
        
    case COUNTER_SDRAM_KIND_ALL:
        break;
        
    default:
        break;
    }
    return OPL_OK;
}

int ipmuxSDCounterUpdate (IPMUX_COUNTER_SDRAM_KIND_e kind,int arg1,int arg2,int arg3)
{
    int oper=OPERATION_UPDATE;
    int hexDisplay=1;
    
    switch(kind) {
    case COUNTER_SDRAM_KIND_FLOW:   
        break;
    case COUNTER_SDRAM_KIND_CLASSIFY:
        break;
    case COUNTER_SDRAM_KIND_PORT:
        accessPortCounterProcess(oper,hexDisplay,arg1,arg2,arg3);
        break;
    case COUNTER_SDRAM_KIND_PORT_VLAN:
        break;
    case COUNTER_SDRAM_KIND_LINK:
        break;
    case COUNTER_SDRAM_KIND_GROUP:
        break;
    case COUNTER_SDRAM_KIND_ALL:
        break;
        
        
    default:
        break;
    }
}


/*******************************************************************************
*
* generatMaskBits
*
* DESCRIPTION:
*    generate the mask according to bitwidth.
*
* INPUTS:
*   kind        - the counter kind.
*
* OUTPUT:
*   The counters current value and accumulative value.
*
* RETURNS:
*   OPL_OK,                       If the oper is successful.
*   OPL_ERROR,                    If the oper is failed.
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


int getBits(UINT32 *p, int widthInWord,int  bitS,int  bitE)
{
    UINT32 dataValid[2]={0,0};
    UINT32 mask;
    int i;
    UINT32 data;
    
    if (bitE-bitS>31) 
    {
        printf("bit define error: bits =%d, bite=%d, bitE-bitS=%d>31\n",bitS,bitE,bitE-bitS);
    }
    
    i=bitS/32;
    /* valid i  */                      
    if ((widthInWord>=(i+1)) && (i+1)>0) 
    {
        
        dataValid[0]=*((UINT32 *)(p+(widthInWord-1-i)));
        if ((widthInWord-i-2)>=0)
        {
            dataValid[1]=*((UINT32 *)(p+(widthInWord-i-2)));
        }
        
    }
    else
    {
        printf("******Total words width=%d, cur word %d invalid\n",widthInWord,i);
        return 0;
    }
    
    if (i==bitE/32) 
    {
        data=dataValid[0]>>(bitS %32) ;
    }
    else
    {
        data=dataValid[1]<<(32-bitS%32) | dataValid[0]>>(bitS%32) ;
    }
    generatMaskBits(&mask,bitE-bitS+1,1);
    
    data &=mask;
    
    return  data;
    
}

int parseTableRec(IPMUX_STATISTC_TBL_DESC_t *curTbl, int recNo,UINT32 *data)
{
    int i;
    int tableIndex;
    int iCIndex;

    if (OPL_NULL==curTbl) 
    {
        printf("Table pointer is OPL_NULL\n");
        return OPL_ERROR;
    }
    tableIndex=tableSearch(curTbl->tabId);
    
    if (tableIndex<0) 
    {
        return OPL_ERROR;
    }
    
    //printf("%s,tab_desc[tableIndex].width =%d\n",tab_desc[tableIndex].desc, tab_desc[tableIndex].width);
    /* change to big endian  */ 
    for(i=0; i<tab_desc[tableIndex].width; i++)
    {
        data[i]=htonl(data[i]);
    }
    
    /* parse data  */ 
    for(i=0; i<curTbl->numField; i++)
    {
        iCIndex=(curTbl->useHugeBuf)?recNo*curTbl->numField+i:i;
        
        curTbl->fields[iCIndex].cur=getBits(data,tab_desc[tableIndex].width, curTbl->fieldsDef[i].bitS, curTbl->fieldsDef[i].bitE);
        
        if (curTbl->useHugeBuf) oplUINT64Add(&(curTbl->fields[iCIndex].sum),curTbl->fields[iCIndex].cur);
    }
    
    return OPL_OK;
}

/*******************************************************************************
*
* CounterProcess
*
* DESCRIPTION:
*   Do the IPMUX counters clear, update and show.
*
* INPUTS:
*   kind        - the counter kind.
*   show        - if print the counters .
*   clear       - if clear the counters statistic.
*   update      - if update the counters from rigisters.
*
* OUTPUT:
*   The counters current value and accumulative value.
*
* RETURNS:
*   OPL_OK,                       If the oper is successful.
*   OPL_ERROR,                    If the oper is failed.
*
* SEE ALSO: 
*
*/
//static
int sdramCounterProcess(UINT32 tabId, int offset, int num, int oper, int outputFmt,IPMUX_COUNTER_SDRAM_KIND_e kind, OUTPUT_MEMBER_T *tag,UINT32 tabId1,UINT32 tabId2,UINT32 tabId3) 
{
    int i,j,iNo,index,iCIndex,m,tabNum,iStatus;
    int regNum, regNumBad;
    IPMUX_STATISTC_TBL_DESC_t *curTbl;
    IPMUX_STATISTC_TBL_DESC_t *curTbls[4];
    
#ifdef DEBUG_IN_WIN32
    UINT32 data[30]={2290649224,2004318071,1717986918,1431655765,1145324612,858993459,572662306,286331153,0xFFF,0xFFF,0xFFF};
#else
    UINT32 *data=OPL_NULL;
#endif
    char output[16];
    sdramCounterInit();
    
    printf("Read counters from tables : ");
    index=sdramCounterTableSearch(tabId);
    
    if (index==OPL_ERROR) 
    {
        return OPL_ERROR;
    }
    curTbl=&(IPMUX_COUNTERS_SDRAM[index]);
    curTbls[0]=curTbl;
    tabNum=1;
    printf(" %d",tabId);
    
    if (-1!=tabId1) 
    {
        index=sdramCounterTableSearch(tabId1);
        
        if (index==OPL_ERROR) 
        {
            return OPL_ERROR;
        }
        curTbls[1]=&(IPMUX_COUNTERS_SDRAM[index]);
        tabNum=2;
        printf(" %d",tabId1);
    }
    if (-1!=tabId2) 
    {
        index=sdramCounterTableSearch(tabId2);
        
        if (index==OPL_ERROR) 
        {
            return OPL_ERROR;
        }
        curTbls[2]=&(IPMUX_COUNTERS_SDRAM[index]);
        tabNum=3;
        printf(" %d",tabId2);
    }
    if (-1!=tabId3) 
    {
        index=sdramCounterTableSearch(tabId3);
        
        if (index==OPL_ERROR) 
        {
            return OPL_ERROR;
        }
        curTbls[3]=&(IPMUX_COUNTERS_SDRAM[index]);
        tabNum=4;
        printf(" %d",tabId3);       
    }
    printf(".\n");
    if (num<=0)  num=curTbl->numEntry;
    
    if (offset>=curTbl->numEntry)
    {
        printf("Invalid offset %d, the total entry is %d.\n",offset,curTbl->numEntry);
        return OPL_ERROR;
    }
    
    j=0;
    i=0;
    regNumBad= 0;
    regNum = 0;
    iNo=0;
    
    if (OPERATION_SHOW & oper) 
    {
        if (OUTPUT_FMT_TEXT & outputFmt )
        {
            /* header in text mode is in the display counter section */ 
        }
        else
        {
            /* print header in table mode  */ 
            if (OUTPUT_FMT_PRINTHEADER & outputFmt)
            {
                /* index width = 5  */ 
                if (tag!=OPL_NULL) 
                    printf("%-5.5s",tag->headerIndexStr);
                else
                    printf("  #  ");
                
                for(m=0;m<tabNum;m++)
                {
                    curTbl=curTbls[m];
                    for(i=0; i< curTbl->numField ; i++)
                    {
                        //printf("%-30.30s ",&(curTbl->header[i*24]));
                        printf("%-13.13s%-12.12s %-10.10s",&(curTbl->header[i*24]),"Total","Current");
                        if (m<tabNum-1) printf("  ");
                        //printf("%-17.17s %3.3s %3.3s",&(curTbl->header[i*24]),"sum","cur");
                    }
                }
                printf("\n");
                
                printf("---- ");
                
                for(m=0;m<tabNum;m++)
                {
                    curTbl=curTbls[m];
                    for(i=0; i< curTbl->numField ; i++)
                    {
                        //printf("%-36.36s",LINE);      
                        printf("%-25.25s",LINE);        
                        printf(" ");        
                        printf("%-10.10s",LINE);        
                        if (m<tabNum-1) printf("  ");
                    }
                    
                }
                printf("\n");
            }
        }
    }
    
    iNo=0;
    
    for(i=offset; ((i< curTbl->numEntry) && (iNo<num)); i++)
    {
        /* process per record  */ 
        for(m=0;m<tabNum;m++)
        {
            /* process per table */ 
            curTbl=curTbls[m];
            
            /* clear data  */ 
            if (OPERATION_CLEAR & oper) 
            {
                for(j=0; j< curTbl->numField ; j++)
                {
                    iCIndex=(curTbl->useHugeBuf)?i*curTbl->numField+j:j;
                    curTbl->fields[iCIndex].cur=0;
                    curTbl->fields[iCIndex].sum.high=0;
                    curTbl->fields[iCIndex].sum.low=0;
                    
                }
            }
            
            /* read registers  */ 
            if (OPERATION_UPDATE & oper) 
            {
                if (1==curTbl->numField) 
                {
                    for(j=0; j<curTbl->numField ; j++)
                    {
                        
                        iCIndex=(curTbl->useHugeBuf)?i*curTbl->numField+j:j;
                        //iCIndex=i*curTbl->numField+j;
                        /* read data  */ 
                        DBGTAB_UPDATE("First:cur=%d\n",curTbl->fields[iCIndex].cur);
                        curTbl->fields[iCIndex].cur=0;
                        DBGTAB_UPDATE("read tab %d rec %d\n",curTbl->tabId,i);
                        iStatus=ipmuxTableRead(curTbl->tabId,i,1,&(curTbl->fields[iCIndex].cur));
                        if (OPL_OK!=iStatus) 
                        {
                            printf("Read table %d failed\n",curTbl->tabId);
                            return iStatus;
                        }
                        DBGTAB_UPDATE("Read:cur=%d\n",curTbl->fields[iCIndex].cur);
                        /* update data  */ 
                        oplUINT64Add(&(curTbl->fields[iCIndex].sum),curTbl->fields[iCIndex].cur);
                    }
                    
                }
                else
                {
                    
#ifndef DEBUG_IN_WIN32
                    data=(UINT32 *)malloc(4*curTbl->numField);
#endif
                    
                    /* read data  */ 
#ifndef DEBUG_IN_WIN32
                    iStatus=ipmuxTableRead(tabId,i,1,data);
                    if (OPL_OK!=iStatus) 
                    {
                        printf("Read table %d failed\n",tabId);
                        return iStatus;
                    }
#endif
                    /* update data  */ 
                    parseTableRec(curTbl,i,data);
                    
                }
            }
            
            /* display counters  in text mode  */ 
            if (OUTPUT_FMT_TEXT & outputFmt) 
            {
                /* display header in text mode */ 
                printf("%-21.21s %-12.12s %-10.10s",tag->headerIndexStr,"Total","Current");     
                if (OUTPUT_FMT_TEXT_2FIELDS_PER_LINE & outputFmt) 
                {
                    printf("    ");
                    printf("%-17.17s %-12.12s %-10.10s","","Total","Current");      
                }
                printf("\n");
                
                
                printf("%-21.21s %-12.12s %-10.10s",LINE,LINE,LINE);        
                if (OUTPUT_FMT_TEXT_2FIELDS_PER_LINE & outputFmt) 
                {
                    printf("    ");
                    printf("%-17.17s %-12.12s %-10.10s",LINE,LINE,LINE);        
                }
                printf("\n");
                
                
                for(m=0;m<tabNum;m++)
                {
                    curTbl=curTbls[m];
                    for(j=0; j< curTbl->numField ; j++)
                    {
                        /* print records  */ 
                        iCIndex=(curTbl->useHugeBuf)?i*curTbl->numField+j:j;
                        //iCIndex=iGroup*curTbl->numField+j;    
                        //printf("%s/",oplAsciiUint64(&(curTbl->fields[iCIndex].sum),0));
                        
                        if (OUTPUT_FMT_HEX & outputFmt) 
                        {
                            if (curTbl->fields[iCIndex].sum.high!=0) 
                            {
                                sprintf(output,"%x%x",curTbl->fields[iCIndex].sum.high,curTbl->fields[iCIndex].sum.low);
                            }
                            else
                            {   
                                sprintf(output,"%x",curTbl->fields[iCIndex].sum.low);
                            }
                            
                            printf("    %-17.17s %12.12s %10x",curTbl->fieldsDef[j].name,output,curTbl->fields[iCIndex].cur);   
                            
                        }
                        else
                        {
#ifdef DEBUG_IN_WIN32
                            printf("%12u ",curTbl->fields[iCIndex].sum.low);
                            printf("%10u ",curTbl->fields[iCIndex].cur);
#else
                            printf("%12.12s ",oplAsciiUint64(&(curTbl->fields[iCIndex].sum),1));
                            printf("%10x ",curTbl->fields[iCIndex].cur);
#endif
                        }
                        if (OUTPUT_FMT_TEXT_2FIELDS_PER_LINE & outputFmt) 
                        {
                            if (1==(j % 2)) printf("\n");
                        }
                        else
                        {
                            printf("\n");
                        }
                        
                    }
                    //printf("\n"); /* separate tx and rx  */ 
                }
            }/* end print in text mode  */ 
            else
            {/* print in table mode  */ 
                
                if (OPERATION_SHOW & oper) 
                {
                    if (curTbl==curTbls[0]) 
                    {
                        if (tag!=OPL_NULL) 
                            
                            printf("%4d ",tag->iNo );
                        else
                            printf("%4d ",iNo );
                    }
                    for(j=0; j< curTbl->numField ; j++)
                    {
                        /* print records  */ 
                        iCIndex=(curTbl->useHugeBuf)?i*curTbl->numField+j:j;
                        
                        if (OUTPUT_FMT_HEX & outputFmt) 
                        {
                            if (curTbl->fields[iCIndex].sum.high!=0) 
                            {
                                sprintf(output,"%x%x",curTbl->fields[iCIndex].sum.high,curTbl->fields[iCIndex].sum.low);
                            }
                            else
                            {   
                                sprintf(output,"%x",curTbl->fields[iCIndex].sum.low);
                            }
                            
                            printf("%25.25s ",output);  
                            printf("%10x",curTbl->fields[iCIndex].cur);
                            if (m<tabNum-1) printf("  ");
                        }
                        else
                        {
#ifdef DEBUG_IN_WIN32
                            printf("%25u ",curTbl->fields[iCIndex].sum.low);
                            printf("%10u",curTbl->fields[iCIndex].cur);
                            if (m<tabNum-1) printf("  ");
                            
#else
                            printf("%25.25s ",oplAsciiUint64(&(curTbl->fields[iCIndex].sum),1));
                            printf("%10u",curTbl->fields[iCIndex].cur);
                            if (m<tabNum-1) printf("  ");
#endif
                        }
                        
                    }
                }
            }   
            }
            if (OPERATION_SHOW & oper) printf("\n");
            iNo++;  
    }
#ifndef DEBUG_IN_WIN32  
    if (OPL_NULL!=data) 
    {
        free(data);
    }
#endif  
    //if (OPERATION_SHOW & oper) printf("\n %d Counters displayed.\n\n", iNum);
    return  (OPL_OK);
}

/*
int sdramCounterKindProcess(IPMUX_COUNTER_SDRAM_KIND_e kind,int oper) 
{
    int                          i=0,iNum=0;
    int outputFmt;
    sdramCounterInit();
    
    if (OPERATION_SHOW & oper) printf("\nIPMUX %s couters show ...\n",kind==-1?"all" : IPMUX_COUNTER_SDRAM_KIND_NAME[kind]);
    outputFmt=OUTPUT_FMT_TEXT | OUTPUT_FMT_PRINTHEADER;
    // lookup table id 
    while (IPMUX_COUNTERS_SDRAM[i].numEntry!=OPL_ZERO) {
        if (IPMUX_COUNTERS_SDRAM[i].valid) 
        {
            
            if ((kind==COUNTER_SDRAM_KIND_ALL) || (IPMUX_COUNTERS_SDRAM[i].kind==kind))
            {
            
                sdramCounterProcess(IPMUX_COUNTERS_SDRAM[i].tabId,0,0, oper, outputFmt,OPL_NULL,-1,-1,-1);
            }
        }
        i++;
    }
    
    if (OPERATION_SHOW & oper) printf("\n   %d Counters displayed.\n", iNum);
    return  (OPL_OK);
}
*/

OPL_STATUS flowCounterProcess(int oper,int hexDisplay,UINT16 flowStart,UINT16 flowEnd)
{
    int i,outputFmt;
    OUTPUT_MEMBER_T tag;
    sdramCounterInit();
    
    if (oper & OPERATION_SHOW)
    {
        printf("\nUp and Down flow Rx packet drop counters:\n");
    }
    
    strcpy(tag.headerIndexStr,"flow");
    for(i=flowStart ; i<=flowEnd ; i++)
    {
        tag.iNo=i;
        outputFmt=(flowStart==i?OUTPUT_FMT_PRINTHEADER:0) | (hexDisplay?OUTPUT_FMT_HEX:0);
        if (OPL_ERROR==sdramCounterProcess(IPMUX_TAB_US_FCNT,i,1,oper,outputFmt,COUNTER_SDRAM_KIND_FLOW,&tag,IPMUX_TAB_US_FCNT_DROP,-1,-1))
        {
            return OPL_ERROR;
        };
    }
    
    for(i=flowStart ; i<=flowEnd ; i++)
    {
        tag.iNo=i;
        outputFmt=(flowStart==i?OUTPUT_FMT_PRINTHEADER:0) | (hexDisplay?OUTPUT_FMT_HEX:0);
        if (OPL_ERROR==sdramCounterProcess(IPMUX_TAB_DS_FCNT,i,1,oper,outputFmt,COUNTER_SDRAM_KIND_FLOW,&tag,IPMUX_TAB_DS_FCNT_DROP,-1,-1))
        {
            return OPL_ERROR;
        }
            
    }
        return OPL_OK;
}

OPL_STATUS clsHitCounterProcess(int oper,int hexDisplay, UINT16 ruleIndexStart,UINT16 ruleIndexEnd)
{
    int i,outputFmt;
    OUTPUT_MEMBER_T tag; 
    sdramCounterInit();
    strcpy(tag.headerIndexStr,"rule");
    if (oper & OPERATION_SHOW)
    {
        printf("\nClassification hit statistic counters:\n");
    }
    
    for(i=ruleIndexStart ; i<=ruleIndexEnd ; i++)
    {
        tag.iNo=i;
        
        outputFmt=(ruleIndexStart==i?OUTPUT_FMT_PRINTHEADER:0) | (hexDisplay?OUTPUT_FMT_HEX:0);
        sdramCounterProcess(IPMUX_TAB_CLS_CNT_HIT, i,1,oper,outputFmt,COUNTER_SDRAM_KIND_CLASSIFY,&tag,-1,-1,-1);
    }
    return OPL_OK;
}

OPL_STATUS sdramCounterTableRead(IPMUX_STATISTC_TBL_DESC_t *curTbl,UINT32 tabid,int index,UINT32 *data)
{
    OPL_STATUS iStatus;
    int i;
    int tableIndex;

    iStatus=sdramCounterTableSearch(tabid);
    if (iStatus==OPL_ERROR) 
    {
        return OPL_ERROR;
    }
    DBGCFGTAB("sdramCounterTableRead iStatus=%d\n",iStatus);
    curTbl=&(IPMUX_COUNTERS_SDRAM[iStatus]);

    if (index>=curTbl->numEntry) 
    {
#ifdef DEBUG_IN_WIN32 
        printf("Read offset %d exceed total entry %d\n",index,curTbl->numEntry);
#endif
        return OPL_ERROR;
    }

    tableIndex=tableSearch(curTbl->tabId);

    data=(UINT32 *)malloc(4*tab_desc[tableIndex].width);
    
    iStatus=ipmuxTableRead(tabid,index,1,data);
    if (OPL_OK!=iStatus) 
    {
        printf("Read table %d failed\n",tabid);
        return iStatus;
    }
                
    parseTableRec(curTbl,index,data);
    for(i=0; i< tab_desc[tableIndex].width ; i++)
    {
        if (0!=data[i]) 
        {
            return data[i]; /* indicate not all zero  */ 
        }
    }
    return OPL_OK;
}

OPL_STATUS linkCounterProcess(int oper,int hexDisplay, UINT16 start,UINT16 end)
{
    int i,outputFmt;
    OUTPUT_MEMBER_T tag; 
    sdramCounterInit();
    
    if (oper & OPERATION_SHOW)
    {
        printf("\nLink statistic counters:\n");
    }
    
    for(i=start ; i<=end ; i++)
    {
        //outputFmt=(start==i?OUTPUT_FMT_PRINTHEADER:0) | (hexDisplay?OUTPUT_FMT_HEX:0) ;
        outputFmt=OUTPUT_FMT_PRINTHEADER | (hexDisplay?OUTPUT_FMT_HEX:0) | OUTPUT_FMT_TEXT;
        tag.iNo=i;
        sprintf(tag.headerIndexStr,"link %3d:",i);
        sdramCounterProcess(IPMUX_TAB_ATB_CNT_RXCL, 0,1,oper,outputFmt,COUNTER_SDRAM_KIND_CLASSIFY,&tag,IPMUX_TAB_ATB_CNT_TXCL,IPMUX_TAB_ATB_CNT_RXAL,IPMUX_TAB_ATB_CNT_TXAL);
    }
    return OPL_OK;
    
}

OPL_STATUS gewPortVlanIndexCal(UINT16 svid, UINT16 cvid, UINT16* pusAddr)
{
#define SAR_GEW_PORT_VLAN_CVID_WIDTH 12
#define SAR_GEW_PORT_VLAN_HASHSRC_WIDTH 22
#define SAR_GEW_PORT_VLAN_INDEX_MASK 0x3FF
    
    UINT32 aucTemp=0;
    OPL_STATUS iStatus;
    
    aucTemp=svid<<SAR_GEW_PORT_VLAN_CVID_WIDTH | cvid;
    
    iStatus = TableAddrCal16(&aucTemp,SAR_GEW_PORT_VLAN_HASHSRC_WIDTH   
        ,SAR_GEW_PORT_VLAN_INDEX_MASK,pusAddr);
    
    if(OPL_OK != iStatus)
    {
        return iStatus;
    }
    return OPL_OK;
}

/*******************************************************************************
*
* accessPortVlanCounterCfg
*
* DESCRIPTION:
*   Do the IPMUX Access Port Vlan Statistic counters configuration.
*
* INPUTS:
*   portId      - the port ID.
*   vlanId      - the vlan ID.
*   groupId     - the group ID for the vlan port.
*   show        - print the information.
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   OPL_OK,                       If the oper is successful.
*   OPL_ERROR,                    If the oper is failed.
*
* SEE ALSO: 
*
*/
OPL_STATUS gewPortVlanCounterCfg(int svid, int cvid,int groupId,int show)
{
    OPL_STATUS iStatus;
    UINT16 index=0;
    UINT32 iEntry=0;
    UINT32 iRehashEntry=0;
    UINT32 data=0;
    UINT32 dataHash[2];


    IPMUX_STATISTC_TBL_DESC_t curTbl;   

    if (show) 
    {
        printf("autoconfig svid %d svid %d to %d.\n",svid,cvid, groupId);
    }
    /* generate hash value  */  
    iStatus=gewPortVlanIndexCal(svid,cvid,&index);
    if (OPL_OK!=iStatus) 
    {
        printf("Calculate port vlan crc16 index failed.\n");
        return iStatus;
    } 

    /* write ptr table  */ 
    if (OPL_ERROR==ipmuxTableRead(IPMUX_TAB_SAR_CNT_VLANPTR,index,1,&data))
    {
        printf("Read SAR port VLAN counter pointer table record %d failed.\n",index);
        return OPL_ERROR;
    }
    /* get enpty hash table record, and fill in ptr table  */ 
    iEntry=tableSearchEmptyEntry(IPMUX_TAB_SAR_CNT_VLANHASH);
    if (OPL_ERROR==iEntry)
    {
        return OPL_ERROR;
    }

    /* Empty ptr entry */ 
    if (OPL_ZERO==data) 
    {
        if (OPL_ERROR==ipmuxTableWrite(IPMUX_TAB_SAR_CNT_VLANPTR,index,1,&iEntry))
        {
            printf("Config SAR gew port VLAN counter pointer table record %d to data %d failed \n",index,iEntry);
            return OPL_ERROR;
        }
    }
    else
    {
        /* get enpty hash table record, and fill in ptr table  */ 
        iRehashEntry=gewPortVlanSearchRhpEnd(data);
        if (OPL_ERROR==iEntry)
        {
            return OPL_ERROR;
        }

        /* get the value  */ 
        iStatus = sdramCounterTableRead(&curTbl,IPMUX_TAB_SAR_CNT_VLANHASH, 1,&iRehashEntry);
        if (OPL_ERROR==iStatus)
        {
            return OPL_ERROR;
        }

        /* filll the RHP field  */ 
        dataHash[0]=curTbl.fields[3].cur  ;//portId;
        dataHash[1]=curTbl.fields[2].cur<<20 | curTbl.fields[1].cur | iEntry;
        if (OPL_ERROR==ipmuxTableWrite(IPMUX_TAB_SAR_CNT_VLANHASH,iRehashEntry,1,dataHash))
        {
            printf("Read SAR gew port VLAN counter hash table record %d failed\n",iRehashEntry);
            return OPL_ERROR;
        }
        
    }

    /* fill current "portid vlanid groupid RHP" to record*/ 
    dataHash[0]=svid;
    dataHash[1]=cvid<<20 | groupId<<10 ;
    if (OPL_ERROR==ipmuxTableWrite(IPMUX_TAB_SAR_CNT_VLANHASH,iEntry,1,dataHash))
    {
        printf("Read SAR gew port VLAN counter hash table record %d failed\n",iEntry);
        return OPL_ERROR;
    }

    return OPL_OK;
}

/*******************************************************************************
*
* gewPortVlanCounterProcess
*
* DESCRIPTION:
*   Do the IPMUX gew Port Statistic counters clear, update and show.
*
* INPUTS:
*   show        - if print the counters .
*   clear       - if clear the counters statistic.
*   update      - if update the counters from rigisters.
*   hexDisplay- print counter in decimal or hex mode.
*   svid,cvid   -
*           Single VLAN mode    :
*               Incoming packet Statistic variable
*               --------------- ---------------------
*               untagged        SVID=0;CVID=0
*               Single tagged   SVID=in_SVID;CVID=0
*               Double tagged   SVID=in_SVID;CVID=0
*           
*           Double VLAN mode    :   
*               Incoming packet Statistic variable
*               --------------- ---------------------
*               untagged        SVID=0;CVID=0
*               Single tagged   SVID=in_SVID;CVID=0
*               Double tagged   SVID=in_SVID;CVID=in_CVID
*   autoconfig  - aoto config APST table.
*
* OUTPUT:
*   The counters current value and accumulative value.
*
* RETURNS:
*   OPL_OK,                       If the oper is successful.
*   OPL_ERROR,                    If the oper is failed.
*   
* SEE ALSO: 
*
*/
int gewPortVlanCounterProcess(int oper, int hexDisplay,int svid,int cvid,int autoconfig)
{

    int iGroup=0;
    OPL_STATUS iStatus;
    UINT32 data[2];
    UINT16 index=0;
    UINT32 iRehash=0;


    OUTPUT_MEMBER_T tag;
    int indexIn,indexOut;
    IPMUX_STATISTC_TBL_DESC_t *curTblIn;
    IPMUX_STATISTC_TBL_DESC_t *curTblOut;
    
    IPMUX_STATISTC_TBL_DESC_t *curTbl;
    static int curGroupCfg=1;
#define IPMUX_ACCESS_PORT_NUM 576   
    
    sdramCounterInit();
    
    if (oper & OPERATION_SHOW)
    {
        printf("\nGEW port VLAN statistic counters:\n");
    }
    
    indexIn=sdramCounterTableSearch(IPMUX_TAB_SAR_CNT_VLANIN);
    
    if (indexIn==OPL_ERROR) 
    {
        return OPL_ERROR;
    }
    curTblIn=&(IPMUX_COUNTERS_SDRAM[indexIn]);
    
    indexOut=sdramCounterTableSearch(IPMUX_TAB_SAR_CNT_VLANOUT);
    
    if (indexOut==OPL_ERROR) 
    {
        return OPL_ERROR;
    }
    curTblOut=&(IPMUX_COUNTERS_SDRAM[indexOut]);
    
    
    /* generate hash value  */  
    iStatus=gewPortVlanIndexCal(svid,cvid,&index);
    if (OPL_OK!=iStatus) 
    {
        printf("Calculate GEW port vlan crc16 index failed\n");
        return iStatus;
    } 
    
    /* configure the Port VLAN Counter Hash Table  */ 
    if (autoconfig) 
    {
        gewPortVlanCounterCfg(svid,cvid, curGroupCfg++,1);
    }
    
    /* get hash pointer */ 
    iStatus=ipmuxTableRead(IPMUX_TAB_SAR_CNT_VLANPTR,index,1,&iRehash);
    if (OPL_OK!=iStatus) 
    {
        printf("Read SAR GEW port VLAN counter pointer failed\n");
        return iStatus;
    }
    index = iRehash;
    
    iStatus=sdramCounterTableSearch(IPMUX_TAB_SAR_CNT_VLANHASH);
    if (iStatus==OPL_ERROR) 
    {
        return OPL_ERROR;
    }
    curTbl=&(IPMUX_COUNTERS_SDRAM[iStatus]);
    
    /* indicate the entry  */ 
    while (1) {
        iStatus=ipmuxTableRead(IPMUX_TAB_SAR_CNT_VLANHASH,index,1,data);        
        if (OPL_OK!=iStatus) 
        {
            return OPL_ERROR;
        }
        
        parseTableRec(curTbl,index,data);
#ifdef DEBUG_IN_WIN32
        curTbl->fields[0].cur=svid;
        curTbl->fields[1].cur=cvid;
#endif
        if ((curTbl->fields[0].cur==svid) && (curTbl->fields[1].cur==cvid)) 
        {
            /* found  */ 
            iGroup=index; /* just the current entry  */ 
            break;
        }
        
        /* search for the rehash entrys  */ 
        iRehash=curTbl->fields[3].cur;
        if (OPL_ZERO==iRehash ) 
        {
            printf("Can't find the entry which svid=%d and cvid=%d in SAR GEW port VLAN counter hash table, stoped at record %d.\n",svid,cvid,index);
            return OPL_ERROR;
        }
        
        if ((iRehash>=IPMUX_TAB_SAR_CNT_PVHASH_LEN) || (iRehash==index)) 
        {
            printf("Invalid rehash value %d for svid %d and cvid%d in SAR GEW port VLAN counter hash table, stoped at record %d.\n",iRehash,svid,cvid,index);
            return OPL_ERROR;
        }
        
        iGroup=iRehash;
    }

    sprintf(tag.headerIndexStr, "svid %d cvid %d:",svid,cvid) ;

    sdramCounterProcess(IPMUX_TAB_SAR_CNT_VLANIN  ,iGroup,1,oper,hexDisplay | OUTPUT_FMT_PRINTHEADER | OUTPUT_FMT_TEXT,COUNTER_SDRAM_KIND_PORT_VLAN,&tag,IPMUX_TAB_SAR_CNT_VLANOUT ,-1,-1);

    
    //printf("Port %d not configed in table APST, no counter data exist!\n",i);
    return OPL_OK;
}

OPL_STATUS groupCounterProcess(int oper, int hexDisplay,UINT16 start,UINT16 end)
{
    int i,outputFmt;
    OUTPUT_MEMBER_T tag;
    
    if (oper & OPERATION_SHOW)
    {
        printf("\nBonding statistic counters:\n");
    }
    
    sdramCounterInit(); 
    for(i=start ; i<=end ; i++)
    {
        tag.iNo=i;
        strcpy(tag.headerIndexStr,"grp");
        outputFmt=(start==i?OUTPUT_FMT_PRINTHEADER:0) | (hexDisplay?OUTPUT_FMT_HEX:0) ;
        sdramCounterProcess(IPMUX_TAB_ATB_CNT_RXCG, 0,1,oper,outputFmt,COUNTER_SDRAM_KIND_CLASSIFY,&tag,IPMUX_TAB_ATB_CNT_TXCG,-1,-1);
    }
    return OPL_OK;
}

OPL_STATUS flowCounterShow(UINT16 flowStart,UINT16 flowEnd)
{
    int oper=OPERATION_SHOW;
    return flowCounterProcess(oper,globalHexDisplay,flowStart,flowEnd); 
}
OPL_STATUS flowCounterClear(UINT16 flowStart,UINT16 flowEnd)
{
    int oper=OPERATION_CLEAR;
    return flowCounterProcess(oper,globalHexDisplay,flowStart,flowEnd); 
}
OPL_STATUS flowCounterUpdate(UINT16 flowStart,UINT16 flowEnd)
{
    int oper=OPERATION_UPDATE;
    return flowCounterProcess(oper,globalHexDisplay,flowStart,flowEnd); 
}


OPL_STATUS clsHitCounterShow(UINT16 ruleIndexStart,UINT16 ruleIndexEnd)
{
    int oper=OPERATION_SHOW;
    return clsHitCounterProcess(oper,globalHexDisplay,ruleIndexStart,ruleIndexEnd); 
}
OPL_STATUS clsHitCounterClear(UINT16 ruleIndexStart,UINT16 ruleIndexEnd)
{
    int oper=OPERATION_CLEAR;
    return clsHitCounterProcess(oper,globalHexDisplay,ruleIndexStart,ruleIndexEnd); 
}
OPL_STATUS clsHitCounterUpdate(UINT16 ruleIndexStart,UINT16 ruleIndexEnd)
{
    int oper=OPERATION_UPDATE;
    return clsHitCounterProcess(oper,globalHexDisplay,ruleIndexStart,ruleIndexEnd); 
}



OPL_STATUS accessPortCounterShow(int portIdStart,int portIdEnd)
{
    int oper=OPERATION_SHOW;
    return accessPortCounterProcess(oper,globalHexDisplay,portIdStart,portIdEnd,0); 
}
OPL_STATUS accessPortCounterClear(int portIdStart,int portIdEnd)
{
    int oper=OPERATION_CLEAR;
    return accessPortCounterProcess(oper,globalHexDisplay,portIdStart,portIdEnd,0); 
}
OPL_STATUS accessPortCounterUpdate(int portIdStart,int portIdEnd)
{
    int oper=OPERATION_UPDATE;
    return accessPortCounterProcess(oper,globalHexDisplay,portIdStart,portIdEnd,0); 
}


OPL_STATUS gewPortVlanCounterShow(int svid,int cvid)
{
    int oper=OPERATION_SHOW;
    return gewPortVlanCounterProcess(oper,globalHexDisplay,svid,cvid,0);    
}
OPL_STATUS gewPortVlanCounterClear(int svid,int cvid)
{
    int oper=OPERATION_CLEAR;
    return gewPortVlanCounterProcess(oper,globalHexDisplay,svid,cvid,0);    
}
OPL_STATUS gewPortVlanCounterUpdate(int svid,int cvid)
{
    int oper=OPERATION_UPDATE;
    return gewPortVlanCounterProcess(oper,globalHexDisplay,svid,cvid,0);    
}


OPL_STATUS linkCounterShow(UINT16 start,UINT16 end)
{
    int oper=OPERATION_SHOW;
    return gewPortVlanCounterProcess(oper,globalHexDisplay,start,end,0);    
}
OPL_STATUS linkCounterClear(UINT16 start,UINT16 end)
{
    int oper=OPERATION_CLEAR;
    return gewPortVlanCounterProcess(oper,globalHexDisplay,start,end,0);    
}
OPL_STATUS linkCounterUpdate(UINT16 start,UINT16 end)
{
    int oper=OPERATION_UPDATE;
    return gewPortVlanCounterProcess(oper,globalHexDisplay,start,end,0);    
}

OPL_STATUS groupCounterShow(UINT16 start,UINT16 end)
{
    int oper=OPERATION_SHOW;
    return groupCounterProcess(oper,globalHexDisplay,start,end);    
}
OPL_STATUS groupCounterClear(UINT16 start,UINT16 end)
{
    int oper=OPERATION_CLEAR;
    return groupCounterProcess(oper,globalHexDisplay,start,end);    
}
OPL_STATUS groupCounterUpdate(UINT16 start,UINT16 end)
{
    int oper=OPERATION_UPDATE;
    return groupCounterProcess(oper,globalHexDisplay,start,end);    
}



void testDramCounter()
{
    int hexDisplay=1;
    int oper=OPERATION_UPDATE | OPERATION_SHOW;
    
    flowCounterProcess(oper,1,0,10);
    
    clsHitCounterProcess(oper,hexDisplay,0,10);
    
    accessPortCounterProcess(oper,hexDisplay,1,10,1);
    
    //accessPortVlanCounterProcess(oper,hexDisplay,1,10,1);
    
    gewPortVlanCounterProcess(oper,hexDisplay ,1,1,1);
    
    linkCounterProcess(oper,hexDisplay,0,10);
    
    groupCounterProcess(oper,hexDisplay ,0,10);

}
#endif
#ifdef INCLUDE_TABLE_RW_FUNC
/*******************************************************************************
*
* ipmuxTableRead - read internal memory table of ipmux
*
* DESCRIPTION:
*   indirect read 32-bit data from  internal table of ipmux implemented in SRAM.
*
* INPUT:
*   tabId   - the id of table Aperture defined in the datasheet
*   offset  - the offset from the space of the specified memory region table, i.e., record NO.
*   nRecNum - the number of records to be read.
*
* OUTPUT:
*   pVal            - hold the value of the memory unit read from
*
* RETURNS:
*   OPL_OK, if the read successful.
*
* SEE ALSO: 
*/



OPL_STATUS ipmuxTableRead (IPMUX_TAB_e region, UINT16 offset, int nRecNum, volatile UINT32* pVal)
{
    int nNum;       /* number of words in a record */
    int nTabLen;    /* number of records in a table */
    int nFromRec = OPL_ZERO;
    UINT32 nSetx = ((IPMUX_TAB_US_FSHP<=region) && (IPMUX_TAB_UPPT > region) ? (region % 2 ? 3 : 2) : (region & REG_BIT_SRS) >> REG_BIT_SRS_OFF);
    UINT32 regVal = OPL_ZERO;
    volatile UINT32 *pDst;
    volatile UINT32 *pSrc;
    int i;
    char * pDesc;
    
    /* validation of parameters */
    if (OPL_NULL == pVal)
    {
        opl_logerr(("OPL_NULL pointer passed.\n"));
        return OPL_ERR_NO(OPL_ERR_INVALID_PARAMETERS);
    }
    
    /* check table id */
    if (IPMUX_TAB_END <= region)
    {
        opl_logerr(("Invlid Table Id %d passed.\n", region));
        return OPL_ERR_NO(OPL_ERR_UNKNOWN_MEM_REGION);
    }
    
    /* lookup table id */
    for (i = 0;  tab_desc[i].desc; i++)
        if (region == tab_desc[i].id) break;
        
        if (OPL_NULL == tab_desc[i].desc) /* no found */
        {
            opl_logerr(("Invlid Table Id %d passed.\n", region));
            return OPL_ERR_NO(OPL_ERR_UNKNOWN_MEM_REGION);
        }
        
        nNum = tab_desc[i].width;       /* number of words in a record */
        nTabLen = tab_desc[i].len;  /* number of records in a table */
        pDesc = tab_desc[i].desc;
        opl_log(("Found Table: Id=%d, %s.\n", region, pDesc));
        
        if ((OPL_ZERO >= nRecNum) || (offset >= nTabLen))
        {
            opl_logerr(("Invlid record number passed, number of records=%d, start from record %d.\n", nRecNum, offset));
            return OPL_ERR_NO(OPL_ERR_INVALID_PARAMETERS);
        }
        
        if (nTabLen < (nRecNum + offset))
        {
            opl_log(("too many records required: Table length is %d.\n", nTabLen));
            nRecNum = nTabLen - offset;
        }
        
        for (nFromRec = offset, pDst = pVal; nFromRec < (offset + nRecNum); nFromRec++)
        {
            
            regVal = 0;
            /* issue the read request command: recorde address and read command */
            regVal = REG_BIT_TBUSY |IPMUX_TABLE_READ | ((REG_BIT_RECNO & (UINT32)nFromRec ) << REG_BIT_RECNO_OFF) | (REG_BIT_TID & (region << REG_BIT_TID_OFF));
            DBGTAB("func_table_tab read : nSetx=%x addr=%x val=%x\n", nSetx,REG_TAB_SHDW_REG_SETx_CTRL(nSetx), regVal);
            ipMuxRegWrite (REG_TAB_SHDW_REG_SETx_CTRL(nSetx), regVal);
            
#if (FPGA_DEMO == 0)
            /* waiting until the IPMux done */
            IPMUX_MEM_ACCESS_POLLING(REG_TAB_SHDW_REG_SETx_CTRL(nSetx), REG_BIT_TBUSY);
#endif
            /*oprintf (("regVal=%#x, after or regVal=%#x\n", regVal, regVal));*/
            
            /* IPMux responses ok, copy data to buffer inputed */
            /* data and register cooresponding:
            *           data buffer: pDst=pVal
            *   |PDst[0]    |PDst[1]        |...        |PDst[n-1]  |PDst[n]    |
            *           data register(pSrc):
            *   |DATAn  |DATAn-1    |...        |DATA1      |DATA0  |
            *
            */
#ifdef OPULAN_RTOS_VXWORKS
            for (i = 0, pSrc = (UINT32 *)IPMUX_REG_ID2ADDR(REG_TAB_SHDW_REG_SETx_DATAx(nSetx, nNum - 1)); i < nNum; i++, pDst++, pSrc--)
                pDst[0] = pSrc[0]; /* note: always the first element, because pDst is changed every time */
#else
            
            for (i = 0, pSrc = (UINT32*)REG_TAB_SHDW_REG_SETx_DATAx(nSetx, nNum - 1); i < nNum; i++, pDst++, pSrc--)
            {
                DBGTAB("fucn_readTab : addr=%x val=%x\n", (UINT32)pSrc, pDst);
                ipMuxRegRead ((UINT32)pSrc, pDst);/* note: always the first element, because pDst is changed every time */
            }
#endif
        }
        
        opl_log(("Read Table %d from Record %d , total %d ok, record length %d W(s).\n", region, offset, nRecNum, nNum));
        return OPL_OK;
}
/*******************************************************************************
*
* ipmuxTableWrite - write internal memory table of ipmux
*
* DESCRIPTION:
*   indirect write 32-bit-width data to  internal memory table of ipmux.
*
* INPUT:
*   tabId   - the id of table Aperture defined in the datasheet
*   offset  - the offset from the space of the specified memory region table, i.e., record NO.
*   nRecNum - the number of records to be written.
*   val     - the 32-bit value array be written to the memory table
*
* OUTPUT:
*   none
*
* RETURNS:
*   OPL_OK, if the read successful.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxTableWrite (IPMUX_TAB_e region, UINT16 offset, int nRecNum, volatile UINT32* pVal)
{
    int nNum;       /* number of words in a record */
    int nTabLen;    /* number of records in a table */
    int nFromRec = OPL_ZERO;
#if 0   
    UINT32 nSetx = (region & REG_BIT_SRS) >> REG_BIT_SRS_OFF;
#else
    UINT32 nSetx = ((IPMUX_TAB_US_FSHP<=region) && (IPMUX_TAB_UPPT > region) ? (region % 2 ? 3 : 2) : (region & REG_BIT_SRS) >> REG_BIT_SRS_OFF);
#endif
    volatile UINT32 *pDst;
    volatile UINT32 *pSrc;
    int i;
    UINT32 regVal = 0;
    char * pDesc;
    
    /* validation of parameters */
    if (OPL_NULL == pVal)
    {
        opl_log(("OPL_NULL pointer passed.\n"));
        return OPL_ERR_NO(OPL_ERR_INVALID_PARAMETERS);
    }
    
    /* check table id */
    if (IPMUX_TAB_END <= region)
    {
        opl_log(("Invlid Table Id %d passed.\n", region));
        return OPL_ERR_NO(OPL_ERR_UNKNOWN_MEM_REGION);
    }
    
    /* lookup table id */
    for (i = 0;  tab_desc[i].desc; i++)
        if (region == tab_desc[i].id) break;
        
        if (OPL_NULL == tab_desc[i].desc) /* no found */
        {
            opl_log(("Invlid Table Id %d passed.\n", region));
            return OPL_ERR_NO(OPL_ERR_UNKNOWN_MEM_REGION);
        }
        
        nNum = tab_desc[i].width;       /* number of words in a record */
        nTabLen = tab_desc[i].len;  /* number of records in a table */
        pDesc = tab_desc[i].desc;
        opl_log(("Found Table: Id=%d, %s.\n", region, pDesc));
        
        if ((OPL_ZERO >= nRecNum) || (offset >= nTabLen))
        {
            opl_log(("Invlid record number passed, number of records=%d, start from record %d.\n", nRecNum, offset));
            return OPL_ERR_NO(OPL_ERR_INVALID_PARAMETERS);
        }
        
        if (nTabLen < (nRecNum + offset))
        {
            opl_log(("too many records required: Table length is %d.\n", nTabLen));
            nRecNum = nTabLen - offset;
        }
        
        for (nFromRec = offset, pSrc = pVal; nFromRec < (offset + nRecNum); nFromRec++)
        {
            
        /* data and register cooresponding:
        *           data buffer: pSrc=pVal
        *   |PSrc[0]    |PSrc[1]        |...        |PSrc[n-1]  |PSrc[n]    |
        *           data register(pDst):
        *   |DATAn  |DATAn-1    |...        |DATA1      |DATA0  |
        *
            */
            /* copy data in buffer to IPMux Shadow Register Set Data Registers */
#ifdef OPULAN_RTOS_VXWORKS
            for (i = 0, pDst = (UINT32 *)IPMUX_REG_ID2ADDR(REG_TAB_SHDW_REG_SETx_DATAx(nSetx, nNum - 1)); i < nNum; i++, pSrc++, pDst--)
                pDst[0] = pSrc[0];  /* note: always the first element, because pSrc is change every time */
#else
            for (i = 0, pDst = (UINT32 *)REG_TAB_SHDW_REG_SETx_DATAx(nSetx, nNum - 1); i < nNum; i++, pSrc++, pDst--)
            {
                DBGTAB("ipMuxRegWrite   addr=%x val=%x\n",(UINT32)&pDst[0], pSrc[0]);
                ipMuxRegWrite ((UINT32)&pDst[0], pSrc[0]);/* note: always the first element, because pSrc is change every time */
            }
            
#endif
            /* issue the read request command: recorde address and read command */
            regVal = 0;
            regVal = REG_BIT_TBUSY | IPMUX_TABLE_WRITE | (REG_BIT_RECNO & (nFromRec << REG_BIT_RECNO_OFF)) | (REG_BIT_TID & (region << REG_BIT_TID_OFF));
            
            DBGTAB("tableWrite addr %x val %x\n",REG_TAB_SHDW_REG_SETx_CTRL(nSetx),regVal);
            ipMuxRegWrite (REG_TAB_SHDW_REG_SETx_CTRL(nSetx), regVal);
            
#if (FPGA_DEMO == 0)
            /* waiting until the IPMux done */
            IPMUX_MEM_ACCESS_POLLING(REG_TAB_SHDW_REG_SETx_CTRL(nSetx), REG_BIT_TBUSY);
#endif
            
        }
        
        /* IPMux responses ok and return OK */
        opl_log(("Write Table %d from Record %d , total %d ok, record length %d W(s).\n", region, offset, nRecNum, nNum));
        return OPL_OK;
}
#endif


