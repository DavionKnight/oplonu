/*
=============================================================================
     Header Name: ipmuxBootextend.c

     General Description:
        port U-boot for OPULAN boot
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
xWang                27/6/07            Initial Version
wwei                 27/6/07            Initial Version
*/
/* include */

#include "opl_driver.h"

#ifdef OPULAN_BOOT_MENU_MODE
#include "common.h"
#include "opulan_bootextend.h"
/* defines */
static char erase_seq[] = "\b \b";      /* erase sequence   */
static char tab_seq[] = "        "; /* used to expand TABs  */

/*------ tbd : 96M ------*/
#ifdef DDR2_USED_BY_CPU_64M
//#define nfsrootargs "setenv bootargs root=/dev/nfs rw ip=$(ipaddr) mac=$(ethaddr) nfsroot=$(nfsserver):$(nfspath) mem=64M nofpu sys_clk=$(sys_clk) cpu_clk=$(cpu_clk)"
//#define emmrootargs "setenv bootargs ip=$(ipaddr) mac=$(ethaddr) root=/dev/ram0 rw load_ramdisk=1 ramdisk_size=$(ramdisk_size) mem=64M nofpu console=ttyS0,115200n8 sys_clk=$(sys_clk) cpu_clk=$(cpu_clk)"
#define nfsrootargs "setenv bootargs root=/dev/nfs rw ip=$(ipaddr) mac=$(ethaddr) nfsroot=$(nfsserver):$(nfspath) mem=$(mem)M nofpu sys_clk=$(sys_clk) cpu_clk=$(cpu_clk)"
#define emmrootargs "setenv bootargs ip=$(ipaddr) mac=$(ethaddr) root=/dev/ram0 rw load_ramdisk=1 ramdisk_size=$(ramdisk_size) mem=$(mem)M nofpu console=ttyS0,$(baudrate)n8 sys_clk=$(sys_clk) cpu_clk=$(cpu_clk)"
#else
//#define nfsrootargs "setenv bootargs root=/dev/nfs rw ip=$(ipaddr) mac=$(ethaddr) nfsroot=$(nfsserver):$(nfspath) mem=32M nofpu  sys_clk=$(sys_clk) cpu_clk=$(cpu_clk)"
//#define emmrootargs "setenv bootargs ip=$(ipaddr) mac=$(ethaddr) root=/dev/ram0 rw load_ramdisk=1 ramdisk_size=$(ramdisk_size) mem=32M nofpu console=ttyS0,115200n8  sys_clk=$(sys_clk) cpu_clk=$(cpu_clk)"
#define nfsrootargs "setenv bootargs root=/dev/nfs rw ip=$(ipaddr) mac=$(ethaddr) nfsroot=$(nfsserver):$(nfspath) mem=$(mem)M nofpu  sys_clk=$(sys_clk) cpu_clk=$(cpu_clk)"
#define emmrootargs "setenv bootargs ip=$(ipaddr) mac=$(ethaddr) root=/dev/ram0 rw load_ramdisk=1 ramdisk_size=$(ramdisk_size) mem=$(mem)M nofpu console=ttyS0,$(baudrate)n8  sys_clk=$(sys_clk) cpu_clk=$(cpu_clk)"
#endif

/* local varialbes */
static MENU_ITEM_t * pNow;
static char oplbootConsoleBuffer[CFG_CBSIZE];
extern MENU_ITEM_t menuMain[];
extern MENU_ITEM_t menuBootConfig[];
extern MENU_ITEM_t menuEtherConfig[];
extern MENU_ITEM_t netUpdateMenu[];
extern MENU_ITEM_t xUpdateMenu[];
// extern MENU_ITEM_t flashBootMenu[];
extern MENU_ITEM_t menuSerialConfig[];
extern MENU_ITEM_t menuBootLocation[];
extern MENU_ITEM_t menuBootFlashType[];
extern MENU_ITEM_t menuBootFromNfs[];

/* function declarations */
void oplbootMainloop(void);
void oplbootInitial(void);
void oplbootMenuShow(MENU_ITEM_t *pmenu);
void oplbootParseInput(MENU_ITEM_t *pnemu, int *pinput);
void oplbootMenuChange(int  value);
void oplbootDataType(void);
int oplbootCharCheck(char str[10], int mode);
void oplbootNameSet( void );
void oplbootCharDataInput(int format);
void oplbootTypeSel(void);
void oplbootSetEthIP(void);
void oplbootSerialBau(void);
int oplbootLocation(void);
void oplbootLoadSystem(void);
void oplbootSaveConfig(void);
int oplReadline(char *repeat_last);
void oplCommandLineInterface (void);




static MENU_ITEM_t menuMain[]={                                                                                                
    /*ID                                   next               command             envVarName         promt                         type              header                                      */    
    {MENU_MAIN_REBOOT                    , OPL_NULL,          "reset",            OPL_NULL,          OPL_NULL                      ,'\0'            ,"Reboot"                                    },
    {MENU_MAIN_BOOT_CONFIG               , menuBootConfig,    OPL_NULL,           OPL_NULL,          OPL_NULL                      ,'\0'            ,"Boot Config"                               },
    {MENU_MAIN_BOOT_SYSTEM               , OPL_NULL,          OPL_NULL,           OPL_NULL,          OPL_NULL                      ,"boot"          ,"Boot System"                               },
    {MENU_MAIN_COMMAND_LINE_INTERFACE    , OPL_NULL,          OPL_NULL,           OPL_NULL,          OPL_NULL                      ,'\0'            ,"Command Line Interface"                    },
    {MENU_MAIN_ETHERNET_SUBMENU          , menuEtherConfig,   OPL_NULL,           OPL_NULL,          OPL_NULL                      ,'\0'            ,"Ethernet Submenu"                          },
    {MENU_MAIN_SERIAL_SUBMENU            , menuSerialConfig,  OPL_NULL,           OPL_NULL,          OPL_NULL                      ,'\0'            ,"Serial Submenu"                            },
    {MENU_MAIN_NETUPDATE_SUBMENU             , netUpdateMenu,     OPL_NULL,           OPL_NULL,          OPL_NULL                      ,'\0'            ,"Net Update Submenu"                             },
    {MENU_MAIN_XMODEM_SUBMENU             , xUpdateMenu,     OPL_NULL,           OPL_NULL,          OPL_NULL                      ,'\0'            ,"Xmodem Update Submenu"                             },
 //   {MENU_MAIN_FLBOOT_SUBMENU             , flashBootMenu,     OPL_NULL,           OPL_NULL,          OPL_NULL                      ,'\0'            ,"Flash Boot Submenu"                             },
    {MENU_MAIN_SAVE_CHANGES              , OPL_NULL,          "saveenv",          OPL_NULL,          OPL_NULL                      ,"save"     ,       "Save Changes"                              },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           OPL_NULL,          "     Main Menu    "          ,'\0'            ,OPL_NULL                                    }
};                                                                                                           

