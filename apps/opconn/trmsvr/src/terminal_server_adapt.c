#include "plat_common.h"
#include "terminal_server.h"
#include "serial_ser.h"
#include "db_instance.h"
#include "cs_cmn.h"
#include "serial.h"
#include "uart_default.h"
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
//#include "vos_config.h"

extern S2E_Ctl S2E_Control;

#define CFG_LEN	250



cs_status ts_uart_init(uart_config_t *uart_cfg)
{
    cs_callback_context_t    context;
    cs_status  res = CS_E_OK;

	
#ifdef SERIAL_8250
	unsigned long ulPort = uart_cfg->uart_id-1;
	int serial_baud = uart_cfg->baud_rate;
	int data_value = uart_cfg->data_bits;
	int stop_value = uart_cfg->stop_bits;
	int parity = uart_cfg->parity;

	ts_serial_open_single(ulPort,serial_baud,data_value,stop_value,parity);

#else

	int value;
	char FCR_reg = 0;
	char MCR_reg;
	unsigned long ulPort = uart_cfg->uart_id-1;
	EnableUart(TRUE);
	Serial_Cs_Set(ulPort);

    UARTClearAllReg();

    /*disable EFR.*/
    UARTEFREnable(FALSE);
    //
    // Set the baud rate.
    //
    value = uart_cfg->baud_rate;
    S2E_SET_info(ulPort, SERIAL_BAUD, &value);
    //
    // Set the data size.
    //
    value = uart_cfg->data_bits;
    S2E_SET_info(ulPort, SERIAL_DATA_SIZE, &value);
    //
    // Set the parity.
    //
    value = uart_cfg->parity;
    S2E_SET_info(ulPort, SERIAL_PARITY_CTRL, &value);
    //
    // Set the stop bits.
    //
    value = uart_cfg->stop_bits;
    S2E_SET_info(ulPort, SERIAL_STOP_BIT, &value);
    //
    // Set the flow control.
    //

    value = SERIAL_DEFAULT_FC;
    S2E_SET_info(ulPort, SERIAL_FC_MODE_CTRL, &value);
    Serial_Cs_Set(ulPort);
    UARTFIFOEnable(TRUE, &FCR_reg);
    UARTDMAEnable(TRUE, &FCR_reg);
    UART_FIRO_Rx_Trigger_Level(SERIAL_DEFAULT_TRIGGER_RX, &FCR_reg);
    UARTSet_FCR(FCR_reg);

    UARTIntEnable(TRUE);



	SerialPurgeData(ulPort);
    //
    // Start the UART.
    //
    UARTFIRO_RDY_Enable(TRUE);
    //
    // (Re)enable the UART transmit and receive interrupts.
    //
    UARTIntType(SERIAL_INT_TYPE_RX, TRUE);
    UARTIntType(SERIAL_INT_TYPE_TX, TRUE);

    UARTRxTimeOutEn(TRUE);


	if((SERIAL_TYPE_485 == S2E_Control.serial_type)&&(SERIAL_DUPLEX_HALF ==serial_duplex))
	{
		Serial_Uart_485_half(ulPort, TRUE);
	}
	else
	{

    		Serial_Uart_485_half(ulPort, FALSE);
	}
		cpldcs2Read(0xc, &MCR_reg);
MCR_reg&=0xef;
cpldcs2Write(0xc,MCR_reg);    //stop loopback  add by zhangjj   2013.1.7

//EnableUart(TRUE);
printf("serial set over..\n");
#endif
    return res;
}

cs_status ts_putc(cs_uint8 uart,char ch)
{
    cs_callback_context_t    context;
    cs_status  res = CS_E_OK;

    res = cs_plat_uart_write(context,0,0,uart,1,&ch);
    
    return res;
}

#if 0
cs_status ts_getc(cs_uint8 uart, cs_uint8 *ch)
{
	cs_callback_context_t context;
	cs_status res = CS_E_OK;

	res = cs_plat_uart_read(context,0,0,uart,ch);

	return res;
}
#endif
void  ts_uart_send(cs_uint8 uart,char * buffer, unsigned int len)
{
    int count = 0;

    for(count = len; count > 0; count--){
        ts_putc(uart,*buffer++);
    }
}

cs_status ts_onu_uart_disable(cs_uint8 uart_id)
{
uart_id--;
    cs_plat_uart_disable(0,0,uart_id);
    return CS_E_OK;
}
#if 0
cs_status ts_onu_uart_enable(cs_uint8 uart_id)
{
    cs_plat_uart_enable(0,0,uart_id);
    return CS_E_OK;
}
#endif

