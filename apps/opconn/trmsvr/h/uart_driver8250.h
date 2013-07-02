#ifndef __UART_DRIVER8250_H__
#define __UART_DRIVER8250_H__


#include <stdio.h>
#include <sys/select.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/termios.h>
#include <unistd.h>
#include <fcntl.h>

#define TTYSA0 "/dev/ttySA0"
#define TTYSA1 "/dev/ttySA1"
#define TTYSA2 "/dev/ttySA2"
#define TTYSA3 "/dev/ttySA3"
#define MAX_COMS  256

int openSerialPort(const char *devname);
static int set_port_speed(int nComId, int speed);
static int set_port_parity(int nComId, int data, int stop, char cparity);
int portComOpen(const char *dev, int baut, int data, int nstop, char cparity);
void portComClose(int nComId);
unsigned int portComSend(int nComId, const char *buf, const int length);
unsigned int portComRecv(int nComId, char *buf, const int buflen, unsigned int timeout);
void dump_pkt(char * buf, const int buflen);


#endif