static MENU_ITEM_t menuBootConfig[]={      
    /*ID                                   next               command             envVarName         promt,                        type             header                                      */    
    {MENU_BOOT_EXIT                      , menuMain,          OPL_NULL,           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Return To Main Menu"                       },
    {MENU_BOOT_FILE_NAME                 , OPL_NULL,          OPL_NULL,           "bootname"       ,"INPUT a new name"             ,"name"         ,"Boot File Name"                            },
    {MENU_BOOT_FILE_TYPE                 , menuBootFlashType, OPL_NULL,           "boottype"       , OPL_NULL                      ,'\0'           ,"Boot File Type"                            },
    {MENU_BOOT_FILE_LOC                  , menuBootLocation,  OPL_NULL,           "bootloc"        , OPL_NULL                      ,'\0'           ,"Boot File Location"                        },
    {MENU_BOOT_ARGS_VX                   , OPL_NULL,          OPL_NULL,           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Boot Args for vxWorks"                     },
    {MENU_BOOT_ARGS_LINUX_NFS            , menuBootFromNfs,   OPL_NULL,           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Boot Args for Linux with Ramdisk on NFS  " },
    {MENU_BOOT_ARGS_LINUX_EMM            , OPL_NULL,          OPL_NULL,           "ramdisk_size"   ,"INPUT the size Linux used"    ,"HEX"          ,"Boot Args for Linux with Embedded Ramdisk" },
    {MENU_BOOT_CPU_CLK                   , OPL_NULL,          OPL_NULL,           "cpu_clk"        ,"Input CPU clock reate"        ,"DEC"          ,"CPU clock reate" },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           OPL_NULL         ,"    Boot Config    "          ,'\0'           ,OPL_NULL                                    }
};                                                                                                                                                 

static MENU_ITEM_t menuEtherConfig[]=
{
    /*ID                                             next               command             envVarName,        promt,                        type            header                                       */
    {MENU_ETH_EXIT                       , menuMain,          OPL_NULL,           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Return To Main Menu"                       },
    {MENU_ETH_LOC_IP                     , OPL_NULL,          OPL_NULL,           "ipaddr"         ,"INPUT a local ip address"     ,"ip"           ,"Local IP Address"                          },
    {MENU_ETH_SER_IP                     , OPL_NULL,          OPL_NULL,           "serverip"       ,"INPUT a server ip address"    ,"ip"           ,"Server IP Address"                         },
    {MENU_ETH_MAC_ADDR               , OPL_NULL,          OPL_NULL,           "ethaddr"       ,"INPUT a MAC address"    ,"name"           ,"MAC Address"                         },
    {MENU_NULL                               , OPL_NULL,           OPL_NULL,           OPL_NULL         ," Ethernet Submenu "           ,'\0'           ,OPL_NULL                                    }
};

static MENU_ITEM_t menuBootFromNfs[]={                                                    
    /*ID                                   next               command             envVarName,        promt,                        type             header                                      */    
    {MENU_BOOT_NFS_EXIT                  , menuBootConfig,    OPL_NULL,           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Return To Boot Config Menu"                },
    {MENU_BOOT_NFS_SERVER                , OPL_NULL,          nfsrootargs,        "nfsserver"      ,"Input NFS root server ip"     ,"ip"           ,"NFS Boot Server"                           },
    {MENU_BOOT_NFS_PATH                  , OPL_NULL,          nfsrootargs,        "nfspath"        ,"Input NFS root path"          ,"name"         ,"NFS Boot Path"                             },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           OPL_NULL         ,"NFS Root Configure"           ,'\0'           ,OPL_NULL                                    }
}; 

static MENU_ITEM_t menuSerialConfig[]={                                                    
    /*ID                                   next               command             envVarName,        promt,                        type             header                                      */    
    {MENU_SERIAL_EXIT                    , menuMain,          OPL_NULL,           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Return To Main Menu"                       },
    {MENU_SERIAL_BAU                     , OPL_NULL,          OPL_NULL,           "baudrate"       ,"choose a serail baud below"   ,"Baud"         ,"Baudrate"                                  },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           OPL_NULL         ,"  Serial Submenu  "           ,'\0'           ,OPL_NULL                                    }
};                                                                                                                                                        

static MENU_ITEM_t netUpdateMenu[]={       
    /*ID                                   next               command             envVarName,        promt,                        type             header                                      */    
    {MENU_FLASH_EXIT                     , menuMain,          OPL_NULL,            OPL_NULL         , OPL_NULL                      ,'\0'          ,"Return To Main Menu"                       },
/*  {MENU_FLASH_INFO                     , OPL_NULL,          "flinfo",           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Flash Info"                                },             */
    {MENU_FLASH_BOOT_FILE_NAME           , OPL_NULL,          OPL_NULL,           "flash_boot_name","INPUT a flash file name"      ,"name"         ,"Bootware File Name"                        },
    {MENU_FLASH_BOOT_UPDATE              , OPL_NULL,          "uutftp",           OPL_NULL         ,OPL_NULL                       ,'\0'           ,"Update Bootware From Network"              },
    {MENU_FLASH_OS_FILE_NAME             , OPL_NULL,          OPL_NULL,           "flash_file_name"  ,"INPUT a new file name"     ,"name"         ,"File Name"                              },
    {MENU_OS1_UPDATE                , OPL_NULL,          "uuos1",             OPL_NULL         , OPL_NULL                      ,'\0'           ,"Update OS1 From Network"                    },
    {MENU_OS2_UPDATE                , OPL_NULL,          "uuos2",             OPL_NULL         , OPL_NULL                      ,'\0'           ,"Update OS2 From Network"                    },
    {MENU_JFFS2_UPDATE                , OPL_NULL,          "jffs2",             OPL_NULL         , OPL_NULL                      ,'\0'           ,"Update JFFS2 From Network"                    },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           OPL_NULL         ,"  Net Update Submenu   "           ,'\0'           ,OPL_NULL                                    }
};                                                                                                                                                                                                                                                                                              

#if 0
static MENU_ITEM_t menuFlashMenu[]={       
    /*ID                                   next               command             envVarName,        promt,                        type             header                                      */    
    {MENU_FLASH_EXIT                     , menuMain,          OPL_NULL,            OPL_NULL         , OPL_NULL                      ,'\0'          ,"Return To Main Menu"                       },
    {MENU_FLASH_INFO                     , OPL_NULL,          "flinfo",           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Flash Info"                                },
    {MENU_FLASH_BOOT_FILE_NAME           , OPL_NULL,          OPL_NULL,           "flash_boot_name","INPUT a flash file name"      ,"name"         ,"Bootware File Name"                        },
    {MENU_FLASH_BOOT_UPDATE              , OPL_NULL,          "uutftp",           OPL_NULL         ,OPL_NULL                       ,'\0'           ,"Update Bootware From Network"              },
    {MENU_FLASH_OS_FILE_NAME             , OPL_NULL,          OPL_NULL,           "flash_os_name"  ,"INPUT a new OS file name"     ,"name"         ,"OS File Name"                              },
    {MENU_FLASH_OS_UPDATE                , xupdateMenu,          OPL_NULL,             OPL_NULL         , OPL_NULL                      ,'\0'           ,"Update OS From Network"                    },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           OPL_NULL         ,"  Flash Submenu   "           ,'\0'           ,OPL_NULL                                    }
}; 
#endif
static MENU_ITEM_t menuBootFlashType[]={                                                        
    /*ID                                   next               command             envVarName,        promt,                       type             header                                       */    
    {MENU_BOOT_FLASH_TYPE_EXIT           , menuBootConfig,    OPL_NULL,           OPL_NULL         , OPL_NULL                     ,'\0'           ,"Return To Boot Config Menu"                },
    {MENU_BOOT_FLASH_TYPE_VX             , OPL_NULL,          OPL_NULL,           OPL_NULL         , OPL_NULL                     ,"type"         ,"Vxworks"                                   },
    {MENU_BOOT_FLASH_TYPE_LINUX_NFS            , OPL_NULL,          OPL_NULL,           OPL_NULL         , OPL_NULL               ,"type"         ,"Linux-Nfs"                                 },
    {MENU_BOOT_FLASH_TYPE_LINUX_RAMDISK_EMBEDED, OPL_NULL,          OPL_NULL,           OPL_NULL         , OPL_NULL               ,"type"         ,"Linux-With-Ramdisk"                       },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           "boottype"       ,"  Boot File Type  "          ,'\0'           ,OPL_NULL                                    }
};

static MENU_ITEM_t menuBootLocation[]={                                                        
    /*ID                                   next               command             envVarName,        promt,                       type             header                                      */    
    {MENU_BOOT_EXIT                      , menuBootConfig,    OPL_NULL,           OPL_NULL         , OPL_NULL                     ,'\0'           ,"Return To Boot Config Menu"                },
    {MENU_BOOT_LAC_NET                   , OPL_NULL,          nfsrootargs,        OPL_NULL         , OPL_NULL                     ,"type"         ,"Network"                                   },
 //   {MENU_BOOT_LAC_FLA                   , OPL_NULL,          emmrootargs,        OPL_NULL         , OPL_NULL                     ,"type"         ,"Flash"                                     },
    {MENU_BOOT_OS1_FLA                   , OPL_NULL,          emmrootargs,        OPL_NULL         , OPL_NULL                     ,"type"         ,"Flash_OS1"                                     },
    {MENU_BOOT_OS2_FLA                   , OPL_NULL,          emmrootargs,        OPL_NULL         , OPL_NULL                     ,"type"         ,"Flash_OS2"                                     },
    {MENU_NULL                           , OPL_NULL,          OPL_NULL,           "bootloc"        ,"Boot File Location"          ,'\0'           ,OPL_NULL                                    }
};

static MENU_ITEM_t xUpdateMenu[]={       
    /*ID                                   next               command             envVarName,        promt,                        type             header                                      */    
    {MENU_XUPDATE_EXIT                   , menuMain,          OPL_NULL,            OPL_NULL         , OPL_NULL                      ,'\0'          ,"Return To Main Menu"                       },
    {MENU_FLASH_UBOOT_UPDATE             , OPL_NULL,          "update 0",           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Update UBOOT with uart"                                  },
 //   {MENU_FLASH_ENV_UPDATE           , OPL_NULL,            "update 1",           OPL_NULL,         OPL_NULL                      ,"\0"         ,"Update ENV with uart"                          },
    {MENU_FLASH_KERNEL1_UPDATE          , OPL_NULL,          "update 2",           OPL_NULL         ,OPL_NULL                       ,'\0'             ,"Update OS1 with uart"                 },
    {MENU_FLASH_KERNEL2_UPDATE           , OPL_NULL,          "update 4",             OPL_NULL         , OPL_NULL                      ,'\0'            ,"Update OS2 with uart"                    },
    {MENU_FLASH_JFFS2_UPDATE             , OPL_NULL,          "update 3",           OPL_NULL  ,    OPL_NULL                           ,"\0"         ,"Update CFG with uart"                                },
    {MENU_NULL                                     , OPL_NULL,          OPL_NULL,           OPL_NULL   ,          "  Xmodem update Submenu   "           ,'\0'           ,OPL_NULL                                    }
};    
#if 0
static MENU_ITEM_t flashBootMenu[]={       
    /*ID                                   next               command             envVarName,        promt,                        type             header                                      */    
    {MENU_FLBOOT_EXIT                   , menuMain,          OPL_NULL,            OPL_NULL         , OPL_NULL                      ,'\0'          ,"Return To Main Menu"                       },
    {MENU_FLASH_BOOT_KERNEL1             , OPL_NULL,          "bootm 0xb0080000",           OPL_NULL         , OPL_NULL                      ,'\0'           ,"Boot Kernel 1"                                  },
    {MENU_FLASH_BOOT_KERNEL2           , OPL_NULL,            "bootm 0xb0460000",           OPL_NULL,         OPL_NULL                      ,"\0"         ,"Boot Kernel 2"                          },
    {MENU_NULL                                     , OPL_NULL,          OPL_NULL,           OPL_NULL   ,          "  Flash boot Submenu   "           ,'\0'           ,OPL_NULL                                    }
}; 
#endif
/* function definitons */
/*******************************************************************************
*
* oplbootMainloop
*
* DESCRIPTION:
*   This function is the entry of the opl boot menu function.
*    -----------------       ---------------        ---------------
*   |initial menu item|  -> |prompt optation|  ->  |input a command|   --
*    -----------------       ---------------        ---------------      |
*                                 /\                                     |
*                                 |      ----------------                |
*                                  ---- |strore configure|   <-----------
*                                        ----------------
*
* INPUT:
*   N/A
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootMainloop(void)
{
    int input = 0;
    int *pInput = &input;

    /*initial menu item*/
    oplbootInitial();

   /*
    * Main Loop for Monitor Command Processing
    */
    printf("\n\n\n\n\n\n\n\n\n");
    printf("                   *********                    *********                     \n");
    printf("             *********  Opulan U-boot Menu Interface  *********               \n");
    printf("                   *********                    *********                     \n");

	
    for (;;)
    {
    	
        oplbootMenuShow(pNow);
        oplbootParseInput(pNow ,pInput); 	
        oplbootMenuChange(*pInput);
	
    }
}

/*******************************************************************************
*
* oplbootStructInitial
*
* DESCRIPTION:
*   This function initialize MENU_ITEM_t menuMain[], menuBootConfig[], menuEtherConfig[], menuSerialConfig[]
*   menuFlashMenu[], menuBootFlashType[].
*
* INPUT:
*   N/A
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootInitial(void)
{
    char buf[20];
    pNow = menuMain;

//   printf("pnow = 0x%x\n",pNow);

    if( OPL_NULL == getenv("flash_boot_name"))
    {
        setenv("flash_boot_name","u-boot.bin");
    }


    sprintf(buf, "%u", FPCLK);
    if( OPL_NULL == getenv("sys_clk"))
    {
        setenv("sys_clk", buf);
    }

    sprintf(buf, "%u", CPU_CLOCK_RATE);
    if( OPL_NULL == getenv("cpu_clk"))
    {
        setenv("cpu_clk", buf);
    }
    
}

/*******************************************************************************
*
* oplbootMenuShow
*
* DESCRIPTION:
*   This function show oplboot menu.
*
* INPUT:
*   pmenu      - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootMenuShow(MENU_ITEM_t *pMenu)
{
    char *pValue;
    int i =0;  
	
//#if  (FLASHSIZE>flash_4M)
   cs0Cfgdefault();
//#endif
    for (;pMenu->header != OPL_NULL; pMenu++)
    {
        i++;
    }
// printf( "addr = 0x%x\n",pMenu->promt);
    printf("\n============================<%-18s >=============================\n", pMenu->promt);
    for (;i > 0; i--)
    {
        pMenu--;
    }

    /*
     * print every menu line
     */ 
    while (OPL_NULL != pMenu->header)
    {
        pValue = getenv(pMenu->envVarName);
        
        if (OPL_NULL != pValue)
        { 
            printf("\r<%d> %-35s    [%s]\n", i, pMenu->header,pValue);
        }
        else if (OPL_NULL != pMenu->envVarName)
        {
            printf("\r<%d> %-35s    [current]\n", i, pMenu->header);
        }
        else
        {    
            printf("\r<%d> %-35s    \n", i, pMenu->header);
        }

        pMenu++;
        i++;
    }
    for(; i<=7; i++)
    {
        printf("\n");
    }
    printf("==============================================================================\n");

}

