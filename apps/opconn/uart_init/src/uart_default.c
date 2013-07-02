//*****************************************************************************
//
// uart.c - Driver for the UART.
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

//*****************************************************************************
//
//! \addtogroup uart_api
//! @{
//
//*****************************************************************************

#include "test.h"
#include "serial_ser.h"
#include "serial.h"
#include "uart_default.h"
#include <stdio.h>



/*��offsetΪ��λ��ַ��len���ȵ�bitȫ������*/
int UART_Bits_Clear(unsigned char offset, unsigned char len, char *value)
{
    char ucMask = 0;/*��һ�����룬ָ��λΪ0������Ϊ1*/
    char i;
    char bit_set = 1;
    
    if (NULL == value)
        return ERROR;

    if (offset + len > 8)
        return ERROR;

    for(i = 0; i <= 8; i++)
    {
        if ((i >= offset + len) || (i < offset))
        {
            ucMask |= bit_set<<i;
        }
    }

    *value &= ucMask;
    return OK;
}

/*��offsetΪ�͵�ַ��len���ȵ�bits�����������ֽ�����*/
int UART_Bits_Remain(unsigned char offset, unsigned char len, char *value)
{
    char temp_value = 0;/*��һ�����룬ָ��λΪ1������Ϊ0*/
    char i;
    char bit_set = 1;

    if (NULL == value)
        return ERROR;

    if (offset + len > 8)
        return ERROR;

    for(i = 0; i<= 8; i++)
    {
        if ((i >= offset) && (i < offset + len))
        {
            temp_value |= (bit_set<<i);
        }
    }

    *value &= temp_value;
    return OK;
}

void UARTClearAllReg(void)
{
	char setData;
    char LCR_reg;
    char IER_reg;
    char FCR_reg;
    char MCR_reg;
    char SPR_reg;
    char DLL_reg;
    char DLH_reg;
    char AFR_reg;
    char EFR_reg;
    char Xon1_reg;
    char Xon2_reg;
    char Xoff1_reg;
    char Xoff2_reg;
    char TCR_reg;
    char TLR_reg;

    LCR_reg = UART_V_CONFIG_AFR;
    cpldcs2Write(UART_ADD_LCR, LCR_reg);

    /*clear AFR*/
    AFR_reg = 0;
    cpldcs2Write(UART_ADD_AFR, AFR_reg);

    LCR_reg = UART_V_CONFIG_EF;
    cpldcs2Write(UART_ADD_LCR, LCR_reg);

    EFR_reg = 0;
    EFR_reg |= UART_V_ENHANCE_FUN;
    cpldcs2Write(UART_ADD_EFR, EFR_reg);

    LCR_reg = 0;
    cpldcs2Write(UART_ADD_LCR, LCR_reg);

    MCR_reg = 0;
    MCR_reg |= UART_V_TCR_TLR_EN;
    cpldcs2Write(UART_ADD_MCR, MCR_reg);

    /*clear TCR*/
    TCR_reg = 0;
    cpldcs2Write(UART_ADD_TCR, TCR_reg);

    /*clear TLR*/
    TLR_reg = 0;
    cpldcs2Write(UART_ADD_TLR, TLR_reg);

    /*clear MCR*/
    MCR_reg = 0;
    cpldcs2Write(UART_ADD_MCR, MCR_reg);

    /*clear IER*/
    IER_reg = 0;
    cpldcs2Write(UART_ADD_IER, IER_reg);

    /*clear FCR*/
    FCR_reg = 0;
    cpldcs2Write(UART_ADD_FCR, FCR_reg);

    /*clear SPR*/
    SPR_reg = 0;
    cpldcs2Write(UART_ADD_SPR, SPR_reg);

    LCR_reg = UART_V_CONFIG_EF;
    cpldcs2Write(UART_ADD_LCR, LCR_reg);

    /*clear EFR*/
    EFR_reg = 0;
    cpldcs2Write(UART_ADD_EFR, EFR_reg);

    /*clear Xon1*/
    Xon1_reg = 0;
    cpldcs2Write(UART_ADD_Xon1, Xon1_reg);

    /*clear Xon2*/
    Xon2_reg = 0;
    cpldcs2Write(UART_ADD_Xon2, Xon2_reg);

    /*clear Xoff1*/
    Xoff1_reg = 0;
    cpldcs2Write(UART_ADD_Xoff1, Xoff1_reg);

    /*clear Xoff2*/
    Xoff2_reg = 0;
    cpldcs2Write(UART_ADD_Xoff2, Xoff2_reg);//modified by zhangjj

    /*clear DLL*/
    DLL_reg = 0;
    cpldcs2Write(UART_ADD_DLL, DLL_reg);

    /*clear DLH*/
    DLH_reg = 0;
    cpldcs2Write(UART_ADD_DLH, DLH_reg);

    /*clear LCR*/
    LCR_reg = 0;
    cpldcs2Write(UART_ADD_LCR, LCR_reg);
}

