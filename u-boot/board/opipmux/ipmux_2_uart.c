/*
 * OPULAN IPMux uart driver
 *
 *
 *  Copyright (c) 2005  xwang@opulan.com
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <config.h>
#if 0
//#ifdef CONFIG_IPMUX_2
#include <common.h>
#include <asm/op_ipmux.h>


#define DEBUG 0 
#undef DEBUG

#include "ipmux_2_uart.h"

int serial_init (void);
void serial_setbrg (void);
void serial_putc (const char c);
void serial_nowait_putc (const char c);
void serial_puts (const char *s);
int serial_getc (void);
unsigned char serial_nowait_getc (void);
int serial_tstc (void);
void set_tx_rx_trig_level(unsigned char t,unsigned char r);
unsigned char get_int_stat(void);
void serial_set_loopback(void);
void serial_disable_loopback(void);
void serial_enable_fifo(void);
void serial_disable_fifo(void);

extern int eth_rx(void);

#define UART0_ADDR  0xBF004000


#define OFFSET_RHR_THR_DLL  0x00
#define OFFSET_IER_DLM      0x04
#define OFFSET_FCR_ISR      0x08
#define OFFSET_LCR      0x0c
#define OFFSET_MCR      0x10
#define OFFSET_LSR      0x14
#define OFFSET_MSR      0x18
#define OFFSET_SPR      0x1c
#define OFFSET_EFR      0x20


/******************************************************************************
*
* serial_init - initialize a channel
*
* This routine initializes the number of data bits, parity
* and set the selected baud rate. Interrupts are disabled.
* Set the modem control signals if the option is selected.
*
* RETURNS: N/A
*/

int serial_init (void)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);

    /* disable UART interrupts for now */
    pUart->w1.uier = 0;  

    /*set the baudrate*/
    serial_setbrg();
    
    /*set the char len, partity mode, stop bit*/
    pUart->lcr = CHAR_LEN_8 | PARITY_NONE | ONE_STOP;

   serial_enable_fifo();


    return 0;
}


/******************************************************************************
*
* serial_setbrg - set baud rate
*
* This routine set default baud rate in DLL and DLM register. 
*
*
* RETURNS: N/A
*/
void serial_setbrg (void)
{
    DECLARE_GLOBAL_DATA_PTR;
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    unsigned short fdiv;
    
    /*set divisor register dll, dlm */
    pUart->lcr |= LCR_DLAB;

    gd->baudrate =CONFIG_BAUDRATE ;
    
    fdiv = (FPCLK /16) / gd->baudrate;
    pUart->w2.dir0 = fdiv & 0xFF;
    pUart->w1.dir1 = fdiv >> 8;
    pUart->lcr &= ~LCR_DLAB;

}

/******************************************************************************
*
* serial_putc - transmitting a single chacter 
*
* This routine poll lsr THRE bit.  If it is empty, then put chacter to be 
* transmited into THR register.  It also translate newline symbol '\n' into '\r'.
*
* INPUT:  c -- character to be transmitted.
* RETURNS: N/A
*/
void serial_putc (const char c)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);

    if (c == '\n') 
    {
        while((pUart->lsr & 0x20/*LSR_THRE*/)==0); 
        pUart->w2.uthr = '\r';
        opipmux_sync();
        
    }
    /* Wait for fifo to shift out some bytes */
    while((pUart->lsr & 0x20/*LSR_THRE*/)==0); 
    pUart->w2.uthr = c;
    
    opipmux_sync();

}

/******************************************************************************
* serial_nowait_putc - transmitting a single chacter 
*
* This routine put chacter directly into  THR register.  It 
* This routine put chacter directly into  THR register.  It 
* also translate newline symbol '\n' into '\r'.
*
* RETURNS: N/A
*/
void serial_nowait_putc (const char c)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);

    if (c == '\n') serial_nowait_putc ('\r');

    pUart->w2.uthr = c;
}

/******************************************************************************
*
* serial_puts - initialize a channel
*
* This routine initializes the number of data bits, parity
* and set the selected baud rate. Interrupts are disabled.
* Set the modem control signals if the option is selected.
*
* RETURNS: N/A
*/
void serial_puts (const char *s)
{
    
    while (*s)
    {
        serial_putc (*s++);
    }
    
}

