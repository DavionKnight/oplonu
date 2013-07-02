#include <stdio.h>
#include "test.h"
#include "serial_ser.h"
#include "serial.h"
#include "uart_default.h"
#include "ringbuf.h"
#include "serial_option.h"
#include "cs_types.h"
#include "terminal_server.h"
#include "osal_api_core.h"

//*****************************************************************************
//
//! The buffer used to hold characters received from the UART.
//
//*****************************************************************************
static unsigned char g_pucRXBuffer[MAX_S2E_PORTS][RING_BUF_SIZE_RX];

//*****************************************************************************
//
//! The ring buffers used to hold characters received from the UARTs.
//
//*****************************************************************************
static tRingBufObject g_sRxBuf[MAX_S2E_PORTS];


//*****************************************************************************
//
//! The buffer used to hold characters to be sent to the UART.
//
//*****************************************************************************
static unsigned char g_pucTXBuffer[MAX_S2E_PORTS][RING_BUF_SIZE_TX];

//*****************************************************************************
//
//! The ring buffers used to hold characters to be sent to the UARTs.
//
//*****************************************************************************
static tRingBufObject g_sTxBuf[MAX_S2E_PORTS];

int receive_flag = 0;

int Serial_Monitor_interval = SERIAL_M_Interval_Default;

void Serial_cpld_ver_get(char *version)
{
    cpldcs2Read(UART_CTL_ADD_VER, version);
    return;
}

void Serial_Cs_Set(unsigned long ulPort)
{
//printf("serial_cs_set start ...\n");
    unsigned char reg_value;
    unsigned char CS_shift;

    CS_shift = ulPort;

    cpldcs2Read(UART_CTL_ADD_CS_WIRE, &reg_value);
//printf("reg_value is 0x%x\n",reg_value);
    if (0 != (reg_value & ((1<<UART_CTL_V_SHIFT_CS)<<CS_shift)))
    {
        return;
    }

    if (OK != UART_Bits_Clear(UART_CTL_V_SHIFT_CS, UART_CTL_V_LEN_CS, &reg_value))
    {
        printf("assert before\n");
    }
    reg_value |= (1<<UART_CTL_V_SHIFT_CS)<<CS_shift;

    cpldcs2Write(UART_CTL_ADD_CS_WIRE, reg_value);

    return;
}
int SerialTxRdy(unsigned long ulPort)
{
    Serial_Cs_Set(ulPort);

    if (OK == UARTTxRdy())
    {
        return OK;
    }
    else
    {
        return ERROR;
    }
}

int SerialIntSource(unsigned long *ulPort)
{
//printf("11aaaaaaaa\n");
    unsigned char reg_value;
    
    cpldcs2Read(UART_CTL_ADD_INT, &reg_value);
//printf("INT value is 0x%x\n",reg_value);
    UART_Bits_Remain(UART_CTL_V_SHIFT_INT, UART_CTL_V_LEN_INT, &reg_value);
//printf("after remain INT value is 0x%x\n",reg_value);
    if (0 != (reg_value&UART_CTL_V_INT_D))
    {
        *ulPort = 3;/*port 3*/
    }
    else if (0 != (reg_value&UART_CTL_V_INT_C))
    {
        *ulPort = 2;
    }
    else if (0 != (reg_value&UART_CTL_V_INT_B))
    {
        *ulPort = 1;
    }
    else if (0 != (reg_value&UART_CTL_V_INT_A))
    {
        *ulPort = 0;
    }
    else
    {
        //S2E_DEBUG(("\r\n after get interrupt, no int found in cpld.\r\n"));
        return ERROR;
    }

    return OK;
}

void UartRing(int enable)
{
    char Ring_reg;
    cpldcs2Read(UART_CTL_ADD_RST_RING, &Ring_reg);

    if (TRUE == enable)
    {
        Ring_reg |= UART_CTL_RING;
    }
    else
    {
        Ring_reg &= ~UART_CTL_RING;
    }
    cpldcs2Write(UART_CTL_ADD_RST_RING, Ring_reg);
}


void Serial_Uart_ring(void)
{
    UartRing(TRUE);
    
    sleep(1);
    
    UartRing(FALSE);
}