//*****************************************************************************
//
//! Sets the type of parity.
//!
//! \param ulBase is the base address of the UART port.
//! \param ulParity specifies the type of parity to use.
//!
//! Sets the type of parity to use for transmitting and expect when receiving.
//! The \e ulParity parameter must be one of \b UART_CONFIG_PAR_NONE,
//! \b UART_CONFIG_PAR_EVEN, \b UART_CONFIG_PAR_ODD, \b UART_CONFIG_PAR_ONE,
//! or \b UART_CONFIG_PAR_ZERO.  The last two allow direct control of the
//! parity bit; it will always be either be one or zero based on the mode.
//!
//! \return None.
//
//*****************************************************************************
void UARTParityModeSet(SERIAL_PARITY ulParity)
{
    unsigned char LCR_reg;
    
    cpldcs2Read(UART_ADD_LCR, &LCR_reg);
    
    //
    // Update the configuration with a new parity.
    //
    switch(ulParity)
    {
        case SERIAL_PARITY_NONE:
        {
            LCR_reg &= ~UART_V_PARITY_EN;
            break;
        }

        case SERIAL_PARITY_ODD:
        {
            LCR_reg |= UART_V_PARITY_EN;
            LCR_reg &= ~UART_V_PARITY_TYPE;
            LCR_reg &= ~UART_V_PARITY_FORCE;
            break;
        }

        case SERIAL_PARITY_EVEN:
        {
            LCR_reg |= UART_V_PARITY_EN;
            LCR_reg |= UART_V_PARITY_TYPE;
            LCR_reg &= ~UART_V_PARITY_FORCE;
            break;
        }

        case SERIAL_PARITY_MARK:
        {
            LCR_reg |= UART_V_PARITY_EN;
            LCR_reg &= ~UART_V_PARITY_TYPE;
            LCR_reg |= UART_V_PARITY_FORCE;
            break;
        }

        case SERIAL_PARITY_SPACE:
        {
            LCR_reg |= UART_V_PARITY_EN;
            LCR_reg |= UART_V_PARITY_TYPE;
            LCR_reg |= UART_V_PARITY_FORCE;
            break;
        }
        default:
        {
            printf("error\n");
            break;
        }
    }

    cpldcs2Write(UART_ADD_LCR, LCR_reg);
}

void UARTStopBitSet(unsigned char ucStopBits)
{
    unsigned char LCR_reg;

    cpldcs2Read(UART_ADD_LCR, &LCR_reg);

    //
    // Update the configuration with a new stop bits.
    //
    switch(ucStopBits)
    {
        case 1:
        {
            LCR_reg &= ~UART_V_STOP_BITS;
            break;
        }

        case 2:
        {
            LCR_reg |= UART_V_STOP_BITS;
            break;
        }

        default:
        {
            printf("error\n");
            return;
        }
    }
    cpldcs2Write(UART_ADD_LCR, LCR_reg);
}

void UARTStopBitGet(unsigned char *ucStopBits)
{
}

UARTFcSet(ulPort, ucFlowControl)
{
    
}

//*****************************************************************************
//
//! Gets the type of parity currently being used.
//!
//! \param ulBase is the base address of the UART port.
//!
//! This function gets the type of parity used for transmitting data, and
//! expected when receiving data.
//!
//! \return Returns the current parity settings, specified as one of
//! \b UART_CONFIG_PAR_NONE, \b UART_CONFIG_PAR_EVEN, \b UART_CONFIG_PAR_ODD,
//! \b UART_CONFIG_PAR_ONE, or \b UART_CONFIG_PAR_ZERO.
//
//*****************************************************************************
void UARTParityModeGet(SERIAL_PARITY *Parity)
{

}

