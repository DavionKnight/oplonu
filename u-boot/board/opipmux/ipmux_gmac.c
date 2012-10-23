/*
=============================================================================
     Header Name: ipmux_gmac.c

     General Description:
All function of interface to hardware, register, table, bitwise operation, are provided in this file.
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
                  Modification
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
yzhang                 07/06/2005      initial
----------------   ------------  ----------------------------------------------
*/


/* include */
#include "opl_debug.h"
//#include "extra.h"
#include "commonforAPI.h"
#include "ipmux_gmac.h"
#include "ipmux_error.h"
#include "ipmux_hw_interface.h"

/* private definitions */

#if !defined (MODULE) && !defined (__KERNEL__)

    /* for debug system */
int nGMACDbgFd = OPL_STDOUT; /* standard output device */

#ifdef opl_log
#undef opl_log
#endif
#define opl_log(fmt)                opllog(&nzDbgFd, nGMACDbgFd, fmt)

#endif
/* global */

/* functions definitions */

#ifdef OPULAN_RTOS_LINUX

    /* Linux Spacial */

#endif /*OPULAN_RTOS_LINUX*/

    /* 
     *  MACRO: GMAC_CHECK_BEGIN(gmacs)
     *      gmacs:  |b31-b3 |b2         |b1         |b0     |
     *              |unused |for GEWL   |for GEW1   |for GEW0           
     */
