#ifndef __TERM_SERVER_H
#define __TERM_SERVER_H

#include <time.h>
#include <sys/time.h>
#include "cs_types.h"

#define SERIAL_8250

#ifdef SERIAL_8250
#define TEST_PORT_NUM 4
extern int serial_fds[TEST_PORT_NUM];
#endif

#define LAN_IP 1
#define WAN_IP 2

/* support UART 1 ~ 4*/
#define MAX_TERM_SERV_NUM       4
#define TS_MIN_UART_NUM         1
#define TS_MAX_UART_NUM         4

/* UART definitions */
#define MIN_UART_BAUD_RATE      300
#define MAX_UART_BAUD_RATE      115200
#define MAX_UART_DATA_BIT       8
#define MIN_UART_STOP_BIT       1
#define MAX_UART_STOP_BIT       2

/* protocol definitions */
#define MIN_SOCKET_PORT         1024
#define MAX_SOCKET_PORT         65535
#define TERM_SERV_CFG_FILE	"/cfg/ts.conf"
#define TELNET_IP 				"/cfg/telnetIP.conf"

#define UART_TO_TERM_SERV(x)    ((x) - TS_MIN_UART_NUM)
#define TS_MAX_PAYLOAD_FLOOR         1024
#define TS_MAX_PAYLOAD_CEILING         1400
//#define TS_MAX_BUF_SIZE         1460
//#define TS_MIN_BUF_SIZE         512
#define TS_BUF_SIZE             1600
#define TS_UART_BUF_MAX         2048
#define TS_MIN_PAD_SIZE         40
#define TS_MIN_RSP_TIME         25
#define TS_MAX_RSP_TIME         1000
#define MAX_RING_BUF_NUM        16
#define MAX_REMOTE_TIMEOUT      65535
//#define TS_MAX_PAYLOAD          1460

#define VALID_CONFIG_FLAG       0xA5
#define TERM_SERV_ENABLE_FLAG   0x5A

/* message queue */
#define TS_UART_RX_QUEUE_DEPTH  16
#define TS_UART_RX_QUEUE_SIZE   4
#define TS_IP_RX_QUEUE_DEPTH    64




typedef struct {
    cs_uint8        uart;
    cs_uint16       len;
    cs_uint8        data[1600];
}uart_msg_t;
#if 0
typedef struct {
    uart_msg_t *    act_buf;
    uart_msg_t *    pp_msg[2];
}ping_pong_buf_t;
#endif
typedef struct
{
    cs_uint8     uart_num;
    cs_uint32    length;
    cs_uint8     net_data[1600];
}net_msg_t;

typedef struct
{
    cs_uint32    pIngress;
    cs_uint32    pEgress;
    //cs_uint32   pkt_nun;
    cs_uint32   wr_count;
    cs_uint32   rd_count;
    net_msg_t * pBuf[MAX_RING_BUF_NUM];
}ring_buf_t;

typedef enum{
	UART_MODE_POLL =0,
	UART_MODE_INTR =1
}cs_uart_mode_t;

typedef enum{
	UART_DUPLEX_FULL =0,
	UART_DUPLEX_HALF =1
}cs_uart_duplex_t;

typedef enum{
	UART_PARITY_NONE =0,
	UART_PARITY_ODD =1,
	UART_PARITY_EVEN =2
}cs_uart_parity_t;


typedef struct{
	unsigned char 	uart_id;
	unsigned char 	data_bits;
	unsigned char 	stop_bits;
	unsigned char 	flow_control;
	cs_uart_parity_t 	parity;
	cs_uart_duplex_t 	duplex;
	cs_uart_mode_t 	mode;
	int 			baud_rate;
}uart_config_t;


typedef struct{
    uart_config_t uart_cfg;         
    cs_uint32 proto_type;     /* protocol type TCP 6/UDP 1*/
    cs_uint32 proto_port;        /* L4 protocol port 1~65535 */
    cs_uint32 client_timeout;  /* remote client timeout 0:never thoer:1~65535s*/
    cs_uint32 max_payload;   /* 512 ~1400 bytes */
    cs_uint32 min_payload;    /* 40 ~ max_payload */
    cs_uint32 max_res_time; /* max response time 25~1000ms */
    cs_int32 server_fd;   /* server socket FD */
    cs_int32 client_fd;    /* client socket FD */
    cs_uint32 client_ip;
    cs_uint32 client_port;
    cs_uint32 client_closed_flag; /* check client connection */
    cs_uint32 term_serv_rx_ip_packets;
    cs_uint32 term_serv_tx_ip_packets;
    cs_uint32 term_serv_tx_lost_packets;
    cs_uint32 term_serv_rx_ip_cnt;
    cs_uint32 term_serv_tx_ip_cnt;
    cs_uint32 term_serv_tx_lost_cnt;
    cs_uint32 uart_rx_char_cnt;
    cs_uint32 uart_tx_char_cnt;
    cs_uint32 enable_flag;  /* terminal server enable/disable*/
    struct timeval last_act_ticks;
    cs_uint8  init_flag;  /* used to check flash area has valid config data */ 
    ring_buf_t ip_rx_buf;
    #if 0
    ping_pong_buf_t    uart_msg; /* store received UART data */
    #endif
    cs_uint32 reserved[1];
} term_server_config_t;
#define TERM_SRV_UART_MAX_SIZE  8192
#define TERM_SRV_MTU 1460

#define TS_ENABLED(ts) (terminal_server_cfg[ts]->enable_flag == TERM_SERV_ENABLE_FLAG )
#define TS_CFG_GET(uart)	terminal_server_cfg[UART_TO_TERM_SERV(uart)]


typedef struct _ts_uart_ring_t
{
    cs_uint32 rd_pos;
    cs_uint32 rd_count;
    cs_uint32 wr_pos;
    cs_uint32 wr_count;
//    cs_uint64 last_act_ticks;
    cs_uint8 data[TERM_SRV_UART_MAX_SIZE];
}ts_uart_ring_t;

extern cs_uint32 term_serv_dbg;
extern cs_uint32 old_port;
extern cs_uint32 serverfd;
extern cs_uint32 never_interrupt;
extern ts_uart_ring_t ts_uart_ring[MAX_TERM_SERV_NUM];

void ts_serial_open_single(int ulPort,int baud,int data,int nstop,int parity_int);
void ts_serial_open();
static void ts_serial_monitor_thread();
void ts_serial_send(unsigned long ulPort, unsigned char *buf, int len);
extern void ts_auto_create(void);
extern cs_status ts_enable(term_server_config_t* pTerm_serv_cfg);
extern void ts_disable(cs_uint8 uart);
extern void ts_reset(cs_uint8 uart);
extern void ts_show(cs_uint8 uart);
extern void ts_cfg_save(cs_uint8 uart);
extern void ts_cfg_clear(cs_uint8 uart);
extern cs_int32 ts_uart_intr_handler(cs_uint8 uart_id, void *data);
extern cs_status ts_uart_id_check(cs_uint8 uart);
extern void ts_init(void);
extern void ts_electricity_meter_test(cs_uint8 uart);
extern term_server_config_t *terminal_server_cfg[MAX_TERM_SERV_NUM];

#endif
