
/********************************************************************************
 *
 * Filename: 
 *
 * Summary:
 *  irq setup
 * 
 * Modification History:
 *  Date	  By	   Summary
 *  -------- -------- ----------------------------------------------------------
 *  2009-11-2       Figo.zhang	 Original Creation
 *
 ********************************************************************************/

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/random.h>
#include <asm/system.h>
#include <asm/irq.h>
#include <asm/irq_cpu.h>
#include <asm/io.h>
#include <asm/irq_regs.h>

#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include <linux/module.h>


#include <asm/mach-onu/onu_irq.h>
#include <asm/mach-onu/onu_reg.h>
#include <asm/mach-onu/onu.h>


/*--------------------------Macro definition------------------------- */
#ifdef INT_DEBUG 
#define dprintk printk
#else
#define dprintk(...)
#endif

#define ipmux_irq_shutdown ipmux_irq_disable



/*-----------------globaipMuxRegWritel varible/function declaration--------------- */
extern void mips_timer_interrupt(void);
u32 g_opl_chip_irq_pending = 0;

/* --------------local  varible declaration and definition------------- */

static spinlock_t ipmux_irq_lock = SPIN_LOCK_UNLOCKED;

static unsigned long valid_irq_mask = 0xE0ffeFF1;
static unsigned long ipmux_irq_mask = 0x20c7a001; 


static void __init init_ipmux_irqs(void); 
static void ipmuxIntEnable(void);
static void ipmuxIntDisable(void);
static void ipmux_irq_ack(unsigned int irq);
static void ipmux_irq_end(unsigned int irq);

static unsigned int ipmux_irq_startup(unsigned int irq);


static struct irq_chip ipmux_irq_type = {
	.typename = "onu-irq",
	.startup = ipmux_irq_startup,
	.shutdown = ipmux_irq_shutdown,
	.enable = ipmux_irq_enable,
	.disable = ipmux_irq_disable,
	.ack = ipmux_irq_ack,
	.end = ipmux_irq_end,
};

static struct irqaction ipmux_action = {
	.handler = no_action,
	.name = "IPMUX IRQ",
};

static struct irqaction corehi_action = {
	.handler = no_action,
	.name = "COREHi",
};


/** description: it handle the hw0 interrupt.
 *  @param struct pt_regs *regs
 *  @return NULL

 *  @see arch/mips/mips-board/generic/mipsIRQ.S
 *  @deprecated 
 */
void malta_hw0_irqdispatch(void)
{

	u32 irq_enable;
	u32 irq_enable1;
	u32 irq_status;
	u32 irq_status1;
	u8  irq_485;

	irq_enable = onu_readl(MOD_INT_ENABLE_0);
	irq_enable1 = onu_readl(MOD_INT_ENABLE_1);

	irq_status = onu_readl(MOD_INT_STATUS_0) & irq_enable;
	irq_status1= onu_readl(MOD_INT_STATUS_1) & irq_enable1;

	//prom_printf("%s, status=0x%x\n ",__FUNCTION__, irq_status);
	
	if(irq_status & IRQ_BIT_DMA0) do_IRQ(OPL_HOSTDMA0_IRQ);
	if(irq_status & IRQ_BIT_DMA1) do_IRQ(OPL_FEDMA1_IRQ);
	if(irq_status & IRQ_BIT_UART0) do_IRQ(OPL_UART0_IRQ);
	if(irq_status & IRQ_BIT_UART1) do_IRQ(OPL_UART1_IRQ);
	if(irq_status & IRQ_BIT_PCM)  do_IRQ(OPL_ONU_PCM_IRQ);
	if(irq_status & IRQ_BIT_GPIO4) do_IRQ(OPL_ONU_GPIO_4);
	if(irq_status & IRQ_BIT_GPIO5) do_IRQ(OPL_ONU_GPIO_5);
	if(irq_status & IRQ_BIT_GPIO10) do_IRQ(OPL_ONU_GPIO_10);
	if(irq_status & IRQ_BIT_UPTM) do_IRQ(OPL_UPTM_IRQ); 
	if(irq_status & IRQ_BIT_TIMER0) do_IRQ(OPL_TIMER0_IRQ);
	if(irq_status & IRQ_BIT_TIMER1) do_IRQ(OPL_TIMER1_IRQ);
	//add for 485-232 card 
    if(irq_status & IRQ_BIT_GPIO11) do_IRQ(OPL_EXAR_PORT_IRQ);

	if(irq_status & ipmux_irq_mask){
		do_IRQ(OPL_IPMUX_IRQ);      /* for OTHERS,mainly for IPMUX-e switch chip */
		//printk("4)en:0x%x pd:0x%x.\n", irq_mask, irq_status);
		g_opl_chip_irq_pending = irq_status &ipmux_irq_mask;
	}

}
void corehi_irqdispatch(void)
{
	struct pt_regs *regs =  get_irq_regs();
	die("CoreHi interrupt",regs);
}