void SerialInit(unsigned long ulPort, SERIAL_TYPE serial_type)
{

	RingBufInit(&g_sTxBuf[ulPort], g_pucTXBuffer[ulPort], sizeof(g_pucTXBuffer[ulPort]));
	RingBufInit(&g_sRxBuf[ulPort], g_pucRXBuffer[ulPort], sizeof(g_pucRXBuffer[ulPort]));
    	
	EnableUart(TRUE);
 
 	SerialSetDefault(ulPort, serial_type);
	
//	printf("serialInit  end...\n");
}
#if 1
void
SerialSend(unsigned long ulPort, unsigned char *buf, int len)
{
    int byte_to_process = 0;
    unsigned int i;
    char UART_TX_RX_reg;
 
    pthread_mutex_lock(&s2e_serial_sem);
    /*之所以先向ringbuf里面写，是因为不能完全否定THR中断被湮没的可能性*/
     while(!RingBufFull(&g_sTxBuf[ulPort]))
    {

        if (byte_to_process >= len)
            break;

        RingBufWriteOne(&g_sTxBuf[ulPort], buf[byte_to_process]);

        byte_to_process++;
    }

 
    if (OK == SerialTxRdy(ulPort))
    {

        for(i = 0; ((!RingBufEmpty(&g_sTxBuf[ulPort])) && (i < TX_FIFO_LEN)); i++)
        {
            //
            // Write the next character into the transmit FIFO.
 
            UARTCharPut(RingBufReadOne(&g_sTxBuf[ulPort]));
 
        }
    }
    pthread_mutex_unlock(&s2e_serial_sem);
}
#else
void
SerialSend(unsigned long ulPort, unsigned char *buf, int len)
{
    int byte_to_process = 0;
    unsigned int i=0;
    char UART_TX_RX_reg;
unsigned char test='a';
    pthread_mutex_lock(&s2e_serial_sem);
    /*之所以先向ringbuf里面写，是因为不能完全否定THR中断被湮没的可能性*/
     while(!RingBufFull(&g_sTxBuf[ulPort]))
    {

        if (byte_to_process >= len)
            break;


        RingBufWriteOne(&g_sTxBuf[ulPort], buf[byte_to_process]);

        byte_to_process++;
    }

 
 while(!RingBufEmpty(&g_sTxBuf[ulPort])) 
        {
            //
            // Write the next character into the transmit FIFO.
            if((OK == SerialTxRdy(ulPort)))
            {
            	UARTCharPut(RingBufReadOne(&g_sTxBuf[ulPort]));
            }
        }
 
    pthread_mutex_unlock(&s2e_serial_sem);

}

