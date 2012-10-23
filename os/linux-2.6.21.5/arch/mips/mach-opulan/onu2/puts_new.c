/********************************************************************************
 *
 * Filename: 
 *
 * Summary:
 *  Early serial from debug
 *  If want to debug early when the console is not done, you can open define "EARLY_DEBUG" and 
 *  use prom_printf().
 * 
 * Modification History:
 *  Date	  By	   Summary
 *  -------- -------- ----------------------------------------------------------
 *  2009-11-2       Figo.zhang	 Original Creation
 *
 ********************************************************************************/


#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/serial_reg.h>
#include <linux/module.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/serial.h>
#include <linux/serial.h>

#include <linux/serial.h>
#include <linux/serial_core.h>

#include <asm/mach-onu2/onu2.h>
#include <asm/mach-onu2/onu2_reg.h>
#include <asm/mach-onu2/onu2_irq.h>

#define EARLY_DEBUG 0

#define UART0_BASE_ADDR (0xbf000000 + UART0_SA_BASE)

static unsigned int serial_in(int offset)
{
	return  *(volatile u32 *)(UART0_BASE_ADDR + (offset * 4)) ;   
}

static void serial_out(int offset, int value)
{
	*(volatile u32 *)(UART0_BASE_ADDR + (offset * 4)) =value;
}


int putPromChar(char c)
{

	while ((serial_in(UART_LSR) & UART_LSR_THRE) == 0)
		;
	serial_out(UART_TX, c);

	return 1;
}

u8 getPromChar(void)
{

	while (!(serial_in( UART_LSR) & 1))
		;
	return(serial_in(UART_RX));    
}

void prom_putchar(char c)
{
	putPromChar(c);
}

u8  prom_getchar(void)
{
	return getPromChar();
}

EXPORT_SYMBOL(prom_putchar);
EXPORT_SYMBOL(getPromChar);

static char buf[1024];

#if EARLY_DEBUG
void prom_printf(char *fmt, ...)
{
	va_list args;
	int l;
	char *p, *buf_end;
	long flags;

	int putPromChar(char);

	/* Low level, brute force, not SMP safe... */
	save_and_cli(flags);
	va_start(args, fmt);
	l = vsprintf(buf, fmt, args); /* hopefully i < sizeof(buf) */
	va_end(args);

	buf_end = buf + l;

	for (p = buf; p < buf_end; p++) {
		/* Crude cr/nl handling is better than none */
		if(*p == '\n')putPromChar('\r');
		putPromChar(*p);
	}
	restore_flags(flags);
}
#else
void prom_printf(char *fmt, ...)
{
}
#endif

EXPORT_SYMBOL(prom_printf);

