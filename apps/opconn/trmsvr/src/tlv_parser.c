#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "tlv_parser.h"
#include "cs_types.h"
//#include "osal_common.h"


unsigned char tlv_get_char(char *pbuf)
{
    return (unsigned char)pbuf[0];
}



unsigned short tlv_get_short(char *pbuf)
{
    unsigned short value = 0;

    memcpy(&value , pbuf , sizeof(unsigned short));
    return value;
}



int tlv_read_char
    (
    unsigned int instance_id,
    unsigned short type, 
    unsigned char *value
    )
{
    unsigned int i;
    tlv_t *pTlv = NULL;
    unsigned short tlv_type = 0;
    unsigned short tlv_len = 0;
    tlv_instance_t *pInst = (tlv_instance_t *)instance_id;

    if(pInst == NULL)
        return TLV_PARSER_ERROR;
    
    if(pInst->tlv_ptr_list == NULL)
    {
     //   startup_cfg_printf("\r\n Maybe startup-cfg hasn't been loaded.");
        return TLV_PARSER_ERROR;
    }

    if(value == NULL)
    {
      //  startup_cfg_printf("\r\n Input an invalid parameter.");
        return TLV_PARSER_ERROR;
    }

    for(i = 0 ; i < pInst->tlv_num ; i++)
    {
        pTlv = (tlv_t *)pInst->tlv_ptr_list[i];
        if(pTlv == NULL)
            continue;

        tlv_type = tlv_get_short((char *)pTlv+TLV_TYPE_OFFSET);
        tlv_len = tlv_get_short((char *)pTlv+TLV_LEN_OFFSET);
        if(type != ntohs(tlv_type))
            continue;

        if(ntohs(tlv_len) > sizeof(char))
        {
       //     startup_cfg_printf("\r\n Attempt to read a cfg which length is %d into a char.",ntohs(pTlv->len));
            return TLV_PARSER_ERROR;
        }

        *value = tlv_get_char((char *)pTlv+TLV_VALUE_OFFSET);
        return TLV_PARSER_OK;
    }

    return TLV_PARSER_ERROR;
}