#define GMAC_CHECK_BEGIN(gmacs, regAddr, regId) { \
    u32 uges = (gmacs) & 7; \
    while (uges){\
        if (IPMUX_GEW0 & uges) {\
            regAddr = (void *)(OP_IPMUX_GEW0_ADDR | (regId));\
            uges &= (~IPMUX_GEW0);\
        }else\
        if (IPMUX_GEW1 & uges) {\
            regAddr = (void*)(OP_IPMUX_GEW1_ADDR | (regId));\
            uges &= (~IPMUX_GEW1);\
        }else\
        if (IPMUX_GEWL & uges) {\
            regAddr = (void *)(OP_IPMUX_GEWL_ADDR | (regId));\
            uges &= (~IPMUX_GEWL);\
        }


#define GMAC_CHECK_END()    }}

void ipmux_gmac_reset(u32 gmacs)
{
    volatile gmac_ctrl_reg *greg = (volatile gmac_ctrl_reg *)OP_IPMUX_GEL_ADDR;
    u32 val;

    GMAC_CHECK_BEGIN(gmacs, greg, REG_GMAC_CFG1);
    
    /* program mac config1 register leve tx and rx disabled */
    val = GMAC_CONFIG_1(    1,  /*soft_reset*/      
                1,  /*sim_reset*/       
                1,  /*reset_rx_cntl*/   
                1,  /*reset_tx_cntl*/  
                1,  /*reset_rx_func*/   
                1,  /*reset_tx_func*/   
                0,  /*loopback*/        
                1,  /*rx_flow_cntl*/    
                1,  /*tx_flow_cntl*/    
                0,  /*rx_enable*/       
                0   /*tx_enable*/
               ) ;
#if defined (MODULE) || defined (__KERNEL__) || defined (OPULAN_RTOS_VXWORKS)
    greg->mac_config1=val;
#else
    greg = (volatile gmac_ctrl_reg *)IPMUX_REG_ADDR2ID((UINT32)greg);
    ipMuxRegWrite (greg->mac_config1, val);
#endif

    GMAC_CHECK_END();
    return;
}

int ipmux_gmac_init(u32 gmacs)
    {
        volatile gmac_ctrl_reg * greg = (volatile gmac_ctrl_reg *)OP_IPMUX_GEL_ADDR;
    u32 val;

    /******** init gel ********/
    ipmux_gmac_reset(gmacs);

    GMAC_CHECK_BEGIN(gmacs, greg, REG_GMAC_CFG1); /* CFG1 is the first of the Control registers */

    val = GMAC_CONFIG_2(    1,  /*full_duplex : 0=half,1=full*/
                1,  /*crc_en*/       
                1,  /*pad_and_crc_en*/   
                0,  /*len_field_check_en*/  
                1,  /*huge_frame_en*/   
                1,  /*interface_mode : 1=10M/100M , 2=1000M*/   
                0x7 /*preamble_length*/
                ) ;
#if defined (MODULE) || defined (__KERNEL__) || defined (OPULAN_RTOS_VXWORKS)
    greg->mac_config2=val;
#else
    greg = (volatile gmac_ctrl_reg *)IPMUX_REG_ADDR2ID((UINT32)greg);
    ipMuxRegWrite (greg->mac_config2, val);
#endif

    /* program mac config1 register leve tx and rx disabled */
    val = GMAC_CONFIG_1(    0,  /*soft_reset*/
                0,  /*sim_reset*/
                0,  /*reset_rx_cntl*/   
                0,  /*reset_tx_cntl*/  
                0,  /*reset_rx_func*/   
                0,  /*reset_tx_func*/
                0,  /*loopback*/
                1,  /*rx_flow_cntl*/
                1,  /*tx_flow_cntl*/
                1,  /*rx_enable*/
                1   /*tx_enable*/
                ) ;
#if defined (MODULE) || defined (__KERNEL__) || defined (OPULAN_RTOS_VXWORKS)
    greg->mac_config1=val;
#else
    ipMuxRegWrite (greg->mac_config1, val);
#endif

    GMAC_CHECK_END();
    
       oprintf (("GMAC initialized ok.\n"));

    return 0;

}



/*******************************************************************************
*
* ipmux_gmac_set_speed
*
* DESCRIPTION:
*   this function sets GMAC speed.  
*
* INPUTS:
*   gmacs       - GE port number.
*   speed       - GE MAC speed: 2=1000M, 1=100M, 0=10M.
*
* OUTPUT:
*   n.a
*
* RETURNS:
*   n.a
*
* SEE ALSO: 
*/
int ipmux_gmac_set_speed(u32 gmacs, u16 speed)
{
    UINT32 regid = 0;
    UINT32 val = 0;

    /******** init gel ********/
    /* ipmux_gmac_reset(gmacs); */

    if ( 0 == gmacs )
        regid = 0x8204;
    else if ( 1 == gmacs )
        regid = 0xA204;

    ipMuxRegRead(regid, &val);

    if (0 == speed || 1 == speed)
    {
        /* 10M or 100M */
        val &= ~(0x3 << 8);
        val |= 0x1 << 8;
    }
    else
    {
        /* 1000M */
        val &= ~(0x3 << 8);
        val |= 0x2 << 8;
    }   

    ipMuxRegWrite (regid, val);

    return 0;
}






/*******************************************************************************
*
* ipmux_gmac_set_duplex
*
* DESCRIPTION:
*   this function sets GMAC speed.  
*
* INPUTS:
*   gmacs       - GE port number.
*   duplex      - GE MAC duplex: 1 = Full duplex, 0 = Half Duplex.
*
* OUTPUT:
*   n.a
*
* RETURNS:
*   n.a
*
* SEE ALSO: 
*/
int ipmux_gmac_set_duplex(u32 gmacs, u16 duplex)
{
    UINT32 regid = 0;
    UINT32 val = 0;

    /******** init gel ********/
    /*  ipmux_gmac_reset(gmacs); */

    if ( 0 == gmacs )
        regid = 0x8204;
    else if ( 1 == gmacs )
        regid = 0xA204;

    ipMuxRegRead(regid, &val);

    if (0 == duplex )
    {
        /* half duplex */
        val &= ~0x1;
    }
    else
    {
        val |= 0x1;
    }   

    ipMuxRegWrite (regid, val);

    return 0;
}





/*Add by xwu 2007/08/14,GE MAC Loopback,GMII/TBI,TBI configure*/

/*******************************************************************************
*
* ipmuxGmacLoopBackSet - GMAC loopback Set
*
* DESCRIPTION:
*   this function sets GMAC loopback.   
*
* INPUTS:
*   gmacs       - GE port number.
*   cfg         - GE MAC Loopback : 1 = loopback, 0 = Normal.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success 
*
* SEE ALSO: 
*/
OPL_STATUS  ipmuxGmacLoopBackSet(UINT32 gmacs,UINT32 cfg)
{

    if((1 != cfg) && (0 != cfg))                                    /*if invaild arg,print Error*/
    {
            oprintf (("ipmuxGmacLoopBackSet :Invaild cfg Num:%d \n", cfg));
            return OPL_FALSE;
    }
    
    switch(gmacs)
    {
        case 0:                                             /*Configure GE0 prot */
            oprintf (("Before ipmuxGmacLoopBackSet :Invaild cfg Num:%d \n", cfg));

            ipMuxRegWrite (OP_IPMUX_GEW0_LPBKCFG, cfg);
            oprintf (("After ipmuxGmacLoopBackSet :Invaild cfg Num:%d \n", cfg));

            break;
        case 1:                                             /*Configure GE1 prot */
            ipMuxRegWrite (OP_IPMUX_GEW1_LPBKCFG, cfg);
            break;
        case 2:                                             /*Configure GE2 prot */
            ipMuxRegWrite (OP_IPMUX_GEW2_LPBKCFG, cfg);
            break;
        default:
            oprintf (("ipmuxGmacLoopBackSet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;
    }

    return OPL_OK;
}


/*******************************************************************************
*
* ipmuxGmacOpModCfg -  Operation Mode Configuration
*
* DESCRIPTION:
*   this function sets GMAC Operation Model.
*                   Operation mode:
*                           00: GMII
*                           01: TBI
*                           1x: Reserved
*
* INPUTS:
*   gmacs       - GE port number.
*   cfg         - GE MAC Loopback : 0 = GMII, 1 = TBI.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
* SEE ALSO: 
*/
OPL_STATUS  ipmuxGmacOpModCfg(UINT32 gmacs,UINT32 cfg)
{
    if((1 != cfg) && (0 != cfg))
    {
            oprintf( ("ipmuxGmacOpModCfg :Invaild cfg Num:%d \n", cfg));
            return OPL_FALSE;
    }
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegWrite (OP_IPMUX_GEW0_OPMODE_CFG, cfg);      /*Configure GE0 prot */
            break;
        case 1:
            ipMuxRegWrite (OP_IPMUX_GEW1_OPMODE_CFG, cfg);      /*Configure GE1 prot */
            break;
        case 2:
            ipMuxRegWrite (OP_IPMUX_GEW2_OPMODE_CFG, cfg);      /*Configure GE2 prot */
            break;
        default:
            oprintf (("ipmuxGmacLpbkSet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;
    }

    return OPL_OK;
}


/*******************************************************************************
*
* gmacTbiCfg0LinkPartCfg - Auto-Negotiation  link partner ability
*
* DESCRIPTION:
*   this function set AN link partner ability register of cfg0.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - link partner value.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
*
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg0LinkPartCfg(UINT32 gmacs,UINT32 val)
{
    UINT32 nTemp = OPL_ZERO;
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG0, &nTemp);      /*Configure GE0 prot */
            nTemp &= (~IPMUX_GEW_CFG0_LP);
            nTemp |= val;
            
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG0, nTemp);
            
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG0, &nTemp);      /*Configure GE1 prot */
            nTemp &= (~IPMUX_GEW_CFG0_LP);
            nTemp |= val;
            
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG0, nTemp);
            break;
        default:
            oprintf( ("gmacTbiCfg0LinkPartCfg :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}

/*******************************************************************************
*
* gmacTbiCfg0AutoNegAdvCfg - Auto-Negotiation advertisement register cfg
*
* DESCRIPTION:
*   this function set Auto-Negotiation advertisement register cfg.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - Auto-Negotiation advertisement.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg0AutoNegAdvCfg(UINT32 gmacs,UINT32 val)
{

    UINT32 nTemp = OPL_ZERO;
    UINT32 vFlag;
    
    vFlag = val;
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG0, &nTemp);      /*Configure GE0 prot */
    
            nTemp &= IPMUX_GEW_CFG1_TX ;
            vFlag     =  vFlag<<16;
            nTemp |= vFlag;
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG0, nTemp);      
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG0, &nTemp);      /*Configure GE1 prot */
            nTemp &= IPMUX_GEW_CFG1_TX;
            vFlag     =  vFlag<<16;
            nTemp |= vFlag;
            
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG0, nTemp);
            break;
        default:
            oprintf( ("gmacTbiCfg0AutoNegAdvCfg :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}

/*******************************************************************************
*
* gmacTbiCfg1NexPageTxCfg - Auto-Negotiation Next page transmit  configure
*
* DESCRIPTION:
*   this function set AN Next page transmit register of cfg1.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - link partner value.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
*
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg1NexPageTxCfg(UINT32 gmacs,UINT32 val)
{
    UINT32 nTemp = OPL_ZERO;
    UINT32 vFlag;

    vFlag = val;
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG1, &nTemp);      /*Configure GE0 prot */

            nTemp &= IPMUX_GEW_CFG1_TX;
            vFlag   = vFlag << 16;
            nTemp |= vFlag;
            
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG1, nTemp);
            
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG1, &nTemp);      /*Configure GE1 prot */
            nTemp &= IPMUX_GEW_CFG1_TX;
            vFlag   = vFlag << 16;
            nTemp |= vFlag;
            
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG1, nTemp);
            break;
        default:
            oprintf( ("gmacTbiCfg1NexPageTxCfg :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}

/*******************************************************************************
*
* gmacTbiCfg1LinkPartNexPagRxCfg - AN link partner next pager cfg
*
* DESCRIPTION:
*   this function set AN link partner next pager ability register of cfg1.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - link partner value.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg1LinkPartNexPagRxCfg(UINT32 gmacs,UINT32 val)
{

    UINT32 nTemp = OPL_ZERO;
    UINT32 vFlag;
    
    vFlag = val;
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG1, &nTemp);      /*Configure GE0 prot */
    
            nTemp &= (IPMUX_GEW_CFG1_TX << 16);
            nTemp |= vFlag;
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG1, nTemp);      
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG1, &nTemp);      /*Configure GE1 prot */
            nTemp &= (IPMUX_GEW_CFG1_TX << 16);
            nTemp |= vFlag;
            
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG1, nTemp);
            break;
        default:
            oprintf( ("GmacTbiCfg1LpNpRxCfg :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}

/*******************************************************************************
*
* gmacTbiCfg2AutoNegEnableSet - TBI Configure 2 Register-Auto-Negotiation enable CFG
*
* DESCRIPTION:
*   this function set Auto-Negotiation enable of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - 1, enable 0 , Disable.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2AutoNegEnableSet(UINT32 gmacs,UINT32 val)
{
    UINT32  cfg;
    
    UINT32 nTemp = OPL_ZERO;
    
    cfg  = ~(IPMUX_REG_BIT0);
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Configure GE0 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG2, nTemp);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Configure GE1 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG2, nTemp);
            break;
        default:
            oprintf( ("gmacTbiCfg2AutoNegEnableSet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}


/*******************************************************************************
*
* GmacTbiCfg2LpbkSet  - TBI Configure 2 Register-Loop back CFG
*
* DESCRIPTION:
*   this function set Loop back of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - 1, enable 0 , Disable.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2LoopBackSet(UINT32 gmacs,UINT32 val)
{
    UINT32  cfg;
    
    UINT32 nTemp = OPL_ZERO;
    
    cfg  = ~(IPMUX_REG_BIT1);
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Configure GE0 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG2, nTemp);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Configure GE1 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG2, nTemp);
            break;
        default:
            oprintf( ("gmacTbiCfg2LoopBackSet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}


/*******************************************************************************
*
* gmacTbiCfg2ResetSet  - TBI Configure 2 Register-Reset CFG
*
* DESCRIPTION:
*   this function set Reset of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - 1, enable 0 , Disable.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2ResetSet(UINT32 gmacs,UINT32 val)
{
    UINT32  cfg;
    
    UINT32 nTemp = OPL_ZERO;
    
    cfg  = ~(IPMUX_REG_BIT2);
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Configure GE0 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG2, nTemp);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Configure GE1 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG2, nTemp);
            break;
        default:
            oprintf( ("GmacTbiCfg2LpbkSet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}

/*******************************************************************************
*
* GmacTbiCfg2NexPagEnSet  - TBI Configure 2 Register-Next page enable CFG
*
* DESCRIPTION:
*   this function set Next page enable of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - 1, enable 0 , Disable.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2NexPagEnSet(UINT32 gmacs,UINT32 val)
{
    UINT32  cfg;
    
    UINT32 nTemp = OPL_ZERO;
    
    cfg  = ~(IPMUX_REG_BIT3);
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Configure GE0 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG2, nTemp);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Configure GE1 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG2, nTemp);
            break;
        default:
            oprintf( ("GmacTbiCfg2NexPagEnSet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}

/*******************************************************************************
*
* GmacTbiCfg2ReStartSet  - TBI Configure 2 Register-Restart
*
* DESCRIPTION:
*   this function set ReStart of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - 1, enable 0 , Disable.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2ReStartSet(UINT32 gmacs,UINT32 val)
{
    UINT32  cfg;
    
    UINT32 nTemp = OPL_ZERO;
    
    cfg  = ~(IPMUX_REG_BIT4);
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Configure GE0 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG2, nTemp);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Configure GE1 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG2, nTemp);
            break;
        default:
            oprintf( ("gmacTbiCfg2ReStartSet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;
    }
    
    return OPL_OK;

}

/*******************************************************************************
*
* GmacTbiCfg2RepModSet  - TBI Configure 2 Register-Repeater mode CFG
*
* DESCRIPTION:
*   this function set Repeater mode of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - 1, enable 0 , Disable.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
*
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2RepModSet(UINT32 gmacs,UINT32 val)
{
    UINT32  cfg;
    
    UINT32 nTemp = OPL_ZERO;
    
    cfg  = ~(IPMUX_REG_BIT5);
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Configure GE0 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG2, nTemp);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Configure GE1 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG2, nTemp);
            break;
        default:
            oprintf( ("GmacTbiCfg2RepModSet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}


/*******************************************************************************
*
* gmacTbiCfg2NexPagLdIndSet   - TBI Configure 2 Register-Next page loaded indicate CFG
*
* DESCRIPTION:
*   this function set Next page loaded indicate register of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*   val         - 1, enable 0 , Disable.
*
* RETURNS:
*   OPL_FALSE     - Configure Failed
*   OPL_OK      - Configure Success
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2NexPagLdIndSet(UINT32 gmacs,UINT32 val)
{
    UINT32  cfg;
    
    UINT32 nTemp = OPL_ZERO;
    
    cfg  = ~(IPMUX_REG_BIT6);
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Configure GE0 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW0_CFG2, nTemp);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Configure GE1 prot */
            nTemp &= cfg;
            nTemp |= val;
            ipMuxRegWrite (OP_IPMUX_GEW1_CFG2, nTemp);
            break;
        default:
            oprintf( ("gmacTbiCfg2NexPagLdIndSet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_FALSE;

    }
    
    return OPL_OK;

}


/*******************************************************************************
*
* gmacTbiCfg2PageRxStatusGet  - TBI Configure 2 Register-Page received 
*
* DESCRIPTION:
*   this function Get Page received flag register of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*
* RETURNS:
*   1 - Page Rcv Flag is set, 0 - Page Rcv Flag not Set.
*
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2PageRxStatusGet(UINT32 gmacs)
{
    UINT32 nTemp = OPL_ZERO;
    INT8     cFlag;
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Read GE0*/
            cFlag = (nTemp & IPMUX_REG_BIT7);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Read GE1*/
            cFlag = (nTemp & IPMUX_REG_BIT7);
            break;
        default:
            oprintf( ("gmacTbiCfg2PageRxStatusGet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_ERROR;

    }

    return cFlag;
}

/*******************************************************************************
*
* gmacTbiCfg2AutoNegStatusGet -  TBI Configure 2 Register-Auto-Negotiation complete Get
*
* DESCRIPTION:
*   this function Get Auto-Negotiation complete flag of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*
* RETURNS:
*   1 - AN Flag is set, 0 - AN Flag not Set.
*
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2AutoNegStatusGet(UINT32 gmacs)
{
    UINT32 nTemp = OPL_ZERO;
    INT8     cFlag;
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Read GE0*/
            cFlag = (nTemp & IPMUX_REG_BIT8);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Read GE1*/
            cFlag = (nTemp & IPMUX_REG_BIT8);
            break;
        default:
            oprintf( ("gmacTbiCfg2AutoNegStatusGet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_ERROR;

    }

    return cFlag;
}

/*******************************************************************************
*
* gmacTbiCfg2LinkSynStatusGet-  TBI Configure 2 Register-Get The status of link synchronization
*
* DESCRIPTION:
*   this function Get The status of link synchronization in TBI mode of cfg2.
*
* INPUTS:
*   gmacs       - GE port number.
*
* RETURNS:
*   1 - AN Flag is set, 0 - AN Flag not Set.
*
* SEE ALSO: 
*/
OPL_STATUS gmacTbiCfg2LinkSynStatusGet(UINT32 gmacs)
{
    UINT32 nTemp = OPL_ZERO;
    INT8     cFlag;
    
    switch(gmacs)
    {
        case 0:
            ipMuxRegRead (OP_IPMUX_GEW0_CFG2, &nTemp);      /*Read GE0*/
            cFlag = (nTemp & IPMUX_REG_BIT9);
            break;
        case 1:
            ipMuxRegRead (OP_IPMUX_GEW1_CFG2, &nTemp);      /*Read GE1*/
            cFlag = (nTemp & IPMUX_REG_BIT9);
            break;
        default:
            oprintf( ("gmacTbiCfg2LinkSynStatusGet :Invaild gmacs Num:%d \n", gmacs));
            return OPL_ERROR;

    }

    return cFlag;
}


/*
#ifdef OPL_IPMUX_E2
*/

/*******************************************************************************
*
* ipmuxGeLinkCfg:   to configure GE Link control
*
* DESCRIPTION:
*   This function configures GE link control register.
* INPUTS:
*   Type    -  indicates which attribute to be set
*   Value   - value to be writen
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, the operation is successful
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxGeLinkCfg(UINT32 Type, UINT32 Value)
{
    IPMUX_GE_LINK_CFG_t regVal; 

    if(IS_IPMUX_E2())
    {
        ipMuxRegRead(IPMUX_REG_GMAC_LAG_CONTROL, (volatile UINT32 *)&regVal);

        switch (Type)
        {
            case IPMUX_GE_LINK_LADA:
                regVal.Lada = Value;
                break;
            case IPMUX_GE_LINK_LCTL:
                regVal.Lctl = Value;
                break;
            case IPMUX_GE_LINK_MODE:
                regVal.Mode = Value;
            break;
                case IPMUX_GE_LINK_DCW:
                regVal.Dcw = Value;
                break;
            case IPMUX_GE_LINK_GUE0:
                regVal.Gue0 = Value;
                break;
            case IPMUX_GE_LINK_GUE1:
                regVal.Gue1 = Value;
                break;
            case IPMUX_GE_LINK_GLD0:
                regVal.Gld0 = Value;
                break;
            case IPMUX_GE_LINK_GLD1:
                regVal.Gld1 = Value;
                break;
            case IPMUX_GE_LINK_GDE0:
                regVal.Gde0 = Value;
                break;
            case IPMUX_GE_LINK_GDE1:
                regVal.Gde1 = Value;
                break;
            case IPMUX_GE_LINK_GRE0:
                regVal.Gre0 = Value;
                break;
            case IPMUX_GE_LINK_GRE1:
                regVal.Gre1 = Value;
                break;
            default:
                return OPL_ERROR;
        }

        /* write to the register */
        ipMuxRegWrite(IPMUX_REG_GMAC_LAG_CONTROL, *(UINT32 *)(&regVal));

        return OPL_OK;
    }else if(IS_IPMUX_E())
    {
        return IPMUX_VERSION_INVALID;
    }else
    {
        return IPMUX_VERSION_INVALID;
    }
    
}

