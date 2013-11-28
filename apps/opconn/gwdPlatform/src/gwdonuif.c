#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include "oam.h"
#include "opl_driver.h"
#include "opl_host.h"
#include "bridgeport.h"
#include <vos.h>
#include "log.h"
#include "stats.h"
#include "hw_interface.h"
#include "hal_oam.h"
#include "hal.h"
#include "version.h"
#include "zte.h"
#include "gw_types.h"
#include "gwdonuif.h"
#include "product_info.h"
#include "dal_port.h"
#include "odm_port.h"
#include "vos_types.h"
#include <math.h>
#include "ctc_2_1.h"
#include "aos_types.h"
#include "fal_vlan.h"
#include "fal_portvlan.h"
#include "fal_fdb.h"
#include "fal_mib.h"
#include "run_led.h"
#include "version.h"
#include "stats.h"
#include "hw_interface.h"
#include "qos.h"
#include "mc_control.h"
#include "odm_vlan.h"
#include "run_led.h"
#include "hal_stats.h"
#include "vos_thread.h"
#include "plat_config.h"
#include "vos_time.h"

//#define GWDDEBUG
//add from gwdonu/oam.h
#define GWONU_873A_FE_PORT_NUM 4

#define DEVICE_TYPE_GT872_A			0x0031
#define DEVICE_TYPE_GT873_A			0x0032

typedef struct onu_system_information_total
{
	unsigned short 	product_serial;					/* 2 */
	unsigned short	product_type;					/* 2 */
	unsigned char	device_name[132];				/* 64 */
	unsigned char	sw_version[12];					/* 12 */
	unsigned char	serial_no[18];					/* 18 */
	unsigned char	hw_version[7];					/* 6 */
	unsigned char	hw_manufature_date[12];			/* 12 */
														/* 68 */
	struct	_pon_transceiver_info_
	{
		unsigned char 	available	: 1;
		unsigned char	reach 		: 1;
		unsigned char	package 	: 1;
		unsigned char	protectEn 	: 1;
		unsigned char	manufacturer 	: 4;
	}pon_transceiver_info;							/* 1 */

	unsigned char	TDM_module_info;				/* 1 */
	unsigned char 	VoIP_module_info;				/* 1 */
	unsigned char	reserved_1;						/* 1 */
														/* 72 */
	unsigned char	mac_addr_primary[6];			/* 6 */
	unsigned char  	mac_addr_second[6];				/* 6 */
														/* 84 */
	unsigned long	ip_addr;						/* 4 */
	unsigned long  	net_mask;						/* 4 */
														/* 92 */
	unsigned long	vos_image_size;					/* 4 */
														/* 96 */
	unsigned char	loadstartupconfig;				/* 1 */
	unsigned char	hardwarediagnosis;				/* 1 */
//	unsigned char	reserved_2[2];					/* 2 */ hw-ver added 2 chars by wangxy 2012-04-28
														/* 100 */
//	unsigned long	reserved_3[47];					/* 256 */  device_name increased to 132 bytes by wangxy 013-05-06
														/* 356 */
	unsigned char 	reserved_4[2];					/* 2 */
	unsigned char 	reboot;							/* 1 */
	unsigned char	valid_flag;						/* 1 */ /* Total: 360 */
}ONU_SYS_INFO_TOTAL;

u8_t 	overall_lanmac[6] = {0x0,0x0,0x0,0x0,0x0,0x0};

static libgwdonu_special_frame_handler_t g_onu_pkt_handler = NULL;
static libgwdonu_out_hw_version g_onu_out_if_hwver_get = NULL;
static libgwdonu_syslog_heandler_t g_onu_syslog_handler = NULL;

int physicalport2logicalport(int port)
{
	int phy2log[7] = {1,2,3,4,5,6,7};
	if((port >= 0)&&(port <= 6))
	{
		return phy2log[port];
	}
	else
	{
		return -1;
	}
}

//opl_dump_data(pkt,len,16);
#if 0
void GwDumpPkt(char *pkt,int len)
{
	char *pucBuf=NULL;
	unsigned int index=0;
	printf("\r\n-----------GwDumpPkt ------------\r\n",len);
	pucBuf = (char *)pkt;
	for(index = 0; index < len; index++)
	{
		if((index != 0) && (index%16 == 0))
		{
			printf("\r\n%02x ",(UINT8)pucBuf[index]);
		}
		else
		{
			printf("%02x ",(UINT8)pucBuf[index]);
		}
	}
	printf("\r\n\n");
}
#endif

extern int OamFrameSend(u8_t *pucFrame, u16_t usFrmLen);

gw_status gwdonu_onu_llid_get(gw_uint32 *llid)
{
	int ret=0;
	if(0 == (ret = dalPonLlidGet(llid)))
		return GW_OK;
	return GW_ERROR;
}


gw_status gwdonu_onu_sys_info_get(gw_uint8 * sysmac, gw_uint32 *uniportnum)
{
//	printf("in sysinfoget fuction ,this function is not defined .......\r\n");
	u8_t 	lanmac[6] = {0x0,0x0,0x0,0x0,0x0,0x0};
	u8_t	*ip_point;
	u8_t 	buf[25];

	*uniportnum = 4;
	strcpy(buf,"Product Information");

	/*-----need judge the mac---*/
	if(GW_OK ==	vosHWAddrGet("eth0", lanmac))
		memcpy(sysmac,lanmac,6);
#if 0
	if(!strcmp((ip_point = vosConfigValueGet(PRODUCT_CFG_FILE,buf,"LAN MAC","00:00:00:00:00:00")),"00:00:00:00:00:00"))
	{
		printf("Get mac error! Please set mac first");
		return GW_ERROR;
	}
	else
	{
		vosStrToMac(ip_point,sysmac);
//		strncpy(sysmac,ip_point,6);
	}
#endif
#ifdef GWDDEBUG
	int i=0;
	printf("local mac is ");
	for(i=0;i<6;i++)
	printf("0x%x ",sysmac[i]);
	printf("\n\n");
#endif

	return GW_OK;
}


gw_uint32 gwdonu_sys_conf_save(gw_uint8 * info, gw_uint32 len)
{

	ONU_SYS_INFO_TOTAL *buff = (ONU_SYS_INFO_TOTAL*)info;
	char section[20];
	char date_value[15];
	int ret=0;

	strcpy(section,PRODCUT_INFO_SECTION);

#if 0
	printf("hw_manufature_date is %s\r\n",buff->hw_manufature_date);
	printf("serial number is %s\r\n",buff->serial_no);
	printf("device_name is %s\r\n",buff->device_name);
	printf("Hardware version is %s\r\n",buff->hw_version);
#endif

	vosSprintf(date_value, "%s",buff->hw_manufature_date);

	/******** Save manufature date *******/
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"Manufature date",date_value)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}

	/******** Save serial num ************/
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"Serial number",buff->serial_no)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}
	/******** Save devicename *************/
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"DeviceName",buff->device_name)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}
	/******** Save Hardware version ********/
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"Hardware version",buff->hw_version)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}
	/*********** Save PON MAC **************/


    cliTextConfigSave();
    vosConfigSave(NULL);
    return GW_OK;
#if 0

	FILE *p;
	if((p = fopen("/cfg/gwdonusys.conf","wb"))!=NULL)
	{
#if 1
		unsigned char i=0;
		printf("gwdonu sys conf save :");
		for(i=0;i<len;i++)
			printf("%c",info[i]);
		printf("\r\n");
#endif
		fwrite(info,len,1,p);
		fclose(p);
		printf("before cat\r\n");
		system("cat /cfg/gwdonusys.conf");
		vosConfigSave(NULL);
		return GW_OK;
	}
	else
		printf("gwdonu_sys_conf_save error!\r\n");
	return GW_ERROR;
#endif
}