#if 0
int ts_get_config(cs_uint8 uart_id ,term_server_config_t *Term_serv_cfg)
{
	FILE *fd;
	char ts_cfg_data[CFG_LEN *4+20];
	
	if((fd = fopen("cfg/ts_cfg","r"))==NULL)
	{
		printf("open failed ,ts_cfg is not exit\n");	
		return ERROR;
	}
	else
	{
		char buf[30];
		char *position;
		char port[6];
		char IP_buf[16];
		int i=0,j=0;
		char parity[10];
		char baud_rate[10];

		memset(ts_cfg_data,0,sizeof(ts_cfg_data));
		memset(buf,0,30);
		sprintf(buf,"Terminal Server%d",uart_id);
		printf("buf is %s\n",buf);	
		i=fread(ts_cfg_data, 1, CFG_LEN*4+20, fd);
		fclose(fd);
		printf("i is %d\n",i);
#if 0
		while(j<i)
		{
		printf("configuration is %c\n",ts_cfg_data[j]);
		j++;
		}
#endif
		if((position = strstr(ts_cfg_data,buf))==NULL)
		{
			printf("position error\n");
			return ERROR;
		}
//		position = ts_cfg_data+CFG_LEN*(uart_id-1);
		if((position = strstr(position,"Enable"))==NULL)
		{
			printf("read Enable error");
			return ERROR;
		}
		if(*(position+9) == '1')
			Term_serv_cfg->enable_flag = TERM_SERV_ENABLE_FLAG;
		else
			Term_serv_cfg->enable_flag = 0;
		if((position = strstr(position,"Uart Id"))==NULL)
		{
			printf("read Uart Id is error\n");
			return ERROR;
		}
		uart_id = *(position +10);
		if(('0'<uart_id)&&(uart_id <'5'))
			Term_serv_cfg->uart_cfg.uart_id = uart_id-'0';
		else 
			{
			printf("read configure uart id is error\n");
			return ERROR;
			}
		if((position = strstr(position,"Protocol Type"))==NULL)
		{
			printf("can not find Protocol Type\n");
			return ERROR;
		}
//		printf("=============\n");
		if((*(position+16)=='T')&&(*(position+17)=='C')&&(*(position+18)=='P'))
			Term_serv_cfg->proto_type = 6;
		else if((*(position+16)=='U')&&(*(position+17)=='D')&&(*(position+18)=='P'))
			Term_serv_cfg->proto_type = 17;
		else
			{
			printf("read configure ProtocolType is error\n");
			return ERROR;
			}
//		printf("2=============\n");
		if((position = strstr(position,"Protocol Port"))==NULL)
		{
			printf("Protocol Port is error");
			return ERROR;
		}
		i=0;
		position +=16;			//port num position
		while(*(position+i)!='\n')
		{
			if(i>5)
			{
				printf("read configure ProtocolPort is error\n");
			return ERROR;
			}
			port[i] = *(position+i);
			i++;
		}
		port[i]='\0';
		printf("port is %s\n",port);
		position = strstr(position,"IP");
		i=0;
		position += 5;			//IP position
		while(*(position +i)!='\n')
		{
			if(i>15)				//the most big IP
			{
				printf("read configure ProtocolPort is error\n");
			return ERROR;
			}
			IP_buf[i]=*(position+i);
			i++;
		}
		IP_buf[i] = '\0';
		printf("IP is %s\n",IP_buf);
		if((position = strstr(position,"Baud Rate"))==NULL)
		{
			printf("read configure Baud Rate is error\n");
			return ERROR;
		}
		else
		{
			position += 12;
			i=0;
			while(*(position+i)!='\n')
			{
				if(i>6)
				{
					printf("Baud Rate read error\n");
					return ERROR;
				}
				baud_rate[i]=*(position+i);
				i++;
			}
			Term_serv_cfg->uart_cfg.baud_rate = atoi(baud_rate);
		}
		if((position = strstr(position,"Data Bits"))==NULL)
		{
			printf("read configure Data Bits is error\n");
			return ERROR;
		}
		else
		{
			Term_serv_cfg->uart_cfg.data_bits = *(position+12)-'0';
		}
		if((position = strstr(position,"Stop Bits"))==NULL)
		{
			printf("read configure Stop Bits is error\n");
			return ERROR;
		}
		else
		{
				Term_serv_cfg->uart_cfg.stop_bits = *(position+12)-'0';
		}
		if((position = strstr(position,"Parity"))==NULL)
		{
			printf("read configure Parity is error\n");
			return ERROR;
		}
		else
		{
			position += 9;
			if((*(position)=='O')&&(*(position+1)=='D')&&(*(position+2)=='D'))
				Term_serv_cfg->uart_cfg.parity = 1;
			if((*(position)=='N')&&(*(position+1)=='O')&&(*(position+2)=='N')&&(*(position+3)=='E'))
				Term_serv_cfg->uart_cfg.parity = 0;
			if((*(position)=='E')&&(*(position+1)=='V')&&(*(position+2)=='E')&&(*(position+3)=='N'))
				Term_serv_cfg->uart_cfg.parity = 2;
		}

	}

    return OK;
}
#else

