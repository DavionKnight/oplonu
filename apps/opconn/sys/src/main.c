/*************************************************************************
*
*  COPYRIGHT (C) 2003-2008 Opulan Technologies Corp. ALL RIGHTS RESERVED.  
*
*                       Proprietary and Confidential
*
* 	This software is made available only to customers and prospective
* 	customers of Opulan Technologies Corporation under license and may be
*	used only with Opulan semi-conductor products. 
*
* FILENAME:  main.c
*
* DESCRIPTION: 
*	
*
* Date Created: Aug 18, 2008
*
* Authors(optional): Gan Zhiheng
*
* Reviewed by (optional):
*
* Edit History:
* $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/opconn/sys/src/main.c#1 $
* $Log:$
*
*
**************************************************************************/

#include <time.h>
#include <dal_lib.h>
#include <vos.h>
#include <errors.h>
#include <cli.h>
#include <system.h>
#include <log.h>
#include <userLib.h>

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "gw_types.h"
#include "gwdonuif.h"
#include "oam.h"

#ifdef LINUX
#include <signal.h>
#else
#include <sigLib.h>
#endif



/*-----------------global varible declaration--------------- */

const char * product_info = "GWdelight";
const char * build_date = __DATE__;
const char * build_time = __TIME__;

#ifdef __REVISION__
const char * product_rev = (const char *)__REVISION__;
#else
const char * product_rev = "0";
#endif

const char *g_pcModule[MAX_MODULE] =
{
    "drivers", 
    "system", 
    "user", 
    "qos", 
    "fdb", 
    "multicast", 
    "vlan", 
    "port", 
    "rstp", 
    "pon", 
    "log", 
    "storm",
    "oam",
    "stats",
    "all"
};

const char *g_pcDebugLevel[DEBUG_LEVEL_MAX] =
{
    "off", 
    "critical", 
    "warning", 
    "information", 
    "debugging"
};


extern UINT8	oam_src_mac[6];

extern int g_nDisableMDIO;

extern ENV_t *g_pstConsole;

#ifdef LINUX

/** description: this is the entry for the CLI.and it is the platform for operating on systems.
 *  @param 
 *  @param 
 *  @param 

 *  @see 
 */
extern gwdonu_im_if_t g_onu_im_ifs;
extern void gw_plat_init();
extern void setEth0MAC();
//extern gw_status reg_gwdonu_im_interfaces(gwdonu_im_if_t *ifs);


//extern void plat_init();
int main(int argc,char *argv[])
{
	UINT32 retVal, regVal;;
	UINT8  hwAddr[6];
	UINT8  enable = FALSE;
    char   acTempDir[STR_SIZE_64_BYTES] = {0};

    DBG_LINE
    printf("%s starting ... \n", product_info);
    DBG_LINE
    if(vosCoreInit() < 0 || vosAppInit() <0)
    {
        printf("SAL initialization failed\r\n");
        return SYS_INIT_ERROR;
    }

    DBG_LINE
    /* configuration database initialize */
	
    printf("Init configuration database ... ");
//		#if 0
    if(vosConfigInit() < 0)
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        printf("done\r\n");
    }

    DBG_LINE
    /* create the local temp dir */
    printf("Init message queue ... ");

    odmSysCfgMsqDirGet(acTempDir);
    if(vosMsqInit(acTempDir) < 0)
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        printf("done\r\n");
    }

    DBG_LINE
    /* Ignore SIGPIPE signal */
    signal(SIGPIPE, SIG_IGN);

    /* optimer initialize */
    printf("Init vos timer ... ");

    if(vosTimerInit() < 0)
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        printf("done\r\n");
    }


    DBG_LINE
    /* session initialize */
    printf("Init command line sessions ... ");
    if (NO_ERROR != cliSessionInit())
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        printf("done\r\n");
    }
//#endif
    DBG_LINE
    /* cli initialize */
    printf("Init command line engine ... ");
	
    cliInit();
    printf("done\r\n");

    printf("Init text config file ... ");
    cliTextConfigInit();
    printf("done\r\n");

    /* sys log initialize */
    printf("Init sysLog task ...\n");
    if(NO_ERROR != sysLogInit())
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        printf("done\r\n");
    }
	
//
//  init eth0 by configuration
	setEth0MAC();

    /* Init PON MAC ID */
    odmPonMacIdInit();
    run_led();         //cs1 init
    printf("Init HAL ... ");
    if(NO_ERROR != halInit())			//oam in it  note by zhangjj 2013-2-27
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        printf("done\r\n");
    }
//PortCfgInit();
    /* Init system default configuration */
    printf("Init ODM system configuration ... ");
    odmSysCfgInit();
    printf("done\r\n");

    printf("Init ODM user configuration ... ");
    if(NO_ERROR != odmUserCfgInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }

    /* Init other module here */
    printf("Init ODM pon configuration ... ");
    if(NO_ERROR != odmPonCfgInit())
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        printf("done\r\n");
    }

    printf("Init ODM port configuration ... ");
    if(NO_ERROR != odmPortCfgInit())
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else {
        printf("done\r\n");
    }

