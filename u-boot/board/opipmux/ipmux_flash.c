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
#include <asm/op_ipmux.h>
#include <configs/opipmux.h>
/*
#ifdef CONFIG_IPMUX_OPULAN
#define DEBUG_FLASH
*/
//#define DEBUG_FLASH
//#ifndef CONFIG_IPMUX_2
//#ifndef CONFIG_IPMUX_ONU
#if defined(CONFIG_IPMUX_E2_C)||defined(CONFIG_IPMUX_E2_ZTE)
//flash_info_t    flash_info[CFG_MAX_FLASH_BANKS]; /* info for FLASH chips    */

/* NOTE - CONFIG_FLASH_16BIT means the CPU interface is 16-bit, it
 *        has nothing to do with the flash chip being 8-bit or 16-bit.
 */
//#ifdef CONFIG_FLASH_16BIT
typedef unsigned short FLASH_PORT_WIDTH;
typedef volatile unsigned short FLASH_PORT_WIDTHV;
#define FLASH_ID_MASK   0xFFFF
//#else
//typedef unsigned long FLASH_PORT_WIDTH;
//typedef volatile unsigned long FLASH_PORT_WIDTHV;
//#define   FLASH_ID_MASK   0xFFFFFFFF
//#endif

#define FPW FLASH_PORT_WIDTH
#define FPWV    FLASH_PORT_WIDTHV

#define ORMASK(size) ((-size) & OR_AM_MSK)

UINT32 CS0_REDEFINED = PHYS_FLASH_1;

#if 0
#define FLASH_CYCLE1    0x0555
#define FLASH_CYCLE2    0x02aa
#else
#define FLASH_CYCLE1    0x0555
#define FLASH_CYCLE2    0x02aa
#endif
/*-----------------------------------------------------------------------
 * Functions
 */
ulong flash_get_size(FPWV *addr, flash_info_t *info);
static void flash_reset(flash_info_t *info);
static int write_word_intel(flash_info_t *info, FPWV *dest, FPW data);
static int write_word_amd(flash_info_t *info, FPWV *dest, FPW data);
static void flash_get_offsets(ulong base, flash_info_t *info);
static flash_info_t *flash_get_info(ulong base);
//#ifdef CONFIG_IPMUX_E2_C

void initIPMuxCCs(void)
{
    /*--------------------------------------------------------*
     * ÎÄ¼þ FLASH£º CS1£¬BE000000~BE7FFFFF£¬ 16MB£¬ 16BIT SIZE*
     *--------------------------------------------------------*/
    /*------ 8M ------*/
    *(volatile unsigned int *)0xbf00104C = 0xF800;
    /*------ be000000 ------*/
    *(volatile unsigned int *)0xbf001024 = 0x1E000;
    /*------ 16bit ------*/
    *(volatile unsigned int *)0xbf001004 = 0x80047FFF;  
}
//#endif
/*-----------------------------------------------------------------------
 * flash_init()
 *
 * sets up flash_info and returns size of FLASH (bytes)
 */
unsigned long flash_init (void)
{
    unsigned long size = 0;
    int i;
//#if  (PHYS_FLASH_1==0xb0000000)
ipMuxRegWrite(REG_CS0_BASE_ADDR, (PHYS_FLASH_1&0x1FFFFFFF)>>12); 

//#ifdef CONFIG_IPMUX_E2_C
  initIPMuxCCs();
//#endif
    /* Init: no FLASHes known */
    for (i=0; i < CFG_MAX_FLASH_BANKS; ++i) {
        ulong flashbase = (i == 0) ? PHYS_FLASH_1 : PHYS_FLASH_2;
        /*ulong * buscon = (ulong *)
            ((i == 0) ? OP_IPMUX_EBU_EBU_BUSCON0 : OP_IPMUX_EBU_EBU_BUSCON2);unused variable `buscon'*/

        /* Disable write protection */
        /*
        *buscon &= ~OP_IPMUX_EBU_EBU_BUSCON1_WRDIS;
        */
#if 1
        memset(&flash_info[i], 0, sizeof(flash_info_t));
#endif

        flash_info[i].size = flash_get_size((FPW *)flashbase, &flash_info[i]);

        if (flash_info[i].flash_id == FLASH_UNKNOWN) {
    
            puts("##Unkonwn FLASH on Bank ");
            putc(i+'0');
            
            printf("flash_id = 0x%x\n",flash_info[i].flash_id);
            printf("- Size = 0x%08x\n",flash_info[i].size);
        }

        size += flash_info[i].size;
    	}

#if CFG_MONITOR_BASE >= CFG_FLASH_BASE
    /* monitor protection ON by default */
    flash_protect(FLAG_PROTECT_SET,
              CFG_MONITOR_BASE,
              CFG_MONITOR_BASE+monitor_flash_len-1,
              flash_get_info(CFG_MONITOR_BASE));
#endif

#ifdef  CFG_ENV_IS_IN_FLASH
    /* ENV protection ON by default */
    flash_protect(FLAG_PROTECT_SET,
              CFG_ENV_ADDR,
              CFG_ENV_ADDR+CFG_ENV_SIZE-1,
              flash_get_info(CFG_ENV_ADDR));
#endif

    #ifdef CONFIG_IPMUX_E2_ASKEY
  //     ipmuxCs0();
    #endif
cs0Cfgdefault();
    return size;
}

