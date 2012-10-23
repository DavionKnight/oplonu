#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/serial_core.h>
#include <linux/signal.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/serial.h>

#include <linux/serial_8250.h>

#include "8250.h"

#include <asm/mach-optrann/reg.h>
#include <asm/mach-optrann/int.h>
#include <asm/mach-optrann/optrann.h>



#define PORT(_base, _irq)				\
	{						\
		.membase	= (void __iomem	*)(_base),\
		.irq		= _irq,			\
		.regshift	= 0,			\
		.uartclk = (SYS_CLK/16), \
		.iotype		= UPIO_MEM32,		\
		.flags		= ASYNC_BOOT_AUTOCONF | ASYNC_SKIP_TEST \
	}

static struct plat_serial8250_port optrann_data[] = {
	PORT((0xbf000000 + UART0_SA_BASE), OPL_UART0_IRQ),
	{ },
};

static struct platform_device optrann_device = {
	.name			= "serial8250",
	.id			= PLAT8250_DEV_OPTRANN,
	.dev			= {
		.platform_data	= optrann_data,
	},
};

static int __init optrann_init(void)
{
	int i;
	unsigned int uartclk;

	prom_printf("%s,%s,%d\n", __FILE__,__FUNCTION__,__LINE__);

	return platform_device_register(&optrann_device);
}


static void __exit optrann_exit(void)
{
	platform_device_unregister(&optrann_device);
}

module_init(optrann_init);
module_exit(optrann_exit);

MODULE_AUTHOR("Figo.zhang");
MODULE_DESCRIPTION("8250 serial probe module for Optrann board");
MODULE_LICENSE("GPL");

