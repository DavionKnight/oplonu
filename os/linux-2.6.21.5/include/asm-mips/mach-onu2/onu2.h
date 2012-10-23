

#include <asm/cpu-info.h>

#define DOPRINTK  printk("%s,%s\n",__FILE__,__FUNCTION__,__LINE__);

#define ONU_REG_BASE   (0xbf000000)

/*read/write 32 bit register*/
#define onu_readl(reg)    (*(volatile u32 *)(ONU_REG_BASE + reg))
#define onu_writel(reg, val)  ((*(volatile u32 *)(ONU_REG_BASE + reg)) = (u32)(val))

/*read/write 16 bit register*/
#define onu_readw(reg)    (*(volatile u16 *)(ONU_REG_BASE + reg))
#define onu_writew(reg, val)  ((*(volatile u16  *)(ONU_REG_BASE + reg)) = (u16)(val))

/*read/write 8 bit register*/
#define onu_readb(reg)    (*(volatile u8 *)(ONU_REG_BASE +reg))
#define onu_writeb(reg, val)  ((*(volatile u8  *)(ONU_REG_BASE +reg)) = (u8)(val))


#define SYS_CLK 100*1000000
#define CPU_CLK 50*1000000

#define BASE_BAUD ( SYS_CLK / 16)


#define ONU_MISC_IRQ_BASE        0x20
#define ONU_GPIO_IRQ_BASE        0x30

/*reset reg*/
#define SOFTRES_REG       0x1f000000
#define GORESET           0x80000000
#define DISINT_REG   0x1f002e18


/* Software's idea of interrupts handled by "CPU Interrupt Controller" */
#define ONU_IRQ_NONE     MIPS_CPU_IRQ_BASE+0
#define ONU_IRQ_CPU_CLOCK    MIPS_CPU_IRQ_BASE+7 /* C0_CAUSE: 0x8000 */

/* Miscellaneous interrupts, which share IP6 */
#define ONU_MISC_IRQ_NONE        ONU_MISC_IRQ_BASE+0
#define ONU_MISC_IRQ_TIMER       ONU_MISC_IRQ_BASE+1
#define ONU_MISC_IRQ_AHB_PROC    ONU_MISC_IRQ_BASE+2
#define ONU_MISC_IRQ_AHB_DMA     ONU_MISC_IRQ_BASE+3
#define ONU_MISC_IRQ_GPIO        ONU_MISC_IRQ_BASE+4
#define ONU_MISC_IRQ_UART0       ONU_MISC_IRQ_BASE+5
#define ONU_MISC_IRQ_UART0_DMA   ONU_MISC_IRQ_BASE+6
#define ONU_MISC_IRQ_WATCHDOG    ONU_MISC_IRQ_BASE+7
#define ONU_MISC_IRQ_LOCAL       ONU_MISC_IRQ_BASE+8
#define ONU_MISC_IRQ_SPI         ONU_MISC_IRQ_BASE+9
#define ONU_MISC_IRQ_COUNT       10

/* GPIO Interrupts [0..7], share ONU_MISC_IRQ_GPIO */
#define ONU_GPIO_IRQ_NONE            ONU_MISC_IRQ_BASE+0
#define ONU_GPIO_IRQ(n)              ONU_MISC_IRQ_BASE+(n)+1
#define ONU_GPIO_IRQ_COUNT           22

#define sysRegRead(phys)    \
	(*(volatile u32 *)KSEG1ADDR(phys))

#define sysRegWrite(phys, val)  \
	((*(volatile u32 *)KSEG1ADDR(phys)) = (val))


