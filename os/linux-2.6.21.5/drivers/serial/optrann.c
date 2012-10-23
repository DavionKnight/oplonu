/********************************************************************************
 *
 * Filename: optrann.c
 *
 * Summary:
 *  UART Driver for optrann
 * 
 * Modification History:
 *  Date	  By	   Summary
 *  -------- -------- ----------------------------------------------------------
 *  2009-11-2       Figo.zhang	 Original Creation
 *
 ********************************************************************************/

#if defined(CONFIG_SERIAL_OPTRANN_CONSOLE) && defined(CONFIG_MAGIC_SYSRQ)
#define SUPPORT_SYSRQ
#endif
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/sysrq.h>
#include <linux/console.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/types.h>

#include <linux/serial_reg.h>

#include "optrann.h"
#include "8250.h"

#ifdef CONFIG_KGDB_CONSOLE
#include <linux/kgdb.h>
#endif

#define debug 0
#define dprintk(fmt, arg...)	if (debug) \
	printk(KERN_EMERG "serial:" fmt, ## arg)



struct uart_optrann_port {
	struct uart_port        port;
	char			*name;
	unsigned char		ier;
	unsigned char		lcr;
	unsigned short		bugs;		/* port bugs */
};

static int serial_optrann_startup(struct uart_port *port);
static void serial_optrann_shutdown(struct uart_port *port);
static const char *serial_optrann_type(struct uart_port *port);
static u_int serial_optrann_tx_empty(struct uart_port * port);
static void serial_optrann_set_mctrl(struct uart_port *port , u_int mctrl);
static unsigned int serial_optrann_get_mctrl(struct uart_port *port );
static void serial_optrann_set_termios(struct uart_port *port, struct termios *termios,struct termios *old);
static void serial_optrann_start_tx(struct uart_port *port);
static void serial_optrann_stop_tx(struct uart_port *port);
static void serial_optrann_stop_rx(struct uart_port *port);
static void receive_chars(struct uart_optrann_port *port, int * status);
static void transmit_chars(struct uart_optrann_port * up);
static inline irqreturn_t serial_optrann_irq(int irq, void *dev_id);
static void serial_optrann_config_port(struct uart_port *port, int flags);
static int serial_optrann_verify_port(struct uart_port *port, struct serial_struct *ser);
//static int serial_optrann_probe(struct platform_device *dev);
static int serial_optrann_remove(struct platform_device *dev);
static int serial_optrann_resume(struct platform_device *dev);
static int serial_optrann_suspend(struct platform_device *dev, pm_message_t state);
int __init serial_optrann_init(void);
void __exit serial_optrann_exit(void);
static inline void wait_for_xmitr(struct uart_optrann_port *up, int bits);


static inline unsigned int serial_in(struct uart_optrann_port *up, int offset)
{
	return readl(up->port.membase + offset*4);
}

static inline void serial_out(struct uart_optrann_port *up, int offset, int value)
{
	//writel(value, (up->port.membase + offset));
	*(volatile u32 *)(up->port.membase + offset*4) = value;
}

static inline void serial_clear_fifos(struct uart_optrann_port *p)
{
	serial_out(p, UART_FCR, UART_FCR_ENABLE_FIFO);
	serial_out(p, UART_FCR, UART_FCR_ENABLE_FIFO |
			UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT);
	serial_out(p, UART_FCR, 1);
}


static int serial_optrann_startup(struct uart_port *port)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;
	int retval;
	int fifo_ctrl =0;
	int isr = 0, iir, lsr;
	unsigned long flags;

	dprintk( "base addr= 0x%x\n",up->port.membase);
	dprintk( "request irq %d\n",up->port.irq);

	/*
	 * Clear the interrupt registers.
	 */
	(void) serial_in(up, UART_LSR);
	(void) serial_in(up, UART_RX);
	(void) serial_in(up, UART_IIR);
	(void) serial_in(up, UART_MSR);

	serial_clear_fifos(up);

	/*
	 * Allocate the IRQ
	 */
	//puts("Begin request_irq\r\n");
	retval = request_irq(up->port.irq, serial_optrann_irq, IRQF_DISABLED, up->name, up);
	if (retval)
		return retval;


	/* Now, initialize the UART
	*/
	serial_out(up, UART_LCR, UART_LCR_WLEN8);

	/*
	 * Finally, enable interrupts.  
	 */
	up->ier = UART_IER_RLSI | UART_IER_RDI | UART_IER_THRI ;
	serial_out(up, UART_IER, up->ier);

	/*
	 * And clear the interrupt registers again for luck.
	 */
	(void) serial_in(up, UART_LSR);
	(void) serial_in(up, UART_RX);
	(void) serial_in(up, UART_IIR);
	(void) serial_in(up, UART_MSR);

	dprintk( "serial_optrann_startup done\n");

	return 0;
}


