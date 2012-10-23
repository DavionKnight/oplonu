/*
 * (C) Copyright 2001
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

/*
 * RTC, Date & Time support: get and set date & time
 */
#include <common.h>
#include <command.h>

#if (CONFIG_COMMANDS & CFG_CMD_UU)
extern unsigned int tftp_file_size;
//unsigned char static g_OplHeadSwitch=1;
#ifdef  OPULAN_IMAGE_HEADER 
#define TYPE_FIRMWARE	 5	/* Firmware Image		*/
#define TYPE_KERNEL		 2	/* Kernel Image			*/
#define TYPE_FILESYSTEM	 7	/* Filesystem Image (any type)	*/
extern UINT32 image_type;
extern  int imagePreCheck(unsigned int fileaddr);
extern  int bootUpdateSimpleCheck(unsigned int fileaddr);
#endif
int promt_cancel(int second);


/*kxu 20110429*/
unsigned char getOplHeadSwitchStatus(void)
{
    char *s;
    s = getenv("oplhead");
    if (strcmp(s, "on") == 0) 
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
*
* orr   -Opulan Read Register
*
* DESCRIPTION:
*    read register and print the value of specified register to STANDARD OUTPUT device.
*
* INPUTS:
*   regid   - register ID.
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, if the read successful.
*
* SEE ALSO: 
*/
OPL_STATUS orr (UINT32 regid)
{
    UINT32 val;
    
    ipMuxRegRead(regid, &val);
    
    printf("register %d, value= 0x%08x.\n", regid, (UINT32)val);
    return OPL_OK;
}

/*******************************************************************************
*
* owr   -Opulan Write Register
*
* DESCRIPTION:
*   write register.
*
* INPUTS:
*   regid   - register ID.
*
* OUTPUT:
*   n/a.
*
* RETURNS:
*   OPL_OK, if the read successful.
*
* SEE ALSO: 
*/
OPL_STATUS owr (UINT32 regid, UINT32 val)
{
    
    ipMuxRegWrite(regid, val);
    return OPL_OK;
}


int do_owr (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    extern OPL_STATUS owr (UINT32 regid, UINT32 val);
    UINT32 regid;
    UINT32 val;

    switch (argc) {
    case 3:     
        regid=simple_strtoul((char *)argv[1],NULL,16);
        val=simple_strtoul((char *)argv[2],NULL,16);
        break;
    default:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    owr(regid, val);
    return 1;
}

int do_orr (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    extern OPL_STATUS orr (UINT32 regid);
    UINT32 regid;

    switch (argc) {
    case 2:     
        regid=simple_strtoul((char *)argv[1],NULL,16);
        break;
    default:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    orr(regid);
    return 1;
}

#if 0
#if defined(CFG_NEW_SPI_FLASH)||defined(CFG_SPI_FLASH) 
/* spi flash */
int do_uutftp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    int rcode = 0;
    char filename[20]="u-boot.bin";
    char cmd[50] = "";
    char pstr[50] = "tftpboot 0x80600000 ";
     int  size =0x40000;
    
    if( OPL_NULL != getenv("flash_boot_name"))
    {
        strcpy(filename,getenv("flash_boot_name")); 
    }   
    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        //printf ("Usage:\n%s\n", cmdtp->usage);
        rcode = 1;
    }

    #ifdef CONFIG_IPMUX_ATHEROS
    printf("We allocate 0xb0000000-0xb004ffff for U-boot, so max size of u-boot.bin should be 256KB!\n");
    #else
    printf("We allocate 0xb0000000-0xb003ffff for U-boot, so max size of u-boot.bin should be 256KB!\n");
    #endif
    printf("Are you sure to update U-boot? (Hit any key to abort!) ");
    if (promt_cancel(5)) return -1;
    
    strcat(pstr,filename);
    strcpy(cmd,pstr);
    printf("\n------ Download file \t------ \n");
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    printf("\n------ Unprotect flash \t------\n");
    #ifdef CONFIG_IPMUX_ATHEROS
    rcode=run_command("protect off 0xb0000000 0xb003ffff",0);
    #else
    rcode=run_command("protect off 0xb0000000 0xb003ffff",0);
    #endif
    if (1!=rcode) return -1;

    printf("\n------ Burn SPI flash \t------\n");
    #ifdef CONFIG_IPMUX_ATHEROS
    rcode=run_command("spi_burn 0x80600000 0x3fff0",0);
    #else
    rcode=run_command("spi_burn 0x80600000 0x3fff0",0);
    #endif
    if (1!=rcode) return -1;

    printf("********************************************************\n");
    printf("*   U-boot is updated, please power down the board!    *\n");
    printf("********************************************************\n");

    return 1;
}
#endif
#else
/* p flash */
int do_uutftp (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    int rcode = 0;
    char filename[20]="u-boot.bin";
    char cmd[50] = "";
    char pstr[50] = "tftpboot 0x80600000 ";
    int  size  ;
    
    if( OPL_NULL != getenv("flash_boot_name"))
    {
        strcpy(filename,getenv("flash_boot_name")); 
    }   
    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        //printf ("Usage:\n%s\n", cmdtp->usage);
        rcode = 1;
    }

    printf("We allocate 0xb0000000-0xb003ffff for U-boot, so max size of u-boot.bin should be 256KB!\n");
    printf("Are you sure to update U-boot? (Hit any key to abort!) ");
    if (promt_cancel(5)) return -1;
    
    strcat(pstr,filename);
    strcpy(cmd,pstr);
    printf("\n------ Download file \t------ \n");
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    size=tftp_file_size;  /*zw add in 1123*/
    if(0!=bootUpdateSimpleCheck(0x80600000))
	return -1; 
    printf("\n------ Unprotect flash \t------\n");
    sprintf(cmd, "protect off 0x%x 0x%x", PHYS_FLASH_1, PHYS_FLASH_1 + 0x3ffff/*(size | 0xFFFF)*/);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    printf("\n------ Erase flash \t------\n");
    sprintf(cmd, "era 0x%x 0x%x", PHYS_FLASH_1, PHYS_FLASH_1 + 0x3ffff/*(size | 0xFFFF)*/);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    printf("\n------ Program flash \t------\n");
    sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", 0x80600000, PHYS_FLASH_1, size);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    printf("\n------ Protect flash \t------\n");
    sprintf(cmd, "protect on 0x%x 0x%x", PHYS_FLASH_1, PHYS_FLASH_1 +  0x3ffff/*(size | 0xFFFF)*/);
    rcode=run_command(cmd,0);

    if (1!=rcode) return -1;

    printf("********************************************************\n");
    printf("*   U-boot is updated, please power down the board!    *\n");
    printf("********************************************************\n");

    return 1;
}
#endif