gw_uint32 gwdonu_sys_conf_restore(gw_uint8 *info, gw_uint32 len)
{
#if 0
FILE *p;
if((p = fopen("/cfg/gwdonusys.conf","rb"))!= NULL)
{
	fread(info,len,1,p);
	fclose(p);
	return GW_OK;
}
else
	printf("There is no gwdonusysconf file\r\n");
return GW_OK;
#else
ONU_SYS_INFO_TOTAL *buff=(ONU_SYS_INFO_TOTAL *)info;
memset(buff,0,sizeof(ONU_SYS_INFO_TOTAL));
	char deviceName[132]={0};
	char serial_num[18] = {0};
	char hw_version[7] = {0};
	char hw_manufature_date[12] = {0};
	char section[20] ={0};
	int ret = GW_ERROR;

	vosStrCpy(section,"Product Information");
	/*** device name ***/
	if(!strcmp(strcpy(deviceName,vosConfigValueGet(PRODUCT_CFG_FILE,section,"DeviceName","00")),"00"))
	{
		printf("DeviceName may be not existing..\r\n");
//		return ret;
	}
	else
	{
		vosStrCpy(buff->device_name,deviceName);
	}
	/*** serial number ***/
	if(!strcmp(strcpy(serial_num,vosConfigValueGet(PRODUCT_CFG_FILE,section,"Serial number","000000")),"000000"))
	{
		printf("Serial number may be not existing..\r\n");
//		return ret;
	}
	else
	{
		vosStrCpy(buff->serial_no,serial_num);
	}
	/*** hardware version ***/
	if(!strcmp(strcpy(hw_version,vosConfigValueGet(PRODUCT_CFG_FILE,section,"Hardware version","V1.0R0")),"V1.0R0"))
	{
		printf("Hardware may be not existing ...\r\n");
//		return ret;
	}
	else
	{
		vosStrCpy(buff->hw_version,hw_version);
	}
	/*** Manufacture date ***/
	if(!strcmp(strcpy(hw_manufature_date,vosConfigValueGet(PRODUCT_CFG_FILE,section,"Manufature date","1970")),"1970"))
	{
		printf("Manufature may be not existing ...\r\n");
//		return ret;
	}
	else
	{
		vosStrCpy(buff->hw_manufature_date,hw_manufature_date);
	}
	buff->valid_flag = 'E';
#endif
	return GW_OK;
}


gw_uint32 gwdonu_sw_conf_save(gw_uint8 * info, gw_uint32 len)
{
	FILE *p;
	if((p = fopen("/cfg/gwdonuswconf","wb"))!=NULL)
	{
		fwrite(info,len,1,p);
		fclose(p);
//		vosConfigSave(NULL);
		return GW_OK;
	}
	else
		printf("gwdonu_sw_conf_save error!\r\n");
	return GW_OK;
}


gw_uint32 gwdonu_sw_conf_restore(gw_uint8 *info, gw_uint32 len)
{
	FILE *p;
	if((p = fopen("/cfg/gwdonuswconf","rb"))!= NULL)
	{
		fread(info,len,1,p);
		return GW_OK;
	}
	else
		printf("There is no gwdonuswconf file!\r\n");
	return GW_OK;
}

extern gw_macaddr_t olt_mac_addr;
gw_status gwdonu_olt_mac_get(gw_uint8 * mac)
{
	memcpy(mac,olt_mac_addr,GW_MACADDR_LEN);
#ifdef GWDDEBUG
	int i=0;
	printf("oltmac is ");
	for(i=0;i<6;i++)
	printf("0x%x ",olt_mac_addr[i]);
	printf("\n");
#endif
	return GW_OK;
}


gw_status gwdonu_port_send(gw_int32 portid, gw_uint8 *buf, gw_uint32 len)
{
	int iStatus = OK;
	int usIdx=0;
	char *pucTmp = buf;
	int i=0;
	u8_t	aucPkt[1518]={0};
	host_outbound_hdr_t	*pstHdr;

#ifdef GWDDEBUG
	if((len>45)&&(*(buf+18+14+8)==0x03))
	{
	printf("********** payload length: %d ***********", len);
	for(usIdx=0; usIdx<len; usIdx++)
	{
		if(0 == usIdx%16)
		{
			printf("\n0x%08x\t", usIdx);
		}
		printf(" 0x%02x", *pucTmp++);
	}
	printf("\n#####################################\n");
	}
#endif

	pstHdr = (host_outbound_hdr_t *)(&aucPkt[0]);
	if(portid == GW_PON_PORT_ID)
	{
		pstHdr->reserved = 0x00;
		pstHdr->eport = 0x01;
		pstHdr->icos = 0x07;
		vosMemCpy(&aucPkt[1], buf, len);

		iStatus = eopl_host_send(aucPkt, len+1);
		if(0 != iStatus)
		{
	   /*OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "send error!\n");*/
		  hostCnt.oamTxFail++;
		  return iStatus;
		}
		hostCnt.oamTx++;
		/*g_ucPduMax++;*/
	}
	else	if((portid == GW_UNI_PORT_ID1)||(portid == GW_UNI_PORT_ID2)||(portid == GW_UNI_PORT_ID3)||(portid == GW_UNI_PORT_ID4))
	{
//			iStatus = eopl_send_to_down_link((portid -1), 0,IGMP_PKT_TYPE,buf,len);
			iStatus = eopl_send_to_down_link(portid, 0,IGMP_PKT_TYPE,buf,len);
//			iStatus = eopl_send_to_down_link_atheros((portid -1), 0,2,buf,len);
#if 0
			pstHdr->eport = 0;
			pstHdr->icos = 7;

			vosMemCpy(&aucPkt[1], buf, len);

			iStatus = eopl_host_send(aucPkt, len+1);
//#ifdef GWDDEBUG
			printf("gwdonu_port_send portid is %d,and status is %d..\r\n",portid,iStatus);
//#endif
			//		if(0 != iStatus)
	//		{
        /*OP_DEBUG(DEBUG_LEVEL_DEBUGGING, "send error!\n");*/
	//				return iStatus;
#endif
//			GwDumpPkt(buf,len);
//			printf("gwdonu_port_send portid is %d,and status is %d..\r\n",portid,iStatus);
    }
	else
	{
//		printf("gwdonu_port_send other GW_UNI_PORT_ID %d...\r\n",portid);
	}
	return GW_OK;

}



extern oam_sate_t oams;
//extern int opl_dump_data_pty(char *p,int len,int width);
gw_uint32 gwdonu_oam_std_hdr_builer(gw_uint8 *a , gw_uint32 b)
{
	gw_uint32 length=0;
	eopl_oam_pdu_hdr(a, oams.flags, OAM_CODE_ORG_SPEC);
	length = 18;
//	printf("************head is like this and length is %d************\r\n",length);
//	opl_dump_data(a, length, 16);
//	printf("in oamhdrbuiler fuction ,this function is not defined .......\r\n");
	return length;
}




gw_status gwdonu_port_admin_status_get(gw_int32 portid, gwd_port_admin_t *status)
{
//	printf("in portadminget fuction ,this function is not defined .......\r\n");

	UINT32 lst;
	gw_status ret = 0;

	ret = dalPhyAdminStateGet(portid, &lst);

	if(ret == OPL_OK)
	{
		*status = lst;
	}

	return (ret == OPL_OK)?GW_OK:GW_ERROR;
}



/*return 0  is OK
 	 else is ERROR		*/
gw_status gwdonu_port_admin_status_set(gw_int32 portid, gwd_port_admin_t status)
{
//	printf("in portadminset fuction ,this function is not defined .......\r\n");
	return(dalPhyAdminControlSet(portid,status+1));
}


gw_status gwdonu_port_oper_status_get(gw_int32 portid, gwd_port_oper_status_t *status)
{
//	printf("in portoperget fuction ,this function is not defined .......\r\n");
	UINT8 status_t;
	UINT32 ret;
	ret = dalPortStateGet(portid,&status_t);

	if(OPL_OK == ret)
	{
		*status = (gwd_port_oper_status_t)(status_t);
	}
//	printf("status is 0x%x\r\n",*status);
	return OPL_OK;
}


gw_status gwdonu_port_mode_get(gw_int32 portid, gw_int32 * auneg_en, gw_int32 *spd, gw_int32 *duplex)
{
//	printf("in gwdonu_port_mode_get fuction ,this function is not defined .......\r\n");

    UINT32 speed = 0,dup = 0,auneg = 0;
    INT32 halStatus = 0;

    halStatus = dalPortSpeedGet(portid,&speed);
    halStatus |= dalPhyAutonegAdminStateGet(portid,&auneg);
    halStatus |= dalPortDuplexGet(portid,&dup);

    	if(0 == halStatus)
    		{
    			*auneg_en = auneg-1;
    			switch(speed)
    				{
    					case ODM_PORT_SPEED_10M:
    							*spd = GWD_PORT_SPD_10;
    						break;
    					case ODM_PORT_SPEED_100M:
    							*spd = GWD_PORT_SPD_100;
    						break;
    					case ODM_PORT_SPEED_1000M:
    							*spd = GWD_PORT_SPD_1000;
    							break;
    					default:
    							OPL_LOG_TRACE();
    						return OPL_ERR_INVALID_PARAMETERS;
    				}
    			if(PHY_HALF_DUPLEX == dup)
    					*duplex = GWD_PORT_DUPLEX_HALF;
    			else if(PHY_FULL_DUPLEX == dup)
    					*duplex = GWD_PORT_DUPLEX_FULL;
    			else
    					*duplex = GWD_PORT_DUPLEX_AUNEG;
    		}


	return 0;
}


