/*
 * $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/pppoe/ppp-2.4.4/pppd/plugins/radius/md5.c#1 $
 */
#include "md5.h"

void rc_md5_calc (unsigned char *output, unsigned char *input, unsigned int inlen)
{
	MD5_CTX         context;

	MD5_Init (&context);
	MD5_Update (&context, input, inlen);
	MD5_Final (output, &context);
}