/******************************************************************************
*
* serial_getc - retrieve one character from buffer 
*
* This routine check the rx buffer staus, if it is not empty, retrieve one 
* character from receiving buffer and return it to calling routine.
*
* input: none.
*
* RETURNS:  receiving character.
*/
int serial_getc (void)
{
    unsigned char c ;
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    
    while((pUart->lsr & LSR_DR ) == 0);
    c = pUart->w2.urhr;

    return c;
}

/******************************************************************************
*
* serial_nowait_getc - retrieve one character from fifo 
*
* This routine read one character from receiving buffer 
* 
* input: none.
*
* RETURNS:  receiving character.
*/
unsigned char serial_nowait_getc (void)
{
    unsigned char c;
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);

    c = pUart->w2.urhr;
    return c;
}

/******************************************************************************
*
* serial_tstc - check receiving status
*
* This routine check line status bit to see whether the receive buffer is ready.
*
* RETURNS: 
*      1:  receive buffer has chacter.
*      0:  receive buffer is empty.
*/
int serial_tstc (void)
{
    unsigned char t;
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    extern int enable_net_polling;
    
    if (enable_net_polling)
  // if (1)
    {
        eth_rx();
    }
    t = pUart->lsr;

    if( t & 0x80)
    {
        serial_puts("fifo data err\n");
    }
    if( t & LSR_DR ) /* LSR_DR 0x1  */
    {
        /* Data in rfifo */
        return(1);
    }
    if( t & 0x02 )
    {
        serial_puts("overrun err\n");
    }
    if( t & 0x04 )
    {
        serial_puts("parity err\n");
    }
    if( t & 0x08 )
    {
        serial_puts("frame err\n");
    }
    if( t & 0x10 )
    {
       serial_puts("break interrupt\n");
    }
    return 0;
}

/******************************************************************************
*
* set_tx_rx_trig_level - set transmiter and receiver trigger level
*
* This routine set fifo transmiter and receiver trigger level
*
* INPUT:   l: the rx trigger level.  It has four mode.  The value 0,1,2,3 
*       designate 8, 16, 32, 48 bytes trigger point. 
*      2: the tx trigger level.  It has four mode.  The value 0,1,2,3 
*       designate 8, 16, 32, 48 bytes trigger point. 
*  
* RETURNS: N/A
*/
void set_tx_rx_trig_level(unsigned char t,unsigned char r)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    
    if( (char)t < 0 || t >3 || (char)r < 0 || r > 3)
        return;
    pUart->w1.uier &= ~(TxFIFO_BIT | RxFIFO_BIT);     /* disable interrupt */
    pUart->w0.ufiocr = (r<<6 | t<<4 |FIFO_ENABLE);  /* set tx rx trig bits */
    pUart->w1.uier |= (TxFIFO_BIT | RxFIFO_BIT);  /* enable tx rx interrupt */
}


/******************************************************************************
*
* get_int_stat - get interrupt status
*
* This routine set fifo receiver trigger level
*
* INPUT:   l: the rx trigger level.  It has four mode.  The value 0,1,2,3 
*       designate 8, 16, 32, 48 bytes trigger point. 
*  
* RETURNS: N/A
*/

unsigned char get_int_stat(void)
{       
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    
    return pUart->w0.uisr;

}


/******************************************************************************
*
* serial_set_loopback   - set loopback mode 
*
* This routine set uart to the loopback mode 
*
* RETURNS: N/A
*/
void serial_set_loopback(void)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    pUart->mcr |= MCR_LOOP;
}

/******************************************************************************
*
* serial_disable_loopback   - disable loopback mode 
*
* This routine disable loopback mode 
*
* RETURNS: N/A
*/
void serial_disable_loopback(void)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    pUart->mcr &= ~MCR_LOOP;
}

/******************************************************************************
*
* serial_enable_fifo    - enable fifo  mode 
*
* This routine enable fifo mode 
*
* RETURNS: N/A
*/
void serial_enable_fifo(void)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    /*pUart->efr    |= 0x10; */             /*Enable efr[4] */  
    pUart->w0.ufiocr = FIFO_ENABLE;            /* | RxCLEAR | TxCLEAR;*/ /* reset fifo */
}

/******************************************************************************
*
* serial_disable_fifo   - disable fifo mode 
*
* This routine disable fifo mode 
*
* RETURNS: N/A
*/
void serial_disable_fifo(void)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    pUart->w0.ufiocr = ~FIFO_ENABLE; 
}

#endif

