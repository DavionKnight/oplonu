#ifndef __SDL_UART_H__
#define __SDL_UART_H__
#include "cs_cmn.h"
typedef enum {
	UART_DISABLE  	 =0,
	UART_ENABLE		 =1
}uart_ctrl_t;


cs_status cs_plat_uart_write(   
    cs_dev_id_t              device,
    cs_llid_t                llidport,
    cs_uint8                 uart_id,
    cs_uint32                len,
    cs_uint8                 *data);

cs_status cs_plat_uart_read(
    cs_dev_id_t              device,
    cs_llid_t                llidport,
    cs_uint8                 uart_id,
    cs_uint8                 *data
);

cs_status uart_ctrl_set(cs_uint8 uart_id,uart_ctrl_t ctrl);

cs_status cs_plat_uart_enable(
    cs_dev_id_t             device,
    cs_llid_t               llidport,
    cs_uint8                uart_id
);
cs_status cs_plat_uart_disable(
    cs_dev_id_t             device,
    cs_llid_t               llidport,
    cs_uint8                uart_id
);

#endif