/*-----------------------------------------------------------------------
 */
static void flash_reset(flash_info_t *info)
{
    FPWV *base = (FPWV *)(info->start[0]);

    /* Put FLASH back in read mode */
    if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL)
        *base = (FPW)0x00FF00FF;    /* Intel Read Mode */
    else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_AMD)
        *base = (FPW)0x00F000F0;    /* AMD Read Mode */
    else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_MX)
        *base = (FPW)0x00F000F0;    /* MXIC Read Mode */
   else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST)
        *base = (FPW)0x00F000F0;    /* MXIC Read Mode */
    else
    {
        printf("flash_reset read unknown flash_id 0x%x\r\n", info->flash_id);
    }
}

/*-----------------------------------------------------------------------
 */
static void flash_get_offsets (ulong base, flash_info_t *info)
{
    int i;
 
    /* set up sector start address table */
    if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL
        && (info->flash_id & FLASH_BTYPE)) {
        int bootsect_size;  /* number of bytes/boot sector  */
        int sect_size;      /* number of bytes/regular sector */

        bootsect_size = 0x00002000 * (sizeof(FPW)/2);
        sect_size =     0x00010000 * (sizeof(FPW)/2);

        /* set sector offsets for bottom boot block type    */
        for (i = 0; i < 8; ++i) {
            info->start[i] = base + (i * bootsect_size);
        }
        for (i = 8; i < info->sector_count; i++) {
            info->start[i] = base + ((i - 7) * sect_size);
        }
    }
    else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_AMD
         && (info->flash_id & FLASH_TYPEMASK) == FLASH_AM640U) {

        int sect_size;      /* number of bytes/sector */

        sect_size = 0x00010000 * (sizeof(FPW)/2);

        dbgFlash("Assign the address for %d sector.",info->sector_count);

        /* set up sector start address table (uniform sector type) */
        for( i = 0; i < info->sector_count; i++ )
            info->start[i] = base + (i * sect_size);
    }
    else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST
         && (info->flash_id & FLASH_TYPEMASK) == FLASH_39VF160) {
        int sect_size;      /* number of bytes/sector */
        sect_size = 0x00000800 * (sizeof(FPW)/2);
        dbgFlash("Assign the address for %d sector.",info->sector_count);
        for( i = 0; i < info->sector_count; i++ )
            info->start[i] = base + (i * sect_size);
    }
    else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_AMD
         && (info->flash_id & FLASH_TYPEMASK) == FLASH_AM160LV) {

        int sect_size;      /* number of bytes/sector */
        info->start[0]=base;        /*16k*/
            
        info->start[1]=base+0x4000;     /*8k*/

        info->start[2]=base+0x6000;     /*8k*/

        info->start[3]=base+0x8000;     /*32k*/

        sect_size = 0x00010000 ;    /*the other sectors are all 64k*/

        for( i = 4; i < info->sector_count; i++ )
            info->start[i] = base + ((i-3) * sect_size);   /*each 64k*/
    }
    /* begin added by jiangmingli for ipmuxc_debug */
    else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_AMD
         && (info->flash_id & FLASH_TYPEMASK) == FLASH_AM29GL256P) {

        int sect_size;      /* number of bytes/sector */
        sect_size = 0x00008000;
        for( i = 0; i < info->sector_count; i++)
        {
            info->start[i] = base + (i * sect_size);
        }
    	}
	
 else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_AMD  
         && (info->flash_id & FLASH_TYPEMASK) == FLASH_AM29GL64M) {

        int sect_size;      /* number of bytes/sector */
        sect_size = 0x00002000 ;  
        for( i = 0; i < 8; i++ )
        	{
            info->start[i] = base + (i* sect_size);   
//	printf("sector %d start from addr 0x%x\n",i,  info->start[i]);
        	}

	 sect_size = 0x00010000 ;    /*64k*/

        for( i = 8; i < info->sector_count; i++ )
        	{
            info->start[i] = base + ((i-7)* sect_size);   /*each 64k*/
		
	//   printf("sector %d start from addr 0x%x\n",i,  info->start[i]);
        	}
    	}
    else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_MX
         && (info->flash_id & FLASH_TYPEMASK) == FLASH_AM160LV) {
        int sect_size;      /* number of bytes/sector */
        sect_size = 0x00002000 ;  
        for( i = 0; i < 4; i++ )
       	{
            info->start[i] = base + (i* sect_size);   
       	}
	 sect_size = 0x00008000 ;    /*128k*/
        for( i = 4; i < info->sector_count; i++ )
        	{
            info->start[i] = base + ((i-3)* sect_size);   /*each 64k*/
        	}
    	}
    else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_MX
         && (info->flash_id & FLASH_TYPEMASK) == FLASH_MXLV640EB) {

        int sect_size;      /* number of bytes/sector */
        /* 0-7 sector size is 0x1000 */
        sect_size = 0x00001000;
        for(i = 0; i < 8; i++)
        {
            info->start[i] = base + (i * sect_size);
        }

        /* 8-134 sector size is 0x8000 */
        sect_size = 0x00008000;
        for(i = 8; i < info->sector_count; i++)
        {
            info->start[i] = base + ((i - 7) * sect_size);
        }
        printf("sector_count=%u, i=%u\r\n", info->sector_count, i);
        printf("info->start[134]=0x%x\r\n", info->start[134]);
    }
    /* end added by jiangmingli for ipmuxc_debug */
    else
    {
        int sect_size;      /* number of bytes/sector */
        sect_size = info->size / info->sector_count;
        /* set sector offsets for bottom boot block type    */
        printf(" flash not fetch\n");
        for (i = 0; i < info->sector_count; i++) {
            info->start[i] = base + i * sect_size;
        }
    }
