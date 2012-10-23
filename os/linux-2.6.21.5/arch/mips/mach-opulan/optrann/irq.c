
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


#include <asm/mach-optrann/optrann_irq.h>
#include <asm/mach-optrann/optrann_reg.h>
#include <asm/mach-optrann/optrann.h>


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


static void __init init_ipmux_irqs(void); 
static void ipmuxIntEnable(void);
static void ipmuxIntDisable(void);
static void ipmux_irq_ack(unsigned int irq);
static void ipmux_irq_end(unsigned int irq);
static void ipmux_irq_mask(unsigned int irq);
static void ipmux_irq_unmask(unsigned int irq);
static void ipmux_irq_mask_ack(unsigned int irq);

static unsigned int ipmux_irq_startup(unsigned int irq);


static struct irq_chip ipmux_irq_type = {
	.typename = "optrann-irq",
	.startup = ipmux_irq_startup,
	.shutdown = ipmux_irq_shutdown,
	.enable = ipmux_irq_enable,
	.disable = ipmux_irq_disable,
	.ack = ipmux_irq_ack,
	.end = ipmux_irq_end,
	//.ack = ipmux_irq_mask,
	.mask = ipmux_irq_mask,
	.unmask = ipmux_irq_unmask,
	.mask_ack = ipmux_irq_mask_ack,
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

	u32 irq_enable0;
	u32 irq_status;
	u32 irq_mask;

	ipMuxRegRead(PPH_EIC_INT_STATUS_0,&irq_status);
	ipMuxRegRead(PPH_EIC_INT_ENABLE_0,&irq_enable0);
	ipMuxRegRead(PPH_EIC_INT_MASK_0,&irq_mask);

	irq_status = irq_status & irq_enable0;

	// prom_printf("irq_enable=0x%x, status=0x%x, irq_mask=0x%x\n",irq_enable, irq_status,irq_mask);

	if(irq_status & PPH_DMAC_INT_STATUS){
		//	printk("have OPL_FEDMA1_IRQ, status=0x%x\n ", irq_status);
		do_IRQ(OPL_FEDMA1_IRQ);
	}

	if(irq_status & PPH_UART0_INT_STATUS){
		do_IRQ(OPL_UART0_IRQ);
	}

	if(irq_status & PPH_UART1_INT_STATUS){
		do_IRQ(OPL_UART1_IRQ);
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
	for(i = 0;i<32;i++){
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = 0;
		irq_desc[i].depth = 1;
		irq_desc[i].chip = &ipmux_irq_type;
	}
	/* enable the ipmux interrupt line */
	ipmuxIntEnable();
}

void ipmux_irq_mask_ack(unsigned int irq)
{
	volatile unsigned int irq_enable = 0;
	unsigned long flags;
	spin_lock_irqsave(&ipmux_irq_lock,flags);


	switch(irq){
		case OPL_FEDMA1_IRQ:irq_enable |= (PPH_DMAC_INT_ENABLE );break;
		case OPL_UART0_IRQ:irq_enable |= (PPH_UART0_INT_ENABLE );break; 
		case OPL_UART1_IRQ:irq_enable |= (PPH_UART1_INT_ENABLE );break;
		default:
				   printk("%s:the irq %d is invalid\n",__FILE__,irq);
				   break;
	}

	/*mask bit*/
	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_MASK_0) |= irq_enable;
	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_ENABLE_0) |= irq_enable;

	spin_unlock_irqrestore(&ipmux_irq_lock,flags);

}


void ipmux_irq_mask(unsigned int irq)
{
	volatile unsigned int irq_enable = 0;
	unsigned long flags;
	spin_lock_irqsave(&ipmux_irq_lock,flags);


	switch(irq){
		case OPL_FEDMA1_IRQ:irq_enable |= (PPH_DMAC_INT_ENABLE );break;
		case OPL_UART0_IRQ:irq_enable |= (PPH_UART0_INT_ENABLE );break; 
		case OPL_UART1_IRQ:irq_enable |= (PPH_UART1_INT_ENABLE );break; 
		default:
				   printk("%s:the irq %d is invalid\n",__FILE__,irq);
				   break;
	}

	/*mask bit*/
	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_MASK_0) |= irq_enable;

	spin_unlock_irqrestore(&ipmux_irq_lock,flags);

}

