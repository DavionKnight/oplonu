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

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/flash.h>

#include <asm/mach-onu/onu.h>
#include <asm/mach-onu/onu_reg.h>
#include <asm/mach-onu/onu_irq.h>


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

/*----------------spi flash information---------------------*/
#define FLASH_SIZE  0x400000 //4MB
#define U_BOOT_SIZE  0x40000  //256kb
#define U_BOOT_ENV   0x10000 //64kb
#define OS_CFG  0x10000  //64kb
#define OS_SIZE   (FLASH_SIZE-U_BOOT_SIZE-U_BOOT_ENV-OS_CFG)/2  
//#define NUM_OS   2       /*we alway use 2 OS, other is backup OS*/

static struct mtd_partition f13_flash_partitions[] = {
	{
		.name = "Config",
		.size = OS_CFG,
		.offset = U_BOOT_SIZE+U_BOOT_ENV,
			
	},
	{
		.name =		"OS 1",
		.size =		OS_SIZE,
		.offset =	U_BOOT_SIZE+U_BOOT_ENV + OS_CFG,
	},
	{
		.name = "OS 2",
		.size = OS_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV + OS_CFG+ OS_SIZE,
	},
};

static struct flash_platform_data f13_flash_data = {
	.name = "f13_flash",
	.parts		= f13_flash_partitions,
	.nr_parts	= ARRAY_SIZE(f13_flash_partitions),
	.type = "at26df321",
};

static struct platform_device f13_flash_device = {
	.name		= "Opulan-flash",
	.id		= 0,
	.dev = {
		.platform_data = &f13_flash_data,
	},
};
/*----------------------------------------------------*/


static struct platform_device *devices[] __initdata = {
	&onu_fe_device,
	&f13_flash_device,
};

int onu_platform_init(void)
{
	return platform_add_devices(devices, ARRAY_SIZE(devices));
}
arch_initcall(onu_platform_init);
