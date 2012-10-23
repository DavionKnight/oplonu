/*
=============================================================================
     Header Name: ipmux_tab_test.c

     General Description:
                IPMux tab test file.
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 xwang             03/5/06       Initial Version
*/
#include <configs/opipmux.h>
#ifdef INCLUDE_TAB_TEST

#include <common.h>
#include "extra.h" 


extern IPMUX_TAB_DESC_t tab_desc[];

typedef struct IPMUX_REGISTER_TEST_s
{
    UINT32 regId;
    UINT32 num;
    UINT32 bits_rw;
    UINT32 bits_w;
    UINT32 bits_r;
    UINT32 bits_default;
    UINT32 flag;    /* RC=2, WC=4 */
}IPMUX_REGISTER_TEST_t;


/* struct for access to IPMux Internal Table */
typedef struct IPMUX_TAB_DESC_TEST_s
{
    int id;            /* table id */
    int skip;
    int len;            /* number of table records */
    int width;        /* length of table record, in words */

    char *desc;        /* description to table */
    int bitWidth;    /* length of table record, in bit */
    int errs;
} IPMUX_TAB_DESC_TEST_t;


IPMUX_TAB_DESC_TEST_t tab_desc_test[] = {
    {IPMUX_TAB_PORT,            0, IPMUX_TAB_PORT_LEN,                  IPMUX_TAB_PORT_WIDTH,              "Bridge Port table (PTable)"                                       ,65    },                
    {IPMUX_TAB_ARL,             0, IPMUX_TAB_ARL_LEN,                   IPMUX_TAB_ARL_WIDTH,               "Bridge Address Resolution logic table "                           ,87   },                
    {IPMUX_TAB_VLAN_PTR,        0, IPMUX_TAB_VLAN_PTR_LEN,              IPMUX_TAB_VLAN_PTR_WIDTH,          "Bridge Vlan Pointer Table"                                        ,18   },                
    {IPMUX_TAB_VLAN_HASH_A,     0, IPMUX_TAB_VLAN_HASH_A_LEN,           IPMUX_TAB_VLAN_HASH_A_WIDTH,       "Bridge Vlan Hash Table A"                                         ,44   },                
    {IPMUX_TAB_VLAN_HASH_B,     0, IPMUX_TAB_VLAN_HASH_B_LEN,           IPMUX_TAB_VLAN_HASH_B_WIDTH,       "Bridge Vlan Hash Table B"                                         ,44   },                
    {IPMUX_TAB_VLAN,            0, IPMUX_TAB_VLAN_LEN,                  IPMUX_TAB_VLAN_WIDTH,              "Bridge VLAN table (VTable)"                                       ,384  },                
    {IPMUX_TAB_VLAN_REP,        0, IPMUX_TAB_VLAN_REP_LEN,              IPMUX_TAB_VLAN_REP_WIDTH,          "Bridge CTAG replacement table"                                    ,25   },                
    {IPMUX_TAB_VLAN_MEM,        0, IPMUX_TAB_VLAN_MEM_LEN,              IPMUX_TAB_VLAN_MEM_WIDTH,          "Bridge VLAN Cross Connection Enable table"                        ,128  },                
    {IPMUX_TAB_SACT  ,          0, IPMUX_TAB_SACT_LEN,                  IPMUX_TAB_SACT_WIDTH,              "Bridge MAC Filter Table, also source address control talbe, SACT" ,49   },                
    {IPMUX_TAB_REMARK,          0, IPMUX_TAB_REMARK_LEN,                IPMUX_TAB_REMARK_WIDTH,            "Bridge remark table"                                              ,6    },                
    {IPMUX_TAB_MC_PTR,          0, IPMUX_TAB_MC_PTR_LEN,                IPMUX_TAB_MC_PTR_WIDTH,            "Bridge MC pointer table, Multicast Group table"                   ,10   },                
    {IPMUX_TAB_MC,              0, IPMUX_TAB_MC_LEN,                    IPMUX_TAB_MC_WIDTH,                "Bridge MC Table, Multicast Group member table"                    ,24   },                
    {IPMUX_TAB_EGRESS_PORT,     0, IPMUX_TAB_EGRESS_PORT_LEN,           IPMUX_TAB_EGRESS_PORT_WIDTH,       "Bridge Port Egress Contrl table (ECT)"                            ,2    },                
    {IPMUX_TAB_LIMIT,           0, IPMUX_TAB_LIMIT_LEN,                 IPMUX_TAB_LIMIT_WIDTH,             "Bridge MAC limitation table"                                      ,27   },                
    {IPMUX_TAB_LIMIT_CNT,       1, IPMUX_TAB_LIMIT_CNT_LEN,             IPMUX_TAB_LIMIT_CNT_WIDTH,         "Bridge MAC limitation counter table"                              ,16   },                
    {IPMUX_TAB_PRIORITY_TAG,    0, IPMUX_TAB_PRIORITY_TAG_LEN,          IPMUX_TAB_PRIORITY_TAG_WIDTH,      "Bridge VLAN priority tag table"                                   ,16   },                
    {IPMUX_TAB_RULE_MASK,       0, IPMUX_TAB_RULE_MASK_LEN,             IPMUX_TAB_RULE_MASK_WIDTH,         "Classify Rule Mask table"                                         ,549  },                
    {IPMUX_TAB_RULE_HASH,       0, IPMUX_TAB_RULE_HASH_LEN,             IPMUX_TAB_RULE_HASH_WIDTH,         "Classify rule hash address table"                                 ,48   },                
    {IPMUX_TAB_RULE,            0, IPMUX_TAB_RULE_LEN,                  IPMUX_TAB_RULE_WIDTH,              "Classify rule table"                                              ,554  },                
    {IPMUX_TAB_FLOW_MASK,       0, IPMUX_TAB_FLOW_MASK_LEN,             IPMUX_TAB_FLOW_MASK_WIDTH,         "Classify Flow ID mask table"                                      ,47   },                
    {IPMUX_TAB_FLOW_HASH,       0, IPMUX_TAB_FLOW_HASH_LEN,             IPMUX_TAB_FLOW_HASH_WIDTH,         "Classify flow address pointer (hash) table"                       ,11   },                
    {IPMUX_TAB_FLOW,            0, IPMUX_TAB_FLOW_LEN,                  IPMUX_TAB_FLOW_WIDTH,              "Classify Flow table"                                              ,61   },                
    {IPMUX_TAB_FLOW_RMK,        0, IPMUX_TAB_FLOW_RMK_LEN,              IPMUX_TAB_FLOW_RMK_WIDTH,          "Classify Flow Remark rule table"                                  ,6    },                
    {IPMUX_TAB_US_FSHP,         0, IPMUX_TAB_US_FSHP_LEN,               IPMUX_TAB_US_FSHP_WIDTH,           "Upstream Flow Shaper Table"                                       ,4    },                
    {IPMUX_TAB_US_FWRED,        0, IPMUX_TAB_US_FWRED_LEN,              IPMUX_TAB_US_FWRED_WIDTH,          "Upstream Flow WRED Table"                                         ,4    },                
    {IPMUX_TAB_US_FLOW_R,       1, IPMUX_TAB_US_FLOW_R_LEN,             IPMUX_TAB_US_FLOW_R_WIDTH,         "Upstream Flow Table (Read Only)"                                  ,10   },                
    {IPMUX_TAB_US_FLOW_W,       1, IPMUX_TAB_US_FLOW_W_LEN,             IPMUX_TAB_US_FLOW_W_WIDTH,         "Upstream Flow Table (Write Only)"                                 ,12   },                
    {IPMUX_TAB_DS_FSHP,         0, IPMUX_TAB_DS_FSHP_LEN,               IPMUX_TAB_DS_FSHP_WIDTH,           "Downstream Flow Shaper Table"                                     ,5    },                
    {IPMUX_TAB_DS_FWRED,        0, IPMUX_TAB_DS_FWRED_LEN,              IPMUX_TAB_DS_FWRED_WIDTH,          "Downstream Flow WRED Table"                                       ,5    },                
    {IPMUX_TAB_DS_FPSEQ,        1, IPMUX_TAB_DS_FPSEQ_LEN,              IPMUX_TAB_DS_FPSEQ_WIDTH,          "Downstream Flow Port Sequence Table (Read Only)"                  ,12   },                
    {IPMUX_TAB_DS_FPWPRI,       1, IPMUX_TAB_DS_FPWPRI_LEN,             IPMUX_TAB_DS_FPWPRI_WIDTH,         "Downstream Port Weight and Priority Table (Read Only)"            ,21   },                
    {IPMUX_TAB_DS_FPWSUM,       1, IPMUX_TAB_DS_FPWSUM_LEN,             IPMUX_TAB_DS_FPWSUM_WIDTH,         "Downstream Port Weight Sum Table,write as 120 bits,read 96 bits"  ,120  },                
    {IPMUX_TAB_DS_FLOW_W,       1, IPMUX_TAB_DS_FLOW_W_LEN,             IPMUX_TAB_DS_FLOW_W_WIDTH,         "Downstream Flow Table (Write Only)"                               ,32   },                
    {IPMUX_TAB_DS_PSEQ_R,       1, IPMUX_TAB_DS_PSEQ_R_LEN,             IPMUX_TAB_DS_PSEQ_R_WIDTH,         "Downstream Port Sequence Table (Read Only)"                       ,10   },                
    {IPMUX_TAB_UPPT,            0, IPMUX_TAB_UPPT_LEN,                  IPMUX_TAB_UPPT_WIDTH,              "SAR UPPT"                                                         ,23   },                 
    {IPMUX_TAB_EPPT,            0, IPMUX_TAB_EPPT_LEN,                  IPMUX_TAB_EPPT_WIDTH,              "SAR EPPT"                                                         ,103  },                
    {IPMUX_TAB_HOST_RX,         0, IPMUX_TAB_HOST_RX_LEN,               IPMUX_TAB_HOST_RX_WIDTH,           "HOST Rx BD table, DMA0 BD table"                                  ,12   },                
    {IPMUX_TAB_HOST_TX,         0, IPMUX_TAB_HOST_TX_LEN,               IPMUX_TAB_HOST_TX_WIDTH,           "HOST Tx BD table, DMA0 BD table"                                  ,12   },                
    {IPMUX_TAB_FE_RX,           1, IPMUX_TAB_FE_RX_LEN,                 IPMUX_TAB_FE_RX_WIDTH,             "Fast Ethernet Rx BD table, DMA1 BD table"                         ,12   },                
    {IPMUX_TAB_FE_TX,           1, IPMUX_TAB_FE_TX_LEN,                 IPMUX_TAB_FE_TX_WIDTH,             "Fast Ethernet Tx BD table, DMA1 BD table"                         ,12   },                
    
    {IPMUX_TAB_SAR_QS,          1, IPMUX_TAB_SAR_QS_LEN,                IPMUX_TAB_SAR_QS_WIDTH,            "SAR Queue Status Table"                                           ,32   },
    {IPMUX_TAB_SAR_CNT_APCTRL,  1, IPMUX_TAB_SAR_CNT_APCTRL_LEN,        IPMUX_TAB_SAR_CNT_APCTRL_WIDTH,    "SAR Access Port Tx/Rx Counter Control Table"                      ,32   },
    {IPMUX_TAB_SAR_CNT_APGRPRX, 1, IPMUX_TAB_SAR_CNT_APGRPRX_LEN,       IPMUX_TAB_SAR_CNT_APGRPRX_WIDTH,   "SAR Access Port Rx Counter Group Table"                           ,32   },
    {IPMUX_TAB_SAR_CNT_APGRPTX, 1, IPMUX_TAB_SAR_CNT_APGRPTX_LEN,       IPMUX_TAB_SAR_CNT_APGRPTX_WIDTH,   "SAR Access Port Tx Counter Group Table"                           ,32   },
    {IPMUX_TAB_SAR_CNT_PVPTR,   1, IPMUX_TAB_SAR_CNT_PVPTR_LEN,         IPMUX_TAB_SAR_CNT_PVPTR_WIDTH,     "SAR Port VLAN Counter Pointer Table"                              ,32   },
    {IPMUX_TAB_SAR_CNT_PVHASH,  1, IPMUX_TAB_SAR_CNT_PVHASH_LEN,        IPMUX_TAB_SAR_CNT_PVHASH_WIDTH,    "SAR Port VLAN Counter Hash Table"                                 ,32   },
    {IPMUX_TAB_SAR_CNT_PVIN,    1, IPMUX_TAB_SAR_CNT_PVIN_LEN,          IPMUX_TAB_SAR_CNT_PVIN_WIDTH,      "SAR Port VLAN In Frame Statistics Group Table"                    ,32   },
    {IPMUX_TAB_SAR_CNT_PVOUT,   1, IPMUX_TAB_SAR_CNT_PVOUT_LEN,         IPMUX_TAB_SAR_CNT_PVOUT_WIDTH,     "SAR Port VLAN Out Frame Statistics Group Table"                   ,32   },
    {IPMUX_TAB_SAR_CNT_VLANPTR, 1, IPMUX_TAB_SAR_CNT_VLANPTR_LEN,       IPMUX_TAB_SAR_CNT_VLANPTR_WIDTH,   "SAR GEW VLAN Counter Pointer Table"                               ,32   },
    {IPMUX_TAB_SAR_CNT_VLANHASH,1, IPMUX_TAB_SAR_CNT_VLANHASH_LEN,      IPMUX_TAB_SAR_CNT_VLANHASH_WIDTH,  "SAR GEW VLAN Counter Hash Table"                                  ,32   },
    {IPMUX_TAB_SAR_CNT_VLANIN,  1, IPMUX_TAB_SAR_CNT_VLANIN_LEN,        IPMUX_TAB_SAR_CNT_VLANIN_WIDTH,    "SAR GEW VLAN In Frame Statistics Group Table"                     ,32   },
    {IPMUX_TAB_SAR_CNT_VLANOUT, 1, IPMUX_TAB_SAR_CNT_VLANOUT_LEN,       IPMUX_TAB_SAR_CNT_VLANOUT_WIDTH,   "SAR GEW VLAN Out Frame Statistics Group Table"                    ,32   },
    {IPMUX_TAB_ATB_CNT_RXCL,    1, IPMUX_TAB_ATB_CNT_RXCL_LEN,          IPMUX_TAB_ATB_CNT_RXCL_WIDTH,      "Rx cells per Link,except asm"                                     ,32   },
    {IPMUX_TAB_ATB_CNT_RXCG,    1, IPMUX_TAB_ATB_CNT_RXCG_LEN,          IPMUX_TAB_ATB_CNT_RXCG_WIDTH,      "Rx cells per Group,except asm"                                    ,32   },
    {IPMUX_TAB_ATB_CNT_RXAL,    1, IPMUX_TAB_ATB_CNT_RXAL_LEN,          IPMUX_TAB_ATB_CNT_RXAL_WIDTH,      "Rx asm cells per Link"                                            ,32   },
    {IPMUX_TAB_ATB_CNT_TXCL,    1, IPMUX_TAB_ATB_CNT_TXCL_LEN,          IPMUX_TAB_ATB_CNT_TXCL_WIDTH,      "Tx cells per Link,except asm"                                     ,32   },
    {IPMUX_TAB_ATB_CNT_TXCG,    1, IPMUX_TAB_ATB_CNT_TXCG_LEN,          IPMUX_TAB_ATB_CNT_TXCG_WIDTH,      "Tx cells per Group,except asm"                                    ,32   },
    {IPMUX_TAB_ATB_CNT_TXAL,    1, IPMUX_TAB_ATB_CNT_TXAL_LEN,          IPMUX_TAB_ATB_CNT_TXAL_WIDTH,      "Tx asm cells per Link"                                            ,32   },
                                                                                                                                                                            
    {IPMUX_TAB_END,             0,                          0,                                     0,          NULL                                              ,0    }
};