void ipmux_irq_unmask(unsigned int irq)
{
	volatile unsigned int val = 0;
	unsigned long flags;
	spin_lock_irqsave(&ipmux_irq_lock,flags);


	switch(irq){
		case OPL_FEDMA1_IRQ: val |= (PPH_DMAC_INT_ENABLE );break;
		case OPL_UART0_IRQ:  val |= (PPH_UART0_INT_ENABLE );break; 
		case OPL_UART1_IRQ:  val |= (PPH_UART1_INT_ENABLE );break; 
		default:
				     printk("%s:the irq %d is invalid\n",__FILE__,irq);
				     break;
	}

	/*unmask bit*/
	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_MASK_0) &=~ val;

	spin_unlock_irqrestore(&ipmux_irq_lock,flags);

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
		case OPL_FEDMA1_IRQ:irq_enable |= (PPH_DMAC_INT_ENABLE );break;
		case OPL_UART0_IRQ:irq_enable |= (PPH_UART0_INT_ENABLE );break; 
		case OPL_UART1_IRQ:irq_enable |= (PPH_UART1_INT_ENABLE );break;
		default:
				   printk("%s:the irq %d is invalid\n",__FILE__,irq);
				   break;
	}

	/* enable bit*/
	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_ENABLE_0) |= irq_enable;
	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_MASK_0) &=~ irq_enable;

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
	unsigned int irq_mask = 0;
	unsigned long flags;
	switch(irq){
		case OPL_UART0_IRQ:irq_mask |=(PPH_UART0_INT_ENABLE);break;
		case OPL_FEDMA1_IRQ:irq_mask |= (PPH_DMAC_INT_ENABLE);break;
		case OPL_UART1_IRQ:irq_mask |= (PPH_UART1_INT_ENABLE );break;
		default:
				    printk("the irq is invalid\n");
				    break;
	}
	spin_lock_irqsave(&ipmux_irq_lock,flags);

	/*disable*/
	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_ENABLE_0) &=~ irq_mask;
	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_MASK_0) |= irq_mask;

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
	ipmux_irq_enable(irq);
}
/** description: to enable the ipmux interrupt function
 *  @param void
 *  @return NULL

 *  @see 
 *  @deprecated 
 */
static void ipmuxIntEnable(void)
{
	unsigned int inteRegValue = 0;

	inteRegValue |= (TOP_PIN_EN );
	inteRegValue &=~ (IRQ0_EN | IRQ1_EN ); /*set IRQ0 and IRQ1 :normal interrupt*/
	ipMuxRegWrite(PPH_EIC_CFG,inteRegValue);

	/*enable all irq interrupt*/
	#if 1
	//*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_MASK_0) = 0;
	//*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_MASK_1) = 0;

	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_ENABLE_0) = 0xffffffff; 
	*(volatile unsigned int *)(IPMUX_INT_REG_BASE +PPH_EIC_INT_ENABLE_1) = 0;

	#endif
}
/** description: to disable the ipmux interrupt function.
 *  @param void
 *  @return NULL


 *  @see 
 *  @deprecated 
 */
static void ipmuxIntDisable(void)
{
	unsigned int inteRegValue = 0;

	inteRegValue &=~ (TOP_PIN_EN );
	ipMuxRegWrite(PPH_EIC_CFG,inteRegValue);
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
	//DOPRINTK;
	prom_printf("%s\n",__FUNCTION__);
	init_ipmux_irqs(); 
	mips_cpu_irq_init();  

	setup_irq(2,&ipmux_action);
	setup_irq(5,&corehi_action);
}

asmlinkage void plat_irq_dispatch()
{
	int i;
	int pending = read_c0_status() & read_c0_cause();

	//printk("%s, %s\n", __FILE__, __FUNCTION__);
	if(pending & CAUSEF_IP2)
		malta_hw0_irqdispatch();
	else if(pending & CAUSEF_IP7)
		do_IRQ(OPTRANN_IRQ_CPU_CLOCK);

#if 0
	{
		for(i = 7;i>0;i--){
			if(pending & (0x100<<i)){
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