/** description: it is used to init the global irq_desc[].
 *  @param void
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void  init_ipmux_irqs(void)
{
	int i;
	for(i = 8;i<32;i++){
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = 0;
		irq_desc[i].depth = 1;
		irq_desc[i].chip = &ipmux_irq_type;
	}
	/* enable the ipmux interrupt line */
	ipmuxIntEnable();
}

/** description: to enable the specified interrupt.
 *  @param unsigned int irq
 *  @return NULL

 *  @see ipmux_irq_type
 *  @deprecated 
 */
void ipmux_irq_enable(unsigned int irq)
{
	volatile unsigned int irq_enable = 0;
	unsigned long flags;
	spin_lock_irqsave(&ipmux_irq_lock,flags);

	switch(irq){
		case OPL_HOSTDMA0_IRQ: irq_enable |= (IRQ_BIT_DMA0);break;
		case OPL_FEDMA1_IRQ: irq_enable |= (IRQ_BIT_DMA1);break;
		case OPL_UART0_IRQ:  irq_enable  |= (IRQ_BIT_UART0);break; 
		case OPL_UART1_IRQ:  irq_enable |= (IRQ_BIT_UART1);break;
		case OPL_ONU_PCM_IRQ: irq_enable |= IRQ_BIT_PCM; break;
		case OPL_ONU_GPIO_4: irq_enable |= IRQ_BIT_GPIO4; break;
		case OPL_ONU_GPIO_5: irq_enable |= IRQ_BIT_GPIO5; break;
		case OPL_ONU_GPIO_10: irq_enable |= IRQ_BIT_GPIO10; break;
		case OPL_ONU_PMAC_IRQ : irq_enable |= IRQ_BIT_PMAC_IRQ; break;
		case OPL_UPTM_IRQ : irq_enable |= IRQ_BIT_UPTM; break; 
		case OPL_IPMUX_IRQ: irq_enable |= ipmux_irq_mask&valid_irq_mask;break;
		case OPL_TIMER0_IRQ: irq_enable |= (IRQ_BIT_TIMER0);break;
	    case OPL_TIMER1_IRQ: irq_enable |= (IRQ_BIT_TIMER1);break;
		/*  -----       Add for 485 card    --------      */
		case OPL_EXAR_PORT_IRQ: irq_enable|= (IRQ_BIT_GPIO11);break;
		
		default:
		printk("%s:the irq %d is invalid\n",__FILE__,irq);
				   break;
	}

	//prom_printf("irq=%d, irq_enable=0x%x\n", irq, irq_enable);
	/* enable bit*/
	*(volatile unsigned int *)(ONU_REG_BASE +MOD_INT_ENABLE_0) |= irq_enable;

	spin_unlock_irqrestore(&ipmux_irq_lock,flags);
}

/** description: it disable the specified irq.
 *  @param unsigned int irq
 *  @return NULL

 *  @see ipmux_irq_type
 *  @deprecated 
 */
void ipmux_irq_disable(unsigned int irq)
{
	unsigned int irq_enable = 0;
	unsigned long flags;
	switch(irq){
		case OPL_HOSTDMA0_IRQ:irq_enable |= (IRQ_BIT_DMA0);break;
		case OPL_FEDMA1_IRQ:irq_enable |= (IRQ_BIT_DMA1);break;
		case OPL_UART0_IRQ:irq_enable |= (IRQ_BIT_UART0);break; 
		case OPL_UART1_IRQ:irq_enable |= (IRQ_BIT_UART1);break;
		case OPL_ONU_PCM_IRQ: irq_enable |= IRQ_BIT_PCM; break;
		case OPL_ONU_GPIO_4: irq_enable |= IRQ_BIT_GPIO4; break;
		case OPL_ONU_GPIO_5: irq_enable |= IRQ_BIT_GPIO5; break;
		case OPL_ONU_GPIO_10: irq_enable |= IRQ_BIT_GPIO10; break;
		case OPL_ONU_PMAC_IRQ : irq_enable |= IRQ_BIT_PMAC_IRQ; break;
		case OPL_UPTM_IRQ : irq_enable |= IRQ_BIT_UPTM; break; 
		case OPL_IPMUX_IRQ: irq_enable |= ipmux_irq_mask;break;
		case OPL_TIMER0_IRQ:irq_enable |= (IRQ_BIT_TIMER0);break;
		case OPL_TIMER1_IRQ:irq_enable |= (IRQ_BIT_TIMER1);break;
		/*  -----       Add for 485 card    --------      */
		case OPL_EXAR_PORT_IRQ: irq_enable|= IRQ_BIT_GPIO11;break;
		default:
				    printk("the irq is invalid\n");
				    break;
	}
	spin_lock_irqsave(&ipmux_irq_lock,flags);

	/*disable*/
	*(volatile unsigned int *)(ONU_REG_BASE +MOD_INT_ENABLE_0) &=~ irq_enable;

	spin_unlock_irqrestore(&ipmux_irq_lock,flags);
	return ;
}


