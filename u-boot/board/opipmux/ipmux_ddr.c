/*
=============================================================================
     Header Name: ipmux_ddr.c

     General Description:
IPMux DDR Controller module head file.
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 xwang             11/5/06       Initial Version
*/
#include <config.h>
#include <common.h>


#ifdef CONFIG_TUNE_DDR

#include <malloc.h>
#include <net.h>
#include <command.h>
#include <asm/io.h>

#include "extra.h"

#include "ipmux_ddr.h" 


#define IPMUX_DDR_SET_DELAY(x)  ipMuxRegWrite(REG_DDR_UP_CTLX(14), x)

OPL_STATUS ipmuxDdrTune(void)
{
    UINT32 delayVal=0;
    UINT32 i;

    /* config the ddr control registers */
    IPMUX_DDR_INIT;


    /* tune the ddr delay values */
    for (i = 0x00; i <= IPMUX_DDR_MAX_DELAY_VAL; i++) {

        delayVal=i<<IPMUX_DDR_DELAY0_SHIFT | i<<IPMUX_DDR_DELAY1_SHIFT | 0x0c00;

        IPMUX_DDR_SET_DELAY(delayVal);

        *(UINT32 *)DDRTUNE_ADDR_TEST=DDRTUNE_VAL_TEST;
        *(UINT32 *)DDRTUNE_ADDR_TEST_REST=DDRTUNE_VAL_TEST_REST;

        if (*(UINT32 *)DDRTUNE_ADDR_TEST!=DDRTUNE_VAL_TEST) {
            continue;
        }

        *(UINT32 *)DDRTUNE_ADDR_TEST1=DDRTUNE_VAL_TEST1;
        *(UINT32 *)DDRTUNE_ADDR_TEST_REST=DDRTUNE_VAL_TEST_REST;

        if (*(UINT32 *)DDRTUNE_ADDR_TEST1!=DDRTUNE_VAL_TEST1) {
            continue;
        }

        printf("DDR tune successfully!");
        return(OPL_OK);
    }

    printf("DDR tune failed!");
    return(OPL_ERROR);
}

#endif