int ts_get_config(cs_uint8 uart_id ,term_server_config_t *Term_serv_cfg)
{
	char buf[20]={0};
	char *proto_type;
	char *ip_msg,*parity;
	int port_num;
	int enable_flag,uart_num,proto_port,baud_rate,data_bits,stop_bits;

//	printf("get config...\n");
	sprintf(buf,"Terminal Server %d",uart_id);

	
	if((enable_flag = vosConfigUInt32Get(TERM_SERV_CFG_FILE,buf,"Enable",-1))==-1)
		return -1;
	else
		Term_serv_cfg->enable_flag = enable_flag;
	if((uart_num = vosConfigUInt32Get(TERM_SERV_CFG_FILE,buf,"Uart Id",-1))==-1)
		return -1;
	else
		Term_serv_cfg->uart_cfg.uart_id = uart_num;
	if(!strcmp((proto_type = vosConfigValueGet(TERM_SERV_CFG_FILE,buf,"Protocol Type","NONE")),"NONE"))
		return -1;
	else if(!strcmp(proto_type,"TCP"))
		Term_serv_cfg->proto_type = IPPROTO_TCP;
	else if(!strcmp(proto_type,"UDP"))
		Term_serv_cfg->proto_type = IPPROTO_UDP;
	else
		return -1;
	if((port_num = vosConfigUInt32Get(TERM_SERV_CFG_FILE,buf,"Port",-1))==-1)
		return -1;
	else
		Term_serv_cfg->proto_port = port_num;
	if(!strcmp((ip_msg = vosConfigValueGet(TERM_SERV_CFG_FILE,buf,"LAN IP","0.0.0.0")),"0.0.0.0"))
		return -1;
	
	if((baud_rate = vosConfigUInt32Get(TERM_SERV_CFG_FILE,buf,"Baud Rate",-1))==-1)
		return -1;
	else
		Term_serv_cfg->uart_cfg.baud_rate = baud_rate;
	
	if((data_bits = vosConfigUInt32Get(TERM_SERV_CFG_FILE,buf,"Data Bits",-1))==-1)
		return -1;
	else
		Term_serv_cfg->uart_cfg.data_bits = data_bits;
	if((stop_bits = vosConfigUInt32Get(TERM_SERV_CFG_FILE,buf,"Stop Bits",-1))==-1)
		return -1;
	else
		Term_serv_cfg->uart_cfg.stop_bits = stop_bits;
	
	if(!strcmp((parity = vosConfigValueGet(TERM_SERV_CFG_FILE,buf,"Parity","NO_PARITY")),"NO_PARITY"))
		return -1;
	else if(!strcmp(parity,"ODD"))
		Term_serv_cfg->uart_cfg.parity = 1;
	else if(!strcmp(parity,"EVEN"))
		Term_serv_cfg->uart_cfg.parity = 2;
	else 
		Term_serv_cfg->uart_cfg.parity = 0;
	return OK;
}

#endif

int ts_get_ip_cfg(char *ip,int ip_type)
{
	char ip_msg[20]={0};
	char *ip_point;
	char buf[20]={0};
	int ret=0;

	sprintf(buf,"Terminal Server %d",1);
if(ip_type == LAN_IP)
{
	if(!strcmp((ip_point = vosConfigValueGet(TERM_SERV_CFG_FILE,buf,"LAN IP","0.0.0.0")),"0.0.0.0"))
		ret =-1;
	else
	{
		strncpy(ip,ip_point,16);
		ret = 0;
	}
}
else if(ip_type == WAN_IP)
{
	if(!strcmp((ip_point = vosConfigValueGet(TERM_SERV_CFG_FILE,buf,"WAN IP","0.0.0.0")),"0.0.0.0"))
		ret =-1;
	else
	{
		strncpy(ip,ip_point,16);
		ret = 0;
	}

}
	return ret;
}