#ifdef  DEBUG_FLASH
    flash_print_allinfo();
#endif
    
}

void flash_print_allinfo(void)
{
    int bank;

    for (bank=0; bank <CFG_MAX_FLASH_BANKS; ++bank) {
           printf ("\nBank # %ld: ", bank+1);
        
           printf("----------&flash_info[bank]=%x",&flash_info[bank]);
           flash_print_info (&flash_info[bank]);
   }
}
/*-----------------------------------------------------------------------
 */

static flash_info_t *flash_get_info(ulong base)
{
    int i;
    flash_info_t * info;

    for (i = 0; i < CFG_MAX_FLASH_BANKS; i ++) {
        info = & flash_info[i];
        if (info->start[0] <= base && base < info->start[0] + info->size)
            break;
    }

    return i == CFG_MAX_FLASH_BANKS ? 0 : info;
}

/*-----------------------------------------------------------------------
 */

void flash_print_info (flash_info_t *info)
{
    int i;
    uchar *boottype;
    uchar *bootletter;
    uchar *fmt;
    uchar botbootletter[] = "B";
    uchar topbootletter[] = "T";
    uchar botboottype[] = "bottom boot sector";
    uchar topboottype[] = "top boot sector";

    if (info->flash_id == FLASH_UNKNOWN) {
        printf ("missing or unknown FLASH type\n");
        return;
    }
 reCfgCs0(flash_info[0].size);
    switch (info->flash_id & FLASH_VENDMASK) {
    case FLASH_MAN_AMD: printf ("AMD ");        break;
    case FLASH_MAN_BM:  printf ("BRIGHT MICRO ");   break;
    case FLASH_MAN_FUJ: printf ("FUJITSU ");        break;
    case FLASH_MAN_SST: printf ("SST ");        break;
    case FLASH_MAN_STM: printf ("STM ");        break;
    case FLASH_MAN_INTEL:   printf ("INTEL ");      break;
    /* begin added by jiangmingli for ipmuxc_debug */
     case FLASH_MAN_MX:   printf ("MXIC ");      break;
    /* end added by jiangmingli for ipmuxc_debug */
    default:        printf ("Unknown Vendor "); break;
    }

    /* check for top or bottom boot, if it applies */
    if (info->flash_id & FLASH_BTYPE) {
        boottype = botboottype;
        bootletter = botbootletter;
    }
    else {
        boottype = topboottype;
        bootletter = topbootletter;
    }

    switch (info->flash_id & FLASH_TYPEMASK) {
    case FLASH_AM29GL256P:
        fmt = "29GL256P (32 Mbit, uniform sectors)\n";
        break;
     case FLASH_AM29GL64M:
        fmt = "29GL64M (64 Mbit, uniform sectors)\n";
        break;
    case FLASH_AM640U:
        fmt = "29LV641D (64 Mbit, uniform sectors)\n";
        break;
    case FLASH_AM160LV:
        fmt = "29LV160DB (16 Mbit, 2M*8 bit bottom booting sectors)\n";
        break;
    case FLASH_28F800C3B:
    case FLASH_28F800C3T:
        fmt = "28F800C3%s (8 Mbit, %s)\n";
        break;
    case FLASH_INTEL800B:
    case FLASH_INTEL800T:
        fmt = "28F800B3%s (8 Mbit, %s)\n";
        break;
    case FLASH_28F160C3B:
    case FLASH_28F160C3T:
        fmt = "28F160C3%s (16 Mbit, %s)\n";
        break;
    case FLASH_INTEL160B:
    case FLASH_INTEL160T:
        fmt = "28F160B3%s (16 Mbit, %s)\n";
        break;
    case FLASH_28F320C3B:
    case FLASH_28F320C3T:
        fmt = "28F320C3%s (32 Mbit, %s)\n";
        break;
    case FLASH_INTEL320B:
    case FLASH_INTEL320T:
        fmt = "28F320B3%s (32 Mbit, %s)\n";
        break;
    case FLASH_28F640C3B:
    case FLASH_28F640C3T:
        fmt = "28F640C3%s (64 Mbit, %s)\n";
        break;
    case FLASH_INTEL640B:
    case FLASH_INTEL640T:
        fmt = "28F640B3%s (64 Mbit, %s)\n";
        break;
    /* begin added by jiangmingli for ipmuxc_debug */
    case FLASH_MXLV640EB:
        fmt = "MX29LV640EB%s (64 Mbit, %s)\n";
        break;
    /* end added by jiangmingli for ipmuxc_debug */
    default:
        fmt = "Unknown Chip Type\n";
        break;
    }

    printf (fmt, bootletter, boottype);

    printf ("  Size: %ld MB in %d Sectors\n",
        info->size >> 20,
        info->sector_count);

    printf ("  Sector Start Addresses:");

    for (i=0; i<info->sector_count; ++i) {
        if ((i % 5) == 0) {
            printf ("\n   ");
        }

        printf (" %08lX%s", info->start[i],
            info->protect[i] ? " (RO)" : "     ");
    }
cs0Cfgdefault();
    printf ("\n");
}

