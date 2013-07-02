//*****************************************************************************
//
// uart.h - Defines and Macros for the UART.
//
// Copyright (c) 2005-2009 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 5228 of the Stellaris Peripheral Driver Library.
//
//*****************************************************************************

#ifndef __UART_DEFAULT_H__
#define __UART_DEFAULT_H__

#include "serial.h"
#include "serial_ser.h"
//#include "serial_ser.h"
//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#define CPLD_OFFSET 		0xc000000
#define PROTECT_OFFSET		0x07
#define V_UNPROTECT      	0x55555555
#define V_PROTECT 		    0

#define OK  				0
#define ERROR  			-1
#define YES				1
#define NO				0





/*������Ƶ�����㹫ʽ:x=����Ƶ��/1.8432*115200(115200Ϊ1.8432��������Ƶ��) */
#define BAUD_MAX_FREQUENCY          1500000/*������Ƶ��*/
#define UART_FREQUENCY_DIVISOR      30/*��Ƶ����Ҫ��������Ƶ��-= ��Ƶ��/����ֵ�����Ա���752C����֪bug*/

#define TX_FIFO_LEN                 50/*д64�������������룬Ϊ����������д����*/
#define RX_FIFO_LEN                 64

/*uart register address. cpld highest bit is set to 1.*/
#define UART_ADD_RHR                0x8/*receiver holding register. r*/
#define UART_ADD_THR                0x8/*transmit holding register. w*/
#define UART_ADD_IER                0x9/*interrupt enable register. r/w*/
#define UART_ADD_FCR                0xA/*fifo control register. w*/
#define UART_ADD_IIR                0xA/*interrupt identification register. r*/
#define UART_ADD_LCR                0xB/*line control register. r/w*/
#define UART_ADD_MCR                0xC/*modem control register. r/w*/
#define UART_ADD_LSR                0xD/*line status register. r*/
#define UART_ADD_MSR                0xE/*modem status register. r*/
#define UART_ADD_SPR                0xF
#define UART_ADD_DLL                0x8/*divisor latches low register. r/w*/
#define UART_ADD_DLH                0x9/*divisor latches high register. r/w*/
#define UART_ADD_AFR                0xA/*alternate function register. r/w*/
#define UART_ADD_EFR                0xA/*enhanced feature register. r/w*/
#define UART_ADD_Xon1               0xC
#define UART_ADD_Xon2               0xD
#define UART_ADD_Xoff1              0xE
#define UART_ADD_Xoff2              0xF
#define UART_ADD_TCR                0xE/*transmission control regiser. for auto-RTS. r/w*/
#define UART_ADD_TLR                0xF/*trigger level register. enhanced RHR. r/w*/
#define UART_ADD_FIFO               0xF/*fifo ready register. r*/

/*uart register value. */
/*ÿһ���Ĵ�������һ��bit.*/
/*IER register.*/
#define UART_V_INT_RX_AVI_EN        0x01/*rx data available interrupt. in IER*/
#define UART_V_INT_THR_EMP_EN       0x02/*thr empty interrupt. in IER*/
#define UART_V_INT_LINE_STA_EN      0x04/*rx line status interrupt. in IER*/
#define UART_V_INT_MOD_STA_EN       0x08/*modem status interrupt. in IER*/
#define UART_V_SLEEP_MODE_EN        0x10/*sleep mode. in IER*/
#define UART_V_INT_XOFF_EN          0x20/*Xoff interrupt enable. in IER*/
#define UART_V_INT_RTS_EN           0x40/*RTS interrupt enable. in IER*/
#define UART_V_INT_CTS_EN           0x80/*CTS interrupt enable. in IER*/

/*FCR register.*/
#define UART_V_EN_FIFO              0x01/*enables fifos. in FCR*/
#define UART_V_RESET_RX_FIFO        0x02/*resets rx fifo. in FCR*/
#define UART_V_RESET_TX_FIFO        0x04/*resets tx fifo. in FCR*/
#define UART_V_DMA_MODE             0x08/*dma mode select. in FCR*/

/*IIR register.*/
#define UART_V_INT_STA              0x01/*interrupt status. 0-no int, 1-an int.  in IIR*/
#define UART_V_INT_XOFF_STA         0x10/*Xoff character has been detected. in IIR*/
#define UART_V_INT_CTS_RTS_STA      0x20/*rts or cts change state from active to inactive. in IIR*/