#endif
/*used to inform IP layer one UART packet received */
extern volatile int msgq_ts_uart_rx;
/* used to inform UART one IP packet received */
extern volatile int msgq_ts_net_rx;
//#define    TS_START_A
void Serial_Read_Rx_ringbuf(void)
{
//printf("Serial_Read_Rx_ringbuf start...\n");
    unsigned char ucChar[S2E_SERIAL_RX_MAX_LEN];
    unsigned int data_len;
    unsigned long ulPort;
    unsigned long ringbuf_avi;
unsigned int i=0;
#ifdef TS_START_A
			struct timeval tv_current_time;
   			cs_uint8  term_id ;
			cs_uint8 *buffer = NULL;
			cs_uint32  offset = 0;
			static uart_msg_t uart_msg;
			uart_msg_t *msg = &uart_msg;
#endif
    while (1)
    {
         pthread_mutex_lock(&s2e_serial_rx_sync_sem);
//		printf("serial_port_num is %d\n",S2E_Control.serial_port_num);
        for (ulPort = 0; ulPort < S2E_Control.serial_port_num; ulPort++)
        {
            while(!RingBufEmpty(&g_sRxBuf[ulPort]))
            {
                if (S2E_SERIAL_RX_MAX_LEN < (ringbuf_avi = RingBufUsed(&g_sRxBuf[ulPort])))
                {
                    data_len = S2E_SERIAL_RX_MAX_LEN;
                }
                else
                {
                    data_len = ringbuf_avi;
                }

                RingBufRead(&g_sRxBuf[ulPort], ucChar, data_len);
	#ifdef    TS_START_A

			term_server_config_t *pTsCfg = TS_CFG_GET(ulPort+1);

			term_id = UART_TO_TERM_SERV(ulPort+1);
			if((ts_uart_ring[term_id].wr_pos+51) % TERM_SRV_UART_MAX_SIZE ==ts_uart_ring[term_id].rd_pos){
				buffer = NULL;
				printf("UART [%d] full. wrpos %d ,  rdpos %d ,  wrcount  %d ,rdcount  %d \n",ulPort,
					ts_uart_ring[term_id].wr_pos, ts_uart_ring[term_id].rd_pos,
					ts_uart_ring[term_id].wr_count,ts_uart_ring[term_id].rd_count);

			}
			else{
				buffer = (cs_uint8 *)&ts_uart_ring[term_id].data[0];
				offset = ts_uart_ring[term_id].wr_pos;
			}
#endif			
		if(receive_flag==1)
		{
			while(i<data_len)
			{

#ifdef 	TS_START_A
				if(buffer){
					buffer[offset] = ucChar[i];
					pTsCfg->uart_rx_char_cnt++;
					ts_uart_ring[term_id].wr_count++;
					offset = (offset + 1)%TERM_SRV_UART_MAX_SIZE;
				}
#endif
//			printf("%c",ucChar[i]);
//			i++;
		}
		printf("data len is %d\n",data_len);
		i=0;
#ifdef TS_START_A
			ts_uart_ring[term_id].wr_pos = offset;
			if(TS_ENABLED(term_id))
			{
			gettimeofday(&tv_current_time,NULL);
				pTsCfg->last_act_ticks.tv_sec= tv_current_time.tv_sec;
				if(ts_uart_ring[term_id].wr_count - ts_uart_ring[term_id].rd_count >0){
//			printf("UART[%d] send message to tx thread  \n",ulPort);
					vosMsqSend(msgq_ts_uart_rx,(void *)&msg,sizeof(cs_uint32));
				}
			}
#endif
		}
                data_len = 0;
            }
        }
//	printf("befor unlock...%d\n",s2e_serial_rx_sync_sem);
    }
	usleep(10);
}


void Serial_read()
{
char ucChar[100]="";
int i=0,j=0;
char setData;
SERIAL_INT_TYPE int_Status;
	while(1)
	{
	 if (OK == UARTIntStatus(&int_Status))
	 	{
	 	printf("status is 0x%x==\n",int_Status);
	 	if(int_Status == SERIAL_INT_TYPE_RX)
 		{
 		i=0;
	 		while((OK == UARTCharsAvail())&&(i<100))
	 		{
	 			UARTCharGet(&ucChar[i]);
				i++;
	 		}
			cpldcs2Read(UART_ADD_LSR, &setData);
			printf("UART_ADD_LSR is 0x%x--\n",setData);
		printf("the number is %d==\n",i);
		
 		}

	 	}
	}
}


