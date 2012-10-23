/********************************************************************************
 *
 * Filename: 
 *
 * Summary:
 *  Platfrom description for Onu board
 * 
 * Modification History:
 *  Date	  By	   Summary
 *  -------- -------- ----------------------------------------------------------
 *  2009-11-2       Figo.zhang	 Original Creation
 *
 ********************************************************************************/


#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/resource.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>

#include <linux/ioport.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <asm/flash.h>

#include <asm/mach-onu2/onu2.h>
#include <asm/mach-onu2/onu2_reg.h>
#include <asm/mach-onu2/onu2_irq.h>


static struct resource onu_fe_resources[] = {
	[0] = {  /*DMAC base addr*/
		.start	= 0xbf000000 + FEDMA_BASE_ADDR,
		.end = 0xbf000000 + FEDMA_BASE_ADDR,
		.flags	= IORESOURCE_MEM,
	},
	[1] = { /*FE irq*/
		.name	= "FE IRQ",
		.start	= OPL_FEDMA1_IRQ,
		.end	= OPL_FEDMA1_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device onu_fe_device = {
	.name		= "onu_fe_ether",
	.id = 0,
	.resource	= onu_fe_resources,
	.num_resources	= ARRAY_SIZE(onu_fe_resources),
};

/*----------------flash information---------------------*/
#define FLASH_SIZE 0x1000000 //16MB

#define U_BOOT_SIZE  0x40000 //256KB
#define U_BOOT_ENV   0x20000  //128kb
#define CFG_SIZE 0x20000       //128kb
#define CFG_NUM  1  /*1 cfg partition for eva*/
#define OS_SIZE  (FLASH_SIZE -U_BOOT_SIZE - U_BOOT_ENV - CFG_NUM*CFG_SIZE)/2  //
#define NUM_OS   2       /*we alway use 2 OS, other is backup OS*/


static struct resource flash_resources[] = {
	[0] = {
		.start	= 0xb0000000,
		.end	= 0xb0000000 + FLASH_SIZE -1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct mtd_partition flash_partitions[] = {
	{
		.name =		"CFG 0",
		.size =		CFG_SIZE,
		.offset =	U_BOOT_SIZE +U_BOOT_ENV ,
	},{
		.name =		"OS 0",
		.size =		OS_SIZE,
		.offset =	U_BOOT_SIZE +U_BOOT_ENV + CFG_SIZE,
	},{
		.name =		"OS 1",
		.size =		OS_SIZE,
		.offset =	U_BOOT_SIZE +U_BOOT_ENV + CFG_SIZE+OS_SIZE,
	},

};

static struct flash_platform_data flash_data = {

	.map_name	= "cfi_probe",
	.name = "onu2 flash",
	.parts		= flash_partitions,
	.nr_parts	= ARRAY_SIZE(flash_partitions),
	.width = 1,

};

static struct platform_device flash_device = {

	.name		= "Opulan-flash",
	.id		= 0,
	.dev = {
		.platform_data = &flash_data,
	},
	.resource = flash_resources,
	.num_resources = ARRAY_SIZE(flash_resources),
};


static struct platform_device *devices[] __initdata = {
	&onu_fe_device,
	&flash_device,
};

int onu_platform_init(void)
{
	return platform_add_devices(devices, ARRAY_SIZE(devices));
}
arch_initcall(onu_platform_init);