gw_status gwdonu_port_mode_set(gw_int32 portid, gw_int32 spd, gw_int32 duplex)
{
//	printf("in gwdonu_port_mode_set fuction ,this function is not defined .......\r\n");

	gw_status ret = 0;
	if(spd != GWD_PORT_SPD_AUNEG && duplex != GWD_PORT_DUPLEX_AUNEG)
	{
		switch( spd )
		{
			case GWD_PORT_SPD_100:
				spd = ODM_PORT_SPEED_100M;
				break;
			case GWD_PORT_SPD_1000:
				spd = ODM_PORT_SPEED_1000M;
				break;
			default:
				spd = ODM_PORT_SPEED_10M;
				break;
		}

		switch( duplex )
		{
			case GWD_PORT_DUPLEX_FULL:
				duplex = PHY_FULL_DUPLEX;
				break;
			default:
				duplex = PHY_HALF_DUPLEX;
				break;
		}

		ret = dalPhyAutonegEnableSet(portid, 1);//disable auto neg

		ret |= dalPortSpeedSet(portid, spd);
		ret |= dalPortDuplexSet(portid, duplex);
	}
	else
	{
		ret = dalPhyAutonegEnableSet(portid, 2);
		ret |= dalPhyAutonegRestart(portid);
	}


	return (ret == OPL_OK)?GW_OK:GW_ERROR;
}

gw_status gwdonu_port_isolate_get(gw_int32 portid, gw_int32 *en)
{
    gw_uint32 localswitchflag;

	if(NULL == en)
		return GW_ERROR;

    /* default local switch disable */
    localswitchflag = vosConfigUInt32Get(VLAN_CONFIG_NAME,
        LOCAL_SWITCH_SEC, LOCAL_SWITCH_KEY, 0);
    *en = (0x1e == localswitchflag)?1:0;
	return GW_OK;
}


gw_status gwdonu_port_isolate_set(gw_int32 portid, gw_int32 en)
{
  int ret = 0;
  int i = 0;

  if(0xff == portid)
  {
	for(i=1;i<=4;i++)
	{
	  ret = odmSetLocalSwitch(i,(en?0:1));
	  if(GW_OK != ret)
		  return GW_ERROR;
	}
  }
  else
	  return GW_ERROR;
  return GW_OK;
}

#define CNT_SW_AR8306	39
extern OPL_CNT_t g_astCntSwhPort[SWITCH_PORT_NUM][CNT_SW_AR8306];
gw_uint64 struct2uint64(char ucPortId,char ucCnt)
{
	gw_uint64 value=0;

	value = g_astCntSwhPort[ucPortId][ucCnt].stAdded.uiHigh;
	value<<=32;
	value += g_astCntSwhPort[ucPortId][ucCnt].stAdded.uiLow;

	return value;
}

gw_uint64 CurrentpktCntIn[GWONU_873A_FE_PORT_NUM] = {0};
gw_uint64 CurrentpktCntOut[GWONU_873A_FE_PORT_NUM] = {0};
gw_uint64 OctRateIn[GWONU_873A_FE_PORT_NUM] = {0};
gw_uint64 OctRateOut[GWONU_873A_FE_PORT_NUM] = {0};
gw_uint32 LastTick4PortMon[GWONU_873A_FE_PORT_NUM] = {0};
gw_uint32 CurrentTick4PortMon[GWONU_873A_FE_PORT_NUM] = {0};

void gw_port_rate_thread()
{
	unsigned char logical_port =0, port= 0, status_t =-1, ret =-1;
	struct timeval tv = {0};
	gw_uint64 ulIntervalTick = 0 ,Rxtotalbytes =0 ,Txtotalbytes =0;
	gw_float fRate =0;
	gw_onu_port_counter_t * pd = NULL;

	while(1)
	{
		for(port =0;port<GWONU_873A_FE_PORT_NUM; port ++)
		{
			logical_port = physicalport2logicalport(port);
			if(-1 == logical_port)
			{
				continue;
			}
			ret = dalPortStateGet(logical_port,&status_t);

			if(status_t)
			{
				stats_getArx8306(port);
				Rxtotalbytes = struct2uint64(port,15) + struct2uint64(port,16);
				Txtotalbytes = struct2uint64(port,31);
				gettimeofday(&tv, NULL);
				CurrentTick4PortMon[port]=tv.tv_sec*100 + tv.tv_usec/10000;

				if (CurrentTick4PortMon[port] > LastTick4PortMon[port])
				{
					ulIntervalTick = CurrentTick4PortMon[port] - LastTick4PortMon[port];

				}
			    else
				{
					ulIntervalTick = (0xFFFFFFFF*100 + 0xFFFFFFFF/10000) - (LastTick4PortMon[port] - CurrentTick4PortMon[port]);

				}
				if (Rxtotalbytes >= CurrentpktCntIn[port] )
				{

					fRate = (gw_float)((Rxtotalbytes - CurrentpktCntIn[port]))/(gw_float)ulIntervalTick*100;
				}
				else
				{
					fRate = (gw_float)((0xFFFFFFFFFFFFFFFF - (CurrentpktCntIn[port] - Rxtotalbytes)))/(gw_float)ulIntervalTick*1000;
				}

				CurrentpktCntIn[port] = Rxtotalbytes;

				OctRateIn[port] = (gw_uint64)fRate;

				if ( Txtotalbytes >= CurrentpktCntOut[port] )
				{
					fRate = (gw_float)((Txtotalbytes - CurrentpktCntOut[port]))/(gw_float)ulIntervalTick*100;
				}
				else
				{
					fRate = (gw_float)((0xFFFFFFFF - (CurrentpktCntOut[port] - Txtotalbytes)))/(gw_float)ulIntervalTick*1000;
				}
				CurrentpktCntOut[port] = Txtotalbytes;
				OctRateOut[port] = (gw_uint64)fRate;
				LastTick4PortMon[port] = CurrentTick4PortMon[port];
			}
			else
			{
				CurrentpktCntOut[port]= 0;
				CurrentpktCntIn[port] = 0;
				OctRateOut[port] = 0;
				OctRateIn[port] = 0;
				CurrentTick4PortMon[port] = 0;
				LastTick4PortMon[port] = 0;
			}
		}
		vosUSleep(500000);
	}
}