/*******************************************************************************
*
* ipmuxGeLinkShow
*
* DESCRIPTION:
*   This function shows GE link configuration information.
*
* INPUTS:
*   n/a.
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, the operation is successful
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxGeLinkShow (void)
{
    IPMUX_GE_LINK_CFG_t regLinkVal; 
    char * mode[4] = {"Redundancy","Aggregation","Daisy-Chain","Reserved"};
    char * redundancy[4] = {"Link0-Primary","Reserved","Reserved","Link1-Primary"};
    char * distributaion[2] = {"6bit-Algo","1bit-Algo"};
    char * aggregation[4] = {"Both-Disable","Link1-Enable","Link0-Enable","Both-Enable"};
    char * daisyChain[4] = {"Link0-Primary","Reserved","Reserved","Link1-Primary"};
    char * enableStr[2];
    OPL_STATUS iStatus;

    enableStr[0] = "Disa";
    enableStr[1] = "Enab";
    
    iStatus = ipMuxRegRead(IPMUX_REG_GMAC_LAG_CONTROL, (UINT32 *)&regLinkVal);
    if (OPL_OK != iStatus)
    {
        return iStatus;
    }

    if(IS_IPMUX_E())
    {
        printf("GUE1 GUE0 DCW       MODE            LCTL         LADA    RegValue\n");
        printf("===============================================================================================\n");
        printf("%4s %4s %3d %10s",
            enableStr[regLinkVal.Gue1],
            enableStr[regLinkVal.Gue0],
            regLinkVal.Dcw,
            mode[regLinkVal.Mode]
        );
    }else if(IS_IPMUX_E2())
    {
        printf("GRE1 GRE0 GDE1 GDE0 GLD1 GLD0 GUE1 GUE0 DCW       MODE            LCTL         LADA    RegValue\n");
        printf("===============================================================================================\n");
        printf("%4s %4s %4s %4s %4s %4s %4s %4s %3d %10s",
            enableStr[regLinkVal.Gre1],
            enableStr[regLinkVal.Gre0],
            enableStr[regLinkVal.Gde1],
            enableStr[regLinkVal.Gde0],
            enableStr[regLinkVal.Gld1],
            enableStr[regLinkVal.Gld0],
            enableStr[regLinkVal.Gue1],
            enableStr[regLinkVal.Gue0],
            regLinkVal.Dcw,
            mode[regLinkVal.Mode]
        );
    }else
    {
        return IPMUX_VERSION_INVALID;
    }
    

    if (IPMUX_GE_LINK_MODE_REDUNDANCY == regLinkVal.Mode)
    {
        printf(" %15s", redundancy[regLinkVal.Lctl]);
    }
    else if (IPMUX_GE_LINK_MODE_AGGREGATION == regLinkVal.Mode)
    {
        printf(" %15s", aggregation[regLinkVal.Lctl]);
    }
    else if (IPMUX_GE_LINK_MODE_DAISYCHAIN == regLinkVal.Mode)
    {
        printf(" %15s", daisyChain[regLinkVal.Lctl]);
    }
    else if (IPMUX_GE_LINK_MODE_RESERVED == regLinkVal.Mode)
    {
        printf(" %15s", "Unknown");
    }
        
    printf(" %12s  0x%08x\n",distributaion[regLinkVal.Lada], *(UINT32*)&regLinkVal);
    printf("===============================================================================================\n");

    return OPL_OK;
}


/*******************************************************************************
*
* ipmuxGeRsvdMacRcvCfg: to configure GE Reserved Mac Receive control
*
* DESCRIPTION:
*   This function configures GE Reserved Mac Receive control register.
* INPUTS:
*   Type    -  indicates which attribute to be set
*   Value   - value to be writen
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, the operation is successful
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxGeRsvdMacRcvCfg(UINT32 Type, UINT32 Value)
{
    IPMUX_GE_RSVD_MAC_RCV_CFG_t regVal; 

    if(IS_IPMUX_E2())
    {
        ipMuxRegRead(IPMUX_REG_RSVD_MAC_RECEIVE, (volatile UINT32 *)&regVal);

        switch (Type)
        {
            case IPMUX_GE_RSVD_MAC_RCV_GE0:
                regVal.Ge0 = Value;
                break;
            case IPMUX_GE_RSVD_MAC_RCV_GE1:
                regVal.Ge1 = Value;
                break;
            default:
                return OPL_ERROR;
        }

        /* write to the register */
        ipMuxRegWrite(IPMUX_REG_RSVD_MAC_RECEIVE, *(UINT32 *)(&regVal));
    
    }else if(IS_IPMUX_E())
    {
        return IPMUX_VERSION_INVALID;
    }else 
    {
        return IPMUX_VERSION_INVALID;
    }
    return OPL_OK;
}