//#ifndef CFG_SPI_FLASH    /**zw 918*/
#if 0
	#ifdef CONFIG_IPMUX_ATHEROS
		#define OS_START 0xbfc50000
	#else
		#define OS_START 0xbfc40000
	#endif
/* spi */
int do_uuos (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    int size = 0;
    int src = 0;
    int dest = 0;
    int rcode = 0;
    char filename[20]="uImage";
    char cmd[50]="";
    char pstr[50] = "tftpb 0x80600000 ";
    if( 0 != strcmp("current",getenv("flash_os_name"))) 
    {
        strcpy(filename,getenv("flash_os_name")); 
    }
    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        //printf ("Usage:\n%s\n", cmdtp->usage);
        rcode = 1;
    }

    printf("Are you sure to update OS image[0x%x~0x%x]? (Hit any key to abort!) ", 
        OS_START, OS_START + OS_MAX_SIZE);


    if (promt_cancel(5)) return -1;

    strcat(pstr,filename);
    strcpy(cmd,pstr);
    printf("\n------ Download file \t------\n");
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    if (tftp_file_size > OS_MAX_SIZE) {
        printf("\nERROR: The Os update size is larger than 0x%x\n", OS_MAX_SIZE);
        return 1;
    }
    
    printf("\n------ Burn SPI OS flash %d\t------\n",tftp_file_size);
    sprintf(cmd, "spi_burn_os 0x80600000 %x", tftp_file_size);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
    
    printf("********************************************************\n");
    printf("*           Os image is updated successfully!          *\n");
    printf("********************************************************\n");

    return 1;
}
#else
/* p */
int do_uuos1 (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    int size = 0;
    int src = 0;
    int dest = 0;
    int rcode = 0;
    char filename[20]="uImage";
    char cmd[50]="";
    char pstr[50] = "tftpb 0x80600000 ";
    if( 0 != strcmp("current",getenv("flash_file_name"))) 
    {
        strcpy(filename,getenv("flash_file_name")); 
    }
    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        //printf ("Usage:\n%s\n", cmdtp->usage);
        rcode = 1;
    }