void UARTEFREnable(int enable)
{
    char LCR_reg;
    char EFR_reg;
    
    char LCR_reg_temp;
    cpldcs2Read(UART_ADD_LCR, &LCR_reg);

    LCR_reg_temp = UART_V_CONFIG_EF;

    cpldcs2Write(UART_ADD_LCR, LCR_reg);

    cpldcs2Read(UART_ADD_EFR, &EFR_reg);

    if (TRUE == enable)
    {
        EFR_reg |= UART_V_ENHANCE_FUN;
    }
    else
    {
        EFR_reg &= ~UART_V_ENHANCE_FUN;
    }

    cpldcs2Write(UART_ADD_EFR, EFR_reg);

    cpldcs2Write(UART_ADD_LCR, LCR_reg);
}



/*�����Ĵ��������������жϺ���Ӱ�졣*/
void UARTFIRO_RDY_Enable(int enable)
{
    char MCR_reg;
    cpldcs2Read(UART_ADD_MCR, &MCR_reg);

    if (TRUE == enable)
    {
        MCR_reg |= UART_V_FIFORDY_EN;
    }
    else
    {
        MCR_reg &= ~UART_V_FIFORDY_EN;
    }
//printf("MCR is 0x%x\n",MCR_reg);
    cpldcs2Write(UART_ADD_MCR, MCR_reg);
}

//*****************************************************************************
//
//! Enables the transmit and receive FIFOs.
//!
//! \param ulBase is the base address of the UART port.
//!
//! This functions enables the transmit and receive FIFOs in the UART.
//!
//! \return None.
//
//*****************************************************************************
void UARTFIFOEnable(int enable, char *FCR_reg)
{
    if (TRUE == enable)
    {
        *FCR_reg |= UART_V_EN_FIFO;
    }
    else
    {
        *FCR_reg &= ~UART_V_EN_FIFO;
    }
}

void UART_FIRO_Rx_Trigger_Level(int level, char *FCR_reg)
{
    char uart_level;

    switch (level)
    {
        case 1:
            uart_level = UART_V_RX_TRI_1C;
            break;
        case 4:
            uart_level = UART_V_RX_TRI_4C;
            break;
        case 56:
            uart_level = UART_V_RX_TRI_56C;
            break;
        case 60:
            uart_level = UART_V_RX_TRI_60C;
            break;
        default:
            printf("error\n");
            return;
    }

    if (OK != UART_Bits_Clear(UART_V_SHIFT_RX_TRI, UART_V_LEN_RX_TRI, FCR_reg))
    {
        printf("error\n");
    }

    *FCR_reg |= (uart_level<<UART_V_SHIFT_RX_TRI);
}

//*****************************************************************************
//
//! Enable UART DMA operation.
//!
//! \param ulBase is the base address of the UART port.
//! \param ulDMAFlags is a bit mask of the DMA features to enable.
//!
//! The specified UART DMA features are enabled.  The UART can be
//! configured to use DMA for transmit or receive, and to disable
//! receive if an error occurs.  The \e ulDMAFlags parameter is the
//! logical OR of any of the following values:
//!
//! - UART_DMA_RX - enable DMA for receive
//! - UART_DMA_TX - enable DMA for transmit
//! - UART_DMA_ERR_RXSTOP - disable DMA receive on UART error
//!
//! \note The uDMA controller must also be set up before DMA can be used
//! with the UART.
//!
//! \return None.
//
//*****************************************************************************
void UARTDMAEnable(int enable, char *FCR_reg)
{
    if (TRUE == enable)
    {
        *FCR_reg |= UART_V_DMA_MODE;
    }
    else
    {
        *FCR_reg &= ~UART_V_DMA_MODE;
    }
}

void UARTFIFO_reset(char *FCR_reg)
{
    *FCR_reg |= UART_V_RESET_RX_FIFO;
    *FCR_reg |= UART_V_RESET_TX_FIFO;
}