/*******************************************************************************
*
* oplbootParseInput
*
* DESCRIPTION:
*   This function oplboot parse input, do some work associate with operate Menu.
*
* INPUT:
*   p           - 
*   pinput      - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootParseInput(MENU_ITEM_t *pMenu, int *pInput)
{
    int num = 0;
    char ch;

    while (OPL_NULL != pMenu->header)
    {
        num++;
        pMenu++;
    }
    num--;

    printf("\nEnter your choice : ");
    do
    {
        ch = getc( );
        printf("%c\n",ch);
        if ((ch>='0')&&(ch<=('0'+num)))
        {
            *pInput = (ch-'0');
        }
        else
        {
            printf("Invalid number! please try again:",num);
        }

    }while (!((ch>='0')&&(ch<=('0'+num))));
// #if  (FLASHSIZE>flash_4M)
//    reCfgCs0 (FLASHSIZE);	
// #endif
}

/*******************************************************************************
*
* oplbootMenuChange
*
* DESCRIPTION:
*   This function is to select configure target,and now find the target *pcnt
*   and now do the configure.meanwhile change the prompt menu.
* INPUT:
*   value       -user input 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A 
*
* SEE ALSO:
*/
void oplbootMenuChange(int  value)
{
    int i;

    /****** Exit to upper Menu *********/
    if (value == 0)
    {
        if (OPL_NULL != pNow->next)
        {
            pNow = pNow->next;
        }
        else
        {       
            run_command("reset",0);
        }
    }

    /****** Enter to the down Menu ******/
    else
    {
        for (i=0; i<value; i++)
        {
            pNow++;
        }

        if (OPL_NULL == pNow->next)
        {
            /* do with date */
            oplbootDataType();
                
            if (OPL_NULL != pNow->command)
            {
                run_command(pNow->command,0);
                
            }
            for (; i>0; i--)
            {
                pNow--;
            }
        }
        else
        {
            pNow = pNow->next;
        }
    }
}
/*******************************************************************************
*
* oplbootDataType
*
* DESCRIPTION:
*   This function do config set.
*
* INPUT:
*   N/A 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootDataType(void)
{ 
    if(MENU_MAIN_COMMAND_LINE_INTERFACE == pNow->ID)
    {
        oplCommandLineInterface();
    }
    else
    {
        if( OPL_NULL != pNow->promt)
        {
            printf("%s, kick 'ENTER' to canel : ",pNow->promt);
        }
        switch(*(pNow->type))
        {
     /*case 'type'*/
        case 't':
            oplbootTypeSel();
            break;
     /*case 'baud'*/
        case 'B':
            oplbootSerialBau();
            break;
     /* case 'name'*/
        case 'n':
            oplbootNameSet();
            break;
     /*case 'ip'*/
        case 'i':
            oplbootSetEthIP();
            break;
     /* case 'HEX'*/
        case 'H':
            oplbootCharDataInput(2);
            break;
     /* case 'DEC' */
        case 'D':
            oplbootCharDataInput(0);
            break;
     /* case 'boot' */
        case 'b':
            oplbootLoadSystem();
            break;
     /* case 'save' */
        case 's':
            oplbootSaveConfig();
            break;
     /* case '\0'*/
        case '\0':
            break;
        default:
            /*printf("ERROR! type'%s' is no disposal function for the 'type' defined in MENU_ITEM_t. \n",pNow->promt);*/
            break;
     }  
    } 
}