#if 0

void ts_save_config(cs_uint8 uart_id ,term_server_config_t *Term_serv_cfg)
{
	char IP_set[100];
	char IP_msg[16];
	char ts_cfg_write[4][CFG_LEN];
	char cfg_data[CFG_LEN];
	int i=0,j=0;
	int enable_flag;
	char proto_type[4];
	int num=0;
	char parity[50];
	FILE *fp,*fip,*fd;

	
	system("touch TS_IP");
	system("ifconfig eth0:0 |grep 'inet addr' >>TS_IP");
	if((fip = fopen("TS_IP","r"))!=NULL)
	{
		num = fread(IP_set,1,100,fip);
		if(num!=0)
		{
			memset(IP_msg,0,sizeof(IP_msg));
			strncpy(IP_msg,IP_set+20,15);
			printf("IP_msg is %s\n",IP_msg);
		}
		fclose(fip);
	}


	for(i=0;i<4;i++)
	{
		memset(ts_cfg_write[i],' ',CFG_LEN);
	}
	i=0;
	if((fp = fopen("/cfg/ts_cfg","r"))!=NULL)
	{	
		for(i=0;i<4;i++)
		{
			num = fread(ts_cfg_write[i],1,CFG_LEN,fp);
			printf("num is %d and i is %d\n",num,i);
			if(0 == num)
				break;
		}
		fclose(fp);
		system("rm /cfg/ts_cfg");
//		return ;
	}
	if(i>=uart_id)
		i--;
//	fclose(fp);


	printf("i is %d===\n",i);
	memset(cfg_data,' ',sizeof(cfg_data));
	cfg_data[CFG_LEN-1]='\n';
	memset(proto_type,0,sizeof(proto_type));
	if(6 == Term_serv_cfg->proto_type)
	strcpy(proto_type,"TCP");
	else if(17 == Term_serv_cfg->proto_type)
		strcpy(proto_type,"UDP");
	else
		{
			printf("unrecognized protocol type!\n");
			printf("save error!\n");
			return;
		}
	if(TERM_SERV_ENABLE_FLAG== Term_serv_cfg->enable_flag)
		enable_flag = 1;
	else 
		enable_flag = 0;
	memset(parity,0,sizeof(parity));
	if(0 == Term_serv_cfg->uart_cfg.parity)
		strcpy(parity,"NONE");
	else if(1 == Term_serv_cfg->uart_cfg.parity)
		strcpy(parity,"ODD");
	else if(2 == Term_serv_cfg->uart_cfg.parity)
		strcpy(parity,"EVEN");
	else
		{
			printf("unrecognized parity type!\n");
			printf("save error!\n");
			return;
		}
	sprintf(cfg_data,"[Terminal Server%d]\nEnable = %d\nUart Id = %d\nProtocol Type = %s\nProtocol Port = %d\nIP = %s\nBaud Rate = %d\nData Bits = %d\nStop Bits = %d\nParity = %s\n",
		Term_serv_cfg->uart_cfg.uart_id,enable_flag,Term_serv_cfg->uart_cfg.uart_id,proto_type,
		Term_serv_cfg->proto_port,IP_msg,Term_serv_cfg->uart_cfg.baud_rate,Term_serv_cfg->uart_cfg.data_bits,
		Term_serv_cfg->uart_cfg.stop_bits,parity);
	if((fd = fopen("/cfg/ts_cfg","w"))==NULL)
	{	
			printf("open error\n");
			return ;
	}
	for(j=0;j<i+1;j++)
	{
		if((j+1) == uart_id)
		{
			fwrite(cfg_data, 1, CFG_LEN, fp);
		}
		else
		{
			fwrite(ts_cfg_write[j], 1, CFG_LEN,fp);
		}
	}
   fclose(fd); 
    return;
}