/*******************************************************************************
*
* ipmuxBrgGeCfg - enable or disable one-bit field in GE control register 1
*
* DESCRIPTION:
*       To enable/disable one-bit field such as receive, transmit, forward
*
* INPUT:
*      Type -   indicates which field to be changed
*        Action -      1 or 0
* OUTPUT:
*        NULL.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxBrgGeCfg(UINT8 Type, UINT8 Action)
{
    OPL_STATUS iStatus;
    IPMUX_BRIDGE_GE_CTL_REG_t stGeCfg;

    if ( (0 != Action) && (1 != Action) )
    {
        return OPL_ERR_INVALID_PARAMETERS;
    }

    iStatus = ipMuxRegRead(IPMUX_REG_GE_PORT_CFG,(volatile UINT32 *)&stGeCfg);
    if (OPL_OK != iStatus)
    {
        return iStatus;
    }

    switch (Type)
    {
        case IPMUX_BRIDGE_GE_TRANSMIT:
            stGeCfg.Transmit = Action;
            break;
        case IPMUX_BRIDGE_GE_LEARN:
            stGeCfg.Learn = Action;
            break;
        case IPMUX_BRIDGE_GE_FLOOD:
            stGeCfg.Flood   = Action;
            break;
        case IPMUX_BRIDGE_GE_TWOTAG:
            stGeCfg.Twotag  = Action;
            break;
        case IPMUX_BRIDGE_GE_ONETAG:
            stGeCfg.Onetag  = Action;
            break;
        case IPMUX_BRIDGE_GE_RECEIVE:
            stGeCfg.Receive = Action;
            break;
        case IPMUX_BRIDGE_GE_FORWARD:
            stGeCfg.Forward = Action;
            break;
        case IPMUX_BRIDGE_GE_UNTAG:
            if(IS_IPMUX_E2())
            {
                stGeCfg.Untag = Action; 
            }else if(IS_IPMUX_E())
            {
                return IPMUX_VERSION_INVALID;
            }else
            {
                return IPMUX_VERSION_INVALID;       
            }
            break;
        default:
            return OPL_ERR_INVALID_PARAMETERS;
    }

    iStatus = ipMuxRegWrite(IPMUX_REG_GE_PORT_CFG, *(UINT32 *)&stGeCfg);
    if (OPL_OK != iStatus)
    {
        return iStatus;
    }
    
    return OPL_OK;
}

/*******************************************************************************
*
* ipmuxBrgGeEpdCfg - configure GE daisy buffer EPD threshold register
*
* DESCRIPTION:
*       To configure GE daisy buffer EPD threshold register
*
* INPUT:
*      GeID  -  Ge port ID 0,1
*      Epdt    -      EPD Threshold value
* OUTPUT:
*        NULL.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxBrgGeEpdCfg(UINT32 GeID, UINT32 Epdt)
{
    OPL_STATUS iStatus;
    IPMUX_BRIDGE_GE_EPD_REG_t stGeEpdCfg;

    if(IS_IPMUX_E())
    {
        return IPMUX_VERSION_INVALID;
    }else if(IS_IPMUX_E2())
    {
        switch (GeID)
        {
            case 0:
                iStatus = ipMuxRegRead(IPMUX_REG_GMAC_DAISY_EPD_THRESHOLD_0,
                            (volatile UINT32 *)&stGeEpdCfg);
                if (OPL_OK != iStatus)
                {
                    return iStatus;
                }
            
                stGeEpdCfg.Epdt = Epdt;

                return ipMuxRegWrite(IPMUX_REG_GMAC_DAISY_EPD_THRESHOLD_0, 
                        *(UINT32 *)&stGeEpdCfg);
            case 1:
                iStatus = ipMuxRegRead(IPMUX_REG_GMAC_DAISY_EPD_THRESHOLD_1,
                            (volatile UINT32 *)&stGeEpdCfg);
                if (OPL_OK != iStatus)
                {
                    return iStatus;
                }
            
                stGeEpdCfg.Epdt = Epdt;

                return ipMuxRegWrite(IPMUX_REG_GMAC_DAISY_EPD_THRESHOLD_1, 
                        *(UINT32 *)&stGeEpdCfg);
            default:
                return OPL_ERR_INVALID_PARAMETERS;
        }
    }else
    {
        return IPMUX_VERSION_INVALID;
    }
}

/*******************************************************************************
*
* ipmuxBrgGeEpdCfg - configure GE daisy pause frame threshold register
*
* DESCRIPTION:
*       To configure GE daisy buffer pause frame threshold register
*
* INPUT:
*   GeID  - Ge port ID 0,1
*   Type    -  indicates which attribute to be set
*   Value   - value to be writen
* OUTPUT:
*        NULL.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxBrgGePauseFrameCfg(UINT32 GeID, UINT32 Type, UINT32 Value)
{
    OPL_STATUS iStatus;
    IPMUX_BRIDGE_GE_PF_REG_t stGePFCfg;

    if(IS_IPMUX_E())
    {
        return IPMUX_VERSION_INVALID;
    }else if(IS_IPMUX_E2())
    {
        switch (GeID)
        {
            case 0:
                iStatus = ipMuxRegRead(IPMUX_REG_GMAC_DAISY_PAUSE_THRESHOLD_0,
                            (volatile UINT32 *)&stGePFCfg);
                if (OPL_OK != iStatus)
                {
                    return iStatus;
                }
            
                if (IPMUX_BRIDGE_GE_PF_MIN == Type)
                {
                    stGePFCfg.Min = Value;
                }
                else if (IPMUX_BRIDGE_GE_PF_MAX == Type)
                {
                    stGePFCfg.Max = Value;
                }
                else
                {
                    return OPL_ERR_INVALID_PARAMETERS;
                }

                return ipMuxRegWrite(IPMUX_REG_GMAC_DAISY_PAUSE_THRESHOLD_0, 
                        *(UINT32 *)&stGePFCfg);
            case 1:
                iStatus = ipMuxRegRead(IPMUX_REG_GMAC_DAISY_PAUSE_THRESHOLD_1,
                            (volatile UINT32 *)&stGePFCfg);
                if (OPL_OK != iStatus)
                {
                    return iStatus;
                }
            
                if (IPMUX_BRIDGE_GE_PF_MIN == Type)
                {
                    stGePFCfg.Min = Value;
                }
                else if (IPMUX_BRIDGE_GE_PF_MAX == Type)
                {
                    stGePFCfg.Max = Value;
                }
                else
                {
                    return OPL_ERR_INVALID_PARAMETERS;
                }

                return ipMuxRegWrite(IPMUX_REG_GMAC_DAISY_PAUSE_THRESHOLD_1, 
                        *(UINT32 *)&stGePFCfg);
            default:
                return OPL_ERR_INVALID_PARAMETERS;
        }   
    }else
    {
        return IPMUX_VERSION_INVALID;
    }

    return OPL_OK;
}



/*******************************************************************************
*
* ipmuxBrgGeMaxPktCfg:  to configure  Daisy Path Maximum Packet Lentgh
*
* DESCRIPTION:
*   This function configures  Daisy Path Maximum Packet Length
* INPUTS:
*   Type    -  indicates which attribute to be set
*   Value   - value to be writen
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, the operation is successful
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxBrgGeMaxPktCfg(UINT32 Type, UINT32 Value)
{
    IPMUX_BRIDGE_GE_MAX_PKTG_REG_t regVal;  

    if(IS_IPMUX_E())
    {
        return IPMUX_VERSION_INVALID;
    }else if(IS_IPMUX_E2())
    {
        ipMuxRegRead(IPMUX_REG_GMAC_DAISY_MAX_PKT_LEN, (volatile UINT32 *)&regVal);

        switch (Type)
        {
            case IPMUX_GE_PKT_MAX:
                regVal.MaxLen = Value;
                break;
            case IPMUX_GE_PKT_PTO:
                regVal.PTO = Value;
                break;
            default:
                return OPL_ERROR;
        }

        /* write to the register */
        ipMuxRegWrite(IPMUX_REG_GMAC_DAISY_MAX_PKT_LEN, *(UINT32 *)(&regVal));
    }else
    {
        return IPMUX_VERSION_INVALID;
    }
    
    return OPL_OK;
}




