#include <stdio.h>

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
//#include <select.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
//#include <codes.h>
//#include <asm/error.h>
#include <unistd.h>
#include "cs_types.h"
#include "terminal_server.h"
#include "mempool.h"
#include "opl_regmmap.h"
#include "osal_common.h"
#include "osal_api_core.h"
#include "plat_config.h"
#include "serial.h"
#include "vos_thread.h"
#include "uart_default.h"
#include "uart_driver8250.h"
#include "vos_types.h"
#include "oam.h"

#if(PRODUCT_CLASS==GT873_A)

cs_uint32 term_serv_dbg = 0;
cs_uint32 old_port = 0;
cs_uint32 serverfd = 0;
cs_uint32 never_interrupt=1;

#ifdef SERIAL_8250
int serial_fds[TEST_PORT_NUM] = {-1,-1,-1,-1};

#endif

/*used to inform IP layer one UART packet received */
volatile int  msgq_ts_uart_rx = -1;
/* used to inform UART one IP packet received */
volatile int msgq_ts_net_rx = -1;

/* Terminal Server Task/Thread parameters */
#if 0
cs_uint32 ip_rx0_thread_id = 0;
cs_uint32 ip_rx1_thread_id = 0;
cs_uint32 ip_rx2_thread_id = 0;
cs_uint32 ip_rx3_thread_id = 0;
cs_uint32 uart2net_thread_id = 0;
cs_uint32 net2uart_thread_id = 0;
#else
VOS_THREAD_t serial_monitor_id=NULL;
VOS_THREAD_t  ip_rx0_thread_id = NULL;
VOS_THREAD_t ip_rx1_thread_id = NULL;
VOS_THREAD_t ip_rx2_thread_id = NULL;
VOS_THREAD_t ip_rx3_thread_id = NULL;
VOS_THREAD_t uart2net_thread_id = NULL;
VOS_THREAD_t net2uart_thread_id = NULL;
#endif

term_server_config_t * terminal_server_cfg[MAX_TERM_SERV_NUM];
ts_uart_ring_t ts_uart_ring[MAX_TERM_SERV_NUM];
extern int errno;

static void ts_def_uart_rx_func( cs_int32 uart, unsigned char *data, cs_int32 len);

cs_int32 ts_client_fd_close(term_server_config_t* pTs_cfg)
{
    cs_int32 client_fd;
    //add sem to protect
    if((client_fd = pTs_cfg->client_fd) >= 0) {
        pTs_cfg->client_fd = -1;
        close(client_fd);
//        printf("%s, close client fd %d, %d\n", __func__, client_fd, __LINE__);
    }
    else {
//        printf("%s, no client fd active, %d\n", __func__, __LINE__);
    }
    
    //add sem to protect
    return 0;
}


cs_int32 ts_server_fd_close(term_server_config_t* pTs_cfg)
{
    cs_int32 server_fd;
    //add sem to protect
    if((server_fd = pTs_cfg->server_fd) >= 0) {
        pTs_cfg->server_fd = -1;
        close(server_fd);
 //       printf("%s, close server fd %d, %d\n", __func__, server_fd, __LINE__);
    }
    else {
 //       printf("%s, server fd is not active, %d\n", __func__, __LINE__);
    }
    //add sem to protect
    return 0;
}


cs_uint32 ts_send_data_2net(cs_uint32 ts_id, cs_uint8 *data, cs_uint32 len)
{
    cs_uint32 offset = 0;
    cs_uint32 term_mtu = terminal_server_cfg[ts_id]->max_payload;

    while(len/term_mtu) {
        ts_def_uart_rx_func(ts_id + 1,&data[offset],term_mtu);
        offset += term_mtu;
        len -= term_mtu;
    }
    if(len > 0) {
        ts_def_uart_rx_func(ts_id + 1,&data[offset],len);
        offset += len;
    }
    
    return offset;
 }

cs_int32 ts_uart2net_proc(cs_uint32 ts_id)
{
    cs_uint32 wrpos = ts_uart_ring[ts_id].wr_pos;
    cs_uint32 rdpos = ts_uart_ring[ts_id].rd_pos;
    cs_uint32 wr_count = ts_uart_ring[ts_id].wr_count;
    cs_uint32 rd_count = ts_uart_ring[ts_id].rd_count;
    cs_uint32 len = 0;

   if(rdpos == wrpos){
    /* empty */
        return -1;
   }
   len = (wrpos>rdpos)? (wrpos-rdpos):(wrpos+TERM_SRV_UART_MAX_SIZE-rdpos);
   
//    printf("UART[%d] len %d, wr_count 0x%x, rd_count 0x%x, wrpos %d, rd_pos %d\n", 
//        ts_id, len, wr_count, rd_count,wrpos, rdpos);  
    
    /* process data here */
    if(rdpos < wrpos) {
        ts_send_data_2net(ts_id, &ts_uart_ring[ts_id].data[rdpos], len);
        ts_uart_ring[ts_id].rd_count += len;
        ts_uart_ring[ts_id].rd_pos = (rdpos + len) % TERM_SRV_UART_MAX_SIZE;//Can not be
    }
    else {
            //assert(len > (TERM_SRV_UART_MAX_SIZE - rdpos -1));
            ts_send_data_2net(ts_id, &ts_uart_ring[ts_id].data[rdpos], TERM_SRV_UART_MAX_SIZE - rdpos -1);
            ts_send_data_2net(ts_id, &ts_uart_ring[ts_id].data[0], len - (TERM_SRV_UART_MAX_SIZE - rdpos -1));
            ts_uart_ring[ts_id].rd_count += len;
            ts_uart_ring[ts_id].rd_pos = wrpos;
    }
    
    return 0;
    
}

cs_status ts_uart_id_check(cs_uint8 uart)
{
	if((uart >= TS_MIN_UART_NUM) && (uart <= TS_MAX_UART_NUM)){
		return CS_E_OK;
		}
	else{
		return CS_E_PARAM;
		}
}
static cs_status check_uart_cfg(
    uart_config_t *uart_cfg
)
{
    if (NULL == uart_cfg) {
        return CS_E_PARAM;
    }

    if(ts_uart_id_check(uart_cfg->uart_id)) {
        TS_DBG("Wrong uart id %d!\n", uart_cfg->uart_id);
        return CS_E_NOT_SUPPORT;
    }
    
    if(uart_cfg->baud_rate > MAX_UART_BAUD_RATE || uart_cfg->baud_rate < MIN_UART_BAUD_RATE ) {
        TS_DBG("Wrong baud rate!\n");
        return CS_E_NOT_SUPPORT;
    }
    if(uart_cfg->data_bits != MAX_UART_DATA_BIT) {
        TS_DBG("Wrong data bits!\n");
        return CS_E_NOT_SUPPORT;
    }
    if(uart_cfg->stop_bits > MAX_UART_STOP_BIT || uart_cfg->stop_bits < MIN_UART_STOP_BIT) {
        TS_DBG("Wrong stop bits!\n");
        return CS_E_NOT_SUPPORT;
    }
    
    if (uart_cfg->parity > UART_PARITY_EVEN) {
        TS_DBG("Wrong parity!\n");
        return CS_E_NOT_SUPPORT;
    }

    if (UART_DUPLEX_HALF != uart_cfg->duplex){
        TS_DBG("Wrong duplex!\n");
        return CS_E_NOT_SUPPORT;
    }

    if (UART_MODE_INTR != uart_cfg->mode){
        TERM_DBG_PRINT("Wrong mode should be interrupt mode!\n");
        return CS_E_NOT_SUPPORT;
    }    
    
    return CS_E_OK;
    
}