/*****  mode=0 represent char str is DEC
     *  mode=2 represent char str is HEX
     */
/*******************************************************************************
*
* bootCharCheck
*
* DESCRIPTION:
*   This function boot char check.
*
* INPUT:
*   str10       - 
*   mode        - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   int         - 
*
* SEE ALSO:
*/
int oplbootCharCheck(char str[10], int mode)
{
    int i;
    i = mode;

    /**** check HEX char only  ****/
    if(2==mode && ('0'!=str[0] || 'x'!=str[1]))
    {
        return -1;
    }

    /*****  check all ******/
    while('\0'!=str[0] && '\0'!=str[i])
    {
        /******  if(str[] is't DEC)   ****/
        if(0==mode && (!(((str[i]-'0')>=0)  &&  ((str[i]-'0')<=9))) )
        {
             return -1;
        }
        /******  if(str[] is't HEX)  ****/
        if(2==mode && (!((((str[i]-'0')>=0)  &&  ((str[i]-'0')<=9)) || (str[i]<='f' && str[i]>='a')  )) )
        {
             return -1;
        }
        i++;  
    }
    return 1;
}

/*******************************************************************************
*
* oplCommandLineInterface
* DESCRIPTION:
*   store name for struct MENU_ITEM_t.
*
* INPUT:
*   N/A
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplCommandLineInterface (void)
{   
    printf("\n\n\n\n\n\n\n\n\n");
    printf("                   *********                   *********                     \n");
    printf("           *********  Opulan U-boot Command Interface  *********              \n");
    printf("                   *********                   *********                     \n");
    printf("==============================================================================\n\n");
    printf("Note: the environment variable 'bootargs' set here will be changed unexpected \n");
    printf("in menu interface. The other environment variables here are shared in the menu \ninterface.\n");

    for(;;)
    {
        printf(CFG_PROMPT);
        oplReadline(0);

        if('e'==oplbootConsoleBuffer[0] && 'x'==oplbootConsoleBuffer[1] && 'i'==oplbootConsoleBuffer[2] && 't'==oplbootConsoleBuffer[3])
        {
            printf("\n\n                   *********                    *********                     \n");
            printf("             *********  Opulan U-boot Menu Interface  *********               \n");
            printf("                   *********                    *********                     \n");
            break;
        }
        else if(( 'h'==oplbootConsoleBuffer[0] && 'e'==oplbootConsoleBuffer[1] && 'l'==oplbootConsoleBuffer[2] && 'p'==oplbootConsoleBuffer[3] && '\0'==oplbootConsoleBuffer[4])||'?'==oplbootConsoleBuffer[0])
        {
            run_command("help",0);
            printf("exit    - exit to menu interface\n");
        }
        else
        {
            run_command(oplbootConsoleBuffer,0);
        }
    }
    oplbootSaveConfig();
}


/*******************************************************************************
*
* oplbootNameSet
*
* DESCRIPTION:
*   store name for struct MENU_ITEM_t.
*
* INPUT:
*   N/A
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootNameSet( void )
{ 
    oplReadline(0);
    if( '\0' != oplbootConsoleBuffer[0] )
    {
        setenv( pNow->envVarName,oplbootConsoleBuffer );
    }   
    
    /* save bootargs */  
    oplbootSaveConfig();
}