#define  TS_START
void Serial_Send_Mon(void)
{
    unsigned long ulPort;
    unsigned long data_len;

#ifdef TS_START
			struct timeval tv_current_time,tv_current_time1;
   			cs_uint8  term_id ;
			cs_uint8 *buffer = NULL;
			cs_uint32  offset = 0;
			static uart_msg_t uart_msg;
			uart_msg_t *msg = &uart_msg;
#endif
        SERIAL_INT_TYPE int_Status;
        unsigned char ucChar[RX_FIFO_LEN];
	  char setData;
	  int i;
    while (1)
    {

        while (OK == SerialIntSource(&ulPort))
        {
#if 1
           pthread_mutex_lock(&s2e_serial_sem);
#endif
            Serial_Cs_Set(ulPort);

            //
            // Get the cause of the interrupt.
            //
            if (OK == UARTIntStatus(&int_Status))
            {
                if (SERIAL_INT_TYPE_RX == int_Status)
                {            
                    unsigned long ringbuf_free_len;
			  char FCR_reg=0;

//add by zhangjj 2013.1.10
#ifdef    TS_START

			term_server_config_t *pTsCfg = TS_CFG_GET(ulPort+1);

			term_id = UART_TO_TERM_SERV(ulPort+1);
			if((ts_uart_ring[term_id].wr_pos+51) % TERM_SRV_UART_MAX_SIZE ==ts_uart_ring[term_id].rd_pos){
				buffer = NULL;
				printf("UART [%d] full. wrpos %d ,  rdpos %d ,  wrcount  %d ,rdcount  %d \n",ulPort,
					ts_uart_ring[term_id].wr_pos, ts_uart_ring[term_id].rd_pos,
					ts_uart_ring[term_id].wr_count,ts_uart_ring[term_id].rd_count);

			}
			else{
				buffer = (cs_uint8 *)&ts_uart_ring[term_id].data[0];
				offset = ts_uart_ring[term_id].wr_pos;
			}
#endif
//end

                    //
                    // Loop while there are characters available in the receive FIFO.
                    //
                    for(i = 0; ((i < RX_FIFO_LEN)&&(OK == UARTCharsAvail())); i++)
                    {
                         //
                        // Get the next character from the receive FIFO.
                        //
                        UARTCharGet(&ucChar[i]);
                         data_len++;

#ifdef 	TS_START          //add by zhangjj 2013.1.10

				if(buffer){
					buffer[offset] = ucChar[i];
					pTsCfg->uart_rx_char_cnt++;
					ts_uart_ring[term_id].wr_count++;
					offset = (offset + 1)%TERM_SRV_UART_MAX_SIZE;
				}
                    	}
			ts_uart_ring[term_id].wr_pos = offset;
			if(TS_ENABLED(term_id))
			{
			gettimeofday(&tv_current_time,NULL);
				pTsCfg->last_act_ticks.tv_sec= tv_current_time.tv_sec;
				if(ts_uart_ring[term_id].wr_count - ts_uart_ring[term_id].rd_count >0){
//			printf("UART[%d] send message to tx thread  \n",ulPort);
					vosMsqSend(msgq_ts_uart_rx,(void *)&msg,sizeof(cs_uint32));
				}
			}
#else
                	}
#endif
//end
					
                    /*直接发效率太低，存在一个缓冲区内，由另一个任务发送。*/

                    ringbuf_free_len = RingBufFree(&g_sRxBuf[ulPort]);
                    if (data_len < ringbuf_free_len)
                    {
                        RingBufWrite(&g_sRxBuf[ulPort], ucChar, data_len);
                    }
                    else
                    {
                        RingBufWrite(&g_sRxBuf[ulPort], ucChar, ringbuf_free_len);
                    }
                    data_len = 0;
                    /*如果放在最后，则在高速大量接收数据时，ringbuf会溢出*/

                    pthread_mutex_unlock(&s2e_serial_rx_sync_sem);
                	}
			
                //
                // See if there is space to be filled in the transmit FIFO.
                //
                else if (SERIAL_INT_TYPE_TX == int_Status)
                {
                    char UART_TX_RX_reg;
                    //
                    // Loop while there is space in the transmit FIFO and characters to be
                    // sent.
                    //

                    for(i = 0; ((!RingBufEmpty(&g_sTxBuf[ulPort])) && (i < TX_FIFO_LEN)); i++)
                    {
                        //
                        // Write the next character into the transmit FIFO.
                        //
                        UARTCharPut(RingBufReadOne(&g_sTxBuf[ulPort]));
                    }
                }
                else if (SERIAL_INT_TYPE_LS == int_Status)
                {
                    char LSR_reg;
                    char FCR_reg;

                    cpldcs2Read(UART_ADD_LSR, &LSR_reg);
//			printf("int status is LS 0x%x\n",LSR_reg);
                    if ((0 != (LSR_reg & UART_V_RX_ERR))&&( 0 != (LSR_reg & UART_V_FM)))
                    {
                        UARTFIFOEnable(TRUE, &FCR_reg);

                        UARTDMAEnable(TRUE, &FCR_reg);

                        UART_FIRO_Rx_Trigger_Level(SERIAL_SPEED_UP_TRIGGER_RX, &FCR_reg);

                        UARTFIFO_reset(&FCR_reg);

                        UARTSet_FCR(FCR_reg);
                    }

                }
                else if (SERIAL_INT_TYPE_MS == int_Status)
                {
                    char MSR_reg;
                    cpldcs2Read(UART_ADD_MSR, &MSR_reg);
           //         S2E_DEBUG(("\r\n int Status is MS(0x%02x)!\r\n", MSR_reg));
                }
                else
                {
                printf("interruput   else..................\n");
           //         S2E_DEBUG(("\r\n int type = %d\r\n", int_Status));
          //          VOS_ASSERT(0);
                }

            }

#if 1
            pthread_mutex_unlock(&s2e_serial_sem);
#endif
        }
		usleep(1);
//        VOS_TaskDelay(Serial_Monitor_interval);

    }

