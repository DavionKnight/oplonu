

#include <asm/cpu-info.h>

#define DOPRINTK  printk("%s,%s\n",__FILE__,__FUNCTION__,__LINE__);


#define SYS_CLK 156*1000000
#define CPU_CLK 300*1000000

#define BASE_BAUD ( SYS_CLK / 16)


#define OPTRANN_MISC_IRQ_BASE        0x20
#define OPTRANN_GPIO_IRQ_BASE        0x30

/*reset reg*/
#define SOFTRES_REG       0x1f000000
#define GORESET           0x80000000
#define DISINT_REG   0x1f002e18


/* Software's idea of interrupts handled by "CPU Interrupt Controller" */
#define OPTRANN_IRQ_NONE     MIPS_CPU_IRQ_BASE+0
#define OPTRANN_IRQ_CPU_CLOCK    MIPS_CPU_IRQ_BASE+7 /* C0_CAUSE: 0x8000 */

/* Miscellaneous interrupts, which share IP6 */
#define OPTRANN_MISC_IRQ_NONE        OPTRANN_MISC_IRQ_BASE+0
#define OPTRANN_MISC_IRQ_TIMER       OPTRANN_MISC_IRQ_BASE+1
#define OPTRANN_MISC_IRQ_AHB_PROC    OPTRANN_MISC_IRQ_BASE+2
#define OPTRANN_MISC_IRQ_AHB_DMA     OPTRANN_MISC_IRQ_BASE+3
#define OPTRANN_MISC_IRQ_GPIO        OPTRANN_MISC_IRQ_BASE+4
#define OPTRANN_MISC_IRQ_UART0       OPTRANN_MISC_IRQ_BASE+5
#define OPTRANN_MISC_IRQ_UART0_DMA   OPTRANN_MISC_IRQ_BASE+6
#define OPTRANN_MISC_IRQ_WATCHDOG    OPTRANN_MISC_IRQ_BASE+7
#define OPTRANN_MISC_IRQ_LOCAL       OPTRANN_MISC_IRQ_BASE+8
#define OPTRANN_MISC_IRQ_SPI         OPTRANN_MISC_IRQ_BASE+9
#define OPTRANN_MISC_IRQ_COUNT       10

/* GPIO Interrupts [0..7], share OPTRANN_MISC_IRQ_GPIO */
#define OPTRANN_GPIO_IRQ_NONE            OPTRANN_MISC_IRQ_BASE+0
#define OPTRANN_GPIO_IRQ(n)              OPTRANN_MISC_IRQ_BASE+(n)+1
#define OPTRANN_GPIO_IRQ_COUNT           22

#define sysRegRead(phys)    \
	(*(volatile u32 *)KSEG1ADDR(phys))

#define sysRegWrite(phys, val)  \
	((*(volatile u32 *)KSEG1ADDR(phys)) = (val))


