#ifndef __SERIAL_OPTION__
#define __SERIAL_OPTION__
#include "ringbuf.h"
typedef void (*SerialSend_t)(unsigned long ulPort, unsigned char *buf, int len);
typedef void (*SerialSetBaudRate_t)(unsigned long ulPort, unsigned long ulBaudRate);
typedef void (*SerialSetDataSize_t)(unsigned long ulPort, unsigned char ucDataSize);
typedef void (*SerialSetStopBits_t)(unsigned long ulPort, unsigned char ucStopBits);
typedef void (*SerialSetParity_t)(unsigned long ulPort, SERIAL_PARITY Parity);
typedef void (*RingBufRead_t)(tRingBufObject *ptRingBuf, unsigned char *pucData,unsigned long ulLength);

typedef struct serial_option{
	int type;
	SerialSetBaudRate_t 	SerialSetBaudRate;
	SerialSetDataSize_t 	SerialSetDataSize;
	SerialSetStopBits_t 		SerialSetStopBits;
	SerialSetParity_t 		SerialSetParity;
	SerialSend_t 			SerialSend;
	RingBufRead_t			SerialRead;
}Serial_Option;

#endif