/*LCR register.*/
#define UART_V_STOP_BITS            0x04/*NO. of stop bits. in LCR*/
#define UART_V_PARITY_EN            0x08/*parity enable. in LCR*/
#define UART_V_PARITY_TYPE          0x10/*parity type select. in LCR*/
#define UART_V_PARITY_FORCE         0x20/*force setting parity. in LCR*/
#define UART_V_BREAK_CTL            0x40/*break control bit. in LCR*/
#define UART_V_DLAB_EFR_EN          0x80/*DLAB and EFR enable. in LCR*/

#define UART_V_CONFIG_EF            0xBF/*after config this value, EFR and Xon can be configed. 1011 1111*/
#define UART_V_CONFIG_AFR           0x80/*after config this value, AFR can be configed. 1000 0000*/

/*MCR register. */
#define UART_V_FORCE_DTR            0x01/*force DTR. in MCR*/
#define UART_V_FORCE_RTS            0x02/*force RTS. in MCR*/
#define UART_V_FIFORDY_EN           0x04/*fifo ready enable. in MCR*/
#define UART_V_IRQ_EN               0x08/*enable all the IRQ outputs to the active state. in MCR*/
#define UART_V_LOOP_EN              0x10/*enable loopback mode. in MCR*/
#define UART_V_XON_ANY              0x20/*enable Xon any. in MCR*/
#define UART_V_TCR_TLR_EN           0x40/*enable access to the TCR and TLR register. in MCR*/
#define UART_V_DIVIDE_CLOCK         0x80/*0-divide by one clock input, 1-divide by four clock input. in MCR*/

/*LSR register.*/
#define UART_V_DATA_REV             0x01/*data in receiver. in LSR*/
#define UART_V_OE                   0x02/*overrun error. in LSR*/
#define UART_V_PE                   0x04/*parity error. in LSR*/
#define UART_V_FM                   0x08/*framing error. in LSR*/
#define UART_V_BREAK_INT            0x10/*break interrupt. in LSR*/
#define UART_V_THR_EMPTY            0x20/*THR empty. in LSR*/
#define UART_V_THR_TSR              0x40/*THR an TSR empty. in LSR*/
#define UART_V_RX_ERR               0x80/*error in RX FIFO. in LSR*/

/*EFR register.*/
#define UART_V_R_XON_OFF2           0x01/*receiver compares Xon2, Xoff2. in EFR*/
#define UART_V_R_XON_OFF1           0x02/*receiver compares Xon1, Xoff1. in EFR*/
#define UART_V_T_XON_OFF2           0x04/*transmit Xon2, Xoff2. in EFR*/
#define UART_V_T_XON_OFF1           0x08/*transmit Xon1, Xoff1. in EFR*/
#define UART_V_ENHANCE_FUN          0x10/*enable enhanced functions. in EFR*/
#define UART_V_SPE_CHA_DET          0x20/*special character detect. in EFR*/
#define UART_V_AUTO_RTS_EN          0x40/*auto RTS flow control enabled. in EFR*/
#define UART_V_AUTO_CTS_EN          0x80/*auto CTS flow control enabled. in EFR*/

/*AFR register.*/
#define UART_V_CONC                 0x01/*CONC enables. in AFR*/
#define UART_V_IREN                 0x02/*IrDA SIR mode. in AFR*/
#define UART_V_485EN                0x04/*enables the half duplex 485 mode. in AFR*/
#define UART_V_485LG                0x08/*enhanced 485EN. in AFR*/
#define UART_V_RCVEN                0x10/*enable reveive when 485 mode enable*/

/*FIFORDY register.*/
#define UART_V_TX_FIFO_A_STA        0x01/*there are at least a TX trigger level number of spaces available in TX FIFO A. in FIFORDY*/
#define UART_V_TX_FIFO_B_STA        0x02/*there are at least a TX trigger level number of spaces available in TX FIFO B. in FIFORDY*/
#define UART_V_TX_FIFO_C_STA        0x04/*there are at least a TX trigger level number of spaces available in TX FIFO C. in FIFORDY*/
#define UART_V_TX_FIFO_D_STA        0x08/*there are at least a TX trigger level number of spaces available in TX FIFO D. in FIFORDY*/
#define UART_V_RX_FIFO_A_STA        0x10/*RX FIFO A has more than RX trigger level characters for read or timeout. in FIFORDY*/
#define UART_V_RX_FIFO_B_STA        0x20/*RX FIFO B has more than RX trigger level characters for read or timeout. in FIFORDY*/
#define UART_V_RX_FIFO_C_STA        0x40/*RX FIFO C has more than RX trigger level characters for read or timeout. in FIFORDY*/
#define UART_V_RX_FIFO_D_STA        0x80/*RX FIFO D has more than RX trigger level characters for read or timeout. in FIFORDY*/

