#include <semaphore.h>
#include <stdio.h>
#include "vos_thread.h"
#include "test.h"
#include "serial_ser.h"
#include "serial.h"
#include "uart_default.h"

S2E_Ctl S2E_Control = {0};/*������á�1Ϊ���ͷ����2ΪӲ������*/
int serial_duplex = SERIAL_DUPLEX_HALF;/*serial duplex full/half, effect if the port is 485.*/

VOS_THREAD_t tid2,tid3,tid4;

pthread_mutex_t s2e_serial_sem=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t s2e_serial_rx_sync_sem=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t s2e_ringbug_sem=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t s2e_enet_sem=PTHREAD_MUTEX_INITIALIZER;
//sem_t s2e_serial_sem;/*�ײ㻥���ź��*/
//sem_t s2e_serial_rx_sync_sem;/*�ײ��հ���ͬ���ź��*/
//sem_t s2e_ringbug_sem;/*ringbuf ���ź��*/
//sem_t s2e_enet_sem;/*��̫�����ź��,����vty_chain*/

int S2E_init(void)
{
//printf("S2E_init start...\n");
	unsigned long ulPort;
	int iret;
	SERIAL_TYPE group_temp;

	if(OK != (iret = Serial_ctrl_init()))
	{
	
		if (SERIAL_E_UNAVA == iret)
       		{
            		printf(" Hardware don't support S2E module.\r\n");
        	}
        	else if (SERIAL_E_PRODUCT == iret)
        	{
            		printf(" Unkown serial card type.\r\n");
        	}
        	else
        	{
            		printf(" When initing s2e, unkown error occured!\r\n");
        	}
        	return ERROR;
	}
	for(ulPort = 0; ulPort < S2E_Control.serial_port_num; ulPort++)
	{
//	printf("ulPort is %d\n",ulPort);
		if (SERIAL_CARD_TYPE_S433A == S2E_Control.Card_type)
        	{
            		if ((0 == ulPort)||(1 == ulPort))
            		{
                		group_temp = SERIAL_TYPE_232;
            		}
            		else
            		{
                		group_temp = SERIAL_TYPE_485;
            		}
        	}
        	else if (SERIAL_CARD_TYPE_S223A == S2E_Control.Card_type)
        	{
        	    group_temp = SERIAL_TYPE_232;
        	}
        	else if (SERIAL_CARD_TYPE_S214A == S2E_Control.Card_type)
        	{
        	    group_temp = SERIAL_TYPE_485;
        	}
        	else if (SERIAL_CARD_TYPE_S443B == S2E_Control.Card_type)
        	{
        	    SERIAL_TYPE serial_type;
        	    if ((0 == ulPort)||(1 == ulPort))
        	    {
        	        serial_type = SERIAL_TYPE_232;
        	    }
        	    else
        	    {
        	        serial_type = SERIAL_TYPE_485;
        	    }
				
        	    SerialSetUartInterface(ulPort, serial_type);
				
        	    group_temp = serial_type;
        	}
		else if (SERIAL_CARD_TYPE_S412B == S2E_Control.Card_type)
        	{
          	    SERIAL_TYPE serial_type;

        	    group_temp  = SERIAL_TYPE_485;
	     	    SerialSetUartInterface(ulPort, group_temp);
		}
        	else
        	{
        	    ;
        	    return ERROR;
        	}
		SerialInit(ulPort, group_temp);
	}
#if 0
pthread_create( &tid3,NULL,(void *)Serial_Read_Rx_ringbuf, NULL);
//pthread_create(&tid2,NULL, (void *)Serial_Send_Mon, NULL );
pthread_create(&tid4,NULL,(void *)Serial_read,NULL);
#else
vosThreadCreate("tSerial_Rx", 4096, 100, (void *)Serial_Read_Rx_ringbuf, NULL);
//vosThreadCreate("tSerial_Mon", 4096, 50, (void *)Serial_Send_Mon ,NULL);
//vosThreadCreate("Serial_read", 4096, 50, (void *)Serial_read ,NULL);
#endif
//Serial_Uart_ring();
//printf("S2E_init end ...\n");
}



int S2E_SET_info(unsigned long serial_port, S2E_CTL_NUM mumber, void *info)
{
    int ret = OK;

        switch (mumber)
        {
            
            case SERIAL_BAUD:
            {
                unsigned long baud_rate;

                baud_rate = *(unsigned long *)info;
                SerialSetBaudRate(serial_port, baud_rate);
                
                break;
            }

            case SERIAL_DATA_SIZE:
            {
                unsigned char data_size;
                
                data_size = *(unsigned int *)info;

                SerialSetDataSize(serial_port, data_size);

                break;
            }

            case SERIAL_PARITY_CTRL:
            {
                SERIAL_PARITY parity;
                
                parity = *(SERIAL_PARITY *)info;

                SerialSetParity(serial_port, parity);
                break;
            }

            case SERIAL_STOP_BIT:
            {
                int stop_bit;
                
                stop_bit = *(int *)info;

                SerialSetStopBits(serial_port, stop_bit);

                break;
            }

            case SERIAL_FC_MODE_CTRL:
            {
                SERIAL_FC_MODE fc_mode;
                
                fc_mode = *(SERIAL_FC_MODE *)info;
                SerialSetFlowControl(serial_port, fc_mode);
                 break;
            }

            default:
                printf("assert\n");
        }
        return OK;
}