#if defined(ONU_4PORT_AR8228)
    printf("Init ODM port mirror configuration ... ");
    if(NO_ERROR != odmPortMirrorCfgInit())
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else {
        printf("done\r\n");
    }
#endif

    printf("Init ODM FDB configuration ... ");
    if(NO_ERROR != odmFdbInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }

#ifdef ONU_1PORT_V1E_RF100_GNA_ATE
    /*add by xukang for test--begin*/
    printf("Init test_module configuration ... ");
    if(NO_ERROR != odmTestInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }
    /*add by xukang for test--end*/
#endif
	
    printf("Init ODM qos configuration ... ");
    if(NO_ERROR != odmClassInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }

    printf("Init ODM VLAN configuration ... ");
    if(NO_ERROR != odmVlanInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }

    printf("Init ODM multicast control ... ");
    if(OK != odmMulticastInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }

    printf("Init ODM RSTP task ... ");
    if(OK != odmRstpInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }

    printf("Init ODM port storm configuration ... ");
    if(NO_ERROR != odmPortStormCfgInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }

    printf("Init ODM service ... ");
    if(OK != odmServiceInit())
    {
        printf("failed\r\n");
        return SYS_INIT_ERROR;
    }
    else
    {
        printf("done\r\n");
    }

	//printf("Led start run...\n");
	

#ifdef OPL_COM_UART1
    odmOnuUartCommEnableGet(&enable);
    if (TRUE == enable)
        odmOnuUart1CommThreadInit();
#endif

    /* Start telnetd */
    if (ENABLED == odmSysCfgTelnetServiceGet())
    {
        retVal = cliTelnetdInit();
        printf("Init telnetd task ... ");
        if (NO_ERROR != retVal &&
            SYS_TELNETD_ALREADY_INITIALIZED != retVal)
        {
            printf("failed\r\n");
        }
        else
        {
            printf("done\r\n");
        }
    }
#if defined(ONU_4PORT_AR8306) || defined(ONU_4PORT_AR8228)|| defined(ONU_4PORT_AR8327)
    printf("Init DMA1 task ... ");
    if(OK != halDma1ThreadInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }
#endif
    #ifdef OPL_DMA0_ETH
    printf("Init mirror task ... ");
    halMirrorClsInit();
    printf("done\r\n");

    halEocClsInit();
    #endif
	
    /* LOS LED init */
    printf("Init LOS LED ... ");
    if(NO_ERROR != odmLOSLedInit())
    {
        printf("failed\r\n");
    }
    else
    {
        printf("done\r\n");
    }
reg_gwdonu_im_interfaces(&g_onu_im_ifs,sizeof(g_onu_im_ifs));
gw_plat_init();

//ret =init_main();		//cs2 init
//if(ret !=-1)
ts_init();			//terminal server init
	

	/* respin version config */
	if (IS_ONU_RESPIN)
	{
		int retVal;
		#if 1
			/*FPGA verify*/
			printf("\n ------------- FPGA verify ---------- \n");
			retVal	= respinInit();
			if(OPL_OK != retVal)
			{
				OPL_DRV_PRINTF(("respin init failed.\n"));
			}
		#else
			/*serdis verify*/
			printf("\n------------- serdis verify version ----------");
			owr(0xe,1);
		#endif
	}

#if 0

	/* Added RGMII configuration */
	
	retVal = oplRegWrite(REG_GMAC_MODE, 0x5);
    if(OPL_OK != retVal)
    {
                  return retVal;
    }          

    retVal = oplRegWrite(REG_GMAC_RGMII_SOFT_CFG, 0x6);
    if(OPL_OK != retVal)
    {
                  return retVal;
    }             

    /* mac6 GMII PHY mode */
    regVal = 0x07400000;
    shiva_reg_set(0, 0xC, (UINT8 *)&regVal, sizeof(UINT32));

	/* end of: Added RGMII configuration */
#endif


#if defined(SWITCH_TEST)
    /* add by zgan - 2009/10/22, test AR8228 reset issue */
    g_nDisableMDIO = 1;
#endif

	if (g_pstConsole)
	{
		cliTextConfigLoad(g_pstConsole, DEFAULT_CONFIG_FILE);
	}

    oplRegWrite(REG_FE_TRANSFER_ENA, 0x3);

	#if 0
    /*start voip module by start_voip.sh*/
    vosSystem("sh /etc/start_voip.sh");
	#endif

    /* Start console CLI */
    cliMainEntry();

	/* destory */
	odmRstpDestroy();
    halDestroy();
    cliTextConfigDestroy();
    odmServiceShutdown();
    vosTimerShutdown();
    sysLogShutdown();
    cliTelnetdShutdown();
    cliSessionDestroy();
    vosMsqDestroy();
    vosConfigDestroy();
    vosAppDestroy();
    vosCoreDestroy();
    odmUart1TestThreadShutdown();

#ifdef OPL_COM_UART1
    if (TRUE == enable)
        odmOnuUart1CommThreadShutdowm();
#endif

#ifdef DEBUG
    vosMemShow();
#endif
  
    return 0;
}
#else  /* for VXWORKS */
void vxspawn()
{
}
#endif
/** @}*/
/** @}*/