static net_msg_t * ring_buffer_get(term_server_config_t* pTerm_serv_cfg)
{
    cs_uint16 pos;

    if(pTerm_serv_cfg->ip_rx_buf.wr_count - pTerm_serv_cfg->ip_rx_buf.rd_count < MAX_RING_BUF_NUM){
        pTerm_serv_cfg->ip_rx_buf.wr_count ++;
        pos = pTerm_serv_cfg->ip_rx_buf.pIngress;
        pTerm_serv_cfg->ip_rx_buf.pIngress = 
            (pTerm_serv_cfg->ip_rx_buf.pIngress + 1) % MAX_RING_BUF_NUM ; 
        
        return (pTerm_serv_cfg->ip_rx_buf.pBuf[pos]);
    }
    else {
        printf("The receive buffer full, uart[%d], wr_count %d, rd_count %d, pIngress\n", 
            pTerm_serv_cfg->uart_cfg.uart_id, pTerm_serv_cfg->ip_rx_buf.wr_count, 
            pTerm_serv_cfg->ip_rx_buf.rd_count, pTerm_serv_cfg->ip_rx_buf.pIngress);
        return NULL;
    }
}

static cs_uint8 ts_net_msg_proc(net_msg_t * pMsg)
{

    cs_uint32 count = 0;
    cs_uint8 uart = pMsg->uart_num;

    if(CS_E_OK == ts_uart_id_check(uart)){
//changed by zhangjj  2013.1.10
//Serial_Cs_Set(uart);
#if 0

 if (OK == SerialTxRdy(uart))
    {
        for(count = 0; count < pMsg->length; count++)
        {
            //
            // Write the next character into the transmit FIFO.
             UARTCharPut(pMsg->net_data[count]);
        }
    }
//	for(count = 0; count < pMsg->length; count++){
//		printf("data is %c\n",pMsg->net_data[count]);
		     //  ts_putc(uart,pMsg->net_data[count]);
//        UARTCharPut(pMsg->net_data[count]);
			   
 //       }
#else 
//	SerialSend(uart,pMsg->net_data,pMsg->length);
ts_serial_send(uart-1,pMsg->net_data,pMsg->length);
#endif
        terminal_server_cfg[UART_TO_TERM_SERV(uart)]->uart_tx_char_cnt += pMsg->length;
        terminal_server_cfg[UART_TO_TERM_SERV(uart)]->ip_rx_buf.rd_count++;
    }
    else {
        printf("Invalid uart ID from message, uart[%d]\n", uart);
    }
    return 1;
}


static void ts_net_msg_send(net_msg_t * pMsg)
{       
    if(CS_E_OSAL_OK != vosMsqSend(msgq_ts_net_rx, (void *)&pMsg, sizeof(cs_uint32) )){ 
        printf("ts_net_msg_send put msg failed\n");
        return;
    }
}
#if 0
//get char function from uart   will not use ?
cs_int32 ts_uart_intr_handler(cs_uint8 uart_id,void *data)
{
    cs_uint8 term_id;
    cs_uint8 *buffer = NULL;
    cs_uint32 offset = 0;
    static uart_msg_t uart_msg;
    uart_msg_t * msg = &uart_msg;
    term_server_config_t *pTsCfg = TS_CFG_GET(uart_id);

    
    term_id = UART_TO_TERM_SERV(uart_id);
    if((ts_uart_ring[term_id].wr_pos+1)%TERM_SRV_UART_MAX_SIZE == ts_uart_ring[term_id].rd_pos) {
        //full 
        buffer = NULL;
       printf("UART[%d] full. wrpos %d, rdpos %d,wrcount %d, rdcount %d\n", uart_id,
                                                    ts_uart_ring[term_id].wr_pos, ts_uart_ring[term_id].rd_pos,
                                                    ts_uart_ring[term_id].wr_count, ts_uart_ring[term_id].rd_count);  
    }
    else {
        buffer = (cs_uint8 *)&ts_uart_ring[term_id].data[0];
        offset = ts_uart_ring[term_id].wr_pos;
    }
    
    if(buffer) {
        while(CS_E_OK == ts_getc(uart_id,&buffer[offset])) {
            pTsCfg->uart_rx_char_cnt++;
            ts_uart_ring[term_id].wr_count ++;
            offset = (offset + 1) % TERM_SRV_UART_MAX_SIZE;
        }
        ts_uart_ring[term_id].wr_pos = offset;
    }
    else {
        cs_uint8 temp;
        while(CS_E_OK == ts_getc(uart_id,&temp)) {
        }
    }
    //ts_uart_int_status_clear(uart_id);
    
    printf("UART[%d] wr_count %d, rd_count %d\n", uart_id,
                                                    ts_uart_ring[term_id].wr_count, ts_uart_ring[term_id].rd_count);  
	
    if(TS_ENABLED(term_id)){
       gettimeofday(&pTsCfg->last_act_ticks,NULL);
       //ts_uart_ring[term_id].last_act_ticks = cyg_current_time();
       if(ts_uart_ring[term_id].wr_count - ts_uart_ring[term_id].rd_count > 1024){
          printf("UART[%d] send message to tx thread \n", uart_id);
          vosMsqSend(msgq_ts_uart_rx, (void *)&msg, sizeof(cs_uint32) );
       }
    }

   return CS_E_OK;
   
}
#endif
/*----------------------------------------------
**
**         TCP Connection
**
**---------------------------------------------*/

#define SO_REUSEPORT  0x0200