/*-----------------------------------------------------------------------
 */

/*
 * The following code cannot be run from FLASH!
 */

ulong flash_get_size (FPWV *addr, flash_info_t *info)
{
    /* Write auto select command: read Manufacturer ID */
    FPWV *base;     /* first address in flash bank  */
    
    base = (FPWV *)(addr);
 #ifdef    CONFIG_IPMUX_E2_ZTE
     base[0x5555] = (FPW)0x00AA00AA;   /* unlock */
    base[0x2aaa] = (FPW)0x00550055;   /* unlock */
    base[0x5555] = (FPW)0x00900090;   /* selects program mode */
 #else
    base[FLASH_CYCLE1] = (FPW)0x00AA00AA;   /* unlock */
    base[FLASH_CYCLE2] = (FPW)0x00550055;   /* unlock */
    base[FLASH_CYCLE1] = (FPW)0x00A00090;   /* selects program mode */
#endif

    /* The manufacturer codes are only 1 byte, so just use 1 byte.
     * This works for any bus width and any FLASH device width.
     */
#ifdef CONFIG_IPMUX_H3C 
    info->flash_id = FLASH_MAN_INTEL;
    switch ((uchar)INTEL_MANUFACT) {
#else
 printf("vendor ID = 0x%x\n",addr[0]);
    switch (addr[0] & 0xff) {
#endif
    case (uchar)AMD_MANUFACT:
        info->flash_id = FLASH_MAN_AMD;
	printf("---------AMD.\n");
        break;
   case (uchar)SST_MANUFACT:
        info->flash_id = FLASH_MAN_SST;
	printf("---------SST.\n");
        break;

    case (uchar)INTEL_MANUFACT:
        printf("--------- INTEL .\n");
        info->flash_id = FLASH_MAN_INTEL;
        break;    
    /* begin added by jiangmingli for ipmuxc_debug */
    case (uchar)MX_MANUFACT:
        info->flash_id = FLASH_MAN_MX;
       printf("---------MXIC.\n");
        break;
    /* end added by jiangmingli for ipmuxc_debug */
    default:
        printf("--------- FLASH_UNKNOWN Factory.\n");
        info->flash_id = FLASH_UNKNOWN;
        info->sector_count = 0;
        info->size = 0;
        break;
    }

    
    /* Check 16 bits or 32 bits of ID so work on 32 or 16 bit bus. */

#ifdef CONFIG_IPMUX_H3C 
    switch ((FPW)INTEL_ID_28F320C3B) {
#else
 printf("device ID = 0x%x\n",addr[1]);
    if (info->flash_id != FLASH_UNKNOWN) switch (addr[1]) {
#endif
    case (FPW)AMD_ID_LV640U:    /* 29LV640 and 29LV641 have same ID */
        info->flash_id += FLASH_AM640U;
        info->sector_count = 128;
        info->size = 0x00800000 * (sizeof(FPW)/2);
        break;              /* => 8 or 16 MB    */
    case   (FPW)SST_ID_39VF160://(FPW)SST_ID_39VF160:    /* 29LV640 and 29LV641 have same ID */
        info->flash_id += 	FLASH_39VF160;
        info->sector_count = 512;
        info->size = 0x00400000 * (sizeof(FPW)/2);
        break; 
    /* begin added by jiangmingli for ipmuxc_debug */
    case (FPW)AMD_ID_GL32M:    /* 32M, 256sectors, 128KB */
	
		   if  ((0x221f==(addr[14]))&&(0x2200==(addr[15]))){ 
		
		  printf("debug: AMD_ID_GL64M\n");
                 info->flash_id += FLASH_AM29GL64M;
                 info->sector_count = 135;
                 info->size = 0x0800000 * (sizeof(FPW)/2);  
		 }
	 else{
		
		   info->flash_id += FLASH_AM29GL256P;
                 info->sector_count = 128;
                 info->size = 0x0800000 * (sizeof(FPW)/2);
		}
	
        break;              /* => 8 or 16 MB    */
		
    	
        /* end added by jiangmingli for ipmuxc_debug */
    /* add support for AM29LV160DB type 2M*8 bottom boot flash  hs */

    case (FPW)AMD_ID_LV160B:
        info->flash_id += FLASH_AM160LV;
		if(0xc2==addr[0] & 0xff){
	 info->sector_count = 35;
        info->size = 0x00400000 * (sizeof(FPW)/2);
		}
		else	{
        info->sector_count = 35;
        info->size = 0x00200000 * (sizeof(FPW)/2);
    	}
        break;
    case (FPW)INTEL_ID_28F800C3B:
        info->flash_id += FLASH_28F800C3B;
        info->sector_count = 23;
        info->size = 0x00100000 * (sizeof(FPW)/2);
        break;              /* => 1 or 2 MB     */

    case (FPW)INTEL_ID_28F800B3B:
        info->flash_id += FLASH_INTEL800B;
        info->sector_count = 23;
        info->size = 0x00100000 * (sizeof(FPW)/2);
        break;              /* => 1 or 2 MB     */

    case (FPW)INTEL_ID_28F160C3B:
        info->flash_id += FLASH_28F160C3B;
        info->sector_count = 39;
        info->size = 0x00200000 * (sizeof(FPW)/2);
        break;              /* => 2 or 4 MB     */

    case (FPW)INTEL_ID_28F160B3B:
        info->flash_id += FLASH_INTEL160B;
        info->sector_count = 39;
        info->size = 0x00200000 * (sizeof(FPW)/2);
        break;              /* => 2 or 4 MB     */

    case (FPW)INTEL_ID_28F320C3B:
        info->flash_id += FLASH_28F320C3B;
        info->sector_count = 71;
        info->size = 0x00400000 * (sizeof(FPW)/2);
        break;              /* => 4 or 8 MB     */

    case (FPW)INTEL_ID_28F320B3B:
        info->flash_id += FLASH_INTEL320B;
        info->sector_count = 71;
        info->size = 0x00400000 * (sizeof(FPW)/2);
        break;              /* => 4 or 8 MB     */

    case (FPW)INTEL_ID_28F640C3B:
        info->flash_id += FLASH_28F640C3B;
        info->sector_count = 135;
        info->size = 0x00800000 * (sizeof(FPW)/2);
        break;              /* => 8 or 16 MB    */

    case (FPW)INTEL_ID_28F640B3B:
        info->flash_id += FLASH_INTEL640B;
        info->sector_count = 135;
        info->size = 0x00800000 * (sizeof(FPW)/2);
        break;              /* => 8 or 16 MB    */
    /* begin added by jiangmingli for ipmuxc_debug */
    case (FPW)MX_ID_LV640EB:
        info->flash_id += FLASH_MXLV640EB;
        info->sector_count = 135;
        info->size = 0x00400000 * (sizeof(FPW)/2);
        break;              /* => 8 or 16 MB    */
    /* end added by jiangmingli for ipmuxc_debug */
    default:
        printf("--------- FLASH_UNKNOWN , ID 0x%x.\n", addr[1]);
        info->flash_id = FLASH_UNKNOWN;
        info->sector_count = 0;
        info->size = 0;
        return (0);         /* => no or unknown flash */
    }

    
    flash_get_offsets((ulong)addr, info);

    
    /* Put FLASH back in read mode */
    flash_reset(info);

    
    return (info->size);
}

int flash_erase (flash_info_t *info, int s_first, int s_last)
{
    FPWV *addr;
    int flag, prot, sect;
    int intel = (info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL;
    ulong start, now, last;
    int rcode = 0;
 reCfgCs0(flash_info[0].size);
    if ((s_first < 0) || (s_first > s_last)) {
        if (info->flash_id == FLASH_UNKNOWN) {
            printf ("- missing\n");
        } else {
            printf ("- no sectors to erase\n");
        }
        return 1;
    }

    switch (info->flash_id & FLASH_TYPEMASK) {
    case FLASH_INTEL800B:
    case FLASH_INTEL160B:
    case FLASH_INTEL320B:
    case FLASH_INTEL640B:
    case FLASH_28F800C3B:
    case FLASH_28F160C3B:
    case FLASH_28F320C3B:
    case FLASH_28F640C3B:
    case FLASH_AM640U:
    case FLASH_AM29GL256P:
    case FLASH_AM29GL64M:
    case FLASH_AM160LV:
    case  FLASH_39VF160:
    /* begin added by jiangmingli for ipmuxc_debug */
    case FLASH_MXLV640EB:
    /* end added by jiangmingli for ipmuxc_debug */
        break;
    case FLASH_UNKNOWN:
    default:
        printf ("Can't erase unknown flash type %08lx - aborted\n",
            info->flash_id);
        return 1;
    }

    prot = 0;
    for (sect=s_first; sect<=s_last; ++sect) {
        if (info->protect[sect]) {
            prot++;
        }
    }

    if (prot) {
        printf ("- Warning: %d protected sectors will not be erased!\n",
            prot);
    } else {
        printf ("\n");
    }
    
    last  = get_timer(0);
    /* Start erase on unprotected sectors */
    for (sect = s_first; sect<=s_last && rcode == 0; sect++) {

        if (info->protect[sect] != 0)   /* protected, skip it */
            continue;

        /* Disable interrupts which might cause a timeout here */
        flag = disable_interrupts();

        addr = (FPWV *)(info->start[sect]);
        if (intel) {
            *addr = (FPW)0x00500050; /* clear status register */
            *addr = (FPW)0x00200020; /* erase setup */
            *addr = (FPW)0x00D000D0; /* erase confirm */
        }
        else {
            /* must be AMD style if not Intel */
            /* MXIC style is same witch AMD, marked by jiangmingli for ipmuxc_debug */
            FPWV *base;     /* first address in bank */
 #ifdef    CONFIG_IPMUX_E2_ZTE
             base = (FPWV *)(info->start[0]);
            base[0x5555] = (FPW)0x00AA00AA;   /* unlock */
            base[0x2aaa] = (FPW)0x00550055;   /* unlock */
            base[0x5555] = (FPW)0x00800080;   /* erase mode */
            base[0x5555] = (FPW)0x00AA00AA;   /* unlock */
            base[0x2aaa] = (FPW)0x00550055;   /* unlock */
            *addr = (FPW)0x00300030;    /* erase sector */
 #else
            base = (FPWV *)(info->start[0]);
            base[FLASH_CYCLE1] = (FPW)0x00AA00AA;   /* unlock */
            base[FLASH_CYCLE2] = (FPW)0x00550055;   /* unlock */
            base[FLASH_CYCLE1] = (FPW)0x00800080;   /* erase mode */
            base[FLASH_CYCLE1] = (FPW)0x00AA00AA;   /* unlock */
            base[FLASH_CYCLE2] = (FPW)0x00550055;   /* unlock */
            *addr = (FPW)0x00300030;    /* erase sector */
#endif			
        }

        /* re-enable interrupts if necessary */
        if (flag)
            enable_interrupts();

        start = get_timer(0);

        /* wait at least 50us for AMD, 80us for Intel.
         * Let's wait 1 ms.
         */
        puts(".");
        udelay (1000);
        puts(".");
        while ((*addr & (FPW)0x00800080) != (FPW)0x00800080) {
            if ((now = get_timer(start)) > CFG_FLASH_ERASE_TOUT) {
                printf ("Timeout\n");

                if (intel) {
                    /* suspend erase    */
                    *addr = (FPW)0x00B000B0;
                }

                flash_reset(info);  /* reset to read mode */
                rcode = 1;      /* failed */
                break;
            }

            /* show that we're waiting */
            if ((get_timer(last)) > CFG_HZ) {/* every second */
                putc ('.');
                last = get_timer(0);
            }
        }

        /* show that we're waiting */
        if ((get_timer(last)) > CFG_HZ) {   /* every second */
            putc ('.');
            last = get_timer(0);
        }

        flash_reset(info);  /* reset to read mode   */
    }

	cs0Cfgdefault();
    printf (" done\n");
    return rcode;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
    FPW data = 0; /* 16 or 32 bit word, matches flash bus width on MPC8XX */
    int bytes;    /* number of bytes to program in current word     */
    int left;     /* number of bytes left to program            */
    int i, res;
 reCfgCs0(flash_info[0].size);
    for (left = cnt, res = 0;
     left > 0 && res == 0;
     addr += sizeof(data), left -= sizeof(data) - bytes) {

    bytes = addr & (sizeof(data) - 1);
    addr &= ~(sizeof(data) - 1);

    /* combine source and destination data so can program
     * an entire word of 16 or 32 bits
     */
    /*wangxin:big endian*/
#ifdef BIGENDIAN    
    for (i = 0; i < sizeof(data); i++) {
        data <<= 8;
        if (i < bytes || i - bytes >= left )
        {   
            data += *((uchar *)addr + i);
        }
        else
        data += *src++;
    }
    
#else
    /*wangxin:little endian*/
    add=0;
    for (i = 0; i < sizeof(data); i++) {
        data <<= 8;
        if (i < bytes || i - bytes >= left )
        data += *((uchar *)addr +(sizeof(data)-i));
        else
        {
            data += *(src+sizeof(data)-1-i);
            add++;
        }
    }
    src+= add;
#endif  

    /* write one word to the flash */
    switch (info->flash_id & FLASH_VENDMASK) {
    case FLASH_MAN_AMD:
        //printf("addr=%x data=%x", (FPWV *)addr,data );
        //udelay(1000);
        res = write_word_amd(info, (FPWV *)addr, data);
        break;
    case FLASH_MAN_INTEL:
        res = write_word_intel(info, (FPWV *)addr, data);
        break;
    /* begin added by jiangmingli for ipmuxc_debug */
    case FLASH_MAN_MX:
        res = write_word_amd(info, (FPWV *)addr, data); //same witch amd
        case FLASH_MAN_SST:
        res = write_word_amd(info, (FPWV *)addr, data); //same witch amd
        break;
    /* end added by jiangmingli for ipmuxc_debug */
    default:
        /* unknown flash type, error! */
        printf ("missing or unknown FLASH type\n");
        res = 1;    /* not really a timeout, but gives error */
        break;
    }
    }
cs0Cfgdefault();
    return (res);
}


/*-----------------------------------------------------------------------
 * Write a word to Flash for AMD FLASH
 * A word is 16 or 32 bits, whichever the bus width of the flash bank
 * (not an individual chip) is.
 *
 * returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_word_amd (flash_info_t *info, FPWV *dest, FPW data)
{
    ulong start;
    int flag;
    int res = 0;    /* result, assume success   */
    FPWV *base;     /* first address in flash bank  */

    /* Check if Flash is (sufficiently) erased */
    if ((*dest & data) != data) {
    return (2);
    }

    base = (FPWV *)(info->start[0]);

    /* Disable interrupts which might cause a timeout here */
    flag = disable_interrupts();
 #ifdef    CONFIG_IPMUX_E2_ZTE
     base[0x5555] = (FPW)0x00AA00AA;   /* unlock */
    base[0x2aaa] = (FPW)0x00550055;   /* unlock */
    base[0x5555] = (FPW)0x00A000A0;   /* selects program mode */
 #else
    base[FLASH_CYCLE1] = (FPW)0x00AA00AA;   /* unlock */
    base[FLASH_CYCLE2] = (FPW)0x00550055;   /* unlock */
    base[FLASH_CYCLE1] = (FPW)0x00A000A0;   /* selects program mode */
#endif
    *dest = data;       /* start programming the data   */
    asm volatile (
    "sync"
    );
    
    /* re-enable interrupts if necessary */
    if (flag)
    enable_interrupts();

    start = get_timer (0);

    /* data polling for D7 */
    while (res == 0 && (*dest & (FPW)0x00800080) != (data & (FPW)0x00800080)) {
    if (get_timer(start) > CFG_FLASH_WRITE_TOUT) {
        printf("time=%d  ",get_timer(start) );
        printf("cfg_flash_tout=%d  \n",CFG_FLASH_WRITE_TOUT);

        *dest = (FPW)0x00F000F0;    /* reset bank */
        res = 1;
    }
    }

    return (res);
}

/*-----------------------------------------------------------------------
 * Write a word to Flash for Intel FLASH
 * A word is 16 or 32 bits, whichever the bus width of the flash bank
 * (not an individual chip) is.
 *
 * returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_word_intel (flash_info_t *info, FPWV *dest, FPW data)
{
    ulong start;
    int flag;
    int res = 0;    /* result, assume success   */

    /* Check if Flash is (sufficiently) erased */
    if ((*dest & data) != data) {
    return (2);
    }

    /* Disable interrupts which might cause a timeout here */
    flag = disable_interrupts();

    *dest = (FPW)0x00500050;    /* clear status register    */
    *dest = (FPW)0x00FF00FF;    /* make sure in read mode   */
    *dest = (FPW)0x00400040;    /* program setup        */

    *dest = data;       /* start programming the data   */

    /* re-enable interrupts if necessary */
    if (flag)
    enable_interrupts();

    start = get_timer (0);

    while (res == 0 && (*dest & (FPW)0x00800080) != (FPW)0x00800080) {
    if (get_timer(start) > CFG_FLASH_WRITE_TOUT) {
        *dest = (FPW)0x00B000B0;    /* Suspend program  */
        res = 1;
    }
    }

    if (res == 0 && (*dest & (FPW)0x00100010))
    res = 1;    /* write failed, time out error is close enough */

    *dest = (FPW)0x00500050;    /* clear status register    */
    *dest = (FPW)0x00FF00FF;    /* make sure in read mode   */

    return (res);
}

/* Change flash base address to 0xbe000000, also change the env addr to 0xbe000000 */
void ipmuxCs0()
{
    int i;
    /* 32M , 0xBE000000 */
    ipMuxRegWrite(REG_CS0_BASE_ADDR, 0x1E000);
    ipMuxRegWrite(REG_CS0_SIZE, 0xE000);

    CS0_REDEFINED = 0xbe000000;
    
    for (i = 0; i<flash_info[0].sector_count;i++) {
        flash_info[0].start[i] = flash_info[0].start[i] - PHYS_FLASH_1 + CS0_REDEFINED;
    }

    changePtr();

    printf("Flash base reallocated to 0x%x.\n", flash_info[0].start[0]);
}
#endif
//#endif
//#endif
