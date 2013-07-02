#ifndef __TERM_SERVER_ADAPT_H__
#define __TERM_SERVER_ADAPT_H__



cs_status ts_uart_init(uart_config_t *uart_cfg);
cs_status ts_putc(cs_uint8 uart,char ch);
cs_status ts_getc(cs_uint8 uart,cs_uint8 * ch);
void ts_uart_send(cs_uint8 uart,char *buffer,unsigned int len);
cs_status ts_onu_uart_disable(cs_uint8 uart_id);
cs_status ts_onu_uart_enable(cs_uint8 uart_id);
int ts_get_config(cs_uint8 uart_id, cs_uint8 * buff, cs_int32 len);
int ts_get_ip_cfg(char *ip,int ip_type);
int ts_save_config(cs_uint8 uart_id, cs_uint8 * buff, cs_int32 len);
int telnet_ip_save(char *telnetIP);
int telnet_ip_get(char *ip);

#endif