#if 0/*为中断所做预留*/
    while (1)
    {
        unsigned char ucChar[TX_RING_BUF_SIZE];
        for(ulPort = 0; (VOS_OK == Serial_port_num_valid_check(S_PORT_PHY_2_L(ulPort))); ulPort++)
        {
            if(!RingBufEmpty(&g_sRxBuf[ulPort]))
            {
                data_len = RingBufUsed(&g_sRxBuf[ulPort]);
                
                RingBufRead(&g_sRxBuf[ulPort], ucChar, data_len);

                if (VOS_YES == gulDebugS2E)
                {
                    cl_vty_all_out("\r\n serial send char: \r\n");
                    for (i = 0; i < data_len; i++)
                    {
                        cl_vty_all_out("%c", ucChar[i]);
                    }
                    cl_vty_all_out("\r\n");
                }

                S2E_Send_data(ulPort, ucChar, data_len);
            }
        }

        VOS_TaskDelay(1000);
    }
#endif
}


unsigned long Serial_Send_Free(unsigned long ulPort)
{
    unsigned long ulRet;
    
    pthread_mutex_lock(&s2e_serial_sem);
    
    ulRet = RingBufFree(&g_sTxBuf[ulPort]);

    pthread_mutex_unlock(&s2e_serial_sem);

    return ulRet;
}



int Serial_ctrl_init(void)
{
    unsigned char CARD_TYPE_reg;
    char cpld_version = 0;


    Serial_cpld_ver_get(&cpld_version);
	printf("cpld_version is 0x%x\n",cpld_version);

    if (S2E_CPLD_ID != ((cpld_version>>4)&0x0F))
    {
        return SERIAL_E_UNAVA;
    }

    S2E_Control.version = cpld_version&0xF;

    cpldcs2Read(UART_CTL_ADD_CARD_TYPE, &CARD_TYPE_reg);

    S2E_Control.Card_type = CARD_TYPE_reg;
printf("CARD_TYPE_reg is 0x%x\n",CARD_TYPE_reg);

    switch ((int)CARD_TYPE_reg)
    {
        case SERIAL_CARD_TYPE_S223A:
        {
            S2E_Control.serial_port_num = 2;

            S2E_Control.serial_type = SERIAL_TYPE_232;

            S2E_Control.wire_num = 3;

            S2E_Control.hard_fc_support = ERROR;

            serial_duplex = SERIAL_DUPLEX_FULL;

            break;
        }
        case SERIAL_CARD_TYPE_S214A:
        {
            S2E_Control.serial_port_num = 2;

            S2E_Control.serial_type = SERIAL_TYPE_485;

            S2E_Control.wire_num = 4;

            S2E_Control.hard_fc_support = ERROR;

            /*even it is 4 wire, wo have to init duplex to half.*/
            serial_duplex = SERIAL_DUPLEX_HALF;

            break;
        }
        case SERIAL_CARD_TYPE_S433A:
        {
            S2E_Control.serial_port_num = 4;

            S2E_Control.serial_type = SERIAL_TYPE_MIX;

            S2E_Control.wire_num = 3;

            S2E_Control.hard_fc_support = ERROR;

            serial_duplex = SERIAL_DUPLEX_HALF;

            break;
        }
       case SERIAL_CARD_TYPE_S443B:
        {
            S2E_Control.serial_port_num = 4;

            S2E_Control.serial_type = SERIAL_TYPE_MIX;

            S2E_Control.wire_num = 3;

            S2E_Control.hard_fc_support = ERROR;

            serial_duplex = SERIAL_DUPLEX_HALF;

            break;
        }
	 case SERIAL_CARD_TYPE_S412B:
        {
            S2E_Control.serial_port_num = 4;

            S2E_Control.serial_type = SERIAL_TYPE_485;

            S2E_Control.wire_num =2 ;

            S2E_Control.hard_fc_support = ERROR;

            serial_duplex = SERIAL_DUPLEX_HALF;

            break;
        }
        default:
        {
            return SERIAL_E_PRODUCT;
        }
		
    }
    return OK;
}