static void serial_optrann_shutdown(struct uart_port *port)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;

	free_irq(up->port.irq, up);

}

static const char *serial_optrann_type(struct uart_port *port)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;
	return up->name;
}

static u_int serial_optrann_tx_empty(struct uart_port * port)
{
	unsigned long flags;
	unsigned int ret;
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;

	spin_lock_irqsave(&up->port.lock, flags);
	ret = serial_in(up, UART_LSR) & UART_LSR_TEMT ? TIOCSER_TEMT : 0;
	spin_unlock_irqrestore(&up->port.lock, flags);

	return ret;
}

static void serial_optrann_set_mctrl(struct uart_port *port , u_int mctrl)
{
	/* nothing */
}

static unsigned int serial_optrann_get_mctrl(struct uart_port *port )
{

	/*nothing*/
	return 0;
}

	static void
serial_optrann_set_termios(struct uart_port *port, struct termios *termios,
		struct termios *old)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;
	unsigned int uacr;
	unsigned long flags;
	unsigned int baud, quot;

	switch (termios->c_cflag & CSIZE) {
		case CS5:
			uacr = UART_LCR_WLEN5;
			break;
		case CS6:
			uacr = UART_LCR_WLEN6;
			break;
		case CS7:
			uacr = UART_LCR_WLEN7;
			break;
		default:
		case CS8:
			uacr = UART_LCR_WLEN8;
			break;
	}

	/* optrann have no Auto flow control  bit*/
	if (termios->c_cflag & CSTOPB)
		uacr |= UART_LCR_STOP;
	if (termios->c_cflag & PARENB) {
		if (termios->c_cflag & PARODD)
			uacr |= UART_LCR_PARITY;
		else
			uacr |= UART_LCR_EPAR;
	} else {
		uacr |= UART_LCR_SPAR;
	}

	/*
	 * for optrann: i dont know the uartclk and baud ?
	 */
	baud = uart_get_baud_rate(port, termios, old, 0, 115200);
	quot = uart_get_divisor(port, baud);

	/*
	 * Ok, we're now changing the port state.  Do it with
	 * interrupts disabled.
	 */
	spin_lock_irqsave(&up->port.lock, flags);

	/*
	 * Update the per-port timeout.
	 */
	uart_update_timeout(port, termios->c_cflag, baud);

	if (termios->c_iflag & INPCK)
		up->port.read_status_mask |= UART_LSR_FE | UART_LSR_PE;
	if(termios->c_iflag & BRKINT)
		up->port.read_status_mask |= UART_LSR_BI;

	/*
	 * Characters to ignore
	 */
	up->port.ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		up->port.ignore_status_mask |= UART_LSR_PE | UART_LSR_FE;
	if (termios->c_iflag & IGNBRK) {
		up->port.ignore_status_mask |= UART_LSR_BI;
	}

	/*
	 * ignore all characters if CREAD is not set
	 */
	if ((termios->c_cflag & CREAD) == 0)
		up->port.ignore_status_mask |= UART_LSR_DR;

	spin_unlock_irqrestore(&up->port.lock, flags);	

}