/*******************************************************************************
*
* oplbootCharDataInput
*
* DESCRIPTION:
*   store data for struct MENU_ITEM_t.
*
* INPUT:
*   format      =0 ; the input number is DEC
*               =2 ; the input number is HEX
*               !!!! any other number will arouse error !!!!
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootCharDataInput(int format)
{
    do
    {
        oplReadline(0);
        if('\0' != oplbootConsoleBuffer[0])
        {
            if(oplbootCharCheck(oplbootConsoleBuffer, format) == -1)
            {
                printf("Invalid \"number\"!please try again: ");
            }
            else
            {
                setenv( pNow->envVarName,oplbootConsoleBuffer );
            }
        }

    }while((-1==oplbootCharCheck(oplbootConsoleBuffer,format)) && ('\0'!=oplbootConsoleBuffer[0])); 
    
}

/*******************************************************************************
*
* oplbootTypeSel
*
* DESCRIPTION:
*   select the type sequence number,and store this type name.
*
* INPUT:
*   p           - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootTypeSel(void)
{
    MENU_ITEM_t *pTmp;
    int i=0;

    /*  store this type name  */
    pTmp = pNow;
    while( OPL_NULL != pTmp->header)
    {
        pTmp++;
    }
    
    setenv(pTmp->envVarName, pNow->header);

#ifndef  CONFIG_IPMUX_E2
    if( 0 == strcmp(pNow->header,"Flash_OS1")){
       setBootFlag(1);
	}else if(0 == strcmp(pNow->header,"Flash_OS2")){
	       setBootFlag(2);
		}
#endif
    /* save bootargs */  
    oplbootSaveConfig();

    /*  exit from current menu  */
    while(pNow->next == OPL_NULL)
    {
        pNow--;
        i--;
    }
    pNow=pNow->next;
    while(i < 0)
    {
        pNow++;
        i++;
    }
}