void SerialPurgeData(unsigned long ulPort)
{
	char FCR_reg = 0;
	pthread_mutex_lock(&s2e_serial_sem);

	Serial_Cs_Set(ulPort);

	UARTFIFOEnable(TRUE,&FCR_reg);
	UARTDMAEnable(TRUE,&FCR_reg);
	UART_FIRO_Rx_Trigger_Level(SERIAL_DEFAULT_TRIGGER_RX,&FCR_reg);
	UARTFIFO_reset(&FCR_reg);
	UARTSet_FCR(FCR_reg);
	RingBufFlush(&g_sTxBuf[ulPort]);
	ts_uart_ring[ulPort].wr_pos = ts_uart_ring[ulPort].rd_pos;
	pthread_mutex_unlock(&s2e_serial_sem);
}
void SerialIntEnable(unsigned long ulPort,int enable)
{
	pthread_mutex_lock(&s2e_serial_sem);

	Serial_Cs_Set(ulPort);
	UARTIntEnable(enable);
	pthread_mutex_unlock(&s2e_serial_sem);
}


void EnableUart(int enable)
{
    char RST_reg;
    cpldcs2Read(UART_CTL_ADD_RST_RING, &RST_reg);
    if (TRUE == enable)
    {
        RST_reg &= ~UART_CTL_RESET;
    }
    else
    {
        RST_reg |= UART_CTL_RESET;
    }
    cpldcs2Write(UART_CTL_ADD_RST_RING, RST_reg);
	
}

void SerialSetDefault(unsigned long ulPort, SERIAL_TYPE serial_type)
{
printf("serialsetdefault start...ulPort is %d\n",ulPort);
    int value;
    char FCR_reg = 0;
 
    Serial_Cs_Set(ulPort);

    UARTClearAllReg();

    /*disable EFR.*/
    UARTEFREnable(FALSE);
    //
    // Set the baud rate.
    //
    value = SERIAL_DEFAULT_BAUD_RATE;
    S2E_SET_info(ulPort, SERIAL_BAUD, &value);
    //
    // Set the data size.
    //
    value = SERIAL_DEFAULT_DATA_SIZE;
    S2E_SET_info(ulPort, SERIAL_DATA_SIZE, &value);
    //
    // Set the parity.
    //
    value = SERIAL_DEFAULT_PARITY;
    S2E_SET_info(ulPort, SERIAL_PARITY_CTRL, &value);
    //
    // Set the stop bits.
    //
 //   printf("11before 485....\n");
    value = SERIAL_DEFAULT_STOP_BITS;
    S2E_SET_info(ulPort, SERIAL_STOP_BIT, &value);
    //
    // Set the flow control.
    //

    value = SERIAL_DEFAULT_FC;
    S2E_SET_info(ulPort, SERIAL_FC_MODE_CTRL, &value);
    Serial_Cs_Set(ulPort);
    UARTFIFOEnable(TRUE, &FCR_reg);
    UARTDMAEnable(TRUE, &FCR_reg);
    UART_FIRO_Rx_Trigger_Level(SERIAL_DEFAULT_TRIGGER_RX, &FCR_reg);
    UARTSet_FCR(FCR_reg);
    UARTIntEnable(TRUE);



	SerialPurgeData(ulPort);
    //
    // Start the UART.
    //
    UARTFIRO_RDY_Enable(TRUE);
    //
    // (Re)enable the UART transmit and receive interrupts.
    //
    UARTIntType(SERIAL_INT_TYPE_RX, TRUE);
    UARTIntType(SERIAL_INT_TYPE_TX, TRUE);

    UARTRxTimeOutEn(TRUE);

    //UARTIntType(SERIAL_INT_TYPE_LS, TRUE);

    //UARTIntType(SERIAL_INT_TYPE_MS, TRUE);
	if((SERIAL_TYPE_485 == serial_type)&&(SERIAL_DUPLEX_HALF ==serial_duplex))
	{
		Serial_Uart_485_half(ulPort, TRUE);
	}
	else
	{
    		Serial_Uart_485_half(ulPort, FALSE);
	}
	printf("after 485....\n");
}