static void serial_optrann_start_tx(struct uart_port *port)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;

	if (!(up->ier & UART_IER_THRI)) {
		up->ier |= UART_IER_THRI;
		serial_out(up, UART_IER, up->ier);
#if 0
		if (up->bugs & UART_BUG_TXEN) {
			unsigned char lsr, iir;
			lsr = serial_in(up, UART_LSR);
			iir = serial_in(up, UART_IIR);
			if (lsr & UART_LSR_TEMT && iir & UART_IIR_NO_INT)
				transmit_chars(up);
		}
#endif
	}

}

static inline void __stop_tx(struct uart_optrann_port *p)
{
	if (p->ier & UART_IER_THRI) {
		p->ier &= ~UART_IER_THRI;
		serial_out(p, UART_IER, p->ier);
	}
}


static void serial_optrann_stop_tx(struct uart_port *port)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;

	__stop_tx(up);

}

static void serial_optrann_stop_rx(struct uart_port *port)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;
	up->ier &= ~UART_IER_RLSI;
	serial_out(up, UART_IER, up->ier);
}

static void receive_chars(struct uart_optrann_port *port, int * status)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;

	struct tty_struct *tty = up->port.info->tty;
	unsigned int ch, flag;
	int max_count = 256;

	do {
		ch = serial_in(up, UART_RX);
		ch = ch & 0xFF;

		flag = TTY_NORMAL;
		up->port.icount.rx++;

		if(unlikely(*status & (UART_LSR_BI | UART_LSR_PE |
						UART_LSR_FE | UART_LSR_OE))) {

			/*
			 * For statistics only
			 */

			if(*status & UART_LSR_BI) {
				up->port.icount.brk++;
			} else if(*status & UART_LSR_PE)
				up->port.icount.parity++;
			else if (*status & UART_LSR_FE)
				up->port.icount.frame++;

			/*
			 * Mask off conditions which should be ignored.
			 */
			*status &= up->port.read_status_mask;

			if (*status & UART_LSR_BI) {
				flag = TTY_BREAK;
			} else if (*status & UART_LSR_PE)
				flag = TTY_PARITY;
			else if (*status & UART_LSR_FE)
				flag = TTY_FRAME;

		}

		if (uart_handle_sysrq_char(&up->port, ch))
			goto ignore_char;

		uart_insert_char(&up->port, *status, UART_LSR_OE, ch, flag);

ignore_char:
		*status = serial_in(up, UART_LSR);

	}while((*status & UART_LSR_DR) && (max_count-- > 0));

	tty_flip_buffer_push(tty);	
}


static void transmit_chars(struct uart_optrann_port * up)
{
	struct circ_buf *xmit = &up->port.info->xmit;
	int count;

	if (up->port.x_char) {
		serial_out(up, UART_TX , up->port.x_char);
		//dprintk("send a chars\n");
		up->port.icount.tx++;
		up->port.x_char = 0;
		return;
	}
	if (uart_tx_stopped(&up->port)) {
		serial_optrann_stop_tx(&up->port);
		return;
	}
	if (uart_circ_empty(xmit)) {
		__stop_tx(up);
		return;
	}

	count = up->port.fifosize;
	do {
		serial_out(up, UART_TX, xmit->buf[xmit->tail]);
		//printk("transmit_chars \n");
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		up->port.icount.tx++;
		if (uart_circ_empty(xmit))
			break;
	} while (--count > 0);


	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(&up->port);


	if (uart_circ_empty(xmit))
		serial_optrann_stop_tx(&up->port);

}