/* #ifdef CONFIG_IPMUX_E2_C */  /*zw changed it in 1214*/
#if 0
    printf("Are you sure to update OS image[0x%x~0x%x]? (Hit any key to abort!) ", PHYS_FLASH_2, PHYS_FLASH_2 + OS_MAX_SIZE);
#else
    printf("Are you sure to update OS image[0x%x~0x%x]? (Hit any key to abort!) ", PHYS_FLASH_1+knl_1offset, PHYS_FLASH_1 + knl_2offset-1);
#endif

    if (promt_cancel(5)) return -1;

    strcat(pstr,filename);
    strcpy(cmd,pstr);
    printf("\n------ Download file \t------\n");
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

#ifdef  OPULAN_IMAGE_HEADER  
    if(getOplHeadSwitchStatus())
    {
         if(0!=imagePreCheck(0x80600000))
    	 	return -1; 
    }
#endif  	
    if (tftp_file_size > OS_MAX_SIZE) {
        printf("\nERROR: The Os update size is larger than 0x%x\n", OS_MAX_SIZE);
        return 1;
    }

 #ifdef  OPULAN_IMAGE_HEADER
    if(getOplHeadSwitchStatus())
    {
        if (image_type!=TYPE_KERNEL){
            printf("ERROR:NOT KERNEL IMAGE\n");
            return -1;
        }
    }
#endif
#if 0
    /* there is a file flash in IPMUXC */
    size = tftp_file_size;
    /*assume all flash sector is 0x10000*/
    src = 0x80600000;
    dest = 0xbe000000;
    sprintf(cmd, "era 0x%x 0x%x", dest, dest + (size | 0xFFFF));
    printf("\n------ Erase flash [%s]\t------\n", cmd);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
    
    sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", src, dest, size);
    printf("\n------ Program flash [%s]\t------\n", cmd);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
#else
    
    /* No separated file flash, os is stored in bootflash*/
  //  #if  FLASHSIZE>flash_4M
        /*tbd : test this condition. */
        printf("This condition hasn't been tested.\n");
        run_command("protect off all",0);
        /* ------------- first 4M ---------------*/
        size = tftp_file_size<=0x3D0000?tftp_file_size:0x3D0000;
        /*assume all flash sector is 0x10000*/
        src = 0x80600000;
        dest = PHYS_FLASH_1+knl_1offset; /*zw change*/
        sprintf(cmd, "era 0x%x 0x%x", dest, dest + (size | 0xFFFF));
        printf("\n------ Erase flash111 [%s]\t------\n", cmd);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
        
        sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", src, dest, size);
        printf("\n------ Program flash [%s]\t------\n", cmd);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
