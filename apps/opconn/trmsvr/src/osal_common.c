#include "osal_common.h"

cs_int8 bitMask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void cs_bit_set(cs_int8 *pbuf , cs_int32 bit_pos)
{
    pbuf[bit_pos/8] |= bitMask[bit_pos%8];
}


cs_int32 cs_bit_tst(cs_int8 *pbuf , cs_int32 bit_pos)
{
    if (pbuf[bit_pos/8] & bitMask[bit_pos%8])
        return 1;
    else
        return 0;
}

void cs_bit_clr(cs_int8 *pbuf , cs_int32 bit_pos)
{
    pbuf[bit_pos/8] &= ~(bitMask[bit_pos%8]);
}



cs_int32 cs_bit_get_1(cs_int8 *pbuf , cs_int32 total_bit)
{
    cs_int32 intNum = 0;
    cs_int32 left_bits = 0;
    cs_int32 *pInt = (cs_int32 *)pbuf;
    int i , j;

    intNum = total_bit / 32;
    left_bits = total_bit % 32;
    for (i = 0 ; i < intNum ; i++) {
        if (pInt[i] == 0)
            continue;

        for (j = 0 ; j < 32 ; j++) {
            if (cs_bit_tst((cs_int8 *)&pInt[i] , j))
                return (i*32 + j);
        }
    }

    for (j = 0 ; j < left_bits ; j++) {
        if (cs_bit_tst((cs_int8 *)&pInt[intNum] , j))
            return (intNum*32 + j);
    }

    return -1;
}