static inline irqreturn_t serial_optrann_irq(int irq, void *dev_id)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)dev_id;
	unsigned int status;

	u32 pending = serial_in(up, UART_IIR);

	//dprintk("have interrupt, pending =0x%x\n", pending);

	if((pending & UART_IIR_NO_INT) == 0){
		/*read line status reg*/
		status = serial_in(up, UART_LSR);
		//dprintk("staus=0x%x\n", status);

		if(status & UART_LSR_DR)
			receive_chars(up, &status);
		if(status & UART_LSR_THRE)
			transmit_chars(up);
	}

	return IRQ_HANDLED;
}

static void serial_optrann_release_port(struct uart_port *port)
{
}

static int serial_optrann_request_port(struct uart_port *port)
{
	return 0;
}

static void serial_optrann_config_port(struct uart_port *port, int flags)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;
	up->port.type = PORT_OPTRANN;
}

static int serial_optrann_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	/* we don't want the core code to modify any port params */
	return -EINVAL;
}



#ifdef CONFIG_SERIAL_OPTRANN_CONSOLE

static struct uart_optrann_port serial_optrann_ports[];
static struct uart_driver serial_optrann_reg;

/*
 *  Wait for transmitter & holding register to empty
 */
static inline void wait_for_xmitr(struct uart_optrann_port *up, int bits)
{
	unsigned int status, tmout = 10000;

	/* Wait up to 10ms for the character(s) to be sent. */
	do {
		status = serial_in(up, UART_LSR);
		if (--tmout == 0)
			break;
		udelay(1);
	} while ((status & bits) != bits);
}


static void serial_optrann_console_putchar(struct uart_port *port, int ch)
{
	struct uart_optrann_port *up = (struct uart_optrann_port *)port;


	wait_for_xmitr(up, UART_LSR_THRE);
	serial_out(up, UART_TX, ch);
	//prom_printf("have putchar to console\n");
}


/*
 * Print a string to the serial port trying not to disturb
 * any possible real use of the port...
 *
 */
	static void
serial_optrann_console_write(struct console *co, const char *s, unsigned int count)
{
	struct uart_optrann_port *up = &serial_optrann_ports[co->index];

	uart_console_write(&up->port, s, count, serial_optrann_console_putchar);

}

	static int __init
serial_optrann_console_setup(struct console *co, char *options)
{
	struct uart_optrann_port *up;
	int baud = 9600;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (co->index == -1 || co->index >= serial_optrann_reg.nr)
		co->index = 0;

	up = &serial_optrann_ports[co->index];

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(&up->port, co, baud, parity, bits, flow);
}

static struct console serial_optrann_console = {
	.name		= "ttyS",
	.write		= serial_optrann_console_write,
	.device		= uart_console_device,
	.setup		= serial_optrann_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &serial_optrann_reg,
};

	static int __init
serial_optrann_console_init(void)
{
	prom_printf("serial_optrann_console_init in\n");
	register_console(&serial_optrann_console);
	return 0;
}

console_initcall(serial_optrann_console_init);
#define OPTRANN_CONSOLE	&serial_optrann_console

#elif defined(CONFIG_KGDB_CONSOLE)
#define OPTRANN_CONSOLE	 &kgdbcons
#else
#define OPTRANN_CONSOLE	NULL
#endif /*end ifdef CONFIG_SERIAL_CONSOLE*/



static struct uart_ops serial_optrann_pops = {
	.tx_empty	= serial_optrann_tx_empty,
	.set_mctrl	= serial_optrann_set_mctrl,
	.get_mctrl	= serial_optrann_get_mctrl,
	.stop_tx	= serial_optrann_stop_tx,
	.start_tx	= serial_optrann_start_tx,
	.stop_rx	= serial_optrann_stop_rx,
	//.enable_ms	= serial_optrann_enable_ms,
	//.break_ctl	= serial_optrann_break_ctl,
	.startup	= serial_optrann_startup,
	.shutdown	= serial_optrann_shutdown,
	.set_termios	= serial_optrann_set_termios,
	//.pm		= serial_optrann_pm,
	.type		= serial_optrann_type,
	.release_port	= serial_optrann_release_port,
	.request_port	= serial_optrann_request_port,
	.config_port	= serial_optrann_config_port,
	.verify_port	= serial_optrann_verify_port,
};


