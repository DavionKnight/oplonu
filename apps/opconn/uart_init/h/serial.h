//*****************************************************************************
//
// serial.h - Prototypes for the Serial Port Driver
//
// Copyright (c) 2008-2009 Luminary Micro, Inc.  All rights reserved.
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
// This is part of revision 5228 of the RDK-S2E Firmware Package.
//
//*****************************************************************************

#ifndef __SERIAL_H__
#define __SERIAL_H__
#include "serial_ser.h"
#include "serial_option.h"
extern  int receive_flag ;
/*cpld register addr.*/
#define UART_CTL_ADD_VER                0x00/*cpld version.*/
#define UART_CTL_ADD_INT                0x01/*int addr.*/
#define UART_CTL_ADD_CS_WIRE            0x02/*cs and wire mode addr.*/
#define UART_CTL_ADD_RST_RING           0x03/*reset and ring.*/
#define UART_CTL_ADD_CARD_TYPE          0x04/*card type*/
#define UART_CTL_ADD_TX_RX_EN           0x05/*enable tx and rx.*/
#define UART_CTL_ADD_OID_INTERFACE      0x06/*chip oid and 232/485 mode.*/


/*cpld register value.*/


/*reset and type reg.*/
#define UART_CTL_RESET                  0x80/*reset whole uarts*/
#define UART_CTL_RING                   0x40/*ring, 1=ring, 0=silence.*/

/*int addr.*/
#define UART_CTL_V_INT_A                0x01/*0000 0001. */
#define UART_CTL_V_INT_B                0x02/*0000 0010*/
#define UART_CTL_V_INT_C                0x04/*0000 0100*/
#define UART_CTL_V_INT_D                0x08/*0000 1000*/

#define UART_CTL_V_SHIFT_INT            0/*shift of int in cpld*/
#define UART_CTL_V_LEN_INT              4/*length of int in cpld*/
/*end*/

#define UART_CTL_V_SHIFT_TX             4/*shift of tx in cpld*/
#define UART_CTL_V_LEN_TX               4/*length of tx in cpld, 1 enable*/

#define UART_CTL_V_SHIFT_RX             0/*shift of rx in cpld*/
#define UART_CTL_V_LEN_RX               4/*length of rx in cpld, 0 enable*/

/*cs and wire mode addr.*/
/*before read/write uart register, you have to set the cs to the specified value.*/
#define UART_CTL_V_SHIFT_WIRE           4
#define UART_CTL_V_LEN_WIRE             4

#define UART_CTL_V_SHIFT_CS             0
#define UART_CTL_V_LEN_CS               4
/*end*/

#define SERIAL_SPEED_UP_TRIGGER_RX      1
#define SERIAL_DEFAULT_TRIGGER_RX       56

/*irq config*/
#define SERIAL_IRQ_GPIO                 4
#define SERIAL_IRQ_ACTIVE               0
/*end*/

/*chip oid and 232/485 mode.*/
#define UART_CTL_V_SHIFT_OID            4
#define UART_CTL_V_LEN_OID              4

#define UART_CTL_V_SHIFT_INTERFACE      0
#define UART_CTL_V_LEN_INTERFACE        4
/*end*/

/*cpld register end.*/

#define SERIAL_M_Interval_Default       3
#define SERIAL_M_Interval_Speedup       1

#ifndef TRUE
#define TRUE                     1
#endif
#ifndef FALSE
#define FALSE                    0
#endif

typedef enum
{
    SERIAL_INT_TYPE_RX,/*rx data available interrupt.*/
    SERIAL_INT_TYPE_TX,/*thr empty interrupt.*/
    SERIAL_INT_TYPE_LS,/*rx line status interrupt.*/
    SERIAL_INT_TYPE_MS/*modem status interrupt.*/
}SERIAL_INT_TYPE;


extern int Serial_Monitor_interval;
extern Serial_Option serial_232;



//*****************************************************************************
//
// Prototypes for the Serial interface functions.
//
//*****************************************************************************

void Serial_cpld_ver_get(char *version);
void Serial_Cs_Set(unsigned long ulPort);
void Serial_cpld_ver_get(char *version);
void SerialSend(unsigned long ulPort, unsigned char *buf, int len);
void Serial_Send_Mon(void);
void Serial_read();
void SerialSetBaudRate(unsigned long ulPort, unsigned long ulBaudRate);
void SerialSetDataSize(unsigned long ulPort, unsigned char ucDataSize);
void SerialSetParity(unsigned long ulPort, SERIAL_PARITY Parity);
void SerialSetStopBits(unsigned long ulPort, unsigned char ucStopBits);
void SerialSetFlowControl(unsigned long ulPort, SERIAL_FC_MODE ucFlowControl);
void SerialSetDefault(unsigned long ulPort, SERIAL_TYPE serial_type);
void EnableUart(int enable);
void Serial_Uart_ring(void);
void SerialInit(unsigned long ulPort, SERIAL_TYPE serial_type);
void Serial_Uart_485_half(unsigned long ulPort, int enable);
void SerialSetUartInterface(unsigned long ulPort, SERIAL_TYPE s_interface);


#endif // __SERIAL_H__
