/********************************************************************************
 *
 * Filename: 
 *
 * Summary:
 *  timer setup
 * 
 * Modification History:
 *  Date	  By	   Summary
 *  -------- -------- ----------------------------------------------------------
 *  2009-11-2       Figo.zhang	 Original Creation
 *
 ********************************************************************************/


#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/mc146818rtc.h>

#include <asm/mipsregs.h>
#include <asm/ptrace.h>
#include <asm/div64.h>
#include <asm/cpu.h>
#include <asm/time.h>

#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/prom.h>

#include <asm/mach-onu/onu.h>
#include <asm/mach-onu/onu_reg.h>


void __init onu_time_init(void)
{
	mips_hpt_frequency = CPU_CLK / 2;
}

void __init plat_timer_setup(struct irqaction *irq)
{
	/* we are using the cpu counter for timer interrupts */	
	unsigned int count;

	//printk("%s,%s\n", __FILE__,__FUNCTION__);

	/* Usually irq is timer_irqaction (timer_interrupt) */
	setup_irq(ONU_IRQ_CPU_CLOCK, irq);

	/* to generate the first CPU timer interrupt */
	count = read_c0_count();
	write_c0_compare(count + 1000);
}