static cs_status ts_tcp_socket_setup(term_server_config_t* pTerm_serv_cfg)
{
    struct sockaddr_in serv_addr;
    cs_uint32 index;
    cs_int32 on = 1;
    cs_int32 ret = 0;
	int rc;
    term_server_config_t *pTsCfg = TS_CFG_GET(pTerm_serv_cfg->uart_cfg.uart_id);
    
    index = UART_TO_TERM_SERV(pTerm_serv_cfg->uart_cfg.uart_id);
    pTsCfg->client_fd = -1;

	  /* create socket*/
	if((pTsCfg->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
	     TS_DBG("socket failed\n");
	     return CS_E_ERROR;
	}
	rc = setsockopt(pTsCfg->server_fd, SOL_SOCKET, SO_REUSEADDR,&on, sizeof(on));
	if(rc<0)
		{

		close(pTsCfg->server_fd);
		printf("set socket opt error\n");
		return CS_E_ERROR;
		}
	
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(pTerm_serv_cfg->proto_port);
    ret = bind(pTsCfg->server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(ret < 0){
        TS_DBG("bind failed, ret %d, uart[%d], server fd %d, port %d,\n", ret,
            pTerm_serv_cfg->uart_cfg.uart_id, pTsCfg->server_fd, serv_addr.sin_port);
        ts_server_fd_close(pTsCfg);
        return CS_E_ERROR;
    }

   /* only accept one client */
    if(listen(pTsCfg->server_fd, 5) < 0){
        close(pTsCfg->server_fd);
        pTsCfg->server_fd = -1;
        TS_DBG("listen failed\n");
        return CS_E_ERROR;
    }
//END_tcp:
    
    return CS_E_OK;

}



cs_int32 ts_new_conn_proc(term_server_config_t* pTs_cfg)
{
    socklen_t cli_len;
    struct sockaddr_in client_addr;  // connector's address information  
    
    memset(&client_addr, 0, sizeof(client_addr));
    cli_len = sizeof(struct sockaddr_in);
    pTs_cfg->client_fd = accept(pTs_cfg->server_fd,(struct sockaddr *)&(client_addr),&cli_len);
    if(pTs_cfg->client_fd < 0){
        printf("server fd %d accept error\n", pTs_cfg->server_fd); 
        return -1;
    }
//	printf("a new client connected ===\n");
    pTs_cfg->client_ip = ntohl(client_addr.sin_addr.s_addr);
    pTs_cfg->client_port = ntohs(client_addr.sin_port);
    pTs_cfg->client_closed_flag = 0;
    ts_uart_ring[UART_TO_TERM_SERV(pTs_cfg->uart_cfg.uart_id)].rd_count = 0;
    ts_uart_ring[UART_TO_TERM_SERV(pTs_cfg->uart_cfg.uart_id)].wr_count = 0;
    ts_uart_ring[UART_TO_TERM_SERV(pTs_cfg->uart_cfg.uart_id)].rd_pos = 0;
    ts_uart_ring[UART_TO_TERM_SERV(pTs_cfg->uart_cfg.uart_id)].wr_pos = 0;
    pTs_cfg->ip_rx_buf.pEgress = 0;
    pTs_cfg->ip_rx_buf.pIngress = 0;
    pTs_cfg->ip_rx_buf.wr_count = 0;
    pTs_cfg->ip_rx_buf.rd_count = 0;

    printf("server fd %d accept new connect fd %d\n", pTs_cfg->server_fd, pTs_cfg->client_fd); 
    return 0;
}


static cs_uint8 ts_dummy_buf[TS_BUF_SIZE];
#define TS_MAX(a, b) ((a) > (b))?(a):(b) 
//#define REMOVE_BUF                //remove buf and send directly        changed by zhangjj 2013-2-22
cs_int32 ts_client_data_proc(term_server_config_t* pTs_cfg)
{
    cs_int32 len;
    net_msg_t * buffer;
#if 1
    if((buffer = ring_buffer_get(pTs_cfg)) == NULL){
        buffer = (net_msg_t *)ts_dummy_buf;
    }

    len = read(pTs_cfg->client_fd, buffer->net_data, TS_BUF_SIZE);
    if(len <= 0){
        printf("read, uart[%d] socket(client) fd %d closed \n", pTs_cfg->uart_cfg.uart_id, pTs_cfg->client_fd);
        return -1;
    }
    pTs_cfg->term_serv_rx_ip_packets ++;
    pTs_cfg->term_serv_rx_ip_cnt += len;

#endif
#if 0

buffer = pTs_cfg->ip_rx_buf.pBuf[0];
    len = read(pTs_cfg->client_fd, buffer->net_data, TS_BUF_SIZE);

    if(len <= 0){
        printf("read, uart[%d] socket(client) fd %d closed \n", pTs_cfg->uart_cfg.uart_id, pTs_cfg->client_fd);
        return -1;
    }
    pTs_cfg->term_serv_rx_ip_packets ++;
    pTs_cfg->term_serv_rx_ip_cnt += len;

        buffer->length = len;
        buffer->uart_num = pTs_cfg->uart_cfg.uart_id;
        ts_net_msg_proc((net_msg_t *)buffer);	

#endif

#if 1
    if(buffer != (net_msg_t *)ts_dummy_buf) {
        buffer->length = len;
        buffer->uart_num = pTs_cfg->uart_cfg.uart_id;
        ts_net_msg_send(buffer);
    }
#endif    
    return 0;
    
}


static void ts_tcp_receive(term_server_config_t* pTerm_serv_cfg)
{

    cs_int32 fdmax;
    cs_int32 sr;
    fd_set rfds;
    struct timeval  tv;
    struct timeval  *p_timeout = NULL;
	struct timeval  tv_current_time;
    
    while (pTerm_serv_cfg->server_fd >= 0)
    {
        FD_ZERO(&rfds);
        FD_SET(pTerm_serv_cfg->server_fd, &rfds);
        fdmax = pTerm_serv_cfg->server_fd;
//        printf("%s, uart[%d], server fd %d, %d\n", __func__, pTerm_serv_cfg->uart_cfg.uart_id, 
//                                                                    pTerm_serv_cfg->server_fd, __LINE__);
	  tv.tv_sec = 1;
        tv.tv_usec = 0;
        p_timeout = &tv;
		
        if((sr = select(fdmax + 1, &rfds, NULL, NULL, p_timeout)) <= 0)
	  {
            if(errno == EINTR){//Interrupt
                continue;
            }

 //         printf("select(for server) error or timeout\n");

		if(-1 == pTerm_serv_cfg->server_fd)
		{
			ts_server_fd_close(pTerm_serv_cfg);
			break;
		}
//			ts_server_fd_close(pTerm_serv_cfg);
 		break;
        }

        if(FD_ISSET(pTerm_serv_cfg->server_fd, &rfds)) {
            if(ts_new_conn_proc(pTerm_serv_cfg) < 0) {
                ts_server_fd_close(pTerm_serv_cfg);
            }
            break;
        }
    }
    /**/
    while (pTerm_serv_cfg->client_fd >= 0)
    {
        FD_ZERO(&rfds);
        FD_SET(pTerm_serv_cfg->client_fd, &rfds);
        fdmax = pTerm_serv_cfg->client_fd;
        if(pTerm_serv_cfg->client_timeout > 0){
            tv.tv_sec = pTerm_serv_cfg->client_timeout;
            tv.tv_usec = 0;
            p_timeout = &tv;
        }
         tv.tv_sec = 1;
         tv.tv_usec = 0;
          p_timeout = &tv;

#if 1      //by zhangjj    2013.1.18  can reconnect by different  socket port 

        if((sr = select(fdmax + 1, &rfds, NULL, NULL, p_timeout)) <= 0){
            if(errno == EINTR ){				//Interrupt 
                continue;
            }  
//			printf("select(for client) error or timeout\n");
			if(-1 == pTerm_serv_cfg->client_fd)
				{
				  ts_client_fd_close(pTerm_serv_cfg);
				break;
				}
//			gettimeofday(&tv_current_time,NULL);
//            if((tv_current_time.tv_sec - pTerm_serv_cfg->last_act_ticks.tv_sec) < pTerm_serv_cfg->client_timeout) {
//                printf("uart[%d] still active, server fd %d, client fd %d\n", 
//                    pTerm_serv_cfg->uart_cfg.uart_id, pTerm_serv_cfg->server_fd, pTerm_serv_cfg->client_fd);
//                continue;
//            }
 //           ts_client_fd_close(pTerm_serv_cfg);
//            break;
        }
#endif

        if(FD_ISSET(pTerm_serv_cfg->client_fd, &rfds)) {
            if(ts_client_data_proc(pTerm_serv_cfg) < 0){
                ts_client_fd_close(pTerm_serv_cfg);
                break;
            }
        }
    }
    if(pTerm_serv_cfg->server_fd==-1)
    printf("%s , uart[%d], client closed.\n", __func__, pTerm_serv_cfg->uart_cfg.uart_id);  
    pTerm_serv_cfg->client_closed_flag = 1;
    pTerm_serv_cfg->term_serv_rx_ip_packets = 0;
    pTerm_serv_cfg->term_serv_rx_ip_cnt = 0;
    pTerm_serv_cfg->term_serv_tx_ip_packets = 0;
    pTerm_serv_cfg->term_serv_tx_ip_cnt = 0;
    pTerm_serv_cfg->term_serv_tx_lost_packets = 0;
    pTerm_serv_cfg->term_serv_tx_lost_cnt = 0;
    pTerm_serv_cfg->uart_rx_char_cnt = 0;
    pTerm_serv_cfg->uart_tx_char_cnt = 0;

    
}

/*----------------------------------------------
**
**           UDP Connection
**
**---------------------------------------------*/
static cs_status ts_udp_socket_setup(term_server_config_t* pTerm_serv_cfg)
{
    struct sockaddr_in serv_addr;
    //cs_uint32 index;
    struct timeval tv;
    cs_int32 on = 1;
    term_server_config_t *pTsCfg = TS_CFG_GET(pTerm_serv_cfg->uart_cfg.uart_id);
/*    	if((-1!=pTsCfg->server_fd)&&(pTerm_serv_cfg->proto_port == old_port))
		goto END_udp;
	else if((-1!=pTsCfg->server_fd)&&(pTerm_serv_cfg->proto_port != old_port))
	{
		ts_server_fd_close(pTsCfg);
	}*/
    // setup socket
    if((pTsCfg->server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        TS_DBG("ts socket setup failed");
        return CS_E_ERROR;
    }
    
   setsockopt(pTsCfg->server_fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    /* set time out option */    
    tv.tv_sec  = pTsCfg->max_res_time / 1000;
    tv.tv_usec = (pTsCfg->max_res_time % 1000) * 1000;
    if (setsockopt(pTsCfg->server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < -1) {  
        TS_DBG("setsockopt SO_RCVTIMEO");  
       // return (-1);  
    }  
    
    tv.tv_sec  = 1;
    tv.tv_usec = 0;    
    if (setsockopt(pTsCfg->server_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < -1) {  
        TS_DBG("setsockopt SO_SNDTIMEO");  
       // return (-1);  
    }  

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
 //   serv_addr.sin_len = sizeof(serv_addr);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(pTerm_serv_cfg->proto_port);
    if (bind(pTsCfg->server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        TS_DBG("bind failed, uart[%d], server fd %d, port %d,\n", 
            pTerm_serv_cfg->uart_cfg.uart_id, pTsCfg->server_fd, serv_addr.sin_port);
        close(pTsCfg->server_fd);
        pTsCfg->server_fd = -1;
        return CS_E_ERROR;
    }
//END_udp:
    return CS_E_OK;

}


static void ts_udp_receive(term_server_config_t* pTerm_serv_cfg)
{
     socklen_t cli_len;
     fd_set in_fds;
     struct timeval   timeout;
     cs_int32 len,sr;
     struct timeval * p_timeout = NULL; /* default never timeout*/
	 struct timeval tv;
     net_msg_t * buffer;
     cs_int32 dbg_loop;
     cs_uint32 index;
     struct sockaddr_in client_addr;

     while(1)
        {
           if(TERM_SERV_ENABLE_FLAG != pTerm_serv_cfg->enable_flag){
                break;
           }
           
           index = UART_TO_TERM_SERV(pTerm_serv_cfg->uart_cfg.uart_id);

           cli_len = sizeof(struct sockaddr_in);
            
           FD_ZERO(&in_fds);
           FD_SET(pTerm_serv_cfg->server_fd, &in_fds); 
           /* client timeout setting */
           if(pTerm_serv_cfg->client_timeout > 0){
                  timeout.tv_sec = pTerm_serv_cfg->client_timeout;
                  timeout.tv_usec = 0;
                  p_timeout = (struct timeval *)&timeout;
           }

		tv.tv_sec=1;
		tv.tv_usec=0;
		p_timeout = &tv; //modified by zhangjj    2013.1.25  can reconnect by different  socket port 
           if ((sr = select(pTerm_serv_cfg->server_fd + 1, &in_fds, NULL, NULL, p_timeout)) < 0){
                  /* select error */
                  if (errno == EINTR){
                       continue;
                  }


                printf("select(UDP for client) error\n");
                  /* error kick off client */
                  break;
           }
        
           if (0 == sr){
                  /* timeout kick off client */
		//add by zhangjj 2013-1-25

				if(-1 == pTerm_serv_cfg->server_fd)
				{
				printf("shutdown server fd !!!!\n");
				  ts_server_fd_close(pTerm_serv_cfg);
				break;
				}

           }
else{			//add by zhangjj 2013-1-25
           if((buffer = ring_buffer_get(pTerm_serv_cfg)) == NULL){
                //cs_thread_delay(10);
                //continue;
                buffer = (net_msg_t *)ts_dummy_buf;
           }

           memset(buffer, 0, TS_BUF_SIZE);
           memset(&client_addr, 0, sizeof(client_addr));
           if ((len = recvfrom(pTerm_serv_cfg->server_fd, buffer->net_data,
                       TS_BUF_SIZE-sizeof(net_msg_t),0,
                       (struct sockaddr *)&client_addr,&cli_len)) < 0  ) {
                  printf("recvfrom fd %d error %d\n", pTerm_serv_cfg->server_fd, len);
            }
            else
            {   
                /* First client or equal to first client */
                if((0 == pTerm_serv_cfg->client_ip) ||
                   ((ntohl(client_addr.sin_addr.s_addr) == pTerm_serv_cfg->client_ip) &&
                    (ntohs(client_addr.sin_port) == pTerm_serv_cfg->client_port))){
                    /* Client connection setup update flag */
                    pTerm_serv_cfg->client_closed_flag = 0;
                    pTerm_serv_cfg->client_ip   = ntohl(client_addr.sin_addr.s_addr);
                    pTerm_serv_cfg->client_port = ntohs(client_addr.sin_port);
                    pTerm_serv_cfg->term_serv_rx_ip_packets ++;
                    pTerm_serv_cfg->term_serv_rx_ip_cnt += len;
                    if(term_serv_dbg){
                        printf("Server %d received Message : ",index);
                        for( dbg_loop = 0; dbg_loop < len; dbg_loop++)
                         printf("%x ",(buffer->net_data[dbg_loop]));
                        printf(" \n");
                    }
                    //ts_uart_send(pTerm_serv_cfg->uart_cfg.uart_id,buffer, len);
                    if(buffer != (net_msg_t *)ts_dummy_buf) {
                        buffer->length = len;
                        buffer->uart_num = pTerm_serv_cfg->uart_cfg.uart_id;
                        ts_net_msg_send(buffer);
                    }
                    //  cs_thread_delay(50);
                }
             }
     }
}
    /* error or timeout kick off client */
    ts_client_fd_close(pTerm_serv_cfg);
    /* Client closed update flag */
    pTerm_serv_cfg->client_closed_flag = 1;
    pTerm_serv_cfg->term_serv_rx_ip_packets = 0;
    pTerm_serv_cfg->term_serv_rx_ip_cnt = 0;
    pTerm_serv_cfg->uart_rx_char_cnt = 0;
    pTerm_serv_cfg->uart_tx_char_cnt = 0;
    pTerm_serv_cfg->term_serv_tx_ip_packets = 0;
    pTerm_serv_cfg->term_serv_tx_lost_packets = 0;
    pTerm_serv_cfg->term_serv_tx_ip_cnt = 0;
    pTerm_serv_cfg->term_serv_tx_lost_cnt = 0;
     
}

/* Send data from UART to socket */
static cs_int32 sock_send(term_server_config_t * pTerm_serv_cfg,cs_uint8 *buffer, cs_int32 length)
{
    cs_int32 count = 0;
    struct sockaddr_in   client_addr;
    
    if (NULL == pTerm_serv_cfg || NULL == buffer || length < 1) {
        return 0;
    }

    if (IPPROTO_TCP == pTerm_serv_cfg->proto_type && pTerm_serv_cfg->client_fd >= 0) {
        count = send(pTerm_serv_cfg->client_fd, buffer, length, 0); // echo
    }
    else if(IPPROTO_UDP == pTerm_serv_cfg->proto_type){
    
        client_addr.sin_family = AF_INET;  
        client_addr.sin_port   = htons(pTerm_serv_cfg->client_port); 
        client_addr.sin_addr.s_addr = htonl(pTerm_serv_cfg->client_ip); 
        memset(client_addr.sin_zero, 0, sizeof(client_addr.sin_zero));

        count = sendto(pTerm_serv_cfg->server_fd, buffer, length, 0,
                (struct sockaddr*)&client_addr, sizeof(client_addr));
    }

    return count;
    
}

static cs_status check_sock_cfg(
    term_server_config_t *pTerm_serv_cfg
)
{
    cs_uint32 i = 0;
    
    if (NULL == pTerm_serv_cfg) {
        return CS_E_PARAM;
    }
    
    if (IPPROTO_TCP != pTerm_serv_cfg->proto_type 
        && IPPROTO_UDP != pTerm_serv_cfg->proto_type) {
        return CS_E_NOT_SUPPORT;
    }

    /*0-reserved 21-FTP 22-SSH 23-Telnet 25-SMTP 80-HTTP 161-SNMP 513-login*/
    if (pTerm_serv_cfg->proto_port > MAX_SOCKET_PORT 
          || pTerm_serv_cfg->proto_port < MIN_SOCKET_PORT){
//        TERM_DBG_PRINT("Reserved TCP/UDP port %d!\n", pTerm_serv_cfg->proto_port);
        return CS_E_NOT_SUPPORT;
    }
    
    if (0 == pTerm_serv_cfg->client_timeout) {
//        TERM_DBG_PRINT("Client will be never timeout!\n");
    }

    if (pTerm_serv_cfg->client_timeout > MAX_REMOTE_TIMEOUT) {
        TERM_DBG_PRINT("Remote timeout time %d is out of range(0 ~ %d) second!\n", 
            pTerm_serv_cfg->client_timeout, MAX_REMOTE_TIMEOUT);
        return CS_E_NOT_SUPPORT;
    }
        
    if (pTerm_serv_cfg->max_payload > TS_MAX_PAYLOAD_CEILING
        || pTerm_serv_cfg->max_payload < TS_MAX_PAYLOAD_FLOOR) {
        TERM_DBG_PRINT("MAX payload %d is out of range(%d ~ %d)!\n", 
            pTerm_serv_cfg->max_payload, TS_MAX_PAYLOAD_FLOOR, TS_MAX_PAYLOAD_CEILING);
        return CS_E_NOT_SUPPORT;
    }
    
    if (pTerm_serv_cfg->min_payload > pTerm_serv_cfg->max_payload
        || pTerm_serv_cfg->min_payload < TS_MIN_PAD_SIZE) {
        
        TERM_DBG_PRINT("MIN payload %d is out of range(%d ~ max_pad)!\n", 
            pTerm_serv_cfg->min_payload, TS_MIN_PAD_SIZE);
        return CS_E_NOT_SUPPORT;
    }
    
    if (pTerm_serv_cfg->max_res_time < TS_MIN_RSP_TIME
        || pTerm_serv_cfg->max_res_time > TS_MAX_RSP_TIME ) {
        TERM_DBG_PRINT("MAX response time %d is out of range(%d ~ %d)!\n", 
            pTerm_serv_cfg->max_res_time, TS_MIN_RSP_TIME, TS_MAX_RSP_TIME);        
        return CS_E_NOT_SUPPORT;
    }

    /* Check if other terminal is using this port */
    for(i = 0; i < MAX_TERM_SERV_NUM; i ++){
        if((terminal_server_cfg[i]->proto_port == pTerm_serv_cfg->proto_port) 
            && (terminal_server_cfg[i]->proto_type == pTerm_serv_cfg->proto_type)){
            cs_printf("Port %d is already inused\r\n",pTerm_serv_cfg->proto_port);
            return CS_E_CONFLICT;
        }
    }
    
    return CS_E_OK;
    
}

/*----------------------------------------------
**
**         Terminal server threads
**
**---------------------------------------------*/
static void ts_net_rx_process(term_server_config_t* pTerm_serv_cfg)
{
    while (1)
    {
        if(TERM_SERV_ENABLE_FLAG != pTerm_serv_cfg->enable_flag){
          sleep(2); // delay 200 ticks, that is, 2 seconds
        }
        else
        {
            if(IPPROTO_TCP == pTerm_serv_cfg->proto_type){
                ts_tcp_receive(pTerm_serv_cfg);
            }
            else if(IPPROTO_UDP == pTerm_serv_cfg->proto_type){
                ts_udp_receive(pTerm_serv_cfg);
            }
        }
    }
       
}

#ifdef SERIAL_8250

void ts_serial_open_single(int ulPort,int baud,int data,int nstop,int parity_int)
{
	char pname[15]="/dev/ttySA";
	char device_pname[15]={0};
	char parity;

	if(1 == parity_int)
		parity = 'o';
	else if (2 == parity_int)
		parity = 'e';
	else 
		parity = 'n';
	vosSprintf(device_pname, "%s%d",pname,ulPort);

	if(-1 != serial_fds[ulPort])
		portComClose(serial_fds[ulPort]);
	serial_fds[ulPort] = portComOpen(device_pname,baud,data,nstop,parity);
	if(-1 == serial_fds[ulPort])
	{
		printf("port %d open error!\n", ulPort+1);
	}
	else
		printf("port %d open success!\n", ulPort+1);
}

void ts_serial_open(int baud,int data,int nstop,int parity_int)
{
	int i=0;
	char parity='n';
	
	serial_fds[0] = portComOpen(TTYSA0,baud,data,nstop,parity);
	serial_fds[1] = portComOpen(TTYSA1,baud,data,nstop,parity);
	serial_fds[2] = portComOpen(TTYSA2,baud,data,nstop,parity);
	serial_fds[3] = portComOpen(TTYSA3,baud,data,nstop,parity);

	for(i=0; i<TEST_PORT_NUM; i++)
	{
		if(-1 == serial_fds[i])
		{
			printf("port %d open error!\n", i+1);
		}
		else
			printf("port %d open success!\n", i+1);
	}
}


static void ts_serial_monitor_thread()
{
	int ret=0;
	char buf[1024], xmit[1024];
	int i=0;

	struct timeval tv_current_time,tv_current_time1;
	cs_uint8  term_id ;
	cs_uint8 *buffer = NULL;
	cs_uint32  offset = 0;
	static uart_msg_t uart_msg;
	uart_msg_t *msg = &uart_msg;

	
//ts_serial_open(); //for test
	
	while(1)
	{
		int len=0;

		if((serial_fds[0]==-1)||(serial_fds[1]==-1)||(serial_fds[2]==-1)||(serial_fds[3]==-1))
			continue;
		else
		{
			for(i=0; i<4; i++)
			{
				len = portComRecv(serial_fds[i], buf, 1024, 200);
				if(len > 0)
				{
					term_server_config_t *pTsCfg = TS_CFG_GET(i+1);
		
					term_id = UART_TO_TERM_SERV(i+1);
					if((ts_uart_ring[term_id].wr_pos+60) % TERM_SRV_UART_MAX_SIZE ==ts_uart_ring[term_id].rd_pos){
						buffer = NULL;
						printf("UART [%d] full. wrpos %d ,  rdpos %d ,  wrcount  %d ,rdcount  %d \n",i,
							ts_uart_ring[term_id].wr_pos, ts_uart_ring[term_id].rd_pos,
							ts_uart_ring[term_id].wr_count,ts_uart_ring[term_id].rd_count);
		
					}
					else{
						buffer = (cs_uint8 *)&ts_uart_ring[term_id].data[0];
						offset = ts_uart_ring[term_id].wr_pos;
					}
					for(i=0;i<len;i++)
					{
						if(buffer){
							buffer[offset] = buf[i];
							pTsCfg->uart_rx_char_cnt++;
							ts_uart_ring[term_id].wr_count++;
							offset = (offset + 1)%TERM_SRV_UART_MAX_SIZE;
	
						}
					}
				ts_uart_ring[term_id].wr_pos = offset;
				if(TS_ENABLED(term_id))
				{
//				gettimeofday(&tv_current_time,NULL);
//					pTsCfg->last_act_ticks.tv_sec= tv_current_time.tv_sec;
					if(ts_uart_ring[term_id].wr_count - ts_uart_ring[term_id].rd_count >0){
	//			printf("UART[%d] send message to tx thread  \n",ulPort);
						vosMsqSend(msgq_ts_uart_rx,(void *)&msg,sizeof(cs_uint32));
					}
				}
				
	//				printf("port %d recieved chars(len=%d):\n", i+1, len);
	//				dump_pkt(buf, len);
				}
			}
		}  //else
	}

}


void ts_serial_send(unsigned long ulPort, unsigned char *buf, int len)
{
	if(-1 !=serial_fds[ulPort])
	{
		portComSend(serial_fds[ulPort],buf, len);
	}
	else
	{
		printf("the device of uart is not open ,error!\r\n");
		return ;
	}
	
}

#endif

static void ts_net_rx_thread(cs_uint32 data )
{
    ts_net_rx_process(terminal_server_cfg[data]); 
}

static void ts_def_uart_rx_func( cs_int32 uart, unsigned char *data, cs_int32 len)
{
    term_server_config_t  *ps = NULL;

    if (ts_uart_id_check(uart)) {
        return ;
    }
      
    ps = terminal_server_cfg[UART_TO_TERM_SERV(uart)];
    
    if (TERM_SERV_ENABLE_FLAG != ps->enable_flag){
        printf("Term is not enable!\n");
        return;
    }
    
//    printf("Uart %d send to client len %d!\n", uart, len);
    if (sock_send(ps, data, len) > 0) {
        ps->term_serv_tx_ip_packets ++;
        ps->term_serv_tx_ip_cnt += len;
    }
    else {
        ps->term_serv_tx_lost_packets ++;
        ps->term_serv_tx_lost_cnt += len;
    }
    
    return;
    
}


static void ts_uart2net_thread(void)
{
    cs_uint8 term_id = 0;
    uart_msg_t msg;
    cs_uint32 ret = 0;
    cs_uint32 size_copy=0;

    while(1)
    {
//        msg = NULL;
        ret = vosMsqReceive(msgq_ts_uart_rx,(void *)&msg,
                sizeof(cs_uint32));
        if (ret == INVALID_32)
            continue;
        /* time out case */
        for(term_id=0; term_id<MAX_TERM_SERV_NUM; term_id++) {
            if(TS_ENABLED(term_id)){
                if(ts_uart_ring[term_id].wr_count - ts_uart_ring[term_id].rd_count > 0){
                    ts_uart2net_proc(term_id);
                }
            }
        }

    }
}


/* process transimit packets from network to uart */
static void ts_net2uart_thread(void)
{
	int ret=0;
    uart_msg_t *msg=NULL;
    cs_uint32 size_copy = 0;
    
    while(1)
    {
//       memset(&msg,0,sizeof(uart_msg_t));
        msg = NULL;
vosMsqReceive(msgq_ts_net_rx,(void *)&msg,sizeof(cs_uint32));
        if(NULL == msg){
            continue;
       }
//        if (ret == INVALID_32)
//        {
//            continue;
//        }
        ts_net_msg_proc((net_msg_t *)msg);

    }
}


static void ts_thread_create(void)
{
#if 0
    /* The tasks process receiveing IP packets */
    cs_thread_create(&ip_rx0_thread_id, "Term Serv Rx0",
            ts_net_rx_thread,(void *)0,TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_IP_RX_THREAD_PRIORITY, 0);        
    printf("term_server0_rx_thread created\n");

    cs_thread_create(&ip_rx1_thread_id, "Term Serv Rx1",
            ts_net_rx_thread,(void *)1,TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_IP_RX_THREAD_PRIORITY, 0);        
    printf("term_server1_rx_thread created\n");

    cs_thread_create(&ip_rx2_thread_id, "Term Serv Rx2",
            ts_net_rx_thread,(void *)2,TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_IP_RX_THREAD_PRIORITY, 0);        
    printf("term_server2_rx_thread created\n");

    cs_thread_create(&ip_rx3_thread_id, "Term Serv Rx3",
            ts_net_rx_thread,(void *)3,TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_IP_RX_THREAD_PRIORITY, 0);        
    printf("term_server3_rx_thread created\n");   

    /* The task process transmitting UART data */
    cs_thread_create(&uart2net_thread_id, "Term Serv uart2net",
            ts_uart2net_thread,NULL,TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_UART2NET_THREAD_PRIORITY, 0);        
    printf("ts_uart2net_thread created\n");

    /* The task process transmitting net data to uart*/
    cs_thread_create(&net2uart_thread_id, "Term Serv net2uart",
            ts_net2uart_thread,NULL,TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_NET2UART_THREAD_PRIORITY, 0);        
    printf("ts_net2uart_thread created\n");
#else


#ifdef SERIAL_8250
	serial_monitor_id  = vosThreadCreate(
		"serial monitor",
		TERM_SERV_THREAD_STACKSIZE,
		TERM_SERV_IP_RX_THREAD_PRIORITY,
            (FUNCPTR)ts_serial_monitor_thread,
            (void *)0);
#endif


    /* The tasks process receiveing IP packets */
	ip_rx0_thread_id = vosThreadCreate (
		"Term Serv Rx0",
		TERM_SERV_THREAD_STACKSIZE,
		TERM_SERV_IP_RX_THREAD_PRIORITY,
            (FUNCPTR)ts_net_rx_thread,
            (void *)0);        
//    printf("term_server0_rx_thread created\n");

    ip_rx1_thread_id = vosThreadCreate (
		"Term Serv Rx1",
		TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_IP_RX_THREAD_PRIORITY,
            (FUNCPTR)ts_net_rx_thread,
            (void *)1);        
//    printf("term_server1_rx_thread created\n");

    ip_rx2_thread_id = vosThreadCreate (
		"Term Serv Rx2",
		TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_IP_RX_THREAD_PRIORITY, 
            (FUNCPTR)ts_net_rx_thread,
            (void *)2);        
//    printf("term_server2_rx_thread created\n");

    ip_rx3_thread_id = vosThreadCreate (
		"Term Serv Rx3",
		TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_IP_RX_THREAD_PRIORITY,
            (FUNCPTR)ts_net_rx_thread,
            (void *)3);        
//    printf("term_server3_rx_thread created\n");   

    /* The task process transmitting UART data */
    uart2net_thread_id = vosThreadCreate ( 
    		"Term Serv uart2net",
    		TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_UART2NET_THREAD_PRIORITY,
            (FUNCPTR)ts_uart2net_thread,
            NULL);        
//    printf("ts_uart2net_thread created\n");

    /* The task process transmitting net data to uart*/
    net2uart_thread_id = vosThreadCreate ( 
    		"Term Serv net2uart",
    		TERM_SERV_THREAD_STACKSIZE,
            TERM_SERV_NET2UART_THREAD_PRIORITY,
            (FUNCPTR)ts_net2uart_thread,
            NULL);        
//    printf("ts_net2uart_thread created\n");
#endif
    
}


/* Create a terminal server and attach it with a given uart. */
cs_status ts_enable(term_server_config_t* pTerm_serv_cfg)
{
//printf("server_fd is %d  proto_port is %d  old_port is %d\n",pTerm_serv_cfg->server_fd,pTerm_serv_cfg->proto_port,old_port);
    cs_status status;
    cs_uint32 i = 0;
    term_server_config_t *pTsCfg = NULL;
    
    if (NULL == pTerm_serv_cfg) {
        return  CS_E_PARAM;
    }
    pTsCfg = TS_CFG_GET(pTerm_serv_cfg->uart_cfg.uart_id);
    if(TERM_SERV_ENABLE_FLAG == pTsCfg->enable_flag){
        printf("Uart %d has been enabled\r\n",pTerm_serv_cfg->uart_cfg.uart_id);
        return CS_E_CONFLICT;
    }

    if(CS_E_OK != check_uart_cfg(&pTerm_serv_cfg->uart_cfg)){
        printf("Wrong uart Configuration !\n");
        return CS_E_PARAM;
    }
    
    if(CS_E_OK != check_sock_cfg(pTerm_serv_cfg)){
        printf("Wrong TCP/IP Configuration !\n");
        return CS_E_PARAM;
    }
    memcpy(pTsCfg, pTerm_serv_cfg, sizeof(term_server_config_t));

    /* init IP receive ring buffer */
    for(i=0; i<MAX_RING_BUF_NUM; i++){
        if((pTsCfg->ip_rx_buf.pBuf[i] = iros_malloc(IROS_MID_SYS,TS_BUF_SIZE)) == NULL){
            printf("Terminal server enable failed for no memory\r\n");
            return CS_E_RESOURCE;
        }
    }

    ts_uart_init((uart_config_t *)&pTerm_serv_cfg->uart_cfg);

    if(IPPROTO_TCP == pTerm_serv_cfg->proto_type){
        status = ts_tcp_socket_setup(pTerm_serv_cfg);
    }
    else if(IPPROTO_UDP == pTerm_serv_cfg->proto_type){
        status = ts_udp_socket_setup(pTerm_serv_cfg);
    }
    if(status == 0){
        pTsCfg->enable_flag = TERM_SERV_ENABLE_FLAG;
	pTerm_serv_cfg->enable_flag = TERM_SERV_ENABLE_FLAG;

    }

    return status;

}
#if 1
/* Disable the service completely and free the related IP/UART */
/* communication resource. */
void ts_disable(cs_uint8 uart)
{
    cs_uint32 i;
    term_server_config_t *pTsCfg = TS_CFG_GET(uart);
    pTsCfg->enable_flag = 0;
    /* Disable UART */
//    ts_onu_uart_disable(uart);
    
    
     /* TCP socket: kick off client connection */
    if(IPPROTO_TCP == pTsCfg->proto_type){
        ts_client_fd_close(pTsCfg);
    }
    
    ts_server_fd_close(pTsCfg);

    for(i=0; i<MAX_RING_BUF_NUM; i++){
        iros_free(pTsCfg->ip_rx_buf.pBuf[i]); 
    }
   
    memset(pTsCfg,0,sizeof(term_server_config_t));
    pTsCfg->client_fd = -1;
    pTsCfg->server_fd = -1;
    /* Client closed update flag */
    pTsCfg->client_closed_flag = 1;

//printf("server_fd is %d  proto_port is %d  old_port is %d\n",pTsCfg->server_fd,pTsCfg->proto_port,old_port);
}
#endif
/* Terminate the current remote connect and reset the service to */
/* wait for new connections. */
void ts_reset(cs_uint8 uart)
{
    term_server_config_t *pTsCfg = TS_CFG_GET(uart);
    /* TCP socket: kick off client connection */
    if(IPPROTO_TCP == pTsCfg->proto_type){
        ts_client_fd_close(pTsCfg);
    }
    /* Client closed update flag */
    pTsCfg->term_serv_rx_ip_packets = 0;
    pTsCfg->term_serv_tx_ip_packets = 0;
    pTsCfg->term_serv_tx_lost_packets = 0;
    pTsCfg->term_serv_rx_ip_cnt = 0;
    pTsCfg->term_serv_tx_ip_cnt = 0;
    pTsCfg->term_serv_tx_lost_cnt = 0;
    /* clear the client information */
    pTsCfg->client_closed_flag = 1;
    pTsCfg->client_ip = 0;
    pTsCfg->client_port = 0;

    pTsCfg->uart_rx_char_cnt = 0;
    pTsCfg->uart_tx_char_cnt = 0;
    
}

/* Dump terminal server status (running or waiting), uart configure */
/* parameters, rx/tx IP packets and remote IP & port numbers.*/
void ts_show(cs_uint8 uart)
{
    cs_uint32 index = 0;
    struct in_addr ipv4;
    uart_config_t *pUart = NULL;
    term_server_config_t *pTsCfg = TS_CFG_GET(uart);
    
    index = UART_TO_TERM_SERV(uart);
    pUart = (uart_config_t *)&(pTsCfg->uart_cfg);
    ipv4.s_addr = htonl(pTsCfg->client_ip);
    if(TERM_SERV_ENABLE_FLAG !=pTsCfg->enable_flag) {
        cs_printf("%-30s: %s\n", "Terminal Server Status", "Disabled");
    }
    else if(0 == pTsCfg->client_closed_flag)
    {
        cs_printf("%-30s: %s\n", "Terminal Server Status", "Running");
        cs_printf("%-30s: %s\n", "Remote connect IP",  inet_ntoa(ipv4));
        cs_printf("%-30s: %d\n", "Remote connect port", ntohs(pTsCfg->client_port));
    }
    else {
        cs_printf("%-30s: %s %d\n", "Terminal Server Status", "Waiting at port",
                                                                pTsCfg->proto_port);
    }
    cs_printf("%-30s: %d\n", "Term Server Rx IP packets", pTsCfg->term_serv_rx_ip_packets);
    cs_printf("%-30s: %d\n", "Term Server Rx IP count", pTsCfg->term_serv_rx_ip_cnt);
    cs_printf("%-30s: %d\n", "Term Server Tx IP packets", pTsCfg->term_serv_tx_ip_packets);
    cs_printf("%-30s: %d\n", "Term Server Tx IP count", pTsCfg->term_serv_tx_ip_cnt);
    cs_printf("%-30s: %d\n", "Term Server Tx lost packets", pTsCfg->term_serv_tx_lost_packets);
    cs_printf("%-30s: %d\n", "Term Server ip_rx_buf.wr_count", pTsCfg->ip_rx_buf.wr_count);
    cs_printf("%-30s: %d\n", "Term Server ip_rx_buf.rd_count", pTsCfg->ip_rx_buf.rd_count);
    cs_printf("%-30s: %d\n", "Term Server ip_rx_buf.pIngress", pTsCfg->ip_rx_buf.pIngress);
    cs_printf("%-30s: %d\n", "Term Server ip_rx_buf.pEgress", pTsCfg->ip_rx_buf.pEgress);


    cs_printf("%-30s:\n", "Term Server Config");
    cs_printf("%30s: %s%d\n", "Uart ID", "uart",pUart->uart_id);
    cs_printf("%30s: %d\n", "Uart baudrate", pUart->baud_rate);
    cs_printf("%30s: %d\n", "Uart databits", pUart->data_bits);
    cs_printf("%30s: %ld\n", "Uart received bytes: ", pTsCfg->uart_rx_char_cnt);
    cs_printf("%30s: %ld\n", "Uart transimitted bytes: ", pTsCfg->uart_tx_char_cnt);
    switch(pUart->parity) {
            case UART_PARITY_NONE:
        {
            cs_printf("%30s: %s\n", "Uart parity", "None");
            break;
        }
            case UART_PARITY_ODD:
        {
            cs_printf("%30s: %s\n", "Uart parity", "Odd");
            break;
        }
            case  UART_PARITY_EVEN:
        {
            cs_printf("%30s: %s\n", "Uart parity", "Even");
            break;
        }
        default:
            break;
    }
    if(pTsCfg->proto_type == IPPROTO_TCP) {
        cs_printf("%30s: %s\n", "Connect protocol ","TCP");
    }
    else if(pTsCfg->proto_type == IPPROTO_UDP)  {
        cs_printf("%30s: %s\n", "Connect protocol ","UDP");
    }
    else {
        cs_printf("%30s: %s\n", "Connect protocol ","Not set");
    }
    cs_printf("%30s: %d\n", "Connect port", pTsCfg->proto_port);
    cs_printf("%30s: %d s\n", "Connect timeout", pTsCfg->client_timeout);
    cs_printf("%30s: %d \n", "Packet min payload", pTsCfg->min_payload);
    cs_printf("%30s: %d\n", "Packet max payload", pTsCfg->max_payload);
    cs_printf("%30s: %d ms\n", "Server max response time", pTsCfg->max_res_time);
    cs_printf("%30s: %d \n", "Server fd", pTsCfg->server_fd);
    cs_printf("%30s: %d \n", "client fd", pTsCfg->client_fd);
    cs_printf("%-30s: %d\n", "Term Server wr_pos", ts_uart_ring[index].wr_pos);
    cs_printf("%-30s: %d\n", "Term Server wr_count", ts_uart_ring[index].wr_count);
    cs_printf("%-30s: %d\n", "Term Server rd_pos", ts_uart_ring[index].rd_pos);
    cs_printf("%-30s: %d\n", "Term Server rd_count", ts_uart_ring[index].rd_count);
}



/* Auto create terminal server base on user data area */
void ts_auto_create(void)
{
#ifdef HAVE_DB_MANAGMENT
//system("touch /cfg/ts_cfg");
    cs_uint32 num;
    int  ret; 

    term_server_config_t* pTemp = NULL;
    term_server_config_t * pConfig = NULL;
    
    /* Auto create terminal server according to saved data */
    if((pConfig = (term_server_config_t *)iros_malloc(IROS_MID_SYS,sizeof(term_server_config_t))) == NULL){
            printf("Terminal server create failed for no memory\r\n");
            return;
    }

    if((pTemp = iros_malloc(IROS_MID_SYS,sizeof(db_ts_cfg_t))) == NULL){
            printf("Terminal server create failed for no memory\r\n");
            iros_free(pConfig);
            return;
    }
	


    for(num = TS_MIN_UART_NUM; num <= MAX_TERM_SERV_NUM; num++)
    {
            /*check saved configuration */
        memset(pConfig, 0, sizeof(term_server_config_t));
 //       memset(pTemp, 0, sizeof(db_ts_cfg_t));
        
        ret = ts_get_config(num,(term_server_config_t *)pConfig);
#if 1
		if(ERROR == ret)
		{
//			ts_init_default();

			pConfig->init_flag = VALID_CONFIG_FLAG;
			pConfig->enable_flag = TERM_SERV_ENABLE_FLAG;
			pConfig->proto_type = 6;
      		pConfig->proto_port = 10000+num;
			pConfig->uart_cfg.data_bits = 8;
           		pConfig->uart_cfg.stop_bits = 1;
			pConfig->uart_cfg.parity = 0;
	            pConfig->uart_cfg.baud_rate = 9600;
		}

        if(TERM_SERV_ENABLE_FLAG == pConfig->enable_flag){
               /* uart config */
		pConfig->enable_flag = 0;
           pConfig->uart_cfg.uart_id = num;

           pConfig->uart_cfg.flow_control = 0;

           pConfig->uart_cfg.duplex = UART_DUPLEX_HALF;
           pConfig->uart_cfg.mode = UART_MODE_INTR;

            
           /* protocol config*/
 //          pConfig->proto_type = pTemp->proto_type;
//           pConfig->proto_port = pTemp->proto_port;
           pConfig->client_timeout = 0;
           pConfig->max_payload = 1024;
           pConfig->min_payload = 40;
           pConfig->max_res_time = 1000;
           
           /* clear status saved before */
           pConfig->server_fd = -1;
           pConfig->client_fd = -1;
           pConfig->client_ip= 0;
           pConfig->client_port= 0;
           pConfig->client_closed_flag = 1;
           pConfig->term_serv_rx_ip_packets = 0;
           pConfig->term_serv_rx_ip_cnt = 0;
           pConfig->uart_rx_char_cnt = 0;
           pConfig->uart_tx_char_cnt = 0;
           pConfig->term_serv_tx_ip_packets = 0;
           pConfig->term_serv_tx_lost_packets = 0;
           pConfig->term_serv_tx_ip_cnt = 0;
           pConfig->term_serv_tx_lost_cnt = 0;
 //          pConfig->enable_flag = 0;
           //pConfig->last_act_ticks = 0;

 #endif
            if(ts_enable(pConfig)){
                    printf("Terminal Server %d create failed\n\r",UART_TO_TERM_SERV(num));
            }
            else {
                    printf("Terminal Server %d create sucess\n\r",UART_TO_TERM_SERV(num));
            }
		ts_save_config(num,(term_server_config_t *)pConfig);
        }

    }
    
    iros_free(pConfig);
    iros_free(pTemp);
#endif    
}


void ts_init(void)
{
    cs_int32 ret = 0;
    cs_uint32 num = 0;

#if 1         //add by zhangjj 2013.1.23      resume  ip from the saved terminal server configuration  

	char ip_msg[20]={0};

    if((ret = ts_get_ip_cfg(ip_msg,LAN_IP))!=-1)
	{
		char ip_set[30];

		printf("Set TS IP (LAN)  as config saved  %s\n",ip_msg);
		vosSprintf(ip_set,"ifconfig eth0 %s",ip_msg );
		system(ip_set);
	}
	else
	{
		printf("Set default TS IP (LAN) as 192.168.0.1\n");
		system("ifconfig eth0 192.168.0.1");
	}
    if((ret = ts_get_ip_cfg(ip_msg,WAN_IP))!=-1)
	{
		char ip_set[30];

		printf("Set TS IP (WAN) as config saved  %s\n",ip_msg);
		vosSprintf(ip_set,"ifconfig eth4 %s",ip_msg );
		system(ip_set);
	}
	else
	{
		printf("Set default TS IP (WAN) as 192.168.1.1\n");
		system("ifconfig eth4 192.168.1.1");
	}
	if((ret = telnet_ip_get(ip_msg))!=-1)
	{
		char ip_set[30];

		printf("Set Telnet IP as config saved  %s\n",ip_msg);
		vosSprintf(ip_set,"ifconfig eth3 %s",ip_msg );
		system(ip_set);
	}
	else
	{
		printf("Set default Telnet IP as 192.168.1.2\n");
		system("ifconfig eth3 192.168.1.2");
	}
	
#endif

	cs_osal_core_init();

    for(num = 0; num < MAX_TERM_SERV_NUM; num++)
    {
        if((terminal_server_cfg[num] = iros_malloc(IROS_MID_SYS,sizeof(term_server_config_t))) == NULL)
	  {
            printf("Terminal server init failed for no memory\r\n");
            return;
        }
        memset(terminal_server_cfg[num], 0, sizeof(term_server_config_t));
    }

#if 0
    /* create queue for uart frame RX */
    ret = cs_queue_create(&msgq_ts_uart_rx, "TS_UART_RX_Q", 
             TS_UART_RX_QUEUE_DEPTH, TS_UART_RX_QUEUE_SIZE, 0);
    
    if(CS_E_OSAL_OK != ret) {
        printf("msgq_ts_uart_rx q create failed, 0x%x\n", ret);
        return;
    }

    /* create queue for ethernet frame RX */
    ret = cs_queue_create(&msgq_ts_net_rx, "TS_IP_RX_Q", 
             TS_IP_RX_QUEUE_DEPTH, TS_UART_RX_QUEUE_SIZE, 0);
    
    if(CS_E_OSAL_OK != ret) {
        printf("msgq_ts_net_rx q create failed, 0x%x\n", ret);
        return;
    }
#else 
/* create queue for uart frame RX */
	msgq_ts_uart_rx = vosMsqCreate(TS_UART_RX_QUEUE_DEPTH+TS_UART_RX_QUEUE_SIZE);
  
    if(-1 == msgq_ts_uart_rx) {
        printf("msgq_ts_uart_rx q create failed, 0x%x\n", ret);
        return;
    }

    /* create queue for ethernet frame RX */
    msgq_ts_net_rx = vosMsqCreate(TS_UART_RX_QUEUE_DEPTH+TS_UART_RX_QUEUE_SIZE); 
    
    if(-1 == msgq_ts_net_rx) {
        printf("msgq_ts_uart_rx q create failed, 0x%x\n", ret);
        return;
    }    
#endif

    ts_thread_create();
     ts_auto_create();
}
#endif