#else
//extern STATUS vosIPAddrGet(UINT8 * ifName, char * ipAddr);
int ts_save_config(cs_uint8 uart_id ,term_server_config_t *Term_serv_cfg)
{
		char buf[20]={0};
		char IP_set[100];
		char parity[50];
		char section[20]={0};
		char protocol_type[5];
		int num;
		int ret;
		int ip_len=0;
		FILE *fip;
		char IP_msg1[256]={0};
		char IP_msg2[256]={0};

		vosSprintf(section,"Terminal Server %d",uart_id);
//		printf("save config ...\n");
#if 1
  if(vosIPAddrGet("eth0", IP_msg1)!= 0);
  if(vosIPAddrGet("eth4",IP_msg2)==0);
//printf("The eth4ip is %s\n",IP_msg2);
#else

		num = uart_id;

		system("rm TS_IP");
		system("touch TS_IP");
		system("ifconfig eth3 |grep 'inet addr' >>TS_IP");
		if((fip = fopen("TS_IP","r"))!=NULL)
		{
			num = fread(IP_set,1,100,fip);
			if(num!=0)
			{
				memset(IP_msg,0,sizeof(IP_msg));
				while(*(IP_set+20+ip_len)!=' ')
					ip_len++;
				strncpy(IP_msg,IP_set+20,ip_len);
			}
			fclose(fip);
		}
#endif
		
		memset(parity,0,sizeof(parity));
		if(0 == Term_serv_cfg->uart_cfg.parity)
			strcpy(parity,"NONE");
		else if(1 == Term_serv_cfg->uart_cfg.parity)
			strcpy(parity,"ODD");
		else if(2 == Term_serv_cfg->uart_cfg.parity)
			strcpy(parity,"EVEN");
		else
			{
				printf("unrecognized parity type!\n");
				printf("save error!\n");
				return;
			}
		if(IPPROTO_TCP == Term_serv_cfg->proto_type)
			strcpy(protocol_type,"TCP");
		else if(IPPROTO_UDP == Term_serv_cfg->proto_type)
			strcpy(protocol_type,"UDP");
		else
			{
			strcpy(protocol_type,"NONE");
			}	
	if(vosConfigSectionIsExisted(TERM_SERV_CFG_FILE,section))
		{
		ret = vosConfigSectionCreate(TERM_SERV_CFG_FILE,section);
		}
	if ((ret = vosConfigUInt32Set(TERM_SERV_CFG_FILE,section,"Enable",Term_serv_cfg->enable_flag)) != 0)
		{
			printf("save error\n");
		    return ERROR;
		}
		vosConfigUInt32Set(TERM_SERV_CFG_FILE,section,"Uart Id",Term_serv_cfg->uart_cfg.uart_id);
		vosConfigValueSet(TERM_SERV_CFG_FILE,section,"Protocol Type",protocol_type);
		vosConfigUInt32Set(TERM_SERV_CFG_FILE,section,"Port",Term_serv_cfg->proto_port);
		vosConfigValueSet(TERM_SERV_CFG_FILE,section,"LAN IP",IP_msg1);
		vosConfigValueSet(TERM_SERV_CFG_FILE,section,"WAN IP",IP_msg2);
		vosConfigUInt32Set(TERM_SERV_CFG_FILE,section,"Baud Rate",Term_serv_cfg->uart_cfg.baud_rate);
		vosConfigUInt32Set(TERM_SERV_CFG_FILE,section,"Data Bits",Term_serv_cfg->uart_cfg.data_bits);
		vosConfigUInt32Set(TERM_SERV_CFG_FILE,section,"Stop Bits",Term_serv_cfg->uart_cfg.stop_bits);
		vosConfigValueSet(TERM_SERV_CFG_FILE,section,"Parity",parity);
//		printf("Save successfully!\n");
		return OK;
}
#endif
int telnet_ip_save(char *telnetIP)
{
	char section[20]="Telnet IP";
	int ret=0;
			
	if(vosConfigSectionIsExisted(TELNET_IP,section))
	{
		ret = vosConfigSectionCreate(TELNET_IP,section);
	}
	if ((ret = vosConfigValueSet(TELNET_IP,section,"TelnetIP",telnetIP)) != 0)
		{
			printf("save error\n");
		    return ERROR;
		}
	return OK;
}

int telnet_ip_get(char *ip)
{
	char ip_msg[20]={0};
	char *ip_point;
	int ret=0;

	if(!strcmp((ip_point = vosConfigValueGet(TELNET_IP,"Telnet IP","TelnetIP","0.0.0.0")),"0.0.0.0"))
		ret =-1;
	else
	{
		strncpy(ip,ip_point,16);
		ret = 0;
	}
	return ret;
}