/** description: it is called by request_irq to enable the specified irq
 *  @param unsigned int irq
 *  @return NULL

 *  @see ipmux_irq_type
 *  @deprecated 
 */
static unsigned int ipmux_irq_startup(unsigned int irq)
{
	ipmux_irq_enable(irq);
	return 0;
}


/** description: it is called by do_IRQ to prevent the interrupt reentry.
 *  @param unsigned int irq
 *  @return NULL

 *  @see ipmux_irq_type
 *  @deprecated 
 */
static void ipmux_irq_ack(unsigned int irq)
{
	ipmux_irq_disable(irq);
}

/** description: it is called by do_IRQ to reenable the specified irq.
 *  @param unsigned int irq
 *  @return NULL
 *  @see ipmux_irq_type
 *  @deprecated 
 */
static void ipmux_irq_end(unsigned int irq)
{
	//ipmux_irq_enable(irq);
 if(!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS))
   #if 0
     && (irq != OPL_IPMUX_IRQ) && (irq!=OPL_HOSTDMA0_IRQ)){
  #else
     && (irq != OPL_IPMUX_IRQ)){
  #endif
    ipmux_irq_enable(irq);
  }
}
/** description: to enable the ipmux interrupt function
 *  @param void
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void ipmuxIntEnable(void)
{
	unsigned int val = 0;
	val = onu_readl(MOD_INT_CONFIG);
	val |= TOP_INT_EN;
	onu_writel(MOD_INT_CONFIG, val);
}


/** description: to disable the ipmux interrupt function.
 *  @param void
 *  @return NULL
 *  @see 
 *  @deprecated 
 */
static void ipmuxIntDisable(void)
{
	unsigned int val = 0;
	val = onu_readl(MOD_INT_CONFIG);
	val &=~ TOP_INT_EN;
	onu_writel(MOD_INT_CONFIG, val);
}
/** description: return the g_opl_chip_irq_pending to opl_minte module to handle it.
 *  @param void
 *  @return g_opl_chip_irq_pending:its the ipmux's interrupt pending,which is the 
 value of the ipmux pending register.

 *  @see 
 *  @deprecated 
 */
u32 get_irq_pending()
{
	return g_opl_chip_irq_pending;
}

/** description: clear the global variable g_opl_chip_irq_pending = 0. 
 *  @param void
 *  @return void

 *  @see 
 *  @deprecated 
 */
void clear_irq_pending()
{
	g_opl_chip_irq_pending = 0;
}

void __init arch_init_irq(void)
{
	//clear_c0_status(ST0_IM);
	//clear_c0_cause(CAUSEF_IP);  
	prom_printf("%s,%s\n",__FILE__,__FUNCTION__);
	
	init_ipmux_irqs(); 
	mips_cpu_irq_init();  

	setup_irq(2,&ipmux_action);
	setup_irq(5,&corehi_action);
	prom_printf("%s,%s end\n",__FILE__,__FUNCTION__);
}

asmlinkage void plat_irq_dispatch()
{
	int i;
	int pending = read_c0_status() & read_c0_cause();

	//printk("%s, %s\n", __FILE__, __FUNCTION__);
	if(pending & CAUSEF_IP2)
		malta_hw0_irqdispatch();
    else if (pending & CAUSEF_IP7)
        do_IRQ(ONU_IRQ_CPU_CLOCK);
    else
		spurious_interrupt();

#if 0
    {
        for (i = 7;i>0;i--) {
            if (pending & (0x100<<i)) {
                do_IRQ(i);
                return;
            }
        }
        spurious_interrupt();
    }
#endif
}

/** export the symbol to opl_minte module */
EXPORT_SYMBOL(get_irq_pending);
EXPORT_SYMBOL(clear_irq_pending);
EXPORT_SYMBOL(ipmux_irq_enable);
EXPORT_SYMBOL(ipmux_irq_disable);