gw_status gwdonu_port_statistic_get(gw_int32 ucPortId, gw_int8 * data, gw_int32 * len)
{

	/************* This is four FE ONU **************/
	if((ucPortId>4)||(ucPortId<1))
		return GW_ERROR;
	ucPortId--;

	stats_getArx8306(ucPortId);
	gw_onu_port_counter_t * pd = (gw_onu_port_counter_t*) data;

	pd->rxrate = OctRateIn[ucPortId];
	pd->txrate = OctRateOut[ucPortId];
	pd->counter.RxFramesOk = struct2uint64(ucPortId,7)+struct2uint64(ucPortId,8)+struct2uint64(ucPortId,9)+struct2uint64(ucPortId,10)+struct2uint64(ucPortId,11)+struct2uint64(ucPortId,12);			//RxGoodByte
	pd->counter.RxUnicasts = 0;
	pd->counter.RxMulticasts = struct2uint64(ucPortId,2);			//RxMulti
	pd->counter.RxBroadcasts = struct2uint64(ucPortId,0);			//RxBroad
	pd->counter.Rx64Octets = 0;
	pd->counter.Rx127Octets = 0;
	pd->counter.Rx255Octets = 0;
	pd->counter.Rx511Octets = 0;
	pd->counter.Rx1023Octets = 0;
	pd->counter.RxMaxOctets = struct2uint64(ucPortId,13);			//RxMaxByte
	pd->counter.RxJumboOctets = 0;
	pd->counter.RxUndersize = struct2uint64(ucPortId,5);			//RxRunt
	pd->counter.RxOversize = struct2uint64(ucPortId,14);			//RxTooLong
	pd->counter.RxFragments = struct2uint64(ucPortId,6);
	pd->counter.RxJabber = 0;
	pd->counter.RxFCSErrors = struct2uint64(ucPortId,3);			//RxFcsErr
	pd->counter.RxDiscards = struct2uint64(ucPortId,18);			//Filtered
	pd->counter.RxAlignErrors = struct2uint64(ucPortId,4);
	pd->counter.RxIntMACErrors = 0;
	pd->counter.RxPppoes = 0;
	pd->counter.RxQueueFull = 0;
	pd->counter.RxPause = struct2uint64(ucPortId,1);
	pd->counter.RxOctetsOkMsb = 0;
	pd->counter.RxOctetsOKLsb = struct2uint64(ucPortId,15);
	pd->counter.RxError = struct2uint64(ucPortId,16);			//RxBadByte
	pd->counter.TxFramesOk = struct2uint64(ucPortId,23)+struct2uint64(ucPortId,24)+struct2uint64(ucPortId,25)+struct2uint64(ucPortId,26)+struct2uint64(ucPortId,27)+struct2uint64(ucPortId,28);			//TxByte
	pd->counter.TxUnicasts = 0;
	pd->counter.TxMulticasts = struct2uint64(ucPortId,21);			//TxMulti
	pd->counter.TxBroadcasts = struct2uint64(ucPortId,19);			//TxBroad
	pd->counter.Tx64Octets = 0;
	pd->counter.Tx127Octets = 0;
	pd->counter.Tx255Octets = 0;
	pd->counter.Tx511Octets = 0;
	pd->counter.Tx1023Octets = 0;
	pd->counter.TxMaxOctets = struct2uint64(ucPortId,29);			//TxMaxByte
	pd->counter.TxJumboOctets = 0;
	pd->counter.TxDeferred = struct2uint64(ucPortId,36);			//TxExcDefer
	pd->counter.TxTooLongFrames = struct2uint64(ucPortId,30);			//TxOverSize
	pd->counter.TxCarrierErrFrames = 0;
	pd->counter.TxSqeErrFrames = 0;
	pd->counter.TxSingleCollisions = struct2uint64(ucPortId,35);			//TxSingleCol
	pd->counter.TxMultipleCollisions = struct2uint64(ucPortId,34);			//TxMultiCol
	pd->counter.TxExcessiveCollisions = 0;
	pd->counter.TxLateCollisions = struct2uint64(ucPortId,38);			//TXLateCol
	pd->counter.TxMacErrFrames = 0;
	pd->counter.TxQueueFull = 0;
	pd->counter.TxPause = struct2uint64(ucPortId,20);			//TxPause
	pd->counter.TxOctetsOk = struct2uint64(ucPortId,31);
	pd->counter.TxError = 0;

	return GW_OK;
}
gw_status gwdonu_port_statistic_clear(gw_int32 portid)
{
	HAL_STATS_POLL_t	stStatsPoll = {0,0,0,0};
	OPL_API_DATA_t		stApiData = {0,0,NULL,NULL};

	if(portid>4 ||portid<0)
	{
		return GW_ERROR;
	}
	stStatsPoll.iModule = CNT_SWITCH;
	stStatsPoll.usStart = portid;
	stStatsPoll.usNum = 1;
	stApiData.apiId = HAL_API_STATS_CLR;
	stApiData.param = (void *)&stStatsPoll;
    stApiData.length = sizeof(HAL_STATS_POLL_t);

	halAppApiCallSync(&stApiData);
	return GW_OK;
}

gw_status gwdonu_port_pvid_get(gw_int32 portid, gw_int16 *vlanid)
{
//	printf("in gwdonu_port_pvid_get fuction ,this function is not defined .......\r\n");
	UINT32 ret=0;

	ret = dalPortPvidGet(portid,vlanid);
//	printf("gwdonu_port_pvid_get Here May form ERROR!\r\n");
	return ret;
}

gw_int32 gwdonu_port_mirror_stat_get(gw_int32 unit,gw_int32* mode)
{

}
gw_int32 gwdonu_port_mirror_stat_set(gw_int32 unit,gw_int32 mode)
{
}

gw_int32 gwdonu_port_ingress_mirror_set(gw_int32 unit,gw_int32 port,gw_int32 stat_val)
{
}

gw_int32 gwdonu_port_egress_mirror_set(gw_int32 unit,gw_int32 port,gw_int32 stat_val)
{
}

gw_int32 gwdonu_port_mirror_to_port_set(gw_int32 port,gw_int32 portmap)
{
}




gw_status gwdonu_vlan_entry_getnext(gw_uint32 index, gw_uint16 *vlanid, gw_uint32 *tag_portlist, gw_uint32 *untag_portlist)
{

	fal_vlan_t vlan;
#if 0
	fal_pt_1q_egmode_t pport_egvlanmode;
	int portnum=0;
	for(portnum=0;portnum<4;portnum++)
	{
		shiva_port_egvlanmode_get(0, portnum,&pport_egvlanmode);
		printf("gwdonu_vlan_entry_get  port state is %d\r\n",pport_egvlanmode);
	}
#endif

	int portnum=0;
	int retVal;
	int i;
	UINT32 vlanTrunkNum;
	UINT32 vlan_id;
	int inputvid = 0;

	*tag_portlist = 0;
	*untag_portlist = 0;

	if(vlanid == NULL || tag_portlist == NULL || untag_portlist == NULL )
		return GW_ERROR;
	else
	{
		if (GW_OK == shiva_vlan_next(0,*vlanid,&vlan))
		{
			if(*vlanid != vlan.vid)
				*vlanid = vlan.vid;
//			printf("\ngetnext  vid is 0x%x ,*vlanid is %d,mem_ports is 0x%x , u_ports is 0x%x,tag_portlist is 0x%x  and untag_portlist is 0x%x \r\n",vlan.vid,*vlanid,vlan.mem_ports,vlan.u_ports,*tag_portlist,*untag_portlist);
			for(portnum=1;portnum<=4;portnum++)
			{
				retVal = odmPortDefaultVlanGet(portnum, &vlan_id);
	//			printf("DefaultVlan is %d  and retVal is %d.......\r\n",vlan_id,retVal);
				if(retVal == OK)
				{
					if(vlan_id == *vlanid)
					{
						*untag_portlist |= 1<<(portnum-1);
						continue;
					}
				}

				/* get vlan trunk entry */
				odmPortVlanTrunkEntryNumGet(portnum, &vlanTrunkNum);
				for(i = 0; i < ODM_MAX_NUM_OF_TRUNK_VLAN; i++)
				{
					retVal = odmPortVlanTrunkEntryValueGet(portnum, i, &vlan_id);

					if(retVal != OK)
					{
						continue;
					}
					if(vlan_id == *vlanid)
					{
						*tag_portlist |= 1<<(portnum-1);
						break;
					}
				}
			}
#ifdef GWDDEBUG
			printf("\nvlanid is 0x%x, tag_portlist is 0x%x ,untag_portlist is 0x%x...\r\n\n",*vlanid,*tag_portlist,*untag_portlist);
#endif
		}
		else
			return GW_ERROR;
	}
	return GW_OK;
}


