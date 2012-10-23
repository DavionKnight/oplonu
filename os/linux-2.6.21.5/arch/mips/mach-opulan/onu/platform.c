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


static struct platform_device *devices[] __initdata = {
	&onu_fe_device,
};

int onu_platform_init(void)
{
	return platform_add_devices(devices, ARRAY_SIZE(devices));
}
arch_initcall(onu_platform_init);