/*******************************************************************************
*
* oplbootSetEthIP
*
* DESCRIPTION:
*   This function do eth i p.
*
* INPUT:
*   N/A
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void  oplbootSetEthIP(void)
{
    int i=0;
    int j=0;
    int k=0;
    int macStart=0;
    char mac[4];
    char tmp[20]="0123456789ABCDEF";

    do
    {
        j=0;
        oplReadline( 0 );
        if('\0' != oplbootConsoleBuffer[0])
        {
            for(i=0 ; oplbootConsoleBuffer[i] != '\0' ; i++)
            {
                if('.' == oplbootConsoleBuffer[i])
                {   
                    j++;
                    /* Opulan technique define that mac address come from local ipaddress 
                     * For example, local IP 192.168.1.162, so mac address is 00:00:A2:CC:CC
                     *                                  |                            ^
                     *                                  -----------------------------| 
                     *                                      note that 162 = 0xA2 
                     */ 
                    /* mark the mac "162" */
                    if(3==j && 0==macStart)
                    {
                        macStart = i+1;
                    }
                }
                else
                {
                    if(!(oplbootConsoleBuffer[i]>='0' && oplbootConsoleBuffer[i]<='9'))
                    {
                         oplbootConsoleBuffer[1]='\0';
                    }
                }
            }

            if(oplbootConsoleBuffer[1]=='\0'||j!=3)
            {
                printf("Invalid ip address!please try INPUT like this '**.**.**.**': ");
                continue;
            }
            else
            {
                setenv( pNow->envVarName,oplbootConsoleBuffer );
            }
        }
    }while((oplbootConsoleBuffer[1]=='\0' || j!=3) && oplbootConsoleBuffer[0]!='\0');
#if 0   //added by kxu
    if(MENU_ETH_LOC_IP == pNow->ID && '\0' != oplbootConsoleBuffer[0] )
    {
        /*svae mac enviroment varialbes */
        for(i=macStart; oplbootConsoleBuffer[i]!='\0'; i++)
        {
            k = k*10 +(oplbootConsoleBuffer[i] - '0');
        }
        mac[0]=tmp[k/16];
        mac[1]=tmp[k%16];
        mac[2]='\0';
        strcpy(tmp,"00:00:");
        strcat(tmp,mac);
        strcat(tmp,":CC:CC:CC");
        setenv( "ethaddr",tmp);
        oplbootSaveConfig();
    }
#endif
}


/*******************************************************************************
*
* oplbootSaveConfig
*
* DESCRIPTION:
*   This function do serial bau.
*
* INPUT:
*   N/A        - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootSaveConfig(void)
{
    char *pType, *pLocation;

    #ifdef CONFIG_BOOT_TRANSFER_PARA
    char *pMemStore = startParaAddr;
    char bootTrasfer[100];
    char *pVariable;
    int i=0;
    char cmd[50]=""; /*zw 925*/
    pVariable = getenv("ipaddr");
    strcpy(bootTrasfer,"ipaddr=");
    strcat(bootTrasfer,pVariable);
    pVariable = getenv("ethaddr");
    strcat(bootTrasfer," ethaddr=");
    strcat(bootTrasfer,pVariable);
    while('\0' != bootTrasfer[i] )
    {
        *pMemStore = bootTrasfer[i];
        pMemStore++;
        i++;
    }
    *pMemStore = '\0';
    #endif
    
    pType = getenv("boottype");
    pLocation = getenv("bootloc");

    if( 0 == strcmp(pType,"Vxworks"))
    {
        if( 0 == strcmp(pLocation,"Network"))
        {
            run_command("setenv nfsbootvx tftp 0x80a00000 $(bootname)\\;bootvx 0x80a00000",0);
            run_command("setenv bootcmd run nfsbootvx",0);
        }
        else if( 0 == strcmp(pLocation,"Flash"))
        {
         #ifdef CONFIG_IPMUX_E2_C
		  sprintf(cmd, "setenv flashbootvx bootelf 0x%x", PHYS_FLASH_2);       
            #else
                sprintf(cmd, "setenv flashbootvx bootelf 0x%x", PHYS_FLASH_1+knl_1offset);  
            #endif
            run_command(cmd,0);
            run_command("setenv bootcmd run flashbootvx",0);
        }
    } else {

        /* linux kernel is at TFTP */
        if( 0 == strcmp(pLocation,"Network"))
        {
            run_command("setenv nfsboot tftp 0x80a00000 $(bootname)\\;bootm 0x80a00000",0);
            run_command("setenv bootcmd run nfsboot",0);
        }
        /* linux kernel is at flash */
 
   else if( 0 == strcmp(pLocation,"Flash_OS1"))   /*zw chang*/
        {
            #ifdef CONFIG_IPMUX_E2_C
		  sprintf(cmd, "setenv flashboot bootm 0x%x", PHYS_FLASH_2);   
            #else
              //  #if FLASHSIZE>=flash_8M
                sprintf(cmd, "setenv flashboot bootm 0x%x", PHYS_FLASH_1+knl_1offset); 
          //      run_command("setenv flashboot bootm 0xb0050000",0);    
               // #else
                  //  #ifdef CONFIG_IPMUX_ATHEROS
                 //   run_command("setenv flashboot bootm 0xbfc50000",0);
                  //  #else
                 //   run_command("setenv flashboot bootm 0xbfc40000",0);
                 //   #endif
             //   #endif
            #endif
	     run_command(cmd,0);
            run_command("setenv bootcmd run flashboot",0);
   	}
    else if( 0 == strcmp(pLocation,"Flash_OS2"))   /*zw chang*/
        {
        //    #if (defined(CONFIG_IPMUX_E2_C)) /*zw change 925*/
          //      run_command("setenv flashboot bootm 0xbe000000",0);
	 //    #if (defined(CONFIG_IPMUX_ONU_A_F23_TW))
		  sprintf(cmd, "setenv flashboot bootm 0x%x", PHYS_FLASH_1+knl_2offset);  
	//	 run_command("setenv flashboot bootm 0xb0460000",0);  /*zw change 925*/
      //      #else
            //    #if FLASHSIZE>=flash_8M
        //        run_command("setenv flashboot bootm 0xb0510000",0);  /*zw chang*/
              //  #else
              //      #ifdef CONFIG_IPMUX_ATHEROS
              //    run_command("setenv flashboot bootm 0xbfc50000",0);
               //     #else
               //     run_command("setenv flashboot bootm 0xbfc40000",0);
                //    #endif
              //  #endif
      //      #endif
            run_command(cmd,0);
            run_command("setenv bootcmd run flashboot",0);
        }

        /* begin added by jiangmingli, 20090309 */
        /* setenv will change boottype address, so get env once again */
        pType = getenv("boottype");
        /* end added by jiangmingli, 20090309 */

        /* linux boot args */
        if( 0 == strcmp(pType,"Linux-Nfs"))
        {
            run_command(nfsrootargs,0);
        }
        else if( 0 == strcmp(pType,"Linux-With-Ramdisk"))
        {
            run_command(emmrootargs,0);
        }
		else
		{
			run_command(emmrootargs,0);
		}

    }
}
/*******************************************************************************
*
* oplbootLoadSystem
*
* DESCRIPTION:
*   This function do serial bau.
*
* INPUT:
*   N/A        - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootLoadSystem(void)
{
    char *pType, *pLocation;
#ifndef  CONFIG_IPMUX_E2   /* we just chaned in ONU */
char cmd[50]="";
#endif
//#if  FLASHSIZE>flash_4M   /*zw add*/
 reCfgCs0(flash_info[0].size);
