#include "iros_config.h"
#include "plat_common.h"
//#include "registers.h"
#include "uart_default.h"
#include "sdl_uart.h"
#include "cs_cmn.h"
//#include "serial.h"


cs_status cs_plat_uart_enable(
    cs_dev_id_t             device,
    cs_llid_t               llidport,
    cs_uint8                uart_id
)
{
/*
    uart_cfg_t config;
    uart_cfg_mask_t mask;
    cs_status ret = CS_E_OK;

    if(uart_id >= MAX_UART_NUM)
        return CS_E_PARAM;

    mask.u32 = 0;
    mask.s.enable = 1;

    config.enable = UART_ENABLE;

    uart_cfg_set(uart_id, mask, &config);
    
    ret = uart_ctrl_set(uart_id, UART_ENABLE);

    return ret;
    */

}

cs_status uart_ctrl_set(cs_uint8 uart_id,uart_ctrl_t ctrl)
{

	Serial_Cs_Set(uart_id);
	UARTIntEnable(ctrl);
}

cs_status cs_plat_uart_disable(
    cs_dev_id_t             device,
    cs_llid_t               llidport,
    cs_uint8                uart_id
)
{
    cs_status ret = CS_E_OK;

    uart_ctrl_set(uart_id, UART_DISABLE);

    return ret;
}
/*
    cs_plat_uart_baudrate_set(context,0,0,uart_cfg->uart_id,uart_cfg->baud_rate);
    cs_plat_uart_databits_set(context,0,0,uart_cfg->uart_id,uart_cfg->data_bits);
    cs_plat_uart_parity_set(context,0,0,uart_cfg->uart_id,uart_cfg->parity);
    cs_plat_uart_mode_set(context,0,0,uart_cfg->uart_id,uart_cfg->mode);
    cs_plat_uart_duplex_set(context,0,0,uart_cfg->uart_id,uart_cfg->duplex);
*/
#if 0

cs_status cs_plat_uart_read(
    cs_dev_id_t              device,
    cs_llid_t                llidport,
    cs_uint8                 uart_id,
    cs_uint8                 *data
)
{
	if(uart_id >= MAX_UART_NUM )
        return CS_E_PARAM;
	if(uart_getc_nonblock(uart_id,data))
		return CS_E_OK;
	else
		return CS_E_ERROR;
}

#endif


cs_status cs_plat_uart_write(
    cs_dev_id_t              device,
    cs_llid_t                llidport,
    cs_uint8                 uart_id,
    cs_uint32                len,
    cs_uint8                 *data
)
{
    cs_uint32 loop;
    cs_status ret = CS_E_OK;

    if(uart_id >= MAX_UART_NUM || 0 == len)
        return CS_E_PARAM;
#if 1
Serial_Cs_Set(uart_id);
    for(loop = 0; loop < len; loop++)
    {
        UARTCharPut(*(data + loop));
        if(ret != CS_E_OK)
        {
            ret = CS_E_ERROR;
            break;
        }
    }
#else
if (OK == SerialTxRdy(ulPort))
    {
        for(i = 0; ((!RingBufEmpty(&g_sTxBuf[ulPort])) && (i < TX_FIFO_LEN)); i++)
        {
            //
            // Write the next character into the transmit FIFO.
            
            UARTCharPut(RingBufReadOne(&g_sTxBuf[ulPort]));
        }
    }
#endif
    return ret;
}

