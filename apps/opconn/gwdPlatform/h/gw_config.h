#ifndef __GW_CONFIG__
#define __GW_CONFIG__

/*----------------------------------------------------------------------*/
/*
 * ���������궨���ˡ������͡��ء�����״̬���������걻�����
 * �����á����磺
 *
 *	#define	RPU_MODULE_RTPRO_OSPF	RPU_NO
 *	#define	RPU_MODULE_RTPRO_RIP	RPU_YES
 *
 * ��ʾ��̬·��Э��OSPF���رգ�����̬·��Э��RIP����������
 * ����ϵͳ�ڱ���ʱ���Ϳ��Բ��OSPFģ�������RIPģ�顣
 * ��Ҫע�������Щ�����ϻ����һ�Ժ꣬���ܱ�ͬʱ��Ϊ
 * ��RPU_YES����RPU_NO������һ�㽫�����ں���������С�
 * ע�⣬�벻Ҫ�޸����������ֵ��
 */
#define	RPU_YES		1
#define	RPU_NO		0

/***************************************************************************/
#define RPU_MODULE_USER_MAC_RELAY RPU_YES
#define RPU_MODULE_IGMP_TVM RPU_YES
#define RPU_MODULE_POE RPU_YES


#define USERMAC_EN 0x1 

#endif