//#endif
    pType = getenv("boottype");
    pLocation = getenv("bootloc");
    if( 0 == strcmp(pType,"Vxworks"))
    {
        if( 0 == strcmp(pLocation,"Network"))
        {
            if( OPL_NULL == strstr(getenv("bootargs"),getenv("bootname")))
            {
                printf("Warning! You haven't configure the name Network vxWorks to be boot\n");
            }
        }
        else if( 0 == strcmp(pLocation,"Flash"))
        {
        }
    }
        
    else if( 0 == strcmp(pType,"Linux"))
    {
        if( 0 == strcmp(pLocation,"Network"))
        {
            if( OPL_NULL == strstr(getenv("bootargs"),getenv("bootname")))
            {
                printf("Warning! You haven't configure the name Network Linux to be boot.\n");
            }
            if( OPL_NULL == strstr(getenv("bootargs"),getenv("ipaddr")))
            {
                printf("Warning! You haven't configure the Local IP Address.\n");
            } 
            if( OPL_NULL == strstr(getenv("bootargs"),getenv("nfsserver")))
            {
                printf("Warning! You haven't configure the Boot server IP Address.\n");
            } 
            if( OPL_NULL == strstr(getenv("bootargs"),getenv("nfspath")))
            {
                printf("Warning! You haven't configure the Boot server Image Location.\n");
            }
            
        }
        else if(( 0 == strcmp(pLocation,"Flash_OS1"))||( 0 == strcmp(pLocation,"Flash_OS2")))
        {

            run_command(emmrootargs,0);
            
            if( OPL_NULL == strstr(getenv("bootargs"),getenv("ipaddr")))
            {
                printf("Warning! You haven't configure the Local IP Address.\n");
            } 
            if( OPL_NULL == strstr(getenv("bootargs"),getenv("ramdisk_size")))
            {
                printf("Warning! You haven't configure the OS Size in flash.\n");
            } 
        }

            
    }
    
//       if(OPL_NULL != pType && OPL_NULL != pLocation)
	if(1)
    {
//#ifdef  CONFIG_IPMUX_E2  
//        printf("Loading %s from %s...\n",pType,pLocation);
//#ifdef BOOTFLASH_LARGER_THAN_4M     /*zw chang*/
//        ipMuxRegWrite(REG_CS0_BASE_ADDR, 0x1f800);
//        ipMuxRegWrite(REG_CS0_SIZE, 0xf800);
//#endif
#ifndef  CONFIG_IPMUX_E2 

        if(( 0 == strcmp(pLocation,"Flash_OS1"))||( 0 == strcmp(pLocation,"Flash_OS2"))){
            if(((*(unsigned int*)(PHYS_FLASH_1+cfg_offset))&(0xFF000000))==0x32000000){
                printf("Loading %s from FLASH_OS2...\n",pType);
                sprintf(cmd, "bootm 0x%x",PHYS_FLASH_1+knl_2offset);
                if (run_command(cmd,0) < 0){
                    printf("Try to load %s from FLASH_OS1...\n",pType);
                    sprintf(cmd, "bootm 0x%x",PHYS_FLASH_1+knl_1offset);
                    run_command(cmd,0);
                }
            }else{
                printf("Loading %s from FLASH_OS1...\n",pType);
                sprintf(cmd, "bootm 0x%x",PHYS_FLASH_1+knl_1offset);
                if (run_command(cmd,0) < 0){
                    printf("Try to load %s from FLASH_OS2...\n",pType);
                    sprintf(cmd, "bootm 0x%x",PHYS_FLASH_1+knl_2offset);
                    run_command(cmd,0);
                }
            }
        }else
        {
            if( 0 != strcmp(pLocation,"Network"))
            {
                if(((*(unsigned int*)(PHYS_FLASH_1+cfg_offset))&(0xFF000000))==0x32000000){
                    printf("Loading linux from FLASH_OS2...\n");
                    sprintf(cmd, "bootm 0x%x",PHYS_FLASH_1+knl_2offset);
                    if (run_command(cmd,0) < 0){
                        printf("Try to load linux from FLASH_OS1...\n");
                        sprintf(cmd, "bootm 0x%x",PHYS_FLASH_1+knl_1offset);
                        run_command(cmd,0);
                    }
                }else{
                    printf("Loading linux from FLASH_OS1...\n");
                    sprintf(cmd, "bootm 0x%x",PHYS_FLASH_1+knl_1offset);
                    if (run_command(cmd,0) < 0){
                        printf("Try to load linux from FLASH_OS2...\n");
                        sprintf(cmd, "bootm 0x%x",PHYS_FLASH_1+knl_2offset);
                        run_command(cmd,0);
                    }
                }
            }
            else
            {
                printf("Loading %s from %s...\n",pType,pLocation);
                run_command("run bootcmd",0);
            }
        }
#else
        printf("Loading %s from %s...\n",pType,pLocation);
        run_command("run bootcmd",0);
#endif
    }
    else
    {
        printf("None OS have been founded, please configure 'Boot Config' at first before boot system.\n");
    }
}


