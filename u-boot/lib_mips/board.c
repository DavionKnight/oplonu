/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <devices.h>
#include <version.h>
#include <net.h>
#include <environment.h>
#include <configs/opipmux.h>


#ifdef CONFIG_IPMUX_H3C
#include "../board/opipmux/flash28.h"
#endif
#if ( ((CFG_ENV_ADDR+CFG_ENV_SIZE) < CFG_MONITOR_BASE) || \
      (CFG_ENV_ADDR >= (CFG_MONITOR_BASE + CFG_MONITOR_LEN)) ) || \
    defined(CFG_ENV_IS_IN_NVRAM)
    #define TOTAL_MALLOC_LEN    (CFG_MALLOC_LEN + CFG_ENV_SIZE)
#else
    #define TOTAL_MALLOC_LEN    CFG_MALLOC_LEN
#endif

#undef DEBUGNET

//#define debug(fmt,args...)    printf (fmt ,##args)

extern int timer_init(void);

extern int incaip_set_cpuclk(void);
extern int opipmux_set_cpuclk(void);

extern ulong uboot_end_data;
extern ulong uboot_end;

ulong monitor_flash_len;
char version_string[100]; 

/*
	PHY_SPEED_10M 	= 0,
	PHY_SPEED_100M 	= 1,
	PHY_SPEED_1000M	= 2
*/
unsigned short g_phy_speed = 1;

#if 0
#ifdef CONFIG_IPMUX
const char version_string[] =
U_BOOT_VERSION" ( R1.6 IPMux @ " __DATE__ " - " __TIME__ ")";
#endif

#ifdef CONFIG_IPMUX_E

const char version_string[] =
U_BOOT_VERSION" ( R1.6 IPMux-E @ " __DATE__ " - " __TIME__ ")";
#endif

#ifdef CONFIG_IPMUX_E2
const char version_string[] =
U_BOOT_VERSION" ( R1.6 IPMux-E2 @ " __DATE__ " - " __TIME__ ")";
#endif

#ifdef CONFIG_IPMUX_CLT
const char version_string[] =
U_BOOT_VERSION" ( R1.6 CLT @ " __DATE__ " - " __TIME__ ")";
#endif

#ifdef CONFIG_IPMUX_ONU
    #ifdef CONFIG_IPMUX_ONU_16BIT
    const char version_string[] =
    U_BOOT_VERSION" ( R1.62 ONU 16B @ " __DATE__ " - " __TIME__ ")";
    #endif
    #ifdef CONFIG_IPMUX_ONU_8BIT
    const char version_string[] =
    U_BOOT_VERSION" ( R1.62 ONU 8B @ " __DATE__ " - " __TIME__ ")";
    #endif

    #ifdef CONFIG_IPMUX_ONU_A_FPGA
    const char version_string[] =
    U_BOOT_VERSION" ( R1.63 ONU FPGA @ " __DATE__ " - " __TIME__ ")";
    #endif
#endif

#ifdef CONFIG_IPMUX_E_MPW
const char version_string[] =
U_BOOT_VERSION" ( R1.6 IPMux-E-MPW @ " __DATE__ " - " __TIME__ ")";
#endif

#ifdef CONFIG_10G_IPMUX_C2
const char version_string[] =
U_BOOT_VERSION" ( R1.6 ONU-10G-C2 16B @ " __DATE__ " - " __TIME__ ")";
#endif
#ifdef CONFIG_IPMUX_A
const char version_string[] =
U_BOOT_VERSION" ( R1.6 IPMux-A @ " __DATE__ " - " __TIME__ ")";
#endif
#endif

static char *failed = "*** failed ***\n";

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */
static ulong mem_malloc_start;
static ulong mem_malloc_end;
static ulong mem_malloc_brk;