void printArrU32(UINT32 arr[],int iArrSize,UINT32* mask)
{
    int j;
    
    for(j=0; j< iArrSize ; j++) {
        printf("%08x",(mask==NULL?arr[j] :arr[j] & mask[j]));
        if ((j % 6 ==0) && (j!=0))   
        {
            printf("\n                          ");
        }
        else
            if (j!= iArrSize-1) printf("-");
        
    }

    printf("\n");
}

/*******************************************************************************
*
* strtoul_range
*
* DESCRIPTION:
*    Convert a string of some u32 to array of u32 .
*
*
* INPUTS:
*    strSrc    - the string of some u32.
*    result    - the result array pointer.
*
* OUTPUT:
*    n/a
*
* RETURNS:
*    OPL_OK,                    convert successfully.
*    other,                     convert failed.
*
* SEE ALSO: 
*/
int strtoul_range(char strSrc[],UINT32 *result)
{
    int i,j,curLen,ilen,iArrSize;
    UINT32 *curResult=result;
    char curStr[9];
    
    /* init var  */ 
    ilen=strlen(strSrc);
    i=0;
    iArrSize=0;
    
    /* convert str to long  */ 
    while (i<ilen) {
        if ((ilen-i)<8) 
        {
            curLen=(ilen-i);
        }
        else
        {
            curLen=8;
        }
        
        i+=curLen;
        
        for(j=0; j<curLen; j++)
        {
            curStr[j]=strSrc[ilen-i+j];
        }
        
        iArrSize++;
        curStr[curLen]=0;
        *curResult=strtoul(curStr,NULL,16);
        //printf("curResult=%u\n",*curResult);
        curResult++;
    }
    return iArrSize;
}