/*******************************************************************************
*
* ipmuxBrgGeDefaultVIDSet - Set GEW default VLAN ID.
*
* DESCRIPTION:
*       Set GEW degfault VLAN ID.
*
* INPUT:
*        NULL.
*
* OUTPUT:
*        NULL.
*
* RETURNS:
*       OPL_OK,                   If the operation is successful.
*       IPMUX_VERSION_INVALID,    If not IPMux-E2.
*
* OUTPUT:
*        NULL.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxBrgGeDefaultVIDSet(UINT16 Vid)
{
    IPMUX_BRIDGE_GE_DEFAULT_VLAN_SCOS_REG_t  vidScosCfg;

    if (!IS_IPMUX_E2())
    {
        return IPMUX_VERSION_INVALID;
    }
    
    ipMuxRegRead(IPMUX_REG_GEW_DEFAULT_VLAN_SCOS, (volatile UINT32 *)&vidScosCfg);  
    vidScosCfg.VID =  Vid;
    ipMuxRegWrite(IPMUX_REG_GEW_DEFAULT_VLAN_SCOS, *(UINT32 *)&vidScosCfg);

    return OPL_OK;
}





/*******************************************************************************
*
* ipmuxBrgGeDefaultVIDGet - Get GEW default VLAN ID.
*
* DESCRIPTION:
*       Get GEW degfault VLAN ID.
*
* INPUT:
*        NULL.
*
* OUTPUT:
*        NULL.
*
* RETURNS:
*       OPL_OK,                   If the operation is successful.
*       IPMUX_VERSION_INVALID,    If not IPMux-E2.
*
* OUTPUT:
*        NULL.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxBrgGeDefaultVIDGet(UINT16 *Vid)
{
    IPMUX_BRIDGE_GE_DEFAULT_VLAN_SCOS_REG_t  vidScosCfg;

    if (!IS_IPMUX_E2())
    {
        return IPMUX_VERSION_INVALID;
    }
        
    ipMuxRegRead(IPMUX_REG_GEW_DEFAULT_VLAN_SCOS, (volatile UINT32 *)&vidScosCfg);  
    *Vid = vidScosCfg.VID;

     return OPL_OK;
}





/*******************************************************************************
*
* ipmuxBrgGeDefaultSCOSSet - Set GEW degfault SCOS.
*
* DESCRIPTION:
*       Set GEW degfault SCOS.
*
* INPUT:
*        NULL.
*
* OUTPUT:
*        NULL.
*
* RETURNS:
*       OPL_OK,                   If the operation is successful.
*       IPMUX_VERSION_INVALID,    If not IPMux-E2.
*
* OUTPUT:
*        NULL.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxBrgGeDefaultSCOSSet(UINT8 Scos)
{
    IPMUX_BRIDGE_GE_DEFAULT_VLAN_SCOS_REG_t  vidScosCfg;

    if (!IS_IPMUX_E2())
    {
        return IPMUX_VERSION_INVALID;
    }
    
    ipMuxRegRead(IPMUX_REG_GEW_DEFAULT_VLAN_SCOS, (volatile UINT32 *)&vidScosCfg);  
    vidScosCfg.SCOS =  Scos;
    ipMuxRegWrite(IPMUX_REG_GEW_DEFAULT_VLAN_SCOS, *(UINT32 *)&vidScosCfg);

     return OPL_OK;
}

 



/*******************************************************************************
*
* ipmuxBrgGeDefaultSCOSSet - Get GEW degfault SCOS.
*
* DESCRIPTION:
*       Get GEW degfault SCOS.
*
* INPUT:
*        NULL.
*
* OUTPUT:
*        NULL.
*
* RETURNS:
*       OPL_OK,                   If the operation is successful.
*       IPMUX_VERSION_INVALID,    If not IPMux-E2.
*
* OUTPUT:
*        NULL.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxBrgGeDefaultSCOSGet(UINT8 *Scos)
{
    IPMUX_BRIDGE_GE_DEFAULT_VLAN_SCOS_REG_t  vidScosCfg;

    if (!IS_IPMUX_E2())
    {
        return IPMUX_VERSION_INVALID;
    }
    
    ipMuxRegRead(IPMUX_REG_GEW_DEFAULT_VLAN_SCOS, (volatile UINT32 *)&vidScosCfg);  
    *Scos = vidScosCfg.SCOS;

     return OPL_OK;
}




/*******************************************************************************
*
* ipmuxBrgGeShow - Show GEW configuration
*
* DESCRIPTION:
*       Show GEW configuration
*
* INPUT:
*        NULL.
*
* OUTPUT:
*        NULL.
*
* RETURNS:
*       OPL_OK,                       If the operation is successful.
*
* OUTPUT:
*        NULL.
*
* SEE ALSO: 
*/
OPL_STATUS ipmuxBrgGeShow(void)
{
    IPMUX_BRIDGE_GE_CTL_REG_t stGeCfg;
    IPMUX_GE_RSVD_MAC_RCV_CFG_t stMacCfg;   
    IPMUX_BRIDGE_GE_EPD_REG_t stGeEpdCfg;
    IPMUX_BRIDGE_GE_PF_REG_t stGePFCfg;
    IPMUX_BRIDGE_GE_MAX_PKTG_REG_t stMax;   
    IPMUX_BRIDGE_GE_DEFAULT_VLAN_SCOS_REG_t  vidScosCfg;
    char * enableStr[2];

    enableStr[0] = "Disa";
    enableStr[1] = "Enab";

    printf("GE configuration:\n");
    if(IS_IPMUX_E())
    {
        printf("Fw Rv OT TT Fl Ln Tx\n");   
    }else if(IS_IPMUX_E2())
    {
        printf("UT Fw Rv OT TT Fl Ln Tx MaxLen PTO\n");
    }else
    {
        return IPMUX_VERSION_INVALID;
    }
    
    printf("==================================\n");

    if(IS_IPMUX_E())
    {
        ipMuxRegRead(IPMUX_REG_GE_PORT_CFG,(UINT32 *)&stGeCfg); 
        printf("%2d %2d %2d %2d %2d %2d %2d\n",  
        stGeCfg.Forward,
        stGeCfg.Receive,
        stGeCfg.Onetag,
        stGeCfg.Twotag,
        stGeCfg.Flood,
        stGeCfg.Learn,
        stGeCfg.Transmit
        );
    }else if(IS_IPMUX_E2())
    {
        ipMuxRegRead(IPMUX_REG_GE_PORT_CFG,(UINT32 *)&stGeCfg); 
        ipMuxRegRead(IPMUX_REG_GMAC_DAISY_MAX_PKT_LEN, (volatile UINT32 *)&stMax);
        printf("%2d %2d %2d %2d %2d %2d %2d %2d %6d %3d\n",  
            stGeCfg.Untag,
            stGeCfg.Forward,
            stGeCfg.Receive,
            stGeCfg.Onetag,
            stGeCfg.Twotag,
            stGeCfg.Flood,
            stGeCfg.Learn,
            stGeCfg.Transmit,
            stMax.MaxLen,
            stMax.PTO
        );
    }else
    {
        return IPMUX_VERSION_INVALID;
    }
    
    printf("==================================\n");

    if(IS_IPMUX_E())
    {
        printf("GE RsvMac\n");      
    }else if(IS_IPMUX_E2())
    {
        printf("GE RsvMac Epdt MaxPF MinPF\n");
    }else
    {
        return IPMUX_VERSION_INVALID;
    }
    
    printf("==========================\n");

    ipMuxRegRead(IPMUX_REG_RSVD_MAC_RECEIVE, (volatile UINT32 *)&stMacCfg);

    if(IS_IPMUX_E())
    {
        printf("--------------------------\n");
        printf(" 0 %6s\n", enableStr[stMacCfg.Ge0]);    
    }else if(IS_IPMUX_E2())
    {
        printf("--------------------------\n");
        ipMuxRegRead(IPMUX_REG_GMAC_DAISY_EPD_THRESHOLD_0,(volatile UINT32 *)&stGeEpdCfg);
        ipMuxRegRead(IPMUX_REG_GMAC_DAISY_PAUSE_THRESHOLD_0,(volatile UINT32 *)&stGePFCfg);
        printf(" 0 %6s %4d %5d %5d\n",  
            enableStr[stMacCfg.Ge0],
            stGeEpdCfg.Epdt,
            stGePFCfg.Max,
            stGePFCfg.Min
        );
    }else 
    {
        return IPMUX_VERSION_INVALID;
    }
    
    if(IS_IPMUX_E())
    {
        printf(" 0 %6s\n",enableStr[stMacCfg.Ge1]); 
    }else if(IS_IPMUX_E2())
    {
        ipMuxRegRead(IPMUX_REG_GMAC_DAISY_EPD_THRESHOLD_1,(volatile UINT32 *)&stGeEpdCfg);
        ipMuxRegRead(IPMUX_REG_GMAC_DAISY_PAUSE_THRESHOLD_1,(volatile UINT32 *)&stGePFCfg);
        printf(" 0 %6s %4d %5d %5d\n",  
        enableStr[stMacCfg.Ge1],
        stGeEpdCfg.Epdt,
        stGePFCfg.Max,
        stGePFCfg.Min
        );
    }else 
    {
        return IPMUX_VERSION_INVALID;
    }
    
     if(IS_IPMUX_E2())
     {
        printf("==========================\n");     
        printf("VID  SCOS \n");     
        printf("==========================\n");     

        ipMuxRegRead(IPMUX_REG_GEW_DEFAULT_VLAN_SCOS,(volatile UINT32 *)&vidScosCfg);       
        printf("%4d %2d\n",  vidScosCfg.VID, vidScosCfg.SCOS);
     }  
     
    return OPL_OK;
}
/*
#endif
*/