/*******************************************************************************
*
* oplbootSerialBau
*
* DESCRIPTION:
*   This function do serial bau.
*
* INPUT:
*   p           - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   N/A
*
* SEE ALSO:
*/
void oplbootSerialBau(void)
{
    char ch;
    char *pValue;

    if('\0' != (pValue=getenv(pNow->envVarName)))
    {
        printf("\nThe current baudrate is [%s].\n\n",pValue);
    }
    printf("<1> 9600  \n<2> 14400 \n<3> 19200 \n<4> 38400 \n<5> 57600 \n<6> 115200 \n\n");
    printf("choose a serial port sequence number, kick 'ENTER' to cancel : ");

    do
    {
     ch = getc( );
        if(('\0' != ch)&&!(ch=='\r'))
        {
            if(!(ch>'0' && ch<='6'))
            {
                printf("Invalid number!please try again: %c%c%c",ch,ch,ch);
            }

        }
    }while(!(ch>'0' && ch<='6')&&!(ch=='\r'));

    switch(ch)
    {
        case '1':
            setenv(pNow->envVarName, "9600");
            break;
        case '2':
            setenv(pNow->envVarName, "14400");
            break;
        case '3':
            setenv(pNow->envVarName, "19200");
            break;
        case '4':
            setenv(pNow->envVarName, "38400");
            break;
        case '5':
            setenv(pNow->envVarName, "57600");
            break;
        case '6':
            setenv(pNow->envVarName, "115200");
            break;
        default:
            break;
    }
}

/*******************************************************************************
*
* delete_char
*
* DESCRIPTION:
*   This function delete_char.
*
* INPUT:
*   buffer      - 
*   p           - 
*   colp        - 
*   np          - 
*   plen        - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   char *      - 
*
* SEE ALSO:
*/
static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
{
    char *s;

    if (*np == 0) {
        return (p);
    }

    if (*(--p) == '\t') {           /* will retype the whole line   */
        while (*colp > plen) {
            puts (erase_seq);
            (*colp)--;
        }
        for (s=buffer; s<p; ++s) {
            if (*s == '\t') {
                puts (tab_seq+((*colp) & 07));
                *colp += 8 - ((*colp) & 07);
            } else {
                ++(*colp);
                putc (*s);
            }
        }
    } else {
        puts (erase_seq);
        (*colp)--;
    }
    (*np)--;
    return (p);
}
/*******************************************************************************
*
* oplReadline
*
* DESCRIPTION:
*   This function read a line.
*
* INPUT:
*   repeat_last - 
*
* OUTPUT:
*   N/A
*
* RETURNS:
*   int         -   number of read characters
*                   -1 if break
*                   -2 if timed out
*                   -3 if print last command    
*
* SEE ALSO:
*/
static int NetBreak1=0;
extern int enable_net_polling;
int oplReadline(char *repeat_last)
{
    char   *p = oplbootConsoleBuffer;
    int n = 0;              /* buffer index     */
    int plen = 0;           /* prompt length    */
    int col;                /* output column cnt    */
    char    c;

    if (repeat_last)
    {
        p=oplbootConsoleBuffer+strlen(oplbootConsoleBuffer);
    }
    
    col = plen;

    for (;;) {
#ifdef CONFIG_BOOT_RETRY_TIME
        while (!tstc()) {   
            /* while no incoming data */
            if (retry_time >= 0 && get_ticks() > endtime)
                return (-2);    /* timed out */
        }
#endif
        /* Trigger watchdog, if needed */
        /*
        WATCHDOG_RESET();       
        */

#ifdef CONFIG_SHOW_ACTIVITY
        while (!tstc()) {
            extern void show_activity(int arg);
            show_activity(0);
        }
#endif
        if (!NetBreak1)
        {   
            enable_net_polling=1;
        }
        while (!tstc()) {
        }
        enable_net_polling=0;
        
        c = getc();
        
        /*printf("key=0x%x\n",c);*/
        
        /*
         * Special character handling
         */
        switch (c) {
        case '\r':              /* Enter        */
        case '\n':
            *p ='\0' ;
            puts ("\r\n");
            return (p - oplbootConsoleBuffer);

        case '\0':              /* nul          */
            continue;
        case 0x60:
            return (-3);
        
        case 0x03:              /* ^C - break       */
            oplbootConsoleBuffer[0] = '\0'; /* discard input */
            puts ("\r\n");
            return (-1);

        case 0x15:              /* ^U - erase line  */
            while (col > plen) {
                puts (erase_seq);
                --col;
            }
            p = oplbootConsoleBuffer;
            n = 0;
            continue;

        case 0x17:              /* ^W - erase word  */
            p = delete_char(oplbootConsoleBuffer, p, &col, &n, plen);
            while ((n > 0) && (*p != ' ')) {
                p=delete_char(oplbootConsoleBuffer, p, &col, &n, plen);
            }
            continue;

        case 0x08:              /* ^H  - backspace  */
        case 0x7F:              /* DEL - backspace  */
            p=delete_char(oplbootConsoleBuffer, p, &col, &n, plen);
            continue;

        default:
            /*
             * Must be a normal character then
             */
            if (n < CFG_CBSIZE-2) {
                if (c == '\t') {    /* expand TABs      */
#ifdef CONFIG_AUTO_COMPLETE
                    /* if auto completion triggered just continue */
                    *p = '\0';
                    if (cmd_auto_complete(prompt, oplbootConsoleBuffer, &n, &col)) {
                        p = oplbootConsoleBuffer + n;   /* reset */
                        continue;
                    }
#endif
                    puts (tab_seq+(col&07));
                    col += 8 - (col&07);
                } else {
                    if  ((c>=0x20) && (c<=0x7F))
                    {   
                    ++col;      /* echo input       */
                    
                    putc (c);
                    }
                }
                *p++ = c;
                ++n;
            } else {            /* Buffer full      */
                putc ('\a');
            }
        }
    }
}
  #ifndef  CONFIG_IPMUX_E2 
int setBootFlag(int choose)
    {
       char *pbuffer;
       char cmd[50]="";
	int rcode;
       reCfgCs0(flash_info[0].size);

       pbuffer=malloc(knl_1offset-cfg_offset);
      memcpy(pbuffer,PHYS_FLASH_1+cfg_offset,knl_1offset-cfg_offset);
       if(choose==1){
           *pbuffer=0xff;
  	     }else
  	      *pbuffer=0x32;
		sprintf(cmd, "era 0x%x 0x%x", PHYS_FLASH_1+cfg_offset, (PHYS_FLASH_1+cfg_offset ) | (knl_1offset-cfg_offset-1));
            rcode=run_command(cmd,0);
             if (1!=rcode) return -1;
	     sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", pbuffer, PHYS_FLASH_1+cfg_offset, knl_1offset-cfg_offset);
           rcode=run_command(cmd,0);
		if (1!=rcode) return -1;
        cs0Cfgdefault();
	free(pbuffer);
	return 0;
}
#endif
#endif