/*ÿ���Ĵ������Ǽ���bit�����ϣ���shift��len��ͬ����*/
/*FCR register.*/
#define UART_V_SHIFT_TX_TRI         4/*tx trigger level. in FCR*/
#define UART_V_LEN_TX_TRI           2/*len of tx trigger level. in FCR*/
#define UART_V_SHIFT_RX_TRI         6/*rx trigger level. in FCR*/
#define UART_V_LEN_RX_TRI           2/*len of rx trigger level. in FCR*/
#define UART_V_RX_TRI_1C            0x0/*rx trigger level 1 characters*/
#define UART_V_RX_TRI_4C            0x1/*rx trigger level 4 characters*/
#define UART_V_RX_TRI_56C           0x2/*rx trigger level 56 characters*/
#define UART_V_RX_TRI_60C           0x3/*rx trigger level 60 characters*/

/*IIR register.*/
#define UART_V_SHIFT_INT_BIT        1/*interrupt priority bit. in IIR*/
#define UART_V_LEN_INT_BIT          3/*len of interrupt priority bit. in IIR*/
#define UART_V_INT_REV_LINE_STA     0x3/*receive line status interrupt. in IIR*/
#define UART_V_INT_RX_TIMEOUT_STA   0x6/*rx time out interrupt. in IIR*/
#define UART_V_INT_RHR_STA          0x2/*rx fifo above trigger level. in IIR*/
#define UART_V_INT_THR_STA          0x1/*THR empty(fifo disable), tx fifo passes above trigger level. in IIR*/
#define UART_V_INT_MODEM_STA        0x0/*modem status interrupt. in IIR*/

/*LCR register.*/
#define UART_V_SHIFT_WORD_LEN       0/*word length. in LCR*/
#define UART_V_LEN_WORD_LEN         2/*len of word length. in LCR*/
#define UART_V_WORD_LEN_5           0x0/*00*/
#define UART_V_WORD_LEN_6           0x1/*01*/
#define UART_V_WORD_LEN_7           0x2/*10*/
#define UART_V_WORD_LEN_8           0x3/*11*/

/*AFR register.*/
#define UART_V_SHIFT_DLY            5/*tx enable after delay time in 485 duplex.*/
#define UART_V_LEN_DLY              3/*tx enable after delay time in 485 duplex.*/
#define UART_V_DLY_2                2/*delay 2 bit time.*/

#ifndef TRUE
#define TRUE                     1
#endif
#ifndef FALSE
#define FALSE                    0
#endif
//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************

void UARTClearAllReg(void);
void UARTParityModeSet(SERIAL_PARITY ulParity);
void UARTStopBitSet(unsigned char ucStopBits);
void UARTEFREnable(int enable);
void UARTParityModeGet(SERIAL_PARITY *Parity);
void UARTFIFOEnable(int enable, char *FCR_reg);
void UART_FIRO_Rx_Trigger_Level(int level, char *FCR_reg);
void UARTFIFO_reset(char *FCR_reg);
void UARTDMAEnable(int enable, char *FCR_reg);
void UARTSet_FCR(char FCR_reg);
void UARTBaudRateSet(unsigned long ulBaud_rate);
void UARTBaudRateGet(unsigned long *ulBaud_rate);
void UARTDataSizeSet(unsigned char ucDataSize);
void UARTDataSizeGet(unsigned char *ucDataSize);
int UARTCharsAvail(void);
int UARTTxRdy(void);
void UARTCharGet(unsigned char *ucData);
void UARTCharPut(unsigned char ucData);
void UARTIntEnable(int enable);
int UARTIntStatus(SERIAL_INT_TYPE *int_state);
void UARTIntType(SERIAL_INT_TYPE int_type, int enable);
unsigned long UARTRxErrorGet(unsigned long ulBase);
void UARTRxErrorClear(unsigned long ulBase);
void UARTFlowControlSet(unsigned long ulBase, unsigned long ulMode);
unsigned long UARTFlowControlGet(unsigned long ulBase);
unsigned long UARTTxIntModeGet(unsigned long ulBase);
void UART485_half_enable(int enable);
void UARTFIRO_RDY_Enable(int enable);


#endif //  __UART_H__