/* begin added by jiangmingli */
#if 0
void version_init(void)
{
#ifdef CONFIG_IPMUX
    (void)sprintf(version_string, "%s (R%u IPMux @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_E
    (void)sprintf(version_string, "%s (R%u IPMux-E @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_E2
    (void)sprintf(version_string, "%s (R%u IPMux-E2 @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_CLT
    (void)sprintf(version_string, "%s (R%u CLT @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_ONU
    #ifdef CONFIG_IPMUX_ONU_16BIT
    (void)sprintf(version_string, "%s (R%u ONU 16B @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
    #endif
    #ifdef CONFIG_IPMUX_ONU_8BIT
    (void)sprintf(version_string, "%s (R%u ONU 8B @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
    #endif

    #ifdef CONFIG_IPMUX_ONU_A_FPGA
    sprintf(version_string, "%s (R%u ONU FPGA @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
    #endif
#endif

#ifdef CONFIG_IPMUX_E_MPW
    sprintf(version_string, "%s (R%u IPMux-E-MPW @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_A
    sprintf(version_string, "%s (R%u IPMux-A @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif
}
#endif
void version_init(char *szVerStr)
{
    if (NULL == szVerStr) 
    {
        return;
    }
#ifdef CONFIG_IPMUX
    (void)sprintf(szVerStr, "%s (R%u IPMux @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_E
    (void)sprintf(szVerStr, "%s (R%u IPMux-E @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_E2
    (void)sprintf(szVerStr, "%s (R%u IPMux-E2 @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_CLT
    (void)sprintf(szVerStr, "%s (R%u CLT @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_ONU
    #ifdef CONFIG_IPMUX_ONU_16BIT
    (void)sprintf(szVerStr, "%s (R%u ONU 16B @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
    #endif
    #ifdef CONFIG_IPMUX_ONU_8BIT
    (void)sprintf(szVerStr, "%s (R%u ONU 8B @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
    #endif
    #ifdef CFG_SPI_FLASH
   (void)sprintf(szVerStr, "%s (R%u ONU SPI @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
    #endif 
    #ifdef CFG_NEW_SPI_FLASH
   (void)sprintf(szVerStr, "%s (R%u ONU N SPI @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
    #endif 
    
    #ifdef CONFIG_IPMUX_ONU_A_FPGA
    sprintf(szVerStr, "%s (R%u ONU FPGA @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
    #endif
#endif

#ifdef CONFIG_IPMUX_E_MPW
    sprintf(szVerStr, "%s (R%u IPMux-E-MPW @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif

#ifdef CONFIG_IPMUX_A
    sprintf(szVerStr, "%s (R%u IPMux-A @ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif
#ifdef CONFIG_10G_IPMUX_C2
(void)sprintf(szVerStr, "%s (R%u optrann 16b DDR2@ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif
#ifdef CONFIG_10G_IPMUX_C3
(void)sprintf(szVerStr, "%s (R%u optrann SPI DDR3@ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif
#ifdef CONFIG_10G_IPMUX_C3_NJZTE
(void)sprintf(szVerStr, "%s (R%u optrann sp SPI DDR3@ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif
#ifdef CONFIG_10G_IPMUX_T3
(void)sprintf(szVerStr, "%s (R%u optrann 16b DDR3@ %s - %s)", U_BOOT_VERSION, BUILDNUMBER, __DATE__, __TIME__);
#endif
}
/* end added by jiangmingli */

/*
 * The Malloc area is immediately below the monitor copy in DRAM
 */
static void mem_malloc_init (void)
{
    DECLARE_GLOBAL_DATA_PTR;

    ulong dest_addr = CFG_MONITOR_BASE + gd->reloc_off;

    mem_malloc_end = dest_addr;
    mem_malloc_start = dest_addr - TOTAL_MALLOC_LEN;
    mem_malloc_brk = mem_malloc_start;
/*
        printf("memset 0xa0000000 to 0xa0100000.\n", mem_malloc_start, mem_malloc_end);
    memset(0xa0000000, 0 , 0x30000);

        printf("memset 0x%x to 0x%x.\n", mem_malloc_start, mem_malloc_end);
*/
    memset ((void *) mem_malloc_start,
            0,
            mem_malloc_end - mem_malloc_start);

}

void *sbrk (ptrdiff_t increment)
{
    ulong old = mem_malloc_brk;
    ulong new = old + increment;

    if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
        return(NULL);
    }
    mem_malloc_brk = new;
    return((void *) old);
}


static int init_func_ram (void)
{
    DECLARE_GLOBAL_DATA_PTR;

#ifdef  CONFIG_BOARD_TYPES
    int board_type = gd->board_type;
#else
    int board_type = 0; /* use dummy arg */
#endif

#ifdef CONFIG_IPMUX_ONU_A
#if defined(DDR2_UP_1G)
    puts("DRAM Total Size:\t128MB\n");
#else
    puts("DRAM Total Size:\t64MB\n");
#endif

#else
#if defined(DDR2_UP_1G)
    puts("UP DRAM Total Size:\t128MB\n");
#else
    puts("UP DRAM Total Size:\t64MB\n");
#endif

#if defined(DDR2_DN_1G)
    puts("DN DRAM Total Size:\t128MB\n");
#else
    puts("DN DRAM Total Size:\t64MB\n");
#endif
#endif

    puts ("DRAM Used By CPU:\t");

    if ((gd->ram_size = initdram (board_type)) > 0) {
        #ifdef DDR2_USED_BY_CPU_64M
            puts("64MB\n");
        #else
            #ifdef DDR2_USED_BY_CPU_96M
                puts("96MB\n");
            #else
                puts("32MB\n");
            #endif
        #endif
        return(0);
    }
    puts (failed);
    
    return(1);
}

static int display_banner(void)
{
    char szVerString[100];
    version_init(szVerString);
    printf ("\n\n%s\n\n", szVerString);
    return(0);
}

/*display_flash_config defined but not used
static void display_flash_config(ulong size)
{
    puts ("Flash: ");
    print_size (size, "\n");
}*/


static int init_baudrate (void)
{
    DECLARE_GLOBAL_DATA_PTR;

    uchar tmp[64];  /* long enough for environment variables */
    int i = getenv_r ("baudrate", tmp, sizeof (tmp));

    if (!( (i==9600) ||  (i==19200) || \
           (i==38400) ||  (i==57600) ||  \
           (i==115200) ||  (i==230400)  )) {
        i = CONFIG_BAUDRATE ;
    }

    gd->baudrate = (i > 0)
                   ? (int) simple_strtoul (tmp, NULL, 10)
                   : CONFIG_BAUDRATE;

    return(0);
}


/*
 * Breath some life into the board...
 *
 * The first part of initialization is running from Flash memory;
 * its main purpose is to initialize the RAM so that we
 * can relocate the monitor code to RAM.
 */

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependend #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */
typedef int (init_fnc_t) (void);
extern OPL_STATUS ipmuxDdrTune(void);
init_fnc_t *init_sequence[] = {
    timer_init,
    env_init,       /* initialize environment */
#ifdef CONFIG_INCA_IP
    incaip_set_cpuclk,  /* set cpu clock according to environment variable */
#endif
#ifdef CONFIG_OP_IPMUX
    opipmux_set_cpuclk, /* set cpu clock according to environment variable */
#endif
    init_baudrate,      /* initialze baudrate settings */
    serial_init,        /* serial communications setup */
    console_init_f,
    display_banner,     /* say that we are here */
    checkboard,
#if 0
#ifdef CONFIG_TUNE_DDR
    ipmuxDdrTune,
#endif
#endif
    init_func_ram,
    NULL,
};


void board_init_f(ulong bootflag)
{
    DECLARE_GLOBAL_DATA_PTR;

    gd_t gd_data, *id;
    bd_t *bd;
    init_fnc_t **init_fnc_ptr;
    ulong addr, addr_sp, len = (ulong)&uboot_end - CFG_MONITOR_BASE;
#ifdef CONFIG_PURPLE
    void copy_code (ulong);
#endif
    /* Pointer is writable since we allocated a register for it.
     */

    gd = &gd_data;
    /* compiler optimization barrier needed for GCC >= 3.4 */
    __asm__ __volatile__("": : :"memory");

    memset ((void *)gd, 0, sizeof (gd_t));
    /* This is temp hack to disable stdin, stdout */
    if ( gd->flags == 2 )
        gd->flags = 0;

    for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
        if ((*init_fnc_ptr)() != 0) {
            printf("hang at %x",init_fnc_ptr);
            hang ();
        }
    }

#ifdef DEBUGMODE
    printf("\n *** Warning : run in debug mode ??? ***\n\n");
#endif

    /*
     * Now that we have DRAM mapped and working, we can
     * relocate the code and continue running from DRAM.
     */
    addr = CFG_SDRAM_BASE + gd->ram_size;

    /* We can reserve some RAM "on top" here.
     */

    /* round down to next 4 kB limit.
     */
    addr &= ~(4096 - 1);
//	printf ("Top of RAM usable for U-Boot at: %08lx\n", addr);

    debug ("Top of RAM usable for U-Boot at: %08lx\n", addr);

    /* Reserve memory for U-Boot code, data & bss
     * round down to next 16 kB limit
     */
    addr -= len;
    addr &= ~(16 * 1024 - 1);

    debug ("Reserving %ldk for U-Boot at: %08lx\n", len >> 10, addr);
    /* Reserve memory for malloc() arena.
    */
    addr_sp = addr - TOTAL_MALLOC_LEN;
    debug ("Reserving %dk for malloc() at: %08lx\n",
           TOTAL_MALLOC_LEN >> 10, addr_sp);

    debug("CFG_MONITOR_BASE=%x\n",CFG_MONITOR_BASE);

#ifdef DEBUGMODE
    addr = CFG_MONITOR_BASE;
#endif

    /*
     * (permanently) allocate a Board Info struct
     * and a permanent copy of the "global" data
     */

    addr_sp -= sizeof(bd_t);
    bd = (bd_t *)addr_sp;
    gd->bd = bd;
    debug ("Reserving %d Bytes for Board Info at: %08lx\n",
           sizeof(bd_t), addr_sp);

    addr_sp -= sizeof(gd_t);
    id = (gd_t *)addr_sp;
    debug ("Reserving %d Bytes for Global Data at: %08lx\n",
           sizeof (gd_t), addr_sp);

    /* Reserve memory for boot params.
     */
    addr_sp -= CFG_BOOTPARAMS_LEN;
    bd->bi_boot_params = addr_sp;
    debug ("Reserving %dk for boot params() at: %08lx\n",
           CFG_BOOTPARAMS_LEN >> 10, addr_sp);

    /*
     * Finally, we set up a new (bigger) stack.
     *
     * Leave some safety gap for SP, force alignment on 16 byte boundary
     * Clear initial stack frame
     */
    addr_sp -= 16;
    addr_sp &= ~0xF;

    *((ulong *) addr_sp)-- = 0;
    *((ulong *) addr_sp)-- = 0;
    debug ("Stack Pointer at: %08lx\n", addr_sp);

    /*
     * Save local variables to board info struct
     */
    bd->bi_memstart = CFG_SDRAM_BASE;   /* start of  DRAM memory */
    bd->bi_memsize  = gd->ram_size;     /* size  of  DRAM memory in bytes */
    bd->bi_baudrate = gd->baudrate;     /* Console Baudrate */
#ifdef CONFIG_IPMUX_ONU_A_F23_D2_131
 #ifndef DEBUGMODE
  configDdr();	
 #endif
#endif

    memcpy (id, (void *)gd, sizeof (gd_t));

    /* On the purple board we copy the code in a special way
     * in order to solve flash problems
     */

    printf("In ram: %x\n",addr);

#ifdef CONFIG_PURPLE
    copy_code(addr);
#endif

 #ifdef  TEST_MEM_IN_EARLY_INIT
    if (*((int *)OPIPMUX_DRAM_TEST_END)==(OPIPMUX_DRAM_TEST_END-4)) {
        printf("Ram test passed.\n");
    } else {
        printf("---------- WARNING : ram test failed, err address 0x%x ---------\n",*((int *)OPIPMUX_DRAM_TEST_END));
    }
#endif

#ifdef DO_MEM_TEST
    testboard("test mem and uart in board_init_f.");
#endif

#ifdef INCLUDE_MEM_EVALUE_RW
    record_time;
    sdram_wr_asm();
    print_record_time;

    record_time;
    sdram_rd_asm();
    print_record_time;
#endif

    debug_boot_ram("Begin copy code to ram ... \n");


debug("sp:%x,id:%x,addr:%x\n");
#ifndef DEBUGMODE
  #ifndef NO_CP_RELO_CODE_TO_RAM
    copy_relocate_code_to_ram(addr_sp, id, addr);  //kxu
  #else
    relocate_code (addr_sp, id, addr);
  #endif
#else
    relocate_code (addr_sp, id, addr);
#endif

	debug_boot_ram("Shouldn't be here !\n");
    /* NOTREACHED - relocate_code() does not return */
}

/************************************************************************
 * add by wangxin:test uart and memeory
 *
 ************************************************************************
 */
void print_gp(char info[])
{
    //asm volatile("mfc0 %0, $0":"=r"(v):"0"(v));
    printf("CFG_SDRAM_BASE=%x,CFG_INIT_SP_OFFSET=%x,sp = %x\n",CFG_SDRAM_BASE , CFG_INIT_SP_OFFSET,CFG_SDRAM_BASE + CFG_INIT_SP_OFFSET);

    printf(" in %s\t",info);
    asm ("li $9,0x8000000c");
    asm ("sw $28, 0($9)");
    printf("gp=%x ",*(u32 *)0x8000000c);

    asm ("li $9,0x8000000c");
    asm ("sw $29, 0($9)");
    printf("sp=%x ",*(u32 *)0x8000000c);

    printf("\n");
}

void testboard(char info[])
{
#if defined(INCLUDE_MEM_TEST) || defined(INCLUDE_UART_TEST)
    printf("\n***************************************************************\n");
    printf("*   test board %s                                   *\n",info);
    printf("***************************************************************\n");
#endif

    

#ifdef INCLUDE_MEM_TEST
    extern void testcopy32(void);
    
    /*add by wangxin:test memory*/
    testcopy32();
#endif

    /*add by wangxin:test uart*/
#ifdef INCLUDE_UART_TEST
    testuart1();
    testuart2();
    testuart();
#endif


}
void board_init_r (gd_t *id, ulong dest_addr)
{
    DECLARE_GLOBAL_DATA_PTR;
    cmd_tbl_t *cmdtp;
    ulong size;
    IPaddr_t serverip;
    int iRet;
    int retVal;
#if 0
#define DEBUGNET  1
#endif


//#ifdef CONFIG_FE_PHY
    //ipmux_MII_write (0, 1, 0x10, 0x3c0);
    //ipMuxRegWrite(0x8220, 0x17);
//#endif   

#define logi(x) /* *((int *)0xa0f70000)=x */

    extern void malloc_bin_reloc (void);
#ifndef CFG_ENV_IS_NOWHERE
    extern char * env_name_spec;
#endif
    char *s, *e;
    bd_t *bd;
    int i;

    debug_boot_ram("in board init ram ... \n");
	

    gd = id;
    gd->flags |= GD_FLG_RELOC;  /* tell others: relocation done */

    gd->reloc_off = dest_addr - CFG_MONITOR_BASE;

    monitor_flash_len = (ulong)&uboot_end_data - dest_addr;
    /*add by wangxin:test board*/
    /* 
    testboard(" in board_init_r ");
    */

    logi(2);
    /*
     * We have to relocate the command table manually
     */
    for (cmdtp = &__u_boot_cmd_start; cmdtp !=  &__u_boot_cmd_end; cmdtp++) {
        ulong addr;

        addr = (ulong) (cmdtp->cmd) + gd->reloc_off;
#if 0
        printf ("Command \"%s\": 0x%08lx => 0x%08lx\n",
                cmdtp->name, (ulong) (cmdtp->cmd), addr);
#endif
        cmdtp->cmd =
        (int (*)(struct cmd_tbl_s *, int, int, char *[]))addr;

        addr = (ulong)(cmdtp->name) + gd->reloc_off;
        cmdtp->name = (char *)addr;

        if (cmdtp->usage) {
            addr = (ulong)(cmdtp->usage) + gd->reloc_off;
            cmdtp->usage = (char *)addr;
        }
#ifdef  CFG_LONGHELP
        if (cmdtp->help) {
            addr = (ulong)(cmdtp->help) + gd->reloc_off;
            cmdtp->help = (char *)addr;
        }
#endif
    }


    logi(3);
    /* there are some other pointer constants we must deal with */
#ifndef CFG_ENV_IS_NOWHERE
    env_name_spec += gd->reloc_off;
#endif

#ifndef CONFIG_NO_FLASH

    /* configure available FLASH banks */
    debug_boot_ram("config flash\n");

    logi(0x49);

#ifdef CONFIG_IPMUX_OPULAN
   size = flash_init();
#endif
#ifdef CONFIG_IPMUX_H3C
{
    /*
    Flash_Init();
    size = g_ulFlashSize;
    */
    size = flash_init();
}
#endif

#endif

    logi(0x40);
    bd = gd->bd;

    logi(0x41);
    /*udelay(1000);*/
    logi(0x42);
    bd->bi_flashstart = CFG_FLASH_BASE;

    bd->bi_flashsize = size;

#if CFG_MONITOR_BASE == CFG_FLASH_BASE
    bd->bi_flashoffset = monitor_flash_len; /* reserved area for U-Boot */
#else
    bd->bi_flashoffset = 0;
#endif

    /* initialize malloc() area */
    debug_boot_ram("init malloc\n");
    mem_malloc_init();
    debug_boot_ram("init reloc\n");
    malloc_bin_reloc();

    /* relocate environment function pointers etc. */
    debug_boot_ram("reloc env\n");
    env_relocate();

    logi(7);
    debug_boot_ram("init fe\n");

    /* board MAC address */
    s = getenv ("ethaddr");
    for (i = 0; i < 6; ++i) {
        bd->bi_enetaddr[i] = s ? simple_strtoul (s, &e, 16) : 0;
        if (s)
            s = (*e) ? e + 1 : e;
    }

    logi(8);

    debug_boot_ram("get ethaddr \n");
    /*debug_boot_ram("get ethaddr = %x-%x-%x-%x-%x-%x \n",bd->bi_enetaddr[0],\
            bd->bi_enetaddr[1],bd->bi_enetaddr[2],bd->bi_enetaddr[3],\
            bd->bi_enetaddr[4],bd->bi_enetaddr[5]);
    */
    /* IP Address */
    bd->bi_ip_addr = getenv_IPaddr("ipaddr");

#if defined(CONFIG_PCI)

    /*
     * Do pci configuration
     */
    pci_init();
#endif

    /** leave this here (after malloc(), environment and PCI are working) **/
    logi(9);
    debug_boot_ram("init devices\n");
    /* Initialize devices */
    devices_init ();

    debug_boot_ram("init jumptable\n");
    logi(90);
    jumptable_init ();

    logi(901);
    /* Initialize the console (after the relocation and devices init) */

    debug_boot_ram("initialize the console\n");
    console_init_r ();

    logi(902);
    /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/
    /* Initialize from environment */
    if ((s = getenv ("loadaddr")) != NULL) {
        load_addr = simple_strtoul (s, NULL, 16);
    }
#if (CONFIG_COMMANDS & CFG_CMD_NET)
    if ((s = getenv ("bootfile")) != NULL) {
        copy_filename (BootFile, s, sizeof (BootFile));
    }
#endif  /* CFG_CMD_NET */

    logi(903);
#if defined(CONFIG_MISC_INIT_R)

    /* miscellaneous platform dependent initialisations */
    misc_init_r ();
#endif

    ipMuxRegWrite(0x0008, 0x1);  //set Chip Initialization Register
    ipMuxRegWrite(0x000C, 0x1);  //set Link List Control Register

#if defined(CONFIG_IPMUX_ONU_A_F13) || defined(CONFIG_IPMUX_ONU_A_F13_TW) || defined(CONFIG_IPMUX_ATHEROS)
    /* Disable ONU PMAC */
    ipMuxRegWrite(0x2800, 0x000);
#endif

#ifdef CONFIG_IPMUX_ATHEROS
    printf("set Receive Maximum Packet Length 0x600\r\n");
    ipMuxRegWrite(0x3408, 0x600);
    #ifdef CONFIG_IPMUX_ATHEROS_HW_RESET
    printf("reset atheros switch\r\n");
    ipMuxRegWrite(0x2c84, 0x400);   //set GPIO 10 output direction
#if 1 /* modified by Gan Zhiheng, 2011-04-09 */
    ipMuxRegWrite(0x2c88, 0x000);   //set GPIO 10 output value
    udelay(15000);
    ipMuxRegWrite(0x2c8c, 0x400);   //clear GPIO 10 output value
    ipMuxRegWrite(0x2c88, 0x400);   //set GPIO 10 output value
#else
    ipMuxRegWrite(0x2c88, 0x400);   //set GPIO 10 output value
    ipMuxRegWrite(0x2c8c, 0x400);   //clear GPIO 10 output value
    printf("delay 10ms\r\n");
    udelay(15000);
    ipMuxRegWrite(0x2c88, 0x000);   //set GPIO 10 output value
    ipMuxRegWrite(0x2c8c, 0x000);   //clear GPIO 10 output value
    ipMuxRegWrite(0x2c84, 0x000);   //reset GPIO 10 direction
#endif
    #endif

	printf("start atheros init\r\n");
    iRet = atheros_init();
    if (0 != iRet)
    {
        printf("init atheros switch error: %d\r\n", iRet);
    }
    else
    {
    	printf("atheros init success\r\n");
    }
#endif

#if defined(CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M_BC)
#define PHY_ADD 0x1D
#define PHY_DAT 0x1E
#define PHY_DBG_REG5 0x05
#define PHY_RGMII_TXCLK_BIT 0x8

    ipMuxRegWrite(REG_GMAC_MODE, 0x1);
    ipMuxRegWrite(REG_GMAC_RGMII_TXDLL_CFG, 0x00800002);

    /******** Respin connect with AR8035 through RGMII, set Phy delay ********/
    mdioRegisterWrite(PHY_DEVICE_ADDR, PHY_ADD,PHY_DBG_REG5);
    mdioRegisterFieldWrite(PHY_DEVICE_ADDR,PHY_DAT,PHY_RGMII_TXCLK_BIT,1,1);
#endif

#if defined(CONFIG_ONU_1_1_RF400)
    /* GMAC as GMII mode */
    ipMuxRegWrite(REG_GMAC_MODE, 0x0);
    //ipMuxRegWrite(REG_MDIO_SELECT, 0x1);

    /* CP DMA enable */
    ipMuxRegWrite(REG_DMA0_EN, 0x3);
#endif

    /* Shutdown ONU UNI PHY */
    if( 0 != strcmp(getenv("bootloc"),"Network"))
    {
#if defined(CONFIG_IPMUX_ATHEROS)
        atheros_shutdown_phy();
#elif defined(CONFIG_IPMUX_ONU_A_F13)
        mdioRegisterFieldWrite(0x2, 0x0, 11, 1, 1);
#elif defined(CONFIG_IPMUX_ONU_A_F13_TW)
        mdioRegisterFieldWrite(0x1, 0x0, 11, 1, 1);
#elif defined(CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M_BC)
	    mdioRegisterFieldWrite(PHY_DEVICE_ADDR,0x0,11,1,1);
#endif
    }
    else
    {
#if defined(CONFIG_IPMUX_ATHEROS)
        atheros_poweron_phy();
#elif defined(CONFIG_IPMUX_ONU_A_F13)
        mdioRegisterFieldWrite(0x2, 0x0, 15, 1, 1);
        udelay(200000);
        /* check if linked */
        i = 100;
        unsigned short linkStatus;
        while(i)
        {
            udelay(100000);
            linkStatus = 0;
            mdioRegisterFieldRead(0x2, 0x11, 10, 1, &linkStatus);
            if (linkStatus)
            {
                printf("PHY linked\n");
                break;
            }
            i--;
        }
        g_phy_speed = 1;
    	mdioRegisterFieldRead(0x2, 0x11, 14, 2, &g_phy_speed);
        if (g_phy_speed == 2)
        {
            printf("PHY speed: 1000M\n");
        }
        else if (g_phy_speed == 0)
        {
            printf("PHY speed: 10M\n");
        }
        else
        {
            printf("PHY speed: 100M\n");
        }

#elif defined(CONFIG_IPMUX_ONU_A_F13_TW)
        mdioRegisterFieldWrite(0x1, 0x0, 15, 1, 1);
        udelay(200000);
        /* check if auto negotiation completed */
        i = 100;
        unsigned short linkStatus;
        while(i)
        {
            udelay(100000);
            linkStatus = 0;
            mdioRegisterFieldRead(0x1, 0x1, 5, 1, &linkStatus);
            if (linkStatus)
            {
                printf("PHY linked\n");
                break;
            }
            i--;
        }
        /* after completing auto negotiation, 
           this bit will reflect the Speed status */
        g_phy_speed = 1;
    	mdioRegisterFieldRead(0x1, 0x0, 13, 1, &g_phy_speed);
        if (g_phy_speed == 2)
        {
            printf("PHY speed: 1000M\n");
        }
        else if (g_phy_speed == 0)
        {
            printf("PHY speed: 10M\n");
        }
        else
        {
            printf("PHY speed: 100M\n");
        }
#elif defined(CONFIG_ONU_1_1_C13_SPI_DDR_SIZE_512M_BC)
        mdioRegisterFieldWrite(PHY_DEVICE_ADDR, 0x0, 15, 1, 1);
        udelay(200000);
        /* check if linked */
        i = 100;
        unsigned short linkStatus;
        while(i)
        {
            udelay(100000);
            linkStatus = 0;
            mdioRegisterFieldRead(PHY_DEVICE_ADDR, 0x11, 10, 1, &linkStatus);
            if (linkStatus)
            {
                printf("PHY linked\n");
                break;
            }
            i--;
        }
        g_phy_speed = 1;
    	mdioRegisterFieldRead(PHY_DEVICE_ADDR, 0x11, 14, 2, &g_phy_speed);
        if (g_phy_speed == 2)
        {
            printf("PHY speed: 1000M\n");
        }
        else if (g_phy_speed == 0)
        {
            printf("PHY speed: 10M\n");
        }
        else
        {
            printf("PHY speed: 100M\n");
        }
#endif
    }


#if (CONFIG_COMMANDS & CFG_CMD_NET) && defined(CONFIG_NET_MULTI)
#ifdef DEBUGNET
    printf(" ------------- enter gmac init driver ----------- \n");
#endif
    puts ("Net:\t");
    eth_initialize(gd->bd);
    eth_init(gd->bd);
    puts ("IP:\t\t");print_IPaddr(bd->bi_ip_addr); puts("\n");
    serverip=getenv_IPaddr ("serverip");
    puts ("Server IP:\t");print_IPaddr(serverip); puts("\n");
    printf ("Sys Clk Rate:\t%d\n", FPCLK);

#if 1
	printf ("ENV Addr   :\t%x\n", ENV_OFFSET+0xbfc00000);
	printf ("Kernel Addr:\t%x\n", KERNEL_ADDR);
    printf ("Svn version:\t%x\n", BUILDNUMBER);
#endif


#endif

/*kxu 20101113*/
{

//    char *config_ddr2_by_cpu;
//    *config_ddr2_by_cpu = CONFIG_DDR2_USED_BY_CPU;
    s = getenv ("mem");
    if (!s)
    {
        setenv ("mem",  "32");
        oplbootSaveConfig();
    }

/*kxu 20110429*/
    s = getenv ("oplhead");
    if (!s)
    {
#ifdef CONFIG_IPMUX_ONU_A
        setenv ("oplhead",  "on");
#else
		setenv ("oplhead",  "off");
#endif
        oplbootSaveConfig();
    }

}

/*end kxu 20101113*/


    debug_boot_ram("Init ok, enter command line.\n");

    /* main_loop() can return to retry autoboot, if so just run it again. */
    logi(0x93);
    for (;;) {
        main_loop ();
    }

    /* NOTREACHED - no way out of command loop except booting */
}

void hang (void)
{
    puts ("### ERROR ### Please RESET the board ###\n");
    for (;;);
}

void configDdr(void)   /**config ddr parm**/
{
   char i,j,k;             /*k :blocks of memory */      
#define val1 0xa1a2a3a4 
#define val2 0xb5b6b7b8
#define val3 0xc9cacbcc
#define val4 0xdddedfd0
  char icount[0x3f];   
  char jcount[0x3f];   
  char counti=0;
  char testpassed=0;
  int ipro,jpro;
  int tmpmax;
  int sum,tmpsum;
  int cnt,tmpcnt;
   for(i=0;i<0x3f;i++)
	  icount[i]=0;
   for(j=0;j<0x3f;j++)
	  jcount[j]=0;
 for(i=0;i<=0x3f;i++){
  	 *(volatile unsigned int*)0xbf014038=(unsigned int)(0x00000019|(i<<16)|(i<<24));    
  	for(j=0;j<=0x3f;j++){
		    *(volatile unsigned int*)0xbf014064=(unsigned int)(0x00005858|(j<<16)|(j<<24));  
        for(k=0;k<0x1;k++){
		*(unsigned int*)(0xa1000000+(k<<4))=val1;
		*(unsigned int*)(0xa1000008+(k<<4))=(0xa1000000+(k<<4));
		if(*(unsigned int*)(*(unsigned int*)(0xa1000008+(k<<4)))!=val1)
	              continue;
		*(unsigned int*)(0xa1000010+(k<<4))=val2;
		*(unsigned int*)(0xa1000018+(k<<4))=(0xa1000010+(k<<4));
		if(*(unsigned int*)(*(unsigned int*)(0xa1000018+(k<<4)))!=val2)
		       continue;
		*(unsigned int*)(0xa1000020+(k<<4))=val3;
		*(unsigned int*)(0xa1000028+(k<<4))=(0xa1000020+(k<<4));
		if(*(unsigned int*)(*(unsigned int*)(0xa1000028+(k<<4)))!=val3)
		       continue;
		*(unsigned int*)(0xa1000030+(k<<4))=val4;
		*(unsigned int*)(0xa1000038+(k<<4))=(0xa1000030+(k<<4));
		if(*(unsigned int*)(*(unsigned int*)(0xa1000038+(k<<4)))!=val4)
	                continue;
	          testpassed++;
        	}
	     if (testpassed==k)	
		 {
                counti++;
	         jcount[j]++;
		 }	 
		  testpassed=0; 	
 	 }
          icount[i]=counti;
          counti=0;
    }
      tmpmax=0;
      sum=0;
      cnt=0;
     for(i=0;i<0x3f;i++)	
        {
          if(tmpmax<icount[i])
          	{
		tmpmax=icount[i]; 
		tmpsum=sum=i;     /*sum of register*/
		tmpcnt=cnt=1;   
          	}
          else{	  
	        if (tmpmax=icount[i])  {
		     sum+=i;	
		     cnt++;	
		     if(tmpcnt<cnt){
			 tmpsum=sum;
                      tmpcnt=cnt;
		     	}
		    }else{
		               sum=0;
		               cnt=0;
	  	              }  	
          	}
        }
        ipro=tmpsum/tmpcnt;
          tmpmax=0;
          sum=0;
          cnt=0;
	  for(j=0;j<0x3f;j++)	
        {
          if(tmpmax<jcount[j])
          	{
		tmpmax=jcount[j]; 
		tmpsum=sum=j;    /*sum of register*/
		tmpcnt=cnt=1;
          	}
          else{	  
	        if (tmpmax=jcount[j])  {
		     sum+=j;	
		     cnt++;	
		     if(tmpcnt<cnt){
			 tmpsum=sum;
                      tmpcnt=cnt;
		     	}
		    }else{
		               sum=0;
		               cnt=0;
	  	              }  	
          	  }
        }
       jpro=tmpsum/tmpcnt;
  *(volatile unsigned int*)0xbf014038=(unsigned int)(0x0000c019|(ipro<<16)|(ipro<<24)); 
  *(volatile unsigned int*)0xbf014064=(unsigned int)(0x00005858|(jpro<<16)|(jpro<<24)); 
}