gw_status gwdonu_vlan_entry_get(gw_uint32 vlanid, gw_uint32 *tag_portlist, gw_uint32 *untag_portlist)
{
	fal_vlan_t vlan;
  INT32 halStatus = OK;
	fal_pt_1q_egmode_t pport_egvlanmode;
  UINT16 pvid = 0;
	int portnum=0;
	int retVal;
	fal_pbmp_t mem_port_map;

#if 0
	for(portnum=0;portnum<4;portnum++)
	{
		shiva_port_egvlanmode_get(0, portnum,&pport_egvlanmode);
//		printf("\ngwdonu_vlan_entry_get  port state is %d\r\n",pport_egvlanmode);
   halStatus = dalPortPvidGet(portnum,&pvid);
   if(OK == halStatus)
	    {
	        printf("gwdonu_vlan_entry_get PortPvid is 0x%x..\n",pvid);
	    }
	shiva_portvlan_member_get(0, portnum, &mem_port_map);
	//printf("gwdonu_vlan_entry_get mem_port_map is 0x%x..\n\n",mem_port_map);
	}

	if (GW_OK == shiva_vlan_find(0, vlanid,&vlan))
	{
		*tag_portlist = vlan.mem_ports>>1;
		*untag_portlist = !vlan.mem_ports>>1 ;
		//printf("get  mem_ports is %d , u_ports is %d,tag_portlist is %d  and untag_portlist is %d \r\n",vlan.mem_ports,vlan.u_ports,*tag_portlist,*untag_portlist);
	}
	else
		return GW_ERROR;
#endif
	int i;
	UINT32 vlanTrunkNum;
	UINT32 vlan_id;

	*tag_portlist = 0;
	*untag_portlist = 0;

	if (GW_OK == shiva_vlan_find(0, vlanid,&vlan))
	{
		//printf("get  mem_ports is %d , u_ports is %d\r\n",vlan.mem_ports,vlan.u_ports);

		for(portnum=1;portnum<=4;portnum++)
		{
			retVal = odmPortDefaultVlanGet(portnum, &vlan_id);
			if(retVal == OK)
			{
				//printf("vlan_id is %d  and vlanid is %d..............\r\n",vlan_id,vlanid);
				if(vlan_id == vlanid)
				{
					*untag_portlist |= 1<<(portnum-1);
					//printf("untag_portlist is 0x%x...\r\n",*untag_portlist);
					continue;
				}
			}

			/* get vlan trunk entry */
			odmPortVlanTrunkEntryNumGet(portnum, &vlanTrunkNum);
			for(i = 0; i < ODM_MAX_NUM_OF_TRUNK_VLAN; i++)
			{
				retVal = odmPortVlanTrunkEntryValueGet(portnum, i, &vlan_id);

				if(retVal != OK)
				{
					continue;
				}
				if(vlan_id == vlanid)
				{
					*tag_portlist |= 1<<(portnum-1);
					break;
				}
			}
		}
#ifdef GWDDEBUG
		printf("\nGwdonu_vlan_entry_get vlanid is 0x%x, tag_portlist is 0x%x ,untag_portlist is 0x%x...\r\n\n",vlanid,*tag_portlist,*untag_portlist);
#endif
	}
	else
		return GW_ERROR;
return GW_OK;
}


gw_status gwdonu_fdb_entry_get(gw_uint32 vid, gw_uint8 * macaddr, gw_uint32 *eg_portlist)
{
	fal_fdb_entry_t entry;
	a_uint32_t  iterator = 0;
	unsigned int tag_ports , untag_ports;
	unsigned int rv,i=0,uiPortVect=0;


	if(NULL == macaddr)
	{
		return GW_ERROR;
	}
	while(1)
	{
		rv = shiva_fdb_iterate(0, &iterator, &entry);
		if (GW_OK != rv)
		{
			return GW_ERROR;
		}
#ifdef GWDDEBUG
		printf("gwdonu_fdb_entry_get rv is %d, entry.vid is %d,vid is %d,eg_portlist is %d\r\n",rv,entry.vid,vid,*eg_portlist);
		printf("gwdonu_fdb_entry_get entry.fid_en is %d\r\n",entry.fid_en);
		printf("gwdonu_fdb_entry_get entry.portmap_en is %d\r\n",entry.portmap_en);
		printf("gwdonu_fdb_entry_get entry.port.id is %d\r\n",entry.port.id);
#endif
		if(!memcmp(entry.addr.uc,macaddr,6))
		{
			gwdonu_vlan_entry_get(vid,&tag_ports,&untag_ports);
			*eg_portlist = ((int)dalArlPortmapReverse(entry.port.map)>>1)&0x0f;
			if(0 == *eg_portlist)
				continue;
			else
			{
				for(i=0;i<4;i++)
				{
					uiPortVect = 0x1 << i;
					if (*eg_portlist & uiPortVect)
					{
						if((0 == (tag_ports & uiPortVect)) &&
						   (0 == (untag_ports & uiPortVect)) )
						{
#ifdef GWDDEBUG
							printf("FDB ENTRY MAC found in wrong vlan %d\r\n",vid);
#endif
						}
						else
						{
#ifdef GWDDEBUG
							printf("FDB ENTRY MAC found in Right vlan %d\r\n",vid);
#endif
							return GW_OK;
						}
					}
				}
				continue;
			}
		}
				//			strncpy(macaddr,entry.addr.uc,6);
#ifdef GWDDEBUG
			char macStr[32];
			vosMacToStr(entry.addr.uc, macStr);
			printf("port id  is 0x%x...\r\n",(int)dalArlPortmapReverse(entry.port.map));
			printf("Mac is %s..\r\n\n",macStr);
#endif
	}
		return GW_ERROR;
}

int portmaptrans(int portmap)
{
	int result=-1;
	switch(portmap)
	{
	case 0x01:
		result = 0;
		break;
	case 0x02:
		result = 1;
		break;
	case 0x04:
		result = 2;
		break;
	case 0x08:
		result = 3;
		break;
	case 0x10:
		result = 4;
		break;
	case 0x20:
		result = 5;
		break;
	case 0x40:
		result = 6;
		break;
	default:
		printf("portmap error...\r\n");
	}
	return result;
}
gw_status gwdonu_fdb_entry_getnext(gw_uint32 vid, gw_uint8 * macaddr, gw_uint32 *nextvid, gw_uint8 *nextmac, gw_uint32 * eg_portlist,gw_uint32* statics)
{
	fal_fdb_entry_t entry;
	static a_uint32_t  iterator = 0;
	int rv;
	int ret;

	if(0 == vid)
		iterator = 0;

	rv = shiva_fdb_iterate(0, &iterator, &entry);
	if (GW_OK != rv)
	{
		return GW_ERROR;
	}
#ifdef GWDDEBUG
		char macStr[32];
		vosMacToStr(entry.addr.uc, macStr);
		printf("\n gwdonu_fdb_entry_getnext  vid is %d...\r\n",entry.vid);
		printf("portmap is %d...\r\n",entry.port.map);
		printf(" gwdonu_fdb_entry_getnext  Mac is %s..\r\n\n",macStr);
#endif
		*nextvid = 1;
		memcpy(macaddr,entry.addr.uc,6);
		memcpy(nextmac,entry.addr.uc,6);
		if(-1 != (ret = portmaptrans(entry.port.map)))
			*eg_portlist = ret;
//		else
//			return GW_ERROR;
		if(entry.static_en)
			*statics = 2;
		else
			*statics = 1;
	return GW_OK;
}


gw_status gwdonu_fdb_mgt_mac_set(gw_uint8 * gw_mac)
{
#if 1
	int ret = GW_ERROR ;
	char destMac[18];
	char digitalMac[6];
	UINT32 auiPortlist[4];     //4 is port num  this array is used for the function
//	GwDumpPkt(gw_mac,6);
//printf("gwdonu_onu_static_mac_add...gw_port is %d\r\n");
	if(NULL == gw_mac)
		return GW_ERROR;
	memset(destMac,0,18);
	memset(digitalMac,0,6);

	/*-----cann't configure multicast mac -------*/
	if((gw_mac[0]&0x1)==1)
	{
		printf("\nerror:cann't configure multicast mac\n");
		return GW_ERROR;
	}
		auiPortlist[0] = 0;
		ret = odmFdbMacDel(gw_mac, 0);
		ret = odmFdbMacPortAdd(1,auiPortlist, gw_mac, 0);

	return ret;
#endif
#if 0
	char className[] = "loopbackPkt";
	UINT8 *mac_a = "00:0f:e9:04:8E:DF";
	unsigned char ret = -1;

	printf("1mac_a is %s\r\n",mac_a);
	ret = odmCreateClass(className);
  switch (ret)
    {
        case QOS_OK:
            break;
        case QOS_CLS_EXIST_CHECK_ERROR:
            printf("error:Class name is existed!\r\n");
        default :
        			return GW_ERROR;
            break;
    }
  printf("2mac_a is %s\r\n",mac_a);
  ret = odmCfgClassDestMac(className,mac_a,CLASS_ADD);
  ret = odmCfgClassTranfAct(className,SENDTOCPU,CLASS_ADD);
  ret = odmInstallClassEntry(className, BOTH_STREAM_, NULL);
#endif

#if 0
#if 0
	fal_fdb_entry_t entry;
	UINT8	hwAddr[6];

	if(NULL == mac)
	{
		printf("Mgt Mac is NULL...\r\n");
		return	GW_ERROR;
	}
	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, mac, 6);
	entry.static_en = 1;
	entry.sacmd = FAL_MAC_FRWRD;
	entry.dacmd = FAL_MAC_RDT_TO_CPU;
	entry.port.map = 0x1;
	entry.portmap_en = 1;
	entry.cross_pt_state = 1;

	shiva_fdb_add(0, &entry);

	vosHWAddrGet("eth3", &hwAddr[0]);
	aos_mem_zero(&entry, sizeof(fal_fdb_entry_t));
	aos_mem_copy((void *)entry.addr.uc, hwAddr, 6);
	entry.static_en = 1;
	entry.dacmd = FAL_MAC_FRWRD;
	entry.port.map = 0x1;
	entry.portmap_en = 1;

	shiva_fdb_add(0, &entry);