int arrReverse(UINT32 *result,int ilen)
{
    

    UINT32 v=0;
    int j=0;
    
    for(j=0; j<ilen/2; j++)
    {
        v=result[j];
        result[j]=result[ilen-1-j];
        result[ilen-1-j]=v;
    }
    
    return 1;
}


/*******************************************************************************
*
* tableSearch
*
* DESCRIPTION:
*    Test the IPMUX registers.
*
*
* INPUTS:
*    pu4RegId     - the register offset.
*
* OUTPUT:
*    n/a
*
* RETURNS:
*    OPL_ERROR,                    Register not founded.
*    other,                        The index of the register in the array ipmuxReg.
*
* SEE ALSO: 
*/
static void tableClearErr(void)
{
    int i;
    
    
    /* lookup table id */
    for (i = 0;  tab_desc_test[i].desc; i++)
    {
        if (NULL == tab_desc_test[i].desc) /* no found */
        {
            return;
        }

        tab_desc_test[i].errs=0;    
    }
    
}



/*******************************************************************************
*
* tableSearch
*
* DESCRIPTION:
*    Test the IPMUX registers.
*
*
* INPUTS:
*    pu4RegId     - the register offset.
*
* OUTPUT:
*    n/a
*
* RETURNS:
*    OPL_ERROR,                    Register not founded.
*    other,                        The index of the register in the array ipmuxReg.
*
* SEE ALSO: 
*/
static int tablePrintErr(void)
{
    int i;
    int pheader=0;
    
    /* lookup table id */
    for (i = 0;  tab_desc_test[i].desc; i++)
    {
        
            
        if (NULL == tab_desc_test[i].desc) /* no found */
        {
            return OPL_OK;
        }
        if(tab_desc_test[i].errs!=0)
        {
            if (0==pheader)
                {
                    printf("\n   Error table list: \n");
                    pheader=1;
                }
            printf("     table %d occured %d errors, table desc : %s \n",tab_desc_test[i].id,tab_desc_test[i].errs,tab_desc_test[i].desc);            

        }
    }

    if (0==pheader) printf("\n   No error table ... \n");

    return OPL_OK;
}

