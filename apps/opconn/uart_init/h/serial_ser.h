
#ifndef _SERIAL_SER_H_
#define _SERIAL_SER_H_
#include <unistd.h>
/*card type of serial.*/
#define SERIAL_CARD_TYPE_BASE       0/*invalid*/
#define SERIAL_CARD_TYPE_S223A      SERIAL_CARD_TYPE_BASE + 1/*S223A-2ports, 232,3wire.*/
#define SERIAL_CARD_TYPE_S214A      SERIAL_CARD_TYPE_BASE + 2/*S214A-2ports, 485,4wire.*/
#define SERIAL_CARD_TYPE_S433A      SERIAL_CARD_TYPE_BASE + 3/*S434A-4ports, 2*232+2*485,3/2wire.*/

#define SERIAL_CARD_TYPE_LOWC_BASE  0x80/*low cost serial card.*/
#define SERIAL_CARD_TYPE_S443B      SERIAL_CARD_TYPE_LOWC_BASE + 1 /*S414A-4ports, 232/485, 3/2wire.*/
#define SERIAL_CARD_TYPE_S412B      SERIAL_CARD_TYPE_LOWC_BASE + 3/*S434A-4ports, 485  2wire,*/
																				/*add by zhangjj */
#define SERIAL_DUPLEX_HALF          0
#define SERIAL_DUPLEX_FULL          1

/*default value*/
#define SERIAL_DEFAULT_DATA_SIZE        8
#define SERIAL_DEFAULT_STOP_BITS        1
#define SERIAL_DEFAULT_BAUD_RATE        9600
#define SERIAL_DEFAULT_PARITY           SERIAL_PARITY_NONE
#define SERIAL_DEFAULT_FC               SERIAL_FC_NONE


//*****************************************************************************
//
//! The number of serial to Ethernet ports supported by this module.
//
//*****************************************************************************
#define MAX_S2E_PORTS               4
#define SERIAL_CONNECT_CHECK_TIME   10
#define NO_NEED_FD                  0xffff

#define SERIAL_ERROR_BASE           1000/*error number��base*/

#define SERIAL_E_PARA               SERIAL_ERROR_BASE + 1/*��������*/
#define SERIAL_E_UNAVA              SERIAL_ERROR_BASE + 2/*Ӳ����֧��*/
#define SERIAL_E_PARA_CONFLICT      SERIAL_ERROR_BASE + 3/*�������˿ڵ������ظ�*/
#define SERIAL_E_PRODUCT            SERIAL_ERROR_BASE + 4/*�Ƿ��Ĳ�Ʒ�ͺ�*/
#define SERIAL_E_UNKOWN             SERIAL_ERROR_BASE + 5/*δ֪��������Զ������*/

//*****************************************************************************
//
//! The size of the ring buffers used for interface between the UART and
//! telnet session.
//
//*****************************************************************************
#define RING_BUF_SIZE_RX            (256 * 2)
#define RING_BUF_SIZE_TX            (256 * 4)

#define S2E_DEBUG(str) if(gulDebugS2E){ cl_vty_all_out str ;}

#define S_PORT_PHY_2_L(x) (x + 1)
#define S_PORT_L_2_PHY(x) (x - 1)

#define S2E_TIMER_CHECK_LENGTH      1000

#define S2E_SERIAL_RX_MAX_LEN       128

#define S2E_CPLD_ID                 0x0A/*only when the cpld 0 byte, high 4 bits is 0x0A, the cpld is valid.*/

#ifndef TRUE
#define TRUE                     1
#endif
#ifndef FALSE
#define FALSE                    0
#endif

typedef enum
{
	SERIAL_FC_NONE,
	SERIAL_FC_MODE_HARD,
	SERIAL_FC_MODE_SOFT
}SERIAL_FC_MODE;

typedef enum
{
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN,
    SERIAL_PARITY_MARK,
    SERIAL_PARITY_SPACE
}SERIAL_PARITY;

typedef enum
{
    GROUP_IT_SELF,
    S2E_CONNECT_ENABLE,/*permit serial port to connect to enet.*/
    S2E_CONNECT_MODE,
    S2E_CONNECT_STATE,/* in tcp server mode, when begin listening, set true; in client mode, after connect, set true; in udp mode, always true.*/
    S2E_LOCAL_PORT,
    S2E_REMOTE_PORT,
    S2E_REMOTE_IP,
    S2E_LOCAL_IP,
    S2E_FLAG,
    S2E_SERIAL_PORT,
    VTY_CHAIN,
    SERIAL_BAUD,/*������*/
    SERIAL_DATA_SIZE,/*����λ*/
    SERIAL_PARITY_CTRL,/*У��λ*/
    SERIAL_STOP_BIT,/*ֹͣλ*/
    SERIAL_FC_MODE_CTRL,/*����*/
    SERIAL_INTERFACE_TYPE/*UART�ӿ�����*/
}S2E_CTL_NUM;

typedef enum
{
    SERIAL_TYPE_NONE,/*no serial card.*/
    SERIAL_TYPE_232,
    SERIAL_TYPE_485,
    SERIAL_TYPE_MIX/*232 and 485*/
}SERIAL_TYPE;



typedef struct
{
    int version;
    int Card_type;
    unsigned long serial_port_num;
    int hard_fc_support;
    SERIAL_TYPE serial_type;
    int wire_num;/*the number of wire.*/
 //   tS2E_Ctl_Group *S2E_group_head;
}S2E_Ctl;

extern S2E_Ctl S2E_Control;
extern int serial_duplex;
extern pthread_mutex_t s2e_serial_sem;
extern pthread_mutex_t s2e_serial_rx_sync_sem;
extern pthread_mutex_t s2e_ringbug_sem;
extern pthread_mutex_t s2e_enet_sem;

int S2E_init(void);
int S2E_SET_info(unsigned long serial_port, S2E_CTL_NUM number, void *info);
void Serial_Read_Rx_ringbuf(void);


#endif