#if 0
        if (tftp_file_size>0x3D0000) {
            size = tftp_file_size - 0x3D0000;
        
            /* ------------- last 4M ---------------*/
            ipMuxRegWrite(REG_CS0_BASE_ADDR, 0x1f800);
            ipMuxRegWrite(REG_CS0_SIZE, 0xf800);
    
            /*assume all flash sector is 0x10000*/
            src = 0x80600000 + 0x3D0000;
            dest = 0xbfc00000;
            sprintf(cmd, "era 0x%x 0x%x", dest, dest + (size | 0xFFFF));
            printf("\n------ Erase flash222 [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
            if (1!=rcode) return -1;
            
            sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", src, dest, size);
            printf("\n------ Program flash [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
            if (1!=rcode) return -1;
    
            ipMuxRegWrite(REG_CS0_BASE_ADDR, 0x1fc00);
            ipMuxRegWrite(REG_CS0_SIZE, 0xfc00);
        }
//#endif
  //  #else 
        sprintf(cmd, "era 0x%x 0x%x", KERNEL_ADDR, KERNEL_ADDR + (tftp_file_size | FLASH_SECTOR_MASK));
        printf("\n------ Erase flash [%s]\t------\n", cmd);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
        printf("\n------ Program flash \t------\n");
        sprintf(cmd, "cp.b 0x80600000 0x%x 0x%x", KERNEL_ADDR, tftp_file_size);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
    #endif
#endif
    printf("********************************************************\n");
    printf("*           Os image is updated successfully!          *\n");
    printf("********************************************************\n");
    return 1;
}
int do_uuos2 (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int size = 0;
    int src = 0;
    int dest = 0;
    int rcode = 0;
    char filename[20]="uImage";
    char cmd[50]="";
    char pstr[50] = "tftpb 0x80600000 ";
    if( 0 != strcmp("current",getenv("flash_file_name"))) 
    {
        strcpy(filename,getenv("flash_file_name")); 
    }
    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        rcode = 1;
    }
/* #ifdef CONFIG_IPMUX_E2_C */  /*zw changed it in 1214*/
#if 0
    printf("Are you sure to update OS image[0x%x~0x%x]? (Hit any key to abort!) ", PHYS_FLASH_2, PHYS_FLASH_2 + OS_MAX_SIZE);
#else
    printf("Are you sure to update OS image[0x%x~0x%x]? (Hit any key to abort!) ", PHYS_FLASH_1+knl_2offset, PHYS_FLASH_1 + 0x7fffff);
#endif
    if (promt_cancel(5)) return -1;
    strcat(pstr,filename);
    strcpy(cmd,pstr);
    printf("\n------ Download file \t------\n");
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
#ifdef  OPULAN_IMAGE_HEADER  
    if(getOplHeadSwitchStatus())
    {
        if(0!=imagePreCheck(0x80600000))
         	return -1; 
    }
 #endif  	
 #ifdef  OPULAN_IMAGE_HEADER
    if(getOplHeadSwitchStatus())
    {
        if (image_type!=TYPE_KERNEL){
            printf("ERROR:NOT KERNEL IMAGE\n");
            return -1;
        }
    }
#endif
    if (tftp_file_size > OS_MAX_SIZE) {
        printf("\nERROR: The Os update size is larger than 0x%x\n", OS_MAX_SIZE);
        return 1;
    }
/* #ifdef CONFIG_IPMUX_E2_C */  /*zw changed it in 1214*/
#if 0
    size = tftp_file_size;
    src = 0x80600000;
    dest = 0xbe000000;
    sprintf(cmd, "era 0x%x 0x%x", dest, dest + (size | 0xFFFF));
    printf("\n------ Erase flash [%s]\t------\n", cmd);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
    sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", src, dest, size);
    printf("\n------ Program flash [%s]\t------\n", cmd);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
#else
 //   #if  FLASHSIZE>flash_4M
        printf("This condition hasn't been tested.\n");
        run_command("protect off all",0);
        size = tftp_file_size<=0x3D0000?tftp_file_size:0x3D0000;
        src = 0x80600000;
        dest = PHYS_FLASH_1+knl_2offset; /*zw change*/
        sprintf(cmd, "era 0x%x 0x%x", dest, dest + (size | 0xFFFF));
        printf("\n------ Erase flash111 [%s]\t------\n", cmd);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
        sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", src, dest, size);
        printf("\n------ Program flash [%s]\t------\n", cmd);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
#if 0
        if (tftp_file_size>0x3D0000) {
            size = tftp_file_size - 0x3D0000;
            ipMuxRegWrite(REG_CS0_BASE_ADDR, 0x1f800);
            ipMuxRegWrite(REG_CS0_SIZE, 0xf800);
            src = 0x80600000 + 0x3D0000;
            dest = 0xbfc00000;
            sprintf(cmd, "era 0x%x 0x%x", dest, dest + (size | 0xFFFF));
            printf("\n------ Erase flash222 [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
            if (1!=rcode) return -1;
            sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", src, dest, size);
            printf("\n------ Program flash [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
            if (1!=rcode) return -1;
            ipMuxRegWrite(REG_CS0_BASE_ADDR, 0x1fc00);
            ipMuxRegWrite(REG_CS0_SIZE, 0xfc00);
        }
//#endif		
  //  #else 
        sprintf(cmd, "era 0x%x 0x%x", KERNEL_ADDR, KERNEL_ADDR + (tftp_file_size | FLASH_SECTOR_MASK));
        printf("\n------ Erase flash [%s]\t------\n", cmd);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
        printf("\n------ Program flash \t------\n");
        sprintf(cmd, "cp.b 0x80600000 0x%x 0x%x", KERNEL_ADDR, tftp_file_size);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
    #endif
#endif
    printf("********************************************************\n");
    printf("*           Os image is updated successfully!          *\n");
    printf("********************************************************\n");
    return 1;
}
int do_uucfg (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int size = 0;
    int src = 0;
    int dest = 0;
    int rcode = 0;
    char filename[20]="uImage";
    char cmd[50]="";
    char pstr[50] = "tftpb 0x80600000 ";
    if( 0 != strcmp("current",getenv("flash_file_name"))) 
    {
        strcpy(filename,getenv("flash_file_name")); 
    }
    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        rcode = 1;
    }
/* #ifdef CONFIG_IPMUX_E2_C */  /*zw changed it in 1214*/
#if 0
    printf("Are you sure to update OS image[0x%x~0x%x]? (Hit any key to abort!) ", PHYS_FLASH_2, PHYS_FLASH_2 + OS_MAX_SIZE);
#else
    printf("Are you sure to update CFG image[0x%x~0x%x]? (Hit any key to abort!) ", PHYS_FLASH_1+cfg_offset, PHYS_FLASH_1 + knl_1offset-1);
#endif
    if (promt_cancel(5)) return -1;
    strcat(pstr,filename);
    strcpy(cmd,pstr);
    printf("\n------ Download file \t------\n");
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
#ifdef  OPULAN_IMAGE_HEADER  
    if(getOplHeadSwitchStatus())
    {
       if(0!=imagePreCheck(0x80600000))
	 	return -1; 
    }
 #endif  	
 #ifdef  OPULAN_IMAGE_HEADER
    if(getOplHeadSwitchStatus())
    {
             if (image_type!=TYPE_FILESYSTEM){
		 printf("ERROR:NOT FOLESYSTEM IMAGE\n");
		 return -1;
             	}
    }
#endif
    if (tftp_file_size > OS_MAX_SIZE) {
        printf("\nERROR: The Os update size is larger than 0x%x\n", OS_MAX_SIZE);
        return 1;
    }
/* #ifdef CONFIG_IPMUX_E2_C */  /*zw changed it in 1214*/
#if 0
    size = tftp_file_size;
    src = 0x80600000;
    dest = PHYS_FLASH_2+0x440000; /*zw change*/
    sprintf(cmd, "era 0x%x 0x%x", dest, dest + (size | 0xFFFF));
    printf("\n------ Erase flash [%s]\t------\n", cmd);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
    sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", src, dest, size);
    printf("\n------ Program flash [%s]\t------\n", cmd);
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;
#else
  //  #if  FLASHSIZE>flash_4M
        printf("This condition hasn't been tested.\n");
        run_command("protect off all",0);
        size = tftp_file_size<=0x3D0000?tftp_file_size:0x3D0000;
        src = 0x80600000;
        dest = PHYS_FLASH_1+cfg_offset;
        sprintf(cmd, "era 0x%x 0x%x", dest, dest + (size | 0xFFFF));
        printf("\n------ Erase flash111 [%s]\t------\n", cmd);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
        sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", src, dest, size);
        printf("\n------ Program flash [%s]\t------\n", cmd);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
#if 0
        if (tftp_file_size>0x3D0000) {
            size = tftp_file_size - 0x3D0000;
            ipMuxRegWrite(REG_CS0_BASE_ADDR, 0x1f800);
            ipMuxRegWrite(REG_CS0_SIZE, 0xf800);
            src = 0x80600000 + 0x3D0000;
            dest = 0xbfc00000;
            sprintf(cmd, "era 0x%x 0x%x", dest, dest + (size | 0xFFFF));
            printf("\n------ Erase flash222 [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
            if (1!=rcode) return -1;
            sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", src, dest, size);
            printf("\n------ Program flash [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
            if (1!=rcode) return -1;
            ipMuxRegWrite(REG_CS0_BASE_ADDR, 0x1fc00);
            ipMuxRegWrite(REG_CS0_SIZE, 0xfc00);
        }
//#endif
//    #else 
        /*assume all flash sector is 0x10000*/
        sprintf(cmd, "era 0x%x 0x%x", KERNEL_ADDR, KERNEL_ADDR + (tftp_file_size | FLASH_SECTOR_MASK));
        printf("\n------ Erase flash [%s]\t------\n", cmd);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
    
        printf("\n------ Program flash \t------\n");
        sprintf(cmd, "cp.b 0x80600000 0x%x 0x%x", KERNEL_ADDR, tftp_file_size);
        rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
    #endif
#endif
    printf("********************************************************\n");
    printf("*           CFG is updated successfully!          *\n");
    printf("********************************************************\n");

    return 1;
}
#endif

int promt_cancel(int second)
{
    u32 i;

    /*delay for 3s to abort*/
    for (i=0;i<second;i++)
    {
        if (tstc()) {   /* we got a key press   */
            (void) getc();  /* consume input    */
            puts ("aborted\n");
            return 1;
        }

        printf("%2x",(second-i));
        udelay(1000000);
        puts("\b\b");
    }

    printf("%2x",0);
    return 0;
    udelay(300000);
    
    puts("\n");
}
#if 0
int do_OplHeadSwitch(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    switch (argc) {
    case 2:         
        break;
    default:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    
    if (strcmp(argv[1], "off") == 0) {
        setOplHeadSwitchStatus(0);
    } else if (strcmp(argv[1], "on") == 0) {
        setOplHeadSwitchStatus(1);
    } else {
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    return 1;
}
#endif

int do_uuram (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    
    int rcode = 0;
    char addr[15]="0x80600000";
    char cmd[50]="u-boot.bin";

    switch (argc) {
    case 2:         
        strcpy(addr,argv[1]);
        break;
    default:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return -1;
    }
    
    printf("Are you sure to update U-boot? (Hit any key to abort!) ");
    if (promt_cancel(5)) return -1;

    printf("\n------ Unprotect flash \t------\n");
    rcode=run_command("protect off 1:0-5",0);
    if (1!=rcode) return -1;

    printf("\n------ Erase flash \t------\n");
    rcode=run_command("era 1:0-5",0);
    if (1!=rcode) return -1;

    printf("\n------ Program flash \t------\n");
    sprintf(cmd,"cp.b %s 0xbfc00000 0x2fff0",addr );
    rcode=run_command(cmd,0);
    if (1!=rcode) return -1;

    printf("\n------ Protect flash \t------\n");
    rcode=run_command("protect on 1:0-5",0);
    if (1!=rcode) return -1;

    printf("\n");
    printf("********************************************************\n");
    printf("*   U-boot is updated, please power down the board!    *\n");
    printf("********************************************************\n");

    return 1;
}

int do_unote (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    
    int rcode = 0;
    char filename[20]="u-boot.bin";
    

    switch (argc) {
    case 2:         
        strcpy(filename,argv[1]);
        break;
    default:
        //printf ("Usage:\n%s\n", cmdtp->usage);
        rcode = 1;
    }
    printf("Configuration list:\n");
#ifdef CONFIG_AUTOSCRIPT 
    printf("    autoscript\n");
#endif
    printf("Update list:\n");
    printf("    Fri Mar  3 10:08:20 CST 2006 : Increase linux gzip size limit 4M to 6M.\n");
    printf("    Fri Mar  3 10:08:20 CST 2006 : Increase memory size to 32M.\n");
    return 1;
}

int do_uloop (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    
    int loopcount=0;
    int i=0;
    int iSuc=0;
    unsigned long idelay=0;
    int iFail=0;

    switch (argc) {
    case 3:         
        idelay=(unsigned long)simple_strtoul(argv[2],NULL,10);  
    case 2:         
        loopcount=(unsigned long)simple_strtoul(argv[1],NULL,10);
        break;
    default:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    printf("start loopcmd for %d times and interval %d usec ...\n",loopcount,idelay);

    for(i=0;i<loopcount;i++)
    {
        printf("------ loop %d ------\n",i);
        
        run_command("run loopcmd", 0);

            
        if (1)
        {
            iSuc++;
            printf("success ... \n");
        }
        else
        {
            iFail++;
            printf("fail ... \n");
        }

        if (had_ctrlc ())
        {
            printf("loop is interrupted!\n");
            break;
        }
        
        if (0!=idelay) udelay(idelay);
    }
    printf("-------------------- SUMMURY --------------------\n");
    printf("    total %d, success %d, fail %d \n",iSuc+iFail,iSuc,iFail);
    printf("-------------------------------------------------\n");
    
    return 1;
}



int do_uloopfe (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    extern int debug_fe_loopback(void); 
    int num_pck=0;
    
    printf("start fe loop test ... press ctrl+c to finish.\n");

    num_pck=debug_fe_loopback();
        
    printf("-------------------- SUMMURY --------------------\n");
    printf("        RX & TX packets : %d\n",num_pck);
    printf("-------------------------------------------------\n");
    
    
    return 1;
}


int do_umtestm (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
extern void memcheck32(void);
extern void testcopy32(void);
extern void testcopy(void);
    u32 mem_start=0;
    u32 mem_end=0;
    u32 mem_copyto=0;

    if (1==argc) 
    {
#ifdef INCLUDE_MEM_TEST
        memcheck32();
#endif

        return -1;
    }
        
    switch (argc) {
    case 4:         
        mem_copyto=(unsigned long)simple_strtoul(argv[3],NULL,16);  
    case 3:         
        mem_end=(unsigned long)simple_strtoul(argv[2],NULL,16); 
    case 2:         
        mem_start=(unsigned long)simple_strtoul(argv[1],NULL,16);
        break;
    default:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    printf("start memory test mem_start=0x%x,mem_end=0x%x,mem_copyto=0x%x...\n",mem_start,mem_end,mem_copyto);

#ifdef INCLUDE_MEM_TEST
    extern void set_memtestrange(u32 start,u32 end,u32 copyto);
    if (0!=mem_end) {
        set_memtestrange(mem_start,mem_end,mem_copyto);
    }

    testcopy32();
    testcopy();
#endif
    

    return 1;
}


int do_umtest (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
extern void memcheck32(void);
extern void testcopy32(void);
extern void testcopy(void);
    u32 mem_start=0;
    u32 mem_end=0;
    u32 mem_copyto=0;

    if (1==argc) 
    {
#ifdef INCLUDE_MEM_TEST
        memcheck32();
#endif

        return -1;
    }
        
    switch (argc) {
    case 4:         
        mem_copyto=(unsigned long)simple_strtoul(argv[3],NULL,16);  
    case 3:         
        mem_end=(unsigned long)simple_strtoul(argv[2],NULL,16); 
    case 2:         
        mem_start=(unsigned long)simple_strtoul(argv[1],NULL,16);
        break;
    default:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    printf("start memory test mem_start=0x%x,mem_end=0x%x,mem_copyto=0x%x...\n",mem_start,mem_end,mem_copyto);

#ifdef INCLUDE_MEM_TEST
    extern void set_memtestrange(u32 start,u32 end,u32 copyto);
    if (0!=mem_end) {
        set_memtestrange(mem_start,mem_end,mem_copyto);
    }

    testcopy32();
    testcopy();
#endif
    

    return 1;
}



                                                                                                                             
int do_uubd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    extern void print_bd_tx(int printAll);
    extern void print_bd_rx(int print_all);
    int rxOrTx=0;                                                                                                      
    int printAll=1;                                                                                                                     
    switch (argc) {
    case 3:
            rxOrTx = (unsigned long)simple_strtoul(argv[1],NULL,16);
            printAll = (unsigned long)simple_strtoul(argv[2],NULL,16);
            break;
    default:
            printf ("Usage:\n%s\n", cmdtp->usage);
            return 0;
    }
    printf("rxorTx = %d, printAll = %d\n",rxOrTx,printAll);

    if ( 1 == rxOrTx )
    {
        print_bd_tx(printAll);

    }
    else
    {
        print_bd_rx(printAll);

    }                                                                                                                    
                                                                                                                             
    return 1;
}

int do_umf (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{

    u32 mem_start=0;
    u32 mem_end=0;
    u32 val=0;

    switch (argc) {
    case 4:         
        val=(unsigned long)simple_strtoul(argv[3],NULL,16); 
    case 3:         
        mem_end=(unsigned long)simple_strtoul(argv[2],NULL,16); 
    case 2:         
        mem_start=(unsigned long)simple_strtoul(argv[1],NULL,16);
        break;
    default:
        printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }
    printf("start memory search start=0x%x,end=0x%x,val=0x%x...\n",mem_start,mem_end,val);
#ifdef INCLUDE_MEM_TEST
extern void mem_search_word(u32 mem_start,u32 mem_end,u32 val);
    mem_search_word(mem_start,mem_end,val);
#endif  
    return 1;
}


void do_mdioRegisterWrite(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    u32 deviceAddr,regAddr,data0;
    
    deviceAddr=(unsigned long)simple_strtoul(argv[1],NULL,16); 
    regAddr=(unsigned long)simple_strtoul(argv[2],NULL,16); 
    data0=(unsigned long)simple_strtoul(argv[3],NULL,16); 
    mdioRegisterWrite( deviceAddr, regAddr, data0);
}

void do_mdioRegisterRead(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    u16 deviceAddr,regAddr,data0;
    
    deviceAddr=(unsigned long)simple_strtoul(argv[1],NULL,16); 
    regAddr=(unsigned long)simple_strtoul(argv[2],NULL,16); 
    mdioRegisterRead( deviceAddr, regAddr, &data0);
    printf("mdio read data:0x%04x\n",data0);
}

void do_atherosRegisterWrite(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    u32 regAddr,data0;
    
    regAddr=(unsigned long)simple_strtoul(argv[1],NULL,16); 
    data0=(unsigned long)simple_strtoul(argv[2],NULL,16); 
    atheros_wr_reg(regAddr, data0);
}

void do_atherosRegisterRead(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    u16 regAddr,data0;
    
    regAddr=(unsigned long)simple_strtoul(argv[1],NULL,16); 
    atheros_show_reg(regAddr, &data0); 
    printf("mdio read data:0x%04x\n",data0);
}


/***************************************************/
//#define U_BOOT_CMD(name,maxargs,rep,cmd,usage,help) 

U_BOOT_CMD(
    umf,    4,  1,  do_umf,
    "umf    - do memory test. mem search mem_start mem_end val.\n",
    "    - do memory test. mem search mem_start mem_end val.\n"
);


U_BOOT_CMD(
    umtest, 4,  1,  do_umtest,
    "umtest - do memory test. umtest mem_start mem_end mem_copyto.\n",
    "    - do memory test. umtest mem_start mem_end mem_copyto.\n"
);

U_BOOT_CMD(
    uloop,  3,  1,  do_uloop,
    "uloop  - loop the cmd loopcmd\n",
    "times delay- loop the cmd loopcmd for times and delay for delay usec.\n"
);

U_BOOT_CMD(
    uloopfe,    1,  1,  do_uloopfe,
    "uloopfe    - do fe loop test: mips send all rx packet back.\n",
    "    - do fe loop test:mips send all rx packet back.\n"
);

U_BOOT_CMD(
    unote,  2,  1,  do_unote,
    "unote  - print release note of u-boot\n",
    "    - print release note of u-boot.\n"
);


U_BOOT_CMD(
    uutftp, 2,  1,  do_uutftp,
    "uutftp - update U-boot from tftp server\n",
    "    - update file u-boot.bin from tftp server to flash.\n"
    "      the file size must be below 190k !\n"        
);

U_BOOT_CMD(
    uuos1,   2,  1,  do_uuos1,
    "uuos1   - update kernel1 Image from tftp server\n",
    "    - update file kernel1 Image from tftp server to flash.\n"
    "      the file size must be below 3.68MB !\n"      
);
U_BOOT_CMD(
    uuos2,   2,  1,  do_uuos2,
    "uuos2   - update kernel2 Image from tftp server\n",
    "    - update file kernel2 Image from tftp server to flash.\n"
    "      the file size must be below 3.68MB !\n"      
);
U_BOOT_CMD(
    uucfg,   2,  1,  do_uucfg,
    "uucfg   - update cfg Image from tftp server\n",
    "    - update file cfg Image from tftp server to flash.\n"
    "      the file size must be below 1MB !\n"      
);


U_BOOT_CMD(
    uuram,  2,  1,  do_uuram,
    "uuram   - update U-boot from ram\n",
    "address \n    - update file u-boot.bin from ram address to flash.\n"
    "      the file size must be below 180k !\n"        
);

                                                                                
U_BOOT_CMD(
        uubd,  3,      1,      do_uubd,
        "uubd   - show the content of uubd\n",
        "  rx/tx       - 0=rx 1=tx.\n"
        "  all/valid   - 0=valid 1 all!\n"
);

U_BOOT_CMD(
    orr,    2,  1,  do_orr,
    "orr     - read ipmux register\n",
    "regid   - ipmux register id\n"
);

U_BOOT_CMD(
    owr,    3,  1,  do_owr,
    "owr     - write ipmux register\n",
    "regid val - write val to ipmux register regid\n"
);
                                                                                
U_BOOT_CMD(
    mdior,    3,  1,  do_mdioRegisterRead,
    "mdior     - read mdio dev \n",
    "mdior  - deviceAddr, regAddr\n"
);
U_BOOT_CMD(
    mdiow,    4,  1,  do_mdioRegisterWrite,
    "mdiow     - read mdio dev \n",
    "mdiow  - deviceAddr, regAddr,data\n"
);

#if defined(CONFIG_IPMUX_ATHEROS)
U_BOOT_CMD(
    arr,    2,  1,  do_atherosRegisterRead,
    "arr     - read atheros switch register \n",
    "regid  - atheros register id\n"
);
U_BOOT_CMD(
    awr,    3,  1,  do_atherosRegisterWrite,
    "awr     - write atheros switch register \n",
    "regid val  - write val to atheros switch register regid\n"
);
#endif

#endif  /* CFG_CMD_UU */