#else

	#define USER_RESERVED_MAC0_32 (0x3030*4)
	#define USER_RESERVED_MAC0_16 (0x3031*4)
	#define USER_RESERVED_MAC1_32 (0x3032*4)
	#define USER_RESERVED_MAC1_16 (0x3033*4)
	#define USER_RESERVED_MAC_CTRL (0x303D*4)

	OPL_STATUS iStatus=OPL_OK;
	UINT32 uiRegVal = 0;
	iStatus = oplRegRead(USER_RESERVED_MAC0_32, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC0 32 ==========0x%x\r\n",uiRegVal);
	iStatus = oplRegRead(USER_RESERVED_MAC0_16, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC0 16 ==========0x%x\r\n",uiRegVal);
	iStatus = oplRegRead(USER_RESERVED_MAC1_32, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC1 32 ==========0x%x\r\n",uiRegVal);
	iStatus = oplRegRead(USER_RESERVED_MAC1_16, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC1 16 ==========0x%x\r\n",uiRegVal);
	iStatus = oplRegRead(USER_RESERVED_MAC_CTRL, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC_CTRL ==========0x%x\r\n",uiRegVal);

	printf("!!!!!!!!!!!!!!!!!!!!before\r\n");


	uiRegVal = 0xe9aabbcc;
	iStatus = oplRegWrite(USER_RESERVED_MAC1_32, uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========write error\r\n");
	}
	uiRegVal = 0x0f;
	iStatus = oplRegWrite(USER_RESERVED_MAC1_16, uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========write error\r\n");
	}

	iStatus = oplRegRead(USER_RESERVED_MAC_CTRL, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	uiRegVal &= ~0xf;
	uiRegVal |= 0x4;
	iStatus = oplRegWrite(USER_RESERVED_MAC_CTRL, uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========write error\r\n");
	}
	printf("!!!!!!!!!!!!!!!!!!!!after\r\n");

	iStatus = oplRegRead(USER_RESERVED_MAC0_32, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC0 32 ==========0x%x\r\n",uiRegVal);
	iStatus = oplRegRead(USER_RESERVED_MAC0_16, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC0 16 ==========0x%x\r\n",uiRegVal);
	iStatus = oplRegRead(USER_RESERVED_MAC1_32, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC1 32 ==========0x%x\r\n",uiRegVal);
	iStatus = oplRegRead(USER_RESERVED_MAC1_16, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC1 16 ==========0x%x\r\n",uiRegVal);
	iStatus = oplRegRead(USER_RESERVED_MAC_CTRL, &uiRegVal);
	if (OPL_OK != iStatus)
	{
			printf("USER_RESERVED_MAC ==========read error\r\n");
	}
	printf("USER_RESERVED_MAC_CTRL ==========0x%x\r\n",uiRegVal);

#endif
#endif
#if 0
	int iStatus=-1;
	int i=0;
	UINT32 uiRegVal = 0;
printf("\n");
	for(i=0x6000;i<0x6087;i++)
	{
	iStatus = oplRegRead(i*4, &uiRegVal);
	if (OPL_OK != iStatus)
	{
		printf("0x%x = -1    ",i);
	}
	printf("0x%x = 0x%x    ",i,uiRegVal);
	if(i%8==0)
		printf("\n");
	}
#endif
	return GW_OK;
}


gw_status gwdonu_atu_learn_get(gw_int32 portid, gw_int32 *en)
{
	a_bool_t *enable;
	shiva_fdb_port_learn_get(0,portid,enable);
	if((*enable == A_FALSE)||(*enable == A_TRUE))
	{
		*en = *enable;
		return GW_OK;
	}
#ifdef GWDDEBUG
	printf("gwdonu_atu_learn_get error the enable is %d\r\n",enable);
#endif
	return GW_ERROR;

}


gw_status gwdonu_atu_learn_set(gw_int32 portid, gw_int32 en)
{
	UINT32 ulRet;

	if((portid >0)&&(portid <5))
	{
		if((en == A_FALSE)||(en == A_TRUE))
		{
		ulRet = shiva_fdb_port_learn_set(0, portid, en);
	  if (GW_OK != ulRet)
		{
		 printf("gwdonu_atu_learn_set error!The ulRet is %d\r\n",ulRet);
			return GW_ERROR;
		}
	  return GW_OK;
		}
		else
			printf("gwdonu_atu_learn_set error,because en is not bool,en is %d\r\n",en);
	}
return GW_ERROR;
}


gw_status gwdonu_atu_age_get(gw_uint32 *age)
{
	UINT32 ulRet;

	ulRet = shiva_fdb_age_time_get(0, age);
  if (GW_OK != ulRet)
    {
    printf("gwdonu_atu_age_get error!The age is %d\r\n",*age);
    return GW_ERROR;
    }
	return GW_OK;
}


gw_status gwdonu_atu_age_set(gw_uint32 age)
{
	UINT32 ulRet;

	ulRet = shiva_fdb_age_time_set(0, &age);
  if (GW_OK != ulRet)
    {
	    printf("gwdonu_atu_age_get error!The age is %d\r\n",age);
	    return GW_ERROR;
	    }
		return GW_OK;
}


gw_status gwdonu_set_mac(gw_uint8 *mac)
{
//	printf("in gwdonu_set_mac fuction ,this function is not defined .......\r\n");
  UINT32  retVal = OK;
  UINT8   macId[6];

	char section[20];
	int ret=0;

	strcpy(section,"Product Information");

    retVal = string2mac(mac,macId); 			//string mac have ':' so need to be translated
    if(OK != retVal)
    {
        return ERR_INVALID_PARAMETERS;
    }

    retVal = odmPonMacIdSet(macId);

    if(OK != retVal)
    	{
     printf("set mac error\r\n");
        		return ERROR;
    	}
	if(vosConfigSectionIsExisted(PRODUCT_CFG_FILE,section))
	{
		ret = vosConfigSectionCreate(PRODUCT_CFG_FILE,section);
	}
	if ((ret = vosConfigValueSet(PRODUCT_CFG_FILE,section,"PON MAC",mac)) != 0)
	{
			printf("save error\n");
		    return ERROR;
	}

    cliTextConfigSave();
    vosConfigSave(NULL);

	return 0;

}


extern int stub_get_onu_optical_transiver_temperature(u16_t *pTemperature);
extern int stub_get_onu_optical_transiver_supply_vcc(u16_t *pVcc);
extern int stub_get_onu_optical_transiver_tx_bias_current(u16_t *pCurrent);
extern int stub_get_onu_optical_transiver_rx_power(u16_t *pRxPower);
extern int stub_get_onu_optical_transiver_tx_power(u16_t *pTxPower);

gw_status gwdonu_opm_get(gw_uint16 *temp,gw_uint16 *vcc,gw_uint16 *bias,gw_uint16 *txpow,gw_uint16 *rxpow)
{
	short int temperature;
	u16_t vcc_t;
	u16_t current;
	u16_t txPower;
	u16_t rxPower;
	double txdbm =0.9,rxdbm = 0.0;

	stub_get_onu_optical_transiver_temperature(temp);
	stub_get_onu_optical_transiver_supply_vcc(vcc);
	stub_get_onu_optical_transiver_tx_bias_current(bias);
	stub_get_onu_optical_transiver_tx_power(&txPower);
	stub_get_onu_optical_transiver_rx_power(&rxPower);

	*txpow = 10*log10(txPower*0.0001);
	*rxpow = 10*log10(rxPower*0.0001);
#if 0
	temperature /=256;
	*temp = temperature;
	*vcc = (u16_t)(vcc_t*0.1);
	current *=0.002;
	*bias = current;
	txdbm = txPower;
	rxdbm = rxPower;

	txdbm = 10*log10(txdbm*0.0001);
	rxdbm = 10*log10(rxdbm*0.0001);

	*txpow = txdbm;
	*rxpow = rxdbm;
	printf("temperature			:%hd C\n",temperature);
	printf("working-voltage			:%hd mV\n",vcc);
	printf("bias-current			:%hd mA\n",current);
	printf("txPower				:%4.1f dbm\n",txdbm);
	printf("rxPower				:%4.1f dbm\n",rxdbm);
#endif
	return 0;
}


gw_status gwdonu_laser_get(gw_EponTxLaserStatus * state)
{
	UINT32 data = 0;

	oplRegFieldRead(REG_PMAC_LASER_ON_CFG, 0, 4, &data);

	if(5 == data)
		*state = GwEponTxLaserNormal;
	else if(2 == data)
		*state = GwEponTxLaserAlwaysOn;
	else if(8 == data)
		*state = GwEponTxLaserDisable;
	else
	{
		printf("gwdonu_laser_get error..The pon state is %d\r\n",data);
		return GW_ERROR;
	}
	return GW_OK;
}


gw_status gwdonu_laser_set(gw_EponTxLaserStatus state)
{
	if(GwEponTxLaserNormal == state)
    oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 5);
	else if(GwEponTxLaserAlwaysOn == state)
	    oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 2);
	else if(GwEponTxLaserDisable == state)
	    oplRegFieldWrite(REG_PMAC_LASER_ON_CFG, 0, 4, 8);
	else
	{
		printf("Can not set this state the state is %d\r\n",state);
		return GW_ERROR;
	}
	return GW_OK;
}

extern oam_instance_tlv_t	port_instance;
gw_status gwdonu_port_loop_event_post(gw_uint32 status)
{
	u16_t port_num;
	u32_t state;
	u16_t port_start;
	u16_t port_end;
	u16_t i;
	u32_t ret=-1;
	u32_t value;

	value = (u32_t)port_instance.value;
	port_num = (u16_t)(((u32_t)value & 0x0000FFFF) >> 0);
	if(0xFFFF == port_num)
	{
		port_start = 1;
		port_end = OAM_MAX_INSTANCE_ID;
	}
	else
	{
		port_start = port_num;
		port_end = port_num;
	}
	state = status;
#if 1
	for(i = port_start; i <= port_end; i++)
	{
		if ((1 == state ) || ( 2 == state))
		{
			ret = odmLoopDetectionSet(i, state - 1);
		}
		else
		{
			printf("gwdonu_port_loop_event_post error the state is %d\r\n",state);
		}
	}
#else
    ret = odmLoopDetectionSet(state - 1);
#endif /* #if 0 */
	if(ret == 0)
		return GW_OK;
	else
		return GW_ERROR;
}

int  gwdonu_special_pkt_handler_pon(void *pkt,int len)
{
	gw_uint8 buff[1600]={0};
	host_inbound_hdr_t *pstInHeader_t = NULL;
	char ucPort_t;

	if(len>1600)
	{
		return GW_ERROR;
	}
  memcpy(buff, pkt+4,len-4);

	pstInHeader_t = (host_inbound_hdr_t *)pkt;
	ucPort_t = (char)pstInHeader_t->iport;

//	gwlib_sendPktToQueue((gw_uint8 *)pkt+4, len-4, ucPort_t);

	//gw_debug_printf("\r\nOUI:%02x %02x %02x\r\n", buff[18], buff[19], buff[20]);
	if(g_onu_pkt_handler)
	{
		(*g_onu_pkt_handler)((gw_int8*)buff, len-4, ucPort_t);
			return GW_OK;
	}
	else
	{
		printf("gwdonu_special_pkt_handler  g_onu_pkt_handler is not registered..\r\n");
   return GW_ERROR;
	}
}
int  gwdonu_special_pkt_handler_fe(void *pkt,int len)
{
	gw_uint8 buff[1600]={0};
	ATHEROS_HEADER_FRAME_t *pstInHeader_t = NULL;
	char ucPort_t;

	if(len>1600)
	{
		return GW_ERROR;
	}
  memcpy(buff, pkt,12);

	pstInHeader_t = (ATHEROS_HEADER_FRAME_t *)pkt;
	ucPort_t = (char)pstInHeader_t->portNum;
	memcpy(buff+12,pkt+14,len-14);
//	gwlib_sendPktToQueue((gw_uint8 *)pkt+4, len-4, ucPort_t);

	//gw_debug_printf("\r\nOUI:%02x %02x %02x\r\n", buff[18], buff[19], buff[20]);
	if(g_onu_pkt_handler)
	{
		(*g_onu_pkt_handler)((gw_int8*)buff, len-2, ucPort_t);
			return GW_OK;
	}
	else
	{
		printf("gwdonu_special_pkt_handler  g_onu_pkt_handler is not registered..\r\n");
   return GW_ERROR;
	}
}
gw_status gwdonu_register_special_pkt_handler( libgwdonu_special_frame_handler_t phandler)
{
	g_onu_pkt_handler = phandler;
	return GW_OK;
}

int  gwdonu_onu_hwver_get(gw_int8 *hw, const gw_int32 hwlen)
{
	if(g_onu_out_if_hwver_get)
	{
		return (*g_onu_out_if_hwver_get)(hw, hwlen);
	}
	else
		printf("onu hw version get if is NULL\r\n");

	return GW_ERROR;
}
gw_status gwdonu_register_hwver_get_handler( void * phandler)
{
	g_onu_out_if_hwver_get = phandler;
	return GW_OK;
}


gw_status gwdonu_onu_console_cli_register(void * handler)
{

}


gw_status gwdonu_onu_current_timer_get(gw_uint32* gw_time)
{
	struct timeval tv;

	if(NULL == gw_time)
		return GW_ERROR;

	gettimeofday(&tv, NULL);
	*gw_time=tv.tv_sec*100 + tv.tv_usec/10000;
	return GW_E_OK;
}


gw_status gwdonu_onu_broadcast_speed_limit_set(gw_uint32 gw_port,gwd_sw_port_inratelimit_mode_t gw_mode,gw_uint32 gw_rate)
{
	UINT32 retVal;
	retVal = 	odmStormCtrlRateSet(gw_port,gw_rate);

	if(retVal != OK)
	{
		printf("gwdonu_onu_broadcast_speed_limit_set error! The port is %d, gw_rate is %d\r\n",gw_port,gw_rate);
		return GW_ERROR;
	}
	return GW_OK;
}


gw_status gwdonu_onu_localtime_get(localtime_tm *gw_tm)
{
	time_t now;
	struct tm *tm = NULL;
	#if 0
	tm = (struct tm *)malloc(sizeof(struct tm));
	if(tm == NULL)
		{
			printf("malloc error\n");
			return GW_E_ERROR;
		}
	memcpy(tm,gw_tm,sizeof(struct tm));
	Now = mktime(tm);
	printf("tick:%d\n",Now);
	now = Now;
	time (&now);
	printf("get time now ...\n");
    tm = localtime (&now);
	memcpy(gw_tm,tm,sizeof(struct tm));
	free(tm);
	#else
	time (&now);
	//printf("now:%d\n",now);
	tm = localtime (&now);
	memcpy(gw_tm,tm,sizeof(struct tm));
	#endif
    return GW_E_OK;
}

/*------ This function is used to trans xxxx.xxxx.xxxx to xx:xx:xx:xx:xx:xx -----*/
int dotMac2colonMac(char *src,char *des)
{
	int ret=GW_ERROR;
	unsigned char length=0,i=0,j=0;

	if((NULL == src)||(NULL == des))
		return ret;
	length = strlen(src);
	if(14 == length)
	{
		for(i=0;i<17;i++)
		{
			if((0==(i+1)%3)&&(i!=0))
				des[i] = ':';
			else
			{
				if('.' == src[j])
					j++;
				des[i] = src[j++];
			}
		}
		return  GW_OK;
	}
	printf("src length error ,length is %d\r\n",length);
	return ret;
}
gw_status gwdonu_onu_static_mac_add(gw_int8* gw_mac,gw_uint32 gw_port,gw_uint32 gw_vlan)
{
	int ret = GW_ERROR ;
	char destMac[18];
	char digitalMac[6];
	UINT32 auiPortlist[4];     //4 is port num  this array is used for the function

//printf("gwdonu_onu_static_mac_add...gw_port is %d\r\n");
	if((NULL == gw_mac)||(gw_port<1)||(gw_port>4))
		return GW_ERROR;
	memset(destMac,0,18);
	memset(digitalMac,0,6);

	/*-----cann't configure multicast mac -------*/
	if(((vosMacCharToDigit(gw_mac[1]))%2)==1)
	{
		printf("\nerror:cann't configure multicast mac\n");
		return GW_ERROR;
	}
	if(GW_OK == dotMac2colonMac(gw_mac,destMac))
	{
		ret = vosStrToMac(destMac, digitalMac);
		if (GW_OK == ret)
		{
			auiPortlist[0] = gw_port;
			ret = odmFdbMacPortAdd(1,auiPortlist, digitalMac, 0);
		}
		else
			ret = GW_ERROR;
	}
	return ret;

}

gw_status gwdonu_onu_static_mac_del(gw_int8* gw_mac,gw_uint32 gw_vlan)
{
	int ret=GW_ERROR;
	char destMac[18];
	char digitalMac[6];

	if(GW_OK == dotMac2colonMac(gw_mac,destMac))
	{
		printf("destMac is %s\r\n",destMac);
		ret = vosStrToMac(destMac, digitalMac);
		if (GW_OK == ret)
		{
			ret = odmFdbMacDel(digitalMac, 0);
		}
	}
	return ret;
}


gw_status gwdonu_onu_register_stat_get(gw_uint8* onuregister)
{
	if(NULL != onuregister)
		if(OPL_OK == OamStatusGet(onuregister))
		return GW_OK;
	else
		return GW_ERROR;
}


#ifndef CYG_LINUX
gw_status gwdonu_onu_reset(gw_int32 a)
{
	system("reboot");
	return GW_OK;
}
#endif

/**** This onu does not have  loopalm led ****/
void gwdonu_onu_set_loopalm_startled()
{
	loopdetect_en = 1;
	printf("Loop Alarm On..\r\n");
}

void gwdonu_onu_set_loopalm_stopled()
{
	loopdetect_en = 0;
	printf("Loop Alarm Off..\r\n");
}

gw_status gwdonu_ver_get(char * sw_ver, const int sw_ver_len)
{
	memset(sw_ver,0,sw_ver_len);
	sprintf(sw_ver,"V%dR%02dB%03d",MAJOR_VERSION,MINOR_VERSION,BUILD_NUMBER);
	return GW_OK;
}


gw_status gwdonu_syslog_register_heandler(libgwdonu_syslog_heandler_t handler)
{
	g_onu_syslog_handler = handler;
	return GW_OK;
}

gw_int32 gwdonu_vfile_open(gw_uint8 * fname, gw_int32 mode, gw_int32 * fd, gw_uint8 ** pv)
{
	gw_int32 lfd = 0, ret = -1, flags = 0;
	gw_uint8 name[80] = "/cfg/defconf";
	if(fname )
		strcpy((char*)name, (const char*)fname);

	if(mode == 1)
		flags = O_RDONLY;
	else
		flags = O_CREAT|O_RDWR;

	lfd = open((const char*)name, flags, 0777);


	if(lfd >= 0)
	{
		*fd = lfd;
		ret = 1;
	}

	*pv = NULL;

	return ret;
}
gw_int32 gwdonu_vfile_close(void *data)
{
	gw_int32 fd =	*(gw_int32*)data;
	close(fd);
	return 0;
}
gw_int32 gwdonu_qos_vlan_queue_map_t(gw_int32 count, gw_qos_vlan_queue_data_t * data)
{
	return GW_OK;
}
gw_status gwdonu_config_write_to_flash_t()
{
	gw_int32 ret = GW_OK;

    cliTextConfigSave();
    vosConfigSave(NULL);

	return ret;
}
extern const char * build_date;
extern const char * build_time;

gw_int32 gwdonu_version_build_time_get(gw_int8* buildtime)
{
	sprintf(buildtime,"%s %s",build_time,build_date);
	return GW_OK;
}

gw_int32 gwdonu_cpld_write_register(gw_uint32 reg,gw_uint32 date)
{
//		return GW_ERROR;
}

gw_int32 gwdonu_cpld_read_register(gw_uint32 reg,gw_uint8 *date)
{
	//	return GW_ERROR;
}
gw_int32 gwdonu_poe_port_operation_set(gw_int32 port,gw_int32 stat)
{
//	return GW_ERROR;
}

gw_status gwdonu_multicast_mode_set(mc_mode_t mode)
{
	gw_uint32 mulconfig = 0 ,ret = 0;

	if((MC_SNOOPING <= mode) && (mode <=MC_PROXY))
	{
		ret = dalMulticastEnable();
		ret += odmMulticastSwitchSet(mode);
		mulconfig = 1;
	}
	else if(MC_DISABLE == mode)
	{
		ret = dalMulticastDisable();
		mulconfig = 0;
	}
	else
		printf("multicast mode set error\r\n");
	if(!ret)
	vosConfigUInt32Set(CFGFILE_MULTICAST, CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
	      CFGKEY_MULTICAST_IGMP_MANAGEMENT_IGMP_MODE, mulconfig);
	return GW_OK;
}
gw_status gwdonu_multicast_mode_get(mc_mode_t *mode)
{
	gw_uint32 mulconfig = MC_DISABLE;
	mulconfig = vosConfigUInt32Get(CFGFILE_MULTICAST,
	    CFGSECTION_MULTICAST_IGMP_MANAGEMENT,
	    CFGKEY_MULTICAST_IGMP_MANAGEMENT_IGMP_MODE, mulconfig);
	if(mulconfig)
	{
		*mode = odmMulticastSwitchGet();
	}
	else
		*mode = MC_DISABLE;
	return GW_OK;
}
gw_status gwdonu_real_product_type_get(char *st)
{
	unsigned char cardstatus;
	cardstatus = get_serialcard_status();
	if(0x1 == cardstatus)
		*st = DEVICE_TYPE_GT872_A;
	else
		*st = DEVICE_TYPE_GT873_A;

	return GW_OK;
}

gwdonu_im_if_t g_onu_im_ifs = {
		gwdonu_onu_llid_get,
		gwdonu_onu_sys_info_get,
		gwdonu_sys_conf_save,
		gwdonu_sys_conf_restore,
		gwdonu_sw_conf_save,
		gwdonu_sw_conf_restore,
		gwdonu_port_send,
		gwdonu_oam_std_hdr_builer,

		gwdonu_port_admin_status_get,
		gwdonu_port_admin_status_set,
		gwdonu_port_oper_status_get,
		gwdonu_port_mode_get,
		gwdonu_port_mode_set,
		gwdonu_port_isolate_get,
		gwdonu_port_isolate_set,
		gwdonu_port_statistic_get,
		gwdonu_port_statistic_clear,
		gwdonu_port_pvid_get,
		gwdonu_port_mirror_stat_get,
		gwdonu_port_mirror_stat_set,
		gwdonu_port_ingress_mirror_set,
		gwdonu_port_egress_mirror_set,
		gwdonu_port_mirror_to_port_set,


		gwdonu_vlan_entry_getnext,
		gwdonu_vlan_entry_get,
		gwdonu_fdb_entry_get,
		gwdonu_fdb_entry_getnext,
		gwdonu_fdb_mgt_mac_set,
		gwdonu_atu_learn_get,
		gwdonu_atu_learn_set,
		gwdonu_atu_age_get,
		gwdonu_atu_age_set,

		gwdonu_set_mac,

		gwdonu_opm_get,
		gwdonu_laser_get,
		gwdonu_laser_set,

		gwdonu_port_loop_event_post,

 		gwdonu_register_special_pkt_handler,


 		gwdonu_register_hwver_get_handler,
 		gwdonu_onu_console_cli_register,

 		gwdonu_onu_current_timer_get,
 		gwdonu_onu_broadcast_speed_limit_set,
 		gwdonu_onu_localtime_get,
 		gwdonu_onu_static_mac_add,
 		gwdonu_onu_static_mac_del,
		gwdonu_onu_register_stat_get,
#ifndef CYG_LINUX
		gwdonu_onu_reset,
#endif
		gwdonu_onu_set_loopalm_startled,
		gwdonu_onu_set_loopalm_stopled,

		gwdonu_olt_mac_get,
		gwdonu_ver_get,
		gwdonu_syslog_register_heandler,

		gwdonu_vfile_open,
		gwdonu_vfile_close,

		gwdonu_qos_vlan_queue_map_t,
		gwdonu_config_write_to_flash_t,
		gwdonu_version_build_time_get,

		gwdonu_cpld_read_register,
		gwdonu_cpld_write_register,
		gwdonu_poe_port_operation_set,
		gwdonu_multicast_mode_set,
		gwdonu_multicast_mode_get,
		gwdonu_real_product_type_get
} ;


void gw_plat_init()
{
	VOS_THREAD_t port_rate_thread_id=NULL;

	port_rate_thread_id  = vosThreadCreate(
		"port rate",
		PORT_RATE_STATE_STACKSIZE,
		PORT_RATE_STATE_THREAD_PRIORITY,
            (FUNCPTR)gw_port_rate_thread,
            (void *)0);
}