void
SerialSetBaudRate(unsigned long ulPort, unsigned long ulBaudRate)
{
pthread_mutex_lock(&s2e_serial_sem);
    /*set cs first.*/
    Serial_Cs_Set(ulPort);
    //
    // Set the baud rate.
    //
    UARTBaudRateSet(ulBaudRate);
pthread_mutex_unlock(&s2e_serial_sem);
}

void
SerialSetDataSize(unsigned long ulPort, unsigned char ucDataSize)
{    
pthread_mutex_lock(&s2e_serial_sem);
    /*set cs first.*/
    Serial_Cs_Set(ulPort);
    //
    // Set the baud rate.
    UARTDataSizeSet(ucDataSize);
pthread_mutex_unlock(&s2e_serial_sem);

}

void SerialSetParity(unsigned long ulPort, SERIAL_PARITY Parity)
{
pthread_mutex_lock(&s2e_serial_sem);
    /*set cs first.*/
    Serial_Cs_Set(ulPort);
    //
    // Set the baud rate.
    //
    UARTParityModeSet(Parity);
pthread_mutex_unlock(&s2e_serial_sem);
}

void
SerialSetStopBits(unsigned long ulPort, unsigned char ucStopBits)
{
pthread_mutex_lock(&s2e_serial_sem);
    /*set cs first.*/
    Serial_Cs_Set(ulPort);

    //
    // Set the baud rate.
    //
    UARTStopBitSet(ucStopBits);
pthread_mutex_unlock(&s2e_serial_sem);
}

void
SerialSetFlowControl(unsigned long ulPort, SERIAL_FC_MODE ucFlowControl)
{
//    printf("SERIAL_FC_MODE nothing\n");
}
void Serial_Uart_485_half(unsigned long ulPort, int enable)
{
    char WIRE_MODE_reg;
    unsigned long WIRE_shift;

pthread_mutex_lock(&s2e_serial_sem);

    WIRE_shift = ulPort;

    /*set cs first.*/
    Serial_Cs_Set(ulPort);

    UART485_half_enable(enable);

    UARTRxTimeOutEn(TRUE);

    cpldcs2Read(UART_CTL_ADD_CS_WIRE, &WIRE_MODE_reg);

    /*if true, 4wire mode, else 2wire mode.*/
    if (TRUE == enable)
    {
        WIRE_MODE_reg &= ~((1<<UART_CTL_V_SHIFT_WIRE)<<WIRE_shift);
    }
    else
    {
        WIRE_MODE_reg |= ((1<<UART_CTL_V_SHIFT_WIRE)<<WIRE_shift);
    }

    cpldcs2Write(UART_CTL_ADD_CS_WIRE, WIRE_MODE_reg);
pthread_mutex_unlock(&s2e_serial_sem);
}


void SerialSetUartInterface(unsigned long ulPort, SERIAL_TYPE s_interface)
{
    char serial_interface_reg;
    unsigned long ulshift;

    ulshift = ulPort;

    pthread_mutex_lock(&s2e_serial_sem);

    cpldcs2Read(UART_CTL_ADD_OID_INTERFACE, &serial_interface_reg);
    if (SERIAL_TYPE_232 == s_interface)
    {
        serial_interface_reg &= ~((1<<UART_CTL_V_SHIFT_INTERFACE)<<ulshift);
    }
    else
    {
        serial_interface_reg |= ((1<<UART_CTL_V_SHIFT_INTERFACE)<<ulshift);
    }
    cpldcs2Write(UART_CTL_ADD_OID_INTERFACE, serial_interface_reg);
    pthread_mutex_unlock(&s2e_serial_sem);
}

Serial_Option serial_232={
	232,
	SerialSetBaudRate,
	SerialSetDataSize,
	SerialSetStopBits,
	SerialSetParity,
	SerialSend,
	RingBufRead,
};