/*fcr register is write only, must write it at one time.*/
void UARTSet_FCR(char FCR_reg)
{
    cpldcs2Write(UART_ADD_FCR, FCR_reg);
}

void UARTBaudRateSet(unsigned long ulBaud_rate)
{
    unsigned int baud_hi;
    unsigned int baud_lo;

    unsigned char LCR;
    unsigned char LCR_temp;

    unsigned char DLL;
    unsigned char DLH;

    unsigned int frequency;/*��Ƶ��*/
    unsigned int frequency_revise;/*�����Ա�����������ϳ����������⡣*/

    /*��Ҫ��������*/
    frequency = (BAUD_MAX_FREQUENCY*2 + ulBaud_rate)/(ulBaud_rate*2);

    /*752C����֪bug�����մ������ʱ�����������ݻ�ʶ�����ҡ�*/
    /*�߲�������ͬ�������⣬����ֻ�ܿ����ֹͣλ������*/
    /*��Ҫ�������룬�����߲������¿��ܼ���1.*/
    frequency_revise = (frequency*2 + UART_FREQUENCY_DIVISOR)/(UART_FREQUENCY_DIVISOR*2);
    
    frequency -= frequency_revise;

    baud_hi = ((frequency>>8)&0xff);
    baud_lo = (frequency&0xff);

printf("baud_hi is 0x%x  lo is 0x%x...============\n");

    cpldcs2Read(UART_ADD_LCR, &LCR);

    LCR_temp = LCR;
    LCR_temp |= UART_V_DLAB_EFR_EN;

    cpldcs2Write(UART_ADD_LCR, LCR_temp);
    cpldcs2Write(UART_ADD_DLL, baud_lo);
    cpldcs2Write(UART_ADD_DLH, baud_hi);
    cpldcs2Write(UART_ADD_LCR, LCR);
}

void UARTBaudRateGet(unsigned long *ulBaud_rate)
{

}

void UARTDataSizeSet(unsigned char ucDataSize)
{
    unsigned char word_len_reg;
    unsigned char LCR_reg;
    
    //
    // Update the configuration with a new data length.
    //
     switch(ucDataSize)
    {
        case 5:
        {
            word_len_reg = UART_V_WORD_LEN_5;
            break;
        }

        case 6:
        {
            word_len_reg = UART_V_WORD_LEN_6;
            break;
        }

        case 7:
        {
            word_len_reg = UART_V_WORD_LEN_7;
            break;
        }

        case 8:
        {
            word_len_reg = UART_V_WORD_LEN_8;
            break;
        }

        default:
        {
            printf("error\n");
            return;
        }
    }

    cpldcs2Read(UART_ADD_LCR, &LCR_reg);
    
    if (OK != UART_Bits_Clear(UART_V_SHIFT_WORD_LEN, UART_V_LEN_WORD_LEN, &LCR_reg))
    {
        printf("bits clear error\n");
    }

    LCR_reg |= (word_len_reg<<UART_V_SHIFT_WORD_LEN);

    cpldcs2Write(UART_ADD_LCR, LCR_reg);
}

void UARTDataSizeGet(unsigned char *ucDataSize)
{
}

//*****************************************************************************
//
//! Sets the UART hardware flow control mode to be used.
//!
//! \param ulBase is the base address of the UART port.
//! \param ulMode indicates the flow control modes to be used.  This is a
//! logical OR combination of values \b UART_FLOWCONTROL_TX and \b
//! UART_FLOWCONTROL_RX to enable hardware transmit (CTS) and receive (RTS)
//! flow control or \b UART_FLOWCONTROL_NONE to disable hardware flow control.
//!
//! Sets the required hardware flow control modes.  If \e ulMode contains
//! flag \b UART_FLOWCONTROL_TX, data is only transmitted if the incoming CTS
//! signal is asserted. If \e ulMode contains flag \b UART_FLOWCONTROL_RX,
//! the RTS output is controlled by the hardware and is asserted only when
//! there is space available in the receive FIFO.  If no hardware flow control
//! is required, UART_FLOWCONTROL_NONE should be passed.
//!
//! \note The availability of hardware flow control varies with the Stellaris
//! part and UART in use.  Please consult the datasheet for the part you are
//! using to determine whether this support is available.
//!
//! \return None.
//
//*****************************************************************************
void
UARTFlowControlSet(unsigned long ulBase, unsigned long ulMode)
{

}