/*******************************************************************************
*
* tableSearch
*
* DESCRIPTION:
*    Test the IPMUX registers.
*
*
* INPUTS:
*    pu4RegId     - the register offset.
*
* OUTPUT:
*    n/a
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
    for (i = 0;  tab_desc_test[i].desc; i++)
        if (eTabId == tab_desc_test[i].id) break;
    
    if (NULL == tab_desc_test[i].desc) /* no found */
    {
        printf("Invlid Table Id %d passed.\n", eTabId);
        return OPL_ERR_NO(OPL_ERR_UNKNOWN_MEM_REGION);
    }
    return i;
}


/*******************************************************************************
*
* generatMaskBits
*
* DESCRIPTION:
*    generate the mask according to bitwidth.
*
* INPUTS:
*    kind         - the counter kind.
*
* OUTPUT:
*    The counters current value and accumulative value.
*
* RETURNS:
*    OPL_OK,                       If the operation is successful.
*    OPL_ERROR,                    If the operation is failed.
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
* ipmuxTableTest
*
* DESCRIPTION:
*    Test the IPMUX tables.
*
*
* INPUTS:
*    eTableId    - the register offset.
*    nNum        - the test number of the registers.
*    u1TestValue - the value to write to tested registers.
*    bPrintAll   - the print format : 0 to print only error registers; 1 to print all tested registers.
*
* OUTPUT:
*    The test result.
*
* RETURNS:
*    OPL_OK,                       If the operation is successful.
*    OPL_ERROR,                    If the operation is failed.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxTableTest(IPMUX_TAB_e eTableId,UINT32 uNumTab, UINT32 uoffset, UINT32 uNumRec, char sTestValue[], UINT32 bPrintAll, UINT32 nLoopTimes)
{
    int                          i,j,iNo,istart,iArrSize,m,bPass,n,iStatus;
    // "i wrong type,etableid  type"
    int NumTab, NumTabBad, NumBadRecord, printHeader;
    
    #define TableMaxWidth 20
    
    UINT32 DataWrite[TableMaxWidth];
    UINT32 DataMask[TableMaxWidth];
        
    UINT32 DataRBACK[TableMaxWidth];
    
    UINT32 DataExpect[TableMaxWidth];
    UINT32 DataExpectMask[TableMaxWidth];

    tableClearErr();

    /* check params */
    if (uNumTab<0)
    {
        printf("Invalid paragram uNumTab %d , please check it.\n",uNumTab);
        return OPL_ERROR;
    }
    if (uNumTab==0) 
    {
        uNumTab=1000000;    /* test all records*/
    }
 
    if (eTableId==-1) 
    {
        istart=0;        /* test all tables*/
    }
    else
    {
        istart=tableSearch(eTableId);
        if (istart<0) 
        {
            printf("Regid %x not found, please check it.\n",eTableId);
            return OPL_ERROR;
        }
    }
    uNumTab=uNumTab*nLoopTimes;
    printf("\nStart IPMUX table test, table id=%d, test table number=%u, test record number=%u, testvalue=%s ...\n\n",eTableId,uNumTab,uNumRec,sTestValue); 
    
    /*---------------do basic read and write test--------------*/
    //printf("*************** basic read and write test *************** \n");
    /* write records  */ 
    NumTabBad= 0;
    NumTab = 0;
    
    iNo=0;

    for(n=0;n<nLoopTimes;n++)
    {
    printf("------ basic read and write test, %d of %d times ------ \n",n+1,nLoopTimes);
    for (i = istart;(tab_desc_test[i].desc) && (NumTab<uNumTab); i++)
    {
        if (tab_desc_test[i].skip==1) continue;
        
        NumBadRecord=0;
        printHeader=0;
        NumTab++;
        
        //printf(" -------------- %d %d\n",i,iNo);
        
        if ((tab_desc_test[i].bitWidth>tab_desc_test[i].width*32)  || tab_desc_test[i].bitWidth<=(tab_desc_test[i].width-1)*32)
        {
            printf("!!!!!!!!!!!!!!!!table %d tablid %d bitwidth define error\n",i,tab_desc_test[i].id );

        }
        if (NULL == tab_desc_test[i].desc) break;
        
        /* generate data to write */ 
        strtoul_range(sTestValue,DataWrite);
        
        iArrSize=generatMaskBits(DataMask,tab_desc_test[i].bitWidth, TableMaxWidth);
        /* mask data */ 
        for(j=0; j< iArrSize ; j++)
        {
            DataWrite[j]&=DataMask[j];
        }
        /* clear data exceed the mask width */ 
        for(j=iArrSize; j< TableMaxWidth ; j++)
        {
            DataWrite[j]=0;
        }
        arrReverse(DataWrite,iArrSize);
        
        
        
        
        /* write data  */ 
        for(j=uoffset; j<((((uNumRec<=tab_desc_test[i].len) && (uNumRec>0))?uNumRec:tab_desc_test[i].len) )+uoffset; j++)
        {
            ipmuxTableWrite(tab_desc_test[i].id,j,1,DataWrite);
        }

        /* read back and compare record data  */ 
        for(j=uoffset; j<((((uNumRec<=tab_desc_test[i].len) && (uNumRec>0))?uNumRec:tab_desc_test[i].len) )+uoffset; j++)
        {
            //printf("    read table:table=%d, rec=%d \n",tab_desc_test[i].id,j);
            memset(DataRBACK,0,4*TableMaxWidth);
        iStatus=ipmuxTableRead(tab_desc_test[i].id,j,1,DataRBACK);
        if (OPL_OK!=iStatus) 
        {
            return OPL_ERROR;
        }
            bPass=1;
            for(m=0; m<iArrSize  ; m++)
            {
                if (DataWrite[m]!= DataRBACK[m]) 
                {
                    //printf("Found error:DataWrite[m]=%x,DataRBACK[m] =%x,DataMask[m]\n",DataWrite[m],DataRBACK[m] ,DataMask[m]);
                    bPass=0;
                    break;
                }
            }
            if (bPass==0) 
            {
                //printf("bPass=%d\n",bPass);
                //printf("Found error:DataWrite[m]=%x,DataRBACK[m] =%x,DataMask[m]\n",DataWrite[m],DataRBACK[m] ,DataMask[m]);
                NumBadRecord++;
                tab_desc_test[i].errs++;
            }
            if (bPrintAll || (bPass==0))
            {
                /*------ print header ------*/ 
                if (0==printHeader)
                {
                    printf("  Table %d : %s \n",tab_desc_test[i].id,tab_desc_test[i].desc);
                    arrReverse(DataMask,iArrSize);
                    printf("               Mask   = 0x");
                    printArrU32(DataMask,iArrSize,NULL);
                    printf("               Expect = 0x");
                    printArrU32(DataWrite,iArrSize,NULL);
                    printf("\n");
                    
                    
                    printf ("      %3s %6s %8s %8s\n","#","offset","result", "readback");
                    printf ("      %3s %6s %8s %8s\n","---","------" ,"--------","--------");
                    printHeader=1;
                }
                /*printdata*/
                printf ("      %3d %6x %8s ", \
                    iNo++,j,bPass?"":"x");
                //printArrU32(DataRBACK,iArrSize,DataMask);
                printArrU32(DataRBACK,iArrSize,OPL_NULL);
            }
        }
        
        if (NumBadRecord>0)
            printf("\n%s     %d error records in table %d ... \n\n",(NumBadRecord>0)?"x":"",NumBadRecord,tab_desc_test[i].id);
        else
        {
            if (bPrintAll) printf("      passed.\n\n");
        }
        
        if (NumBadRecord>0) 
        {
            NumTabBad++;    
        }
    }
    
    //printf("\n  --------------------------------------------------------------------\n");
    //printf("      IPMUX table test finished : %d tested, %d passed, %d failed.\n", NumTab, NumTab-NumTabBad,NumTabBad);
    //printf("  --------------------------------------------------------------------\n");


    continue;
    /*---------------do bit redundancy test--------------*/
    printf(" *************** bit redundancy test *************** \n");
    /* write records  */ 
    //NumTabBad= 0;
    NumTab = 0;
    
    iNo=0;
        
    for (i = istart;(tab_desc_test[i].desc) && (NumTab<uNumTab); i++)
    {
        if (tab_desc_test[i].skip==1) continue;
        
        NumBadRecord=0;
        printHeader=0;
        NumTab++;
        
        //printf(" -------------- %d %d\n",i,iNo);
        
        if ((tab_desc_test[i].bitWidth>tab_desc_test[i].width*32)  || tab_desc_test[i].bitWidth<=(tab_desc_test[i].width-1)*32)
        {
            printf("!!!!!!!!!!!!!!!!table %d tablid %d bitwidth define error\n",i,tab_desc_test[i].id );

        }
        if (NULL == tab_desc_test[i].desc) break;
        
        /* generate data to write */ 
        strtoul_range(sTestValue,DataWrite);
        
        iArrSize=generatMaskBits(DataMask,(tab_desc_test[i].width+1)*32, TableMaxWidth);
        /* mask data */ 
        for(j=0; j< iArrSize ; j++)
        {
            DataWrite[j]&=DataMask[j];
        }
        /* clear data exceed the mask width */ 
        for(j=iArrSize; j< TableMaxWidth ; j++)
        {
            DataWrite[j]=0;
        }
        arrReverse(DataWrite,iArrSize);
        /* write data  */ 
        ipmuxTableWrite(tab_desc_test[i].id,0,1,DataWrite);
        
        /* generate expect data */ 
        strtoul_range(sTestValue,DataExpect);
        
            
        iArrSize=generatMaskBits(DataExpectMask,tab_desc_test[i].bitWidth, TableMaxWidth);
        arrReverse(DataExpect,iArrSize);
        arrReverse(DataExpectMask,iArrSize);
        /* mask data */ 
        for(j=0; j< iArrSize ; j++)
        {
            DataExpect[j]&=DataExpectMask[j];
        }
        /* clear data exceed the mask width */ 
        for(j=iArrSize; j< TableMaxWidth ; j++)
        {
            DataExpect[j]=0;
        }
    
        //printf(" write over\n");
        

        /* read back and compare record data  */ 
        for(j=uoffset; j<((((uNumRec<=tab_desc_test[i].len) && (uNumRec>0))?uNumRec:tab_desc_test[i].len) )+uoffset; j++)
        {
            //printf("    read table:table=%d, rec=%d \n",tab_desc_test[i].id,j);
            memset(DataRBACK,0,4*TableMaxWidth);
            iStatus=ipmuxTableRead(tab_desc_test[i].id,j,1,DataRBACK);
            if (OPL_OK!=iStatus) 
        {
            return OPL_ERROR;
        }
            bPass=1;
            for(m=0; m<iArrSize  ; m++)
            {
                if (DataExpect[m]!= DataRBACK[m]) 
                {
                    //printf("Found error:DataWrite[m]=%x,DataRBACK[m] =%x,DataMask[m]\n",DataWrite[m],DataRBACK[m] ,DataMask[m]);
                    bPass=0;
                    break;
                }
            }
            if (bPass==0) 
            {
                //printf("bPass=%d\n",bPass);
                //printf("Found error:DataWrite[m]=%x,DataRBACK[m] =%x,DataMask[m]\n",DataWrite[m],DataRBACK[m] ,DataMask[m]);
                NumBadRecord++;
                tab_desc_test[i].errs++;
            }
            if (bPrintAll || (bPass==0))
            {
                /*------ print header ------*/ 
                if (0==printHeader)
                {
                    printf("  Table %d : %s \n",tab_desc_test[i].id,tab_desc_test[i].desc);
                    printf("               Mask   = 0x");
                    printArrU32(DataExpectMask,iArrSize,NULL);
                    printf("               Expect = 0x");
                    printArrU32(DataExpect,iArrSize,NULL);
                    printf("\n");
                    
                    
                    printf ("      %3s %6s %8s %8s\n","#","offset","result", "readback");
                    printf ("      %3s %6s %8s %8s\n","---","------" ,"--------","--------");
                    printHeader=1;
                }
                /*printdata*/
                printf ("      %3d %6x %8s ", \
                    iNo++,j,bPass?"":"x");
                //printArrU32(DataRBACK,iArrSize,DataMask);
                printArrU32(DataRBACK,iArrSize,OPL_NULL);
            }
        }
        
        if (NumBadRecord>0)
            printf("\n%s     %d error records in table %d ... \n\n",(NumBadRecord>0)?"x":"",NumBadRecord,tab_desc_test[i].id);
        else
        {
            if (bPrintAll) printf("      passed.\n\n");
        }
        
        if (NumBadRecord>0) 
        {
            NumTabBad++;    
        }
    }
}
    printf("\n  ----------------------------------------------------------------------------\n");
    printf("   IPMUX table test finished:  %d tested, %d passed, %d failed.\n", NumTab*2, NumTab*2-NumTabBad,NumTabBad);
    tablePrintErr();
    printf("  ----------------------------------------------------------------------------\n");
    
    return    (OPL_OK);
}

#endif
