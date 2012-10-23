/********************************************************************************
 *
 * Filename: 
 *
 * Summary:
 *  Platfrom description for Optrann board
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
#include <asm/flash.h>

#include <asm/mach-optrann/optrann.h>
#include <asm/mach-optrann/optrann_reg.h>
#include <asm/mach-optrann/optrann_irq.h>


static struct resource optrann_fe_resources[] = {
	[0] = {  /*DMAC base addr*/
		.start	= 0xbf000000 + IPMUX_DMAC_REG,
		.end = 0xbf000000 + IPMUX_DMAC_REG,
		.flags	= IORESOURCE_MEM,
	},
	[1] = { /*MSG base addr*/
		.start	= 0xbf000000 + MSG_SA_BASE,
		.end	= 0xbf000000 + MSG_SA_BASE,
		.flags	= IORESOURCE_MEM,
	},
	[2] = { /*FE irq*/
		.name	= "FE IRQ",
		.start	= OPL_FEDMA1_IRQ,
		.end	= OPL_FEDMA1_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device optrann_fe_device = {
	.name		= "optrann_fe_ether",
	.id = 0,
	.resource	= optrann_fe_resources,
	.num_resources	= ARRAY_SIZE(optrann_fe_resources),
};

/*-----flash------------------------------------------*/

#define FLASH_SIZE 0x2000000 //32MB

#define U_BOOT_SIZE  0x40000  //256kb
#define U_BOOT_ENV   0x20000 //128kb
#define OS_CFG  0x20000  //128kb
#define OS_SIZE   (FLASH_SIZE-U_BOOT_SIZE-U_BOOT_ENV-OS_CFG)/3


static struct resource flash_resources[] = {
	[0] = {
		.start	= 0xb0000000,
		.end	= 0xb0000000 + FLASH_SIZE -1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct mtd_partition fpga_flash_partitions[] = {
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
	{
		.name = "Jffs2",
		.size = OS_SIZE,
		.offset = U_BOOT_SIZE+U_BOOT_ENV + OS_CFG+ OS_SIZE+OS_SIZE,
	},
};

static struct flash_platform_data fpga_flash_data = {

	.map_name	= "cfi_probe",
	.name = "Fpga image flash",
	.parts		= fpga_flash_partitions,
	.nr_parts	= ARRAY_SIZE(fpga_flash_partitions),
	.width = 2,

};

static struct platform_device fpga_flash_device = {

	.name		= "Opulan-flash",
	.id		= 0,
	.dev = {
		.platform_data = &fpga_flash_data,
	},
	.resource = flash_resources,
	.num_resources = ARRAY_SIZE(flash_resources),
};


/*-----------------------------------------------------*/

static struct platform_device *devices[] __initdata = {
	&optrann_fe_device,
	&fpga_flash_device,
};

int optrann_platform_init(void)
{
	return platform_add_devices(devices, ARRAY_SIZE(devices));
}
arch_initcall(optrann_platform_init);