//*****************************************************************************
//
//! Returns the UART hardware flow control mode currently in use.
//!
//! \param ulBase is the base address of the UART port.
//!
//! Returns the current hardware flow control mode.
//!
//! \note The availability of hardware flow control varies with the Stellaris
//! part and UART in use.  Please consult the datasheet for the part you are
//! using to determine whether this support is available.
//!
//! \return Returns the current flow control mode in use.  This is a
//! logical OR combination of values \b UART_FLOWCONTROL_TX if transmit
//! (CTS) flow control is enabled and \b UART_FLOWCONTROL_RX if receive (RTS)
//! flow control is in use.  If hardware flow control is disabled, \b
//! UART_FLOWCONTROL_NONE will be returned.
//
//*****************************************************************************
unsigned long
UARTFlowControlGet(unsigned long ulBase)
{

}

//*****************************************************************************
//
//! Returns the current operating mode for the UART transmit interrupt.
//!
//! \param ulBase is the base address of the UART port.
//!
//! This function returns the current operating mode for the UART transmit
//! interrupt.  The return value will be \b UART_TXINT_MODE_EOT if the
//! transmit interrupt is currently set to be asserted once the transmitter is
//! completely idle - the transmit FIFO is empty and all bits, including any
//! stop bits, have cleared the transmitter.  The return value will be \b
//! UART_TXINT_MODE_FIFO if the interrupt is set to be asserted based upon the
//! level of the transmit FIFO.
//!
//! \note The availability of end-of-transmission mode varies with the
//! Stellaris part in use.  Please consult the datasheet for the part you are
//! using to determine whether this support is available.
//!
//! \return Returns \b UART_TXINT_MODE_FIFO or \b UART_TXINT_MODE_EOT.
//
//*****************************************************************************
unsigned long
UARTTxIntModeGet(unsigned long ulBase)
{

}

//*****************************************************************************
//
//! Determines if there are any characters in the receive FIFO.
//!
//! \param ulBase is the base address of the UART port.
//!
//! This function returns a flag indicating whether or not there is data
//! available in the receive FIFO.
//!
//! \return Returns \b true if there is data in the receive FIFO, and \b false
//! if there is no data in the receive FIFO.
//
//*****************************************************************************
int
UARTCharsAvail(void)
{
    unsigned char LSR_reg;

    cpldcs2Read(UART_ADD_LSR, &LSR_reg);

    if (0 != (LSR_reg&UART_V_DATA_REV))
    {
        return OK;
    }
    else
    {
        return ERROR;
    }
}

int UARTTxRdy(void)
{
    char LSR_reg;
    char compare_reg;

    cpldcs2Read(UART_ADD_LSR, &LSR_reg);

    if (0 != (LSR_reg&UART_V_THR_EMPTY))
    {
        return OK;
    }
    else
    {
        return ERROR;
    }
    
    return OK;
}

//*****************************************************************************
//
//! Waits for a character from the specified port.
//!
//! \param ulBase is the base address of the UART port.
//!
//! Gets a character from the receive FIFO for the specified port.  If there
//! are no characters available, this function will wait until a character is
//! received before returning.
//!
//! \return Returns the character read from the specified port, cast as an
//! \e long.
//
//*****************************************************************************
void
UARTCharGet(unsigned char *ucData)
{
    char RHR_reg;

    cpldcs2Read(UART_ADD_RHR, &RHR_reg);

    *ucData = RHR_reg;
    return;
}

//*****************************************************************************
//
//! Waits to send a character from the specified port.
//!
//! \param ulBase is the base address of the UART port.
//! \param ucData is the character to be transmitted.
//!
//! Sends the character \e ucData to the transmit FIFO for the specified port.
//! If there is no space available in the transmit FIFO, this function will
//! wait until there is space available before returning.
//!
//! \return None.
//
//*****************************************************************************
void
UARTCharPut(unsigned char ucData)
{
    char THR_reg;
	char data;

    THR_reg = ucData;

    cpldcs2Write(UART_ADD_THR, THR_reg);
    return;
}

