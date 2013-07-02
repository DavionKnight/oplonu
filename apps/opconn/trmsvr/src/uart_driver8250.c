#include "uart_driver8250.h"

static int g_com_fds[MAX_COMS] ={0};

int openSerialPort(const char *devname)
{
	int ret=-1,i, fd=0;

	if((fd = open(devname, O_RDWR)) != -1)
	{
		for(i=0;i<MAX_COMS;i++)
		{
			if(g_com_fds[i]==0)
			{
				g_com_fds[i]=fd;
				return i;
			}
		}
	}
	

	return ret;		
}

static int set_port_speed(int nComId, int speed)
{
	int ret = 0;
	int speeds[]={1200,2400,4800,9600,19200,38400,57600,115200};
	int b_spds[]={B1200,B2400,B4800,B9600,B19200,B38400,B57600,B115200};
	struct termios tms;
	
	int i=0;
	for(;i<sizeof(speeds)/sizeof(int);i++)
		if(speeds[i]==speed)
		break;
	
	if(i==sizeof(speeds)/sizeof(int))
		i = 3;
	
	tcgetattr(g_com_fds[nComId], &tms);
	cfsetispeed(&tms, b_spds[i]);
	cfsetospeed(&tms, b_spds[i]);
	
	tcsetattr(g_com_fds[nComId],TCSANOW, &tms);
	
	return ret;
}

static int set_port_parity(int nComId, int data, int stop, char cparity)
{
	int ret=0;
	struct termios tms;
	tcgetattr(g_com_fds[nComId],&tms);
	
	tms.c_cflag &= ~CSIZE;
	switch( data )
	{
		case 7:
			tms.c_cflag |= CS7;
			break;
		case 8:
			tms.c_cflag |= CS8;
			break;
		default:
			printf("\r\ninvalid data width!");
			break;
	}
	
	switch(stop)
	{
		case 1:
			tms.c_cflag &= ~CSTOPB;
			break;
		case 2:
			tms.c_cflag |= CSTOPB;
			break;
		default:
			fprintf(stderr, "\r\nunsurpported stop bits");
			break;
	}
	
	switch(cparity)
	{
		case 'n':
			case 'N':
			tms.c_cflag &= ~PARENB;
			tms.c_iflag &= ~INPCK;
			break;
		case 'o':
			case 'O':
			tms.c_cflag |= (PARENB|PARODD);
			tms.c_iflag |= INPCK;			
			break;
		case 'e':
			case 'E':
			tms.c_cflag &= ~PARODD;
			tms.c_iflag |= INPCK;
			tms.c_cflag |= PARENB;
			break;
		case 's':
			case 'S':
			tms.c_cflag &= ~PARENB;
			tms.c_iflag &= ~INPCK;
			break;
		default:
			fprintf(stderr, "\r\ninvalid parity value set");
			break;
	}
	
	if(cparity == 'n' || cparity == 'N')
	{
		tms.c_lflag &= ~ICANON;
		tms.c_cc[VTIME]=50;
	}

	// disable echo local char
	tms.c_lflag &= ~ECHO;

	tcsetattr(g_com_fds[nComId], TCSANOW, &tms);
	return ret;
}

int portComOpen(const char *dev, int baut, int data, int nstop, char cparity)
{
	int comid=-1;
	
    comid = openSerialPort(dev);
	if(comid != -1 )
	{
		if(set_port_speed(comid, baut) == 0
	   	&&set_port_parity(comid, data, nstop, cparity) == 0 )
			return comid;
	}
	else
		return (-1);
}

void portComClose(int nComId)
{
	if(nComId < MAX_COMS)
	{
		close(g_com_fds[nComId]);
		g_com_fds[nComId]=0;
	}
}

unsigned int portComSend(int nComId, const char *buf, const int length)
{
	unsigned int nsend = 0;
	while(nsend < length)
	{
		unsigned int offset = write(g_com_fds[nComId], buf, length-nsend);
		if(offset > 0)
		{
			nsend += offset;
			buf += nsend;
		}
	}
	return nsend;
}

unsigned int portComRecv(int nComId, char *buf, const int buflen, unsigned int timeout)
{
	unsigned int nRecv = 0,left = buflen,estm=0, secs = 0, usecs = 0;
	int ret;
	int fd = g_com_fds[nComId]; 
	fd_set reads;

	secs = timeout/1000;
	usecs = (timeout%1000)*1000;

	struct timeval Timeout; 
	Timeout.tv_usec=usecs;
	Timeout.tv_sec = secs;


	do{
		bzero(&reads,sizeof(fd_set));
		FD_SET(fd, &reads);
		ret = select(fd+1, &reads, NULL, NULL, &Timeout);
		if(ret > 0)
		{
			if(FD_ISSET(fd, &reads))
			{
				nRecv = read(fd, buf, left);
				if(nRecv > 0)
				{
					left -= nRecv;
					buf += nRecv;
				}
				else
					break;
			}
		}
	}while(ret > 0);

	return (buflen-left);
}

 void dump_pkt(char * buf, const int buflen)
{
	int i = 0;

	if(buflen > 0)
	{
		printf("\r\n");

		while(i<buflen)
		{
			if(!(i&0xf))
				printf("\r\n");
			printf("%02x ",(unsigned char)buf[i]);
			i++;
		}

		printf("\r\n");
	}
}

#if 0
#define TEST_PORT_NUM 4
int main(int argc, char *argv[])
{
	int fds[TEST_PORT_NUM] = {0,0,0,0};
	int ret=0;
	char buf[1024], xmit[1024];
	int i=0, xmitlen = 0, baud, flag;
	char parity='n';

	if(argc < 3)
		return (-1);

	baud = atoi(argv[1]);

	flag = atoi(argv[2]);

	if(flag == 1) //send test
	{
		if(argc == 4)
			xmitlen = atoi(argv[3]);
		else
			xmitlen = 256;

	}

	fds[0] = portComOpen(TTYSA0,baud,8,1,parity);
	fds[1] = portComOpen(TTYSA1,baud,8,1,parity);
	fds[2] = portComOpen(TTYSA2,baud,8,1,parity);
	fds[3] = portComOpen(TTYSA3,baud,8,1,parity);

	for(i=0; i<TEST_PORT_NUM; i++)
	{
		if(-1 == fds[i])
		{
			printf("port %d open error!\n", i+1);
		}
	}

	while(1)
	{
		int len=0;

		for(i=0; i<1; i++)
		{

			if(flag != 1)
			{
				len = portComRecv(fds[i], buf, 1024, 200);
				if(len > 0)
				{
					printf("port %d recieved chars(len=%d):\n", i+1, len);
					dump_pkt(buf, len);
				}
			}
			else
			{
				memset(xmit, 0x55, xmitlen);
				portComSend(fds[i], xmit, xmitlen);
				sleep(5);
			}
		}
	}
	
	
}
#endif