static struct uart_optrann_port serial_optrann_ports[] = {
	{	/* UART 0 */
		.name	= "UART 0",
		.port	= {
			.type		= PORT_OPTRANN,
			.iotype		= UPIO_MEM,
			.membase	= (0xbf000000 + UART0_SA_BASE),
			.mapbase	= 0xbf000000 + UART0_SA_BASE,
			.irq		= OPL_UART0_IRQ,
			/*.uartclk	= SYS_CLK/16,*/
			.fifosize	= 1,
			.ops		= &serial_optrann_pops,
			.line		= 0,
		},
	}, 
	{	/* UART 1*/
		.name	= "UART 1",
		.port	= {
			.type		= PORT_OPTRANN,
			.iotype		= UPIO_MEM,
			.membase	= (0xbf000000 + UART1_SA_BASE),
			.mapbase	= 0xbf000000 + UART1_SA_BASE,
			.irq		= OPL_UART1_IRQ,
			/*.uartclk	= SYS_CLK/16,*/
			.fifosize	= 1,
			.ops		= &serial_optrann_pops,
			.line		= 0,
		},
	}, 

};

static struct uart_driver serial_optrann_reg = {
	.owner		= THIS_MODULE,
	.driver_name	= "optrann serial",
	.dev_name	= "ttyS",
	.major		= TTY_MAJOR,
	.minor		= 64,
	.nr		= ARRAY_SIZE(serial_optrann_ports),
	.cons		= OPTRANN_CONSOLE,
};

/*
   static struct platform_driver serial_optrann_driver = {
   .probe          = serial_optrann_probe,
   .remove         = serial_optrann_remove,

   .suspend	= serial_optrann_suspend,
   .resume		= serial_optrann_resume,
   .driver		= {
   .name	= "optrann serial",
   },
   };
   */

static int  serial_optrann_probe(/*struct platform_device *dev*/)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(serial_optrann_ports); i++)
	{
		//serial_optrann_ports[i].port.dev = NULL;
		uart_add_one_port(&serial_optrann_reg, &serial_optrann_ports[i].port);
		prom_printf("uart add one port done \n");
		//platform_set_drvdata(dev, &serial_optrann_ports[i]);
	}

	return 0;
}

static int __devexit serial_optrann_remove(struct platform_device *dev)
{
	struct uart_optrann_port *sport = platform_get_drvdata(dev);

	platform_set_drvdata(dev, NULL);

	if (sport)
		uart_remove_one_port(&serial_optrann_reg, &sport->port);

	return 0;
}

static int serial_optrann_resume(struct platform_device *dev)
{
	struct uart_optrann_port *sport = platform_get_drvdata(dev);

	if (sport)
		uart_resume_port(&serial_optrann_reg, &sport->port);

	return 0;
}
static int serial_optrann_suspend(struct platform_device *dev, pm_message_t state)
{
	struct uart_optrann_port *sport = platform_get_drvdata(dev);

	if (sport)
		uart_suspend_port(&serial_optrann_reg, &sport->port);

	return 0;
}

int __init serial_optrann_init(void)
{
	int ret;

	printk(KERN_INFO "Serial: optrann serial driver $Revision: 1.0\n");
	ret = uart_register_driver(&serial_optrann_reg);
	if (ret < 0) {
		printk(KERN_ERR "failed to register UART driver\n");
		return -1;
	}
	printk(KERN_EMERG "uart_register_driver finished\n");

	serial_optrann_probe();

	//return platform_driver_register(&serial_optrann_driver);
	return 0;
}

void __exit serial_optrann_exit(void)
{
	//	platform_driver_unregister(&serial_optrann_driver);
	uart_unregister_driver(&serial_optrann_reg);
}


module_init(serial_optrann_init);
module_exit(serial_optrann_exit);

MODULE_LICENSE("GPL");