//*****************************************************************************
//
//! Enables individual UART interrupt sources.
//!
//! \param ulBase is the base address of the UART port.
//! \param ulIntFlags is the bit mask of the interrupt sources to be enabled.
//!
//! Enables the indicated UART interrupt sources.  Only the sources that are
//! enabled can be reflected to the processor interrupt; disabled sources have
//! no effect on the processor.
//!
//! The \e ulIntFlags parameter is the logical OR of any of the following:
//!
//! - \b UART_INT_OE - Overrun Error interrupt
//! - \b UART_INT_BE - Break Error interrupt
//! - \b UART_INT_PE - Parity Error interrupt
//! - \b UART_INT_FE - Framing Error interrupt
//! - \b UART_INT_RT - Receive Timeout interrupt
//! - \b UART_INT_TX - Transmit interrupt
//! - \b UART_INT_RX - Receive interrupt
//! - \b UART_INT_DSR - DSR interrupt
//! - \b UART_INT_DCD - DCD interrupt
//! - \b UART_INT_CTS - CTS interrupt
//! - \b UART_INT_RI - RI interrupt
//!
//! \return None.
//
//*****************************************************************************
void UARTIntType(SERIAL_INT_TYPE int_type, int enable)
{
    char type;
    
    char IER_reg;

    switch (int_type)
    {
        case SERIAL_INT_TYPE_RX:
            type = UART_V_INT_RX_AVI_EN;
            break;

        case SERIAL_INT_TYPE_TX:
            type = UART_V_INT_THR_EMP_EN;
            break;

        case SERIAL_INT_TYPE_LS:
            type = UART_V_INT_LINE_STA_EN;
            break;

        case SERIAL_INT_TYPE_MS:
            type = UART_V_INT_MOD_STA_EN;
            break;

        default:
            printf("uartIntType error\n");
        return;
    }

    cpldcs2Read(UART_ADD_IER, &IER_reg);

    if (TRUE == enable)
    {
        IER_reg |= type;
    }
    else
    {
        IER_reg &= ~type;
    }

    cpldcs2Write(UART_ADD_IER, IER_reg);
}

void UART485_half_enable(int enable)
{
    char AFR_reg;
    char LCR_reg;
    char AFR_DLY_value;
    
    cpldcs2Read(UART_ADD_LCR, &LCR_reg);
    cpldcs2Write(UART_ADD_LCR, UART_V_CONFIG_AFR);

    cpldcs2Read(UART_ADD_AFR, &AFR_reg);

    if (TRUE == enable)
    {
        AFR_reg |= UART_V_485EN;
        AFR_reg |= UART_V_485LG;
        AFR_reg |= UART_V_RCVEN;
        
        if (OK != UART_Bits_Clear(UART_V_SHIFT_DLY, UART_V_LEN_DLY, &AFR_reg))
        {
            printf("bits clear error\n");
        }
        
        AFR_DLY_value = UART_V_DLY_2;

        AFR_reg |= (AFR_DLY_value<<UART_V_SHIFT_DLY);
    }
    else
    {
        AFR_reg &= ~UART_V_485EN;
        AFR_reg &= ~UART_V_485LG;
        AFR_reg &= ~UART_V_RCVEN;
        
        if (OK != UART_Bits_Clear(UART_V_SHIFT_DLY, UART_V_LEN_DLY, &AFR_reg))
        {
            printf("bits clear error\n");
        }
    }
    
    cpldcs2Write(UART_ADD_AFR, AFR_reg);

    cpldcs2Write(UART_ADD_LCR, LCR_reg);
}

void UARTRxTimeOutEn(int enable)
{
    char AFR_reg;
    char LCR_reg;
    
    cpldcs2Read(UART_ADD_LCR, &LCR_reg);
    cpldcs2Write(UART_ADD_LCR, UART_V_CONFIG_AFR);

    cpldcs2Read(UART_ADD_AFR, &AFR_reg);

    if (TRUE == enable)
    {
        AFR_reg |= UART_V_RCVEN;
    }
    else
    {
        AFR_reg &= ~UART_V_RCVEN;
    }
    
    cpldcs2Write(UART_ADD_AFR, AFR_reg);

    cpldcs2Write(UART_ADD_LCR, LCR_reg);
}

void UARTIntEnable(int enable)
{
    char MCR_reg;
    cpldcs2Read(UART_ADD_MCR, &MCR_reg);
//printf("00MCR is 0x%x\n",MCR_reg);
    if (TRUE == enable)
    {
        MCR_reg |= UART_V_IRQ_EN;
    }
    else
    {
        MCR_reg &= ~UART_V_IRQ_EN;
    }
//printf("00MCR is 0x%x\n",MCR_reg);
    cpldcs2Write(UART_ADD_MCR, MCR_reg);
}

//*****************************************************************************
//
//! Gets the current interrupt status.
//!
//! \param ulBase is the base address of the UART port.
//! \param bMasked is false if the raw interrupt status is required and true
//! if the masked interrupt status is required.
//!
//! This returns the interrupt status for the specified UART.  Either the raw
//! interrupt status or the status of interrupts that are allowed to reflect to
//! the processor can be returned.
//!
//! \return Returns the current interrupt status, enumerated as a bit field of
//! values described in UARTIntEnable().
//
//*****************************************************************************
int
UARTIntStatus(SERIAL_INT_TYPE *int_state)
{
    char IIR_reg;
    char IIR_reg_for_com;
    
    cpldcs2Read(UART_ADD_IIR, &IIR_reg);

    if (NULL == int_state)
    {
        printf("uartIntStatus error\n");
        return ERROR;
    }

    if (0 != (IIR_reg&UART_V_INT_STA))/*no interrupt found.*/
    {
        return ERROR;/*no interrupt.*/
    }

    IIR_reg_for_com = IIR_reg;

    if (OK != UART_Bits_Remain(UART_V_SHIFT_INT_BIT, UART_V_LEN_INT_BIT, &IIR_reg_for_com))
    {
        printf("uart bits remain error\n");
        return OK;
    }

    if (UART_V_INT_REV_LINE_STA<<UART_V_SHIFT_INT_BIT == IIR_reg_for_com)
    {
        *int_state = SERIAL_INT_TYPE_LS;
    }
    else if (UART_V_INT_MODEM_STA<<UART_V_SHIFT_INT_BIT == IIR_reg_for_com)
    {
        *int_state = SERIAL_INT_TYPE_MS;
    }
    else if (UART_V_INT_THR_STA<<UART_V_SHIFT_INT_BIT == IIR_reg_for_com)
    {
        *int_state = SERIAL_INT_TYPE_TX;
    }
    else if ((UART_V_INT_RX_TIMEOUT_STA<<UART_V_SHIFT_INT_BIT == IIR_reg_for_com)
            || (UART_V_INT_RHR_STA<<UART_V_SHIFT_INT_BIT == IIR_reg_for_com))
    {
        *int_state = SERIAL_INT_TYPE_RX;
    }
    else
    {
        printf("uartIntStatus error\n");
    }
    return OK;
    
}


//*****************************************************************************
//
//! Gets current receiver errors.
//!
//! \param ulBase is the base address of the UART port.
//!
//! This function returns the current state of each of the 4 receiver error
//! sources.  The returned errors are equivalent to the four error bits
//! returned via the previous call to UARTCharGet() or UARTCharGetNonBlocking()
//! with the exception that the overrun error is set immediately the overrun
//! occurs rather than when a character is next read.
//!
//! \return Returns a logical OR combination of the receiver error flags,
//! \b UART_RXERROR_FRAMING, \b UART_RXERROR_PARITY, \b UART_RXERROR_BREAK
//! and \b UART_RXERROR_OVERRUN.
//
//*****************************************************************************
unsigned long
UARTRxErrorGet(unsigned long ulBase)
{

}

//*****************************************************************************
//
//! Clears all reported receiver errors.
//!
//! \param ulBase is the base address of the UART port.
//!
//! This function is used to clear all receiver error conditions reported via
//! UARTRxErrorGet().  If using the overrun, framing error, parity error or
//! break interrupts, this function must be called after clearing the interrupt
//! to ensure that later errors of the same type trigger another interrupt.
//!
//! \return None.
//
//*****************************************************************************
void
UARTRxErrorClear(unsigned long ulBase)
{

}

