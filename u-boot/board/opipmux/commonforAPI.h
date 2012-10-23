

#ifndef IPMUX_TABLE_GENERAL_DEFINE
#define IPMUX_TABLE_GENERAL_DEFINE


#ifndef UINT32
/*temp define for local debug*/
#define UINT32  unsigned int
#define INT32   int
#define UINT16  unsigned short
#define INT16   short
#define UINT    unsigned int
#define INT     int
#define UINT8   unsigned char
#define INT8    char



#ifndef NULL
#define NULL                  ((void *) 0)
#endif



/*error code deinfe*/
#define OPL_OK                    0
#define OPL_ERROR			      (-1)
#define OPL_STATUS			      int
#endif


#define FIX_TABLE_ITEM_LENGTH                0
#define VARIABLE_TALBE_ITEM_LENGTH           1

#define IPMUX_WINDOWS                        1 /*Used to debug hardware independent code.*/
#define IPMUX_LINUX                          2
#define IPMUX_VXWORKS                        3

#define IPMUX_OS                             IPMUX_LINUX

#if IPMUX_OS == IPMUX_WINDOWS/*Used to debug hardware independent code.*/
extern void * __cdecl malloc(unsigned int);
extern void   __cdecl free(void *);
extern int    __cdecl memcmp(const void *, const void *, unsigned int);
extern void *  __cdecl memcpy(void *, const void *, unsigned int);
extern void *  __cdecl memset(void *, int, unsigned int);

#endif

#if IPMUX_OS == IPMUX_LINUX

#endif

#if IPMUX_OS == IPMUX_VXWORKS

#endif



#undef  IPMUX_BIG_ENDIAN
#define IPMUX_LITTLE_ENDIAN


#ifdef  IPMUX_BIG_ENDIAN

#ifndef NTOHL
#define	ntohl(x)	(x)
#define	ntohs(x)	(x)
#define	htonl(x)	(x)
#define	htons(x)	(x)

#define NTOHL(x)	(x) = ntohl((UINT32)(x))
#define NTOHS(x)	(x) = ntohs((UINT16)(x))
#define HTONL(x)	(x) = htonl((UINT32)(x))
#define HTONS(x)	(x) = htons((UINT16)(x))
#endif


#else /* _BYTE_ORDER==_LITTLE_ENDIAN */

#ifndef NTOHL
#define ntohl(x)	((((x) & 0x000000ff) << 24) | \
			 (((x) & 0x0000ff00) <<  8) | \
			 (((x) & 0x00ff0000) >>  8) | \
			 (((x) & 0xff000000) >> 24))

#define htonl(x)	((((x) & 0x000000ff) << 24) | \
			 (((x) & 0x0000ff00) <<  8) | \
			 (((x) & 0x00ff0000) >>  8) | \
			 (((x) & 0xff000000) >> 24))

#define ntohs(x)	((((x) & 0x00ff) << 8) | \
			 (((x) & 0xff00) >> 8))

#define htons(x)	((((x) & 0x00ff) << 8) | \
			 (((x) & 0xff00) >> 8))
#define	NTOHL(x)	(x) = ntohl((UINT32)(x))
#define	NTOHS(x)	(x) = ntohs((UINT16)(x))
#define	HTONL(x)	(x) = htonl((UINT32)(x))
#define	HTONS(x)	(x) = htons((UINT16)(x))

#endif
#endif	



#define BITS_PER_BYTE                8

#define IPMUX_ACCESS_PORT_NUMBER     576

/*classify error code define*/
#define CLASSIFY_MASK_TABLE_FULL                0x80000001
#define CLASSIFY_MASK_ITEM_SAME                 0x80000002
#define CLASSIFY_MASKF_SELECT_NULL              0x80000003

#define CLASSIFY_ADDR_TABLE_COLLISION           0x80000011
#define CLASSIFY_ADDR_TABLE_FULL                0x80000012
#define CLASSIFY_ADDR_TABLE_8COLLISION          0x80000013

#define CLASSIFY_RULE_TABLE_FULL                0x80000021
#define CLASSIFY_RULE_ITEM_SAME                 0x80000022
#define CLASSIFY_RULE_NULL_ITEM                 0x80000023

#define CLASSIFY_ERROR_PACKET_TYPE              0x80000031
#define CLASSIFY_ERROR_PARAMETER_LENGTH         0x80000032

#define CLASSIFY_INVALID_PKT_TYPE               0x80000041
#define CLASSIFY_INVALID_FIELD_NUM              0x80000042
#define CLASSIFY_INVALID_ADJUST_LEN             0x80000043
#define CLASSIFY_NO_INVALID_FIELD               0x80000044
#define CLASSIFY_INVALID_FIELD_TYPE             0x80000045
#define CLASSIFY_NULL_FIELDMAP                  0x80000046
#define CLASSIFY_WINDOWS_OUTOF_RANGE            0x80000047
#define CLASSIFY_INVALID_FOFFSET_NUM            0x80000048
#define CLASSIFY_FREE_INVALID_MENTRY            0x80000049

/*flow error code define*/
#define FLOW_MASK_TABLE_FULL                    0x80000051
#define FLOW_MASK_ITEM_SAME                     0x80000052

#define FLOW_ADDR_TABLE_COLLISION               0x80000061
#define FLOW_ADDR_TABLE_FULL                    0x80000062
#define FLOW_ADDR_TABLE_2COLLISION              0x80000063

#define FLOW_RULE_TABLE_FULL                    0x80000071
#define FLOW_RULE_ITEM_SAME                     0x80000072
#define FLOW_RULE_NULL_ITEM                     0x80000073
#define FLOW_RULE_NONE_EMPTY                    0x80000074

#define FLOW_WINDOWS_OUTOF_RANGE                0x80000090
#define FLOW_INVALID_FOFFSET_NUM                0x80000091
#define FLOW_MASKF_SELECT_NULL                  0x80000092
#define FLOW_4HASH_COLLISION                    0x80000093
#define FLOW_FIELD_MAP_NULL                     0x80000094
#define FLOW_PARAMETER_OUTOF_RANGE              0x80000095
#define FLOW_FREE_INVALID_MENTRY                0x80000096

/*remark error code define*/
#if 0
#define REMARK_CONTENT_TABLE_FULL               0x800000A1
#define REMARK_CMD_SAME                         0x800000A2

#define REMARK_DELETE_NULL_ITEM                 0x800000A3
#define REMARK_CONTENT_MEMORY_SLICE             0x800000A4
#define REMARK_CONTENT_NULL                     0x800000A5
#define REMARK_RULE_FULL                        0x800000A6
#define REMARK_DUPLICATE_OPERATE                0x800000A7
#define REMARK_NO_INSERT_FIELD                  0x800000A8
#define REMARK_FIELD_NOT_BYTE_ALIGNMENT         0x800000A9
#define REMARK_UNDEF_CMD                        0x800000AA
#define REMARK_END_BIT_BYTE_ALIGNMENT           0x800000AB
#endif

#define REMARK_PARAMETER_OUTOF_RANGE            0x800000AC

/*bridge error code define*/
#define BRIDGE_PORTID_OUTOF_RANGE               0x80000100
#define BRIDGE_PORT_GET_INACTIVE_ENTRY          0x80000101
#define BRIDGE_ARL_4HASH_COLLISION              0x80000102
#define BRIDGE_ARL_NULL_ENTRY                   0x80000103
#define BRIDGE_ARL_OUTOF_RANGE                  0x80000104
#define BRIDGE_VLAN_OUTOF_RANGE                 0x80000105
#define BRIDGE_VLAN_ERROR_OPTYPE                0x80000106
#define BRIDGE_VLAN_NUMBER_SAME                 0x80000107
#define BRIDGE_VLAN_NULL                        0x80000108
#define BRIDGE_VLAN_TABLE_FULL                  0x80000109
#define BRIDGE_VLANPORT_TABLE_FULL              0x8000010A
#define BRIDGE_VLANPORT_MEMBER_SAME             0x8000010B
#define BRIDGE_VLANPORT_MEMBER_NULL             0x8000010C
#define BRIDGE_VLANPORT_ERROR_OFFSET            0x8000010D

#define BRIDGE_SACT_FULL                        0x8000020D
#define BRIDGE_SACT_NULL_ENTRY                  0x8000020E
#define BRIDGE_VRPT_NULL_ENTRY                  0x8000020F
#define BRIDGE_VRPT_FULL                        0x80000210

#define BRIDGE_TPID_OUTOF_RANGE        0x80000211
#define BRIDGE_REVMAC_NUM_OUTOF_RANGE 0X80000212



#define BRIDGE_MCAST_MIDT_TABLE_FULL            0x80002000
#define BRIDGE_MCAST_MET_TABLE_FULL             0x80002001  
#define BRIDGE_MCAST_INVALID_MEMBER_NUM         0x80002002
#define BRIDGE_MCAST_INVALID_GROUT_NUM          0x80002003

#define IWF_UPPT_ENTRY_SAME                     0x80002004
#define IWF_UPPT_ENTRY_NULL                     0x80002005
#define IWF_PARAMETER_OUTOF_RANGE               0x80002006
#define IWF_GET_INVALID_ENTRY                   0x80002007
#define IWF_SAME_PORT_ID                        0x80002008
#define IWF_PORT_PHY_MISMATCH                   0x80002009
#define IWF_UPPT_LATENCY_TYPE_ERROR             0x80002010

#define UTOPIA_INTERFACE_NUM_ERR                0x80003000
#define UTOPIA_MODE_ERR                         0x80003001

/*List operation*/
#define LIST_NULL                               0x80001000
#define LIST_POINTER_NULL                       0x80001001
#define PARAMETER_INPUT_POINTER_NULL            0x80001002
#define TABLE_INIT_MEMORY_NULL                  0x80001003


#define IPMUX_LINK_NUMBER_ERROR                 0x80004003
#define IPMUX_LINK_REV_SEND_TIMEOUT          0x80004004
#define PPPOE_INVALID_STEP                      0x80004000
#define PPPOE_PACKET_ERR_TAG                     0x80004001

#define IPMUX_AGGREGATION_INIT_TASK_ERROR  0x80010000

/*Bridge register define*/
#define IPMUX_BRIDGE_ENABLE_REG                 0x00020000
#define IPMUX_BRIDGE_ARLAGE_ENA_REG             0x0002007C          

#define IPMUX_BRIDGE_MAC_AGE_REG                0x00020004
#define IPMUX_BRIDGE_TPID_0_REG                 0x00020040              
#define IPMUX_BRIDGE_TPID_1_REG                 0x00020044  
#define IPMUX_BRIDGE_TPID_2_REG                 0x00020048              
#define IPMUX_BRIDGE_TPID_3_REG                 0x0002004C  

#define IPMUX_BRIDGE_REVMAC_CTL_REG1            0x00020054
#define IPMUX_BRIDGE_REVMAC_CTL_REG0            0x00020050

#define IPMUX_BRIDGE_REVMAC_REG0_HIGH           0x0002005C
#define IPMUX_BRIDGE_REVMAC_REG0_LOW            0x00020060

#define IPMUX_BRIDGE_REVMAC_REG1_HIGH           0x00020064
#define IPMUX_BRIDGE_REVMAC_REG1_LOW            0x00020068

#define IPMUX_BRIDGE_REVMAC_REG2_HIGH           0x0002006C
#define IPMUX_BRIDGE_REVMAC_REG2_LOW            0x00020070

#define IPMUX_BRIDGE_REVMAC_REG3_HIGH           0x00020074
#define IPMUX_BRIDGE_REVMAC_REG3_LOW            0x00020078

#define IPMUX_VLAN_REHASH_NUM_REG               0x0002003C



/*packet engine register define*/
#define IPMUX_CLASSIFY_INIT_REG                 0x00018000
#define IPMUX_CLASSIFY_xxx_REG                  0x00018004

/*IWF register define*/
#define IPMUX_UPPT_AGE_ENABLE_REG         0x00013C3C
#define IPMUX_UPPT_AGE_INTERVAL_REG     0x00013C2C


/*Link AggreGation register define*/
#define IPMUX_LINK_CONTRL_REG            0x0000C00C
#define IPMUX_LINK0_SOURCE_MAC_REG_HIGH  0x0000C014
#define IPMUX_LINK0_SOURCE_MAC_REG_LOW   0x0000C010
#define IPMUX_LINK1_SOURCE_MAC_REG_HIGH  0x0000C01C
#define IPMUX_LINK1_SOURCE_MAC_REG_LOW   0x0000C018

#define IPMUX_RESERVE_SEND_DATA0_LOW     0x0000C020
#define IPMUX_RESERVE_SEND_DATA0_HIGH    0x0000C024
#define IPMUX_RESERVE_SEND_DATA1_LOW     0x0000C028
#define IPMUX_RESERVE_SEND_DATA1_HIGH    0x0000C02C
#define IPMUX_RESERVE_SEND_RDY           0x0000C030

#define IPMUX_CLASSIFY_INIT_VALUE         1
#define IPMUX_CLASSIFY_xxx_INIT_VALUE  0x000007df

/*register control bit define*/
#define IPMUX_BRIDGE_TPID0                 0
#define IPMUX_BRIDGE_TPID1                 1
#define IPMUX_BRIDGE_TPID2                 2
#define IPMUX_BRIDGE_TPID3                 3

#define IPMUX_RESERVE_MAC0                 0
#define IPMUX_RESERVE_MAC1                 1
#define IPMUX_RESERVE_MAC2                 2
#define IPMUX_RESERVE_MAC3                 3

#define IPMUX_RESERVE_MAC_00               0  /*01-80-C2-00-00-00*/
#define IPMUX_RESERVE_MAC_01               1
#define IPMUX_RESERVE_MAC_02               2
#define IPMUX_RESERVE_MAC_03               3
#define IPMUX_RESERVE_MAC_04               4
#define IPMUX_RESERVE_MAC_05               5
#define IPMUX_RESERVE_MAC_06               6
#define IPMUX_RESERVE_MAC_07               7
#define IPMUX_RESERVE_MAC_08               8
#define IPMUX_RESERVE_MAC_09               9
#define IPMUX_RESERVE_MAC_0A               10
#define IPMUX_RESERVE_MAC_0B               11
#define IPMUX_RESERVE_MAC_0C               12
#define IPMUX_RESERVE_MAC_0D               13
#define IPMUX_RESERVE_MAC_0E               14
#define IPMUX_RESERVE_MAC_0F               15
#define IPMUX_RESERVE_MAC_10               16 /*01-80-C2-00-00-10*/

#define IPMUX_RESERVE_MAC_20               32 /*01-80-C2-00-00-20*/
#define IPMUX_RESERVE_MAC_21               33
#define IPMUX_RESERVE_MAC_22               34
#define IPMUX_RESERVE_MAC_23               35
#define IPMUX_RESERVE_MAC_24               36
#define IPMUX_RESERVE_MAC_25               37
#define IPMUX_RESERVE_MAC_26               38
#define IPMUX_RESERVE_MAC_27               39
#define IPMUX_RESERVE_MAC_28               40
#define IPMUX_RESERVE_MAC_29               41
#define IPMUX_RESERVE_MAC_2A               42
#define IPMUX_RESERVE_MAC_2B               43
#define IPMUX_RESERVE_MAC_2C               44
#define IPMUX_RESERVE_MAC_2D               45 /*01-80-C2-00-00-2D*/
#define IPMUX_RESERVE_MAC_2E               46 /*01-80-C2-00-00-2E*/
#define IPMUX_RESERVE_MAC_2F               47 /*01-80-C2-00-00-2F*/
#define IPMUX_RESERVE_MAC_REG0             48
#define IPMUX_RESERVE_MAC_REG1             49
#define IPMUX_RESERVE_MAC_REG2             50
#define IPMUX_RESERVE_MAC_REG3             51


#define IPMUX_LINK_AGGREGATION_0           0
#define IPMUX_LINK_AGGREGATION_1           1

#define IPMUX_LINK_MODE_REDUNDANCY         0
#define IPMUX_LINK_MODE_AGGREGATION        1
#define IPMUX_LINK_MODE_DAISYCHAIN         2
#define IPMUX_LINK_MODE_RESERVED           3

#define IPMUX_LINK_0_PRIMARY               0
#define IPMUX_LINK_1_PRIMARY               3

#define IPMUX_LINK_BOTH_DISABLE            0
#define IPMUX_LINK_1_ENABLE                1
#define IPMUX_LINK_0_ENABLE                2
#define IPMUX_LINK_BOTH_ENABLE             3

#define IPMUX_LINK_MODE_START_BIT          2
#define IPMUX_LINK_MODE_BITS               3
#define IPMUX_LINK_MODE_MASK               0xC


#define IPMUX_LINK_CONTROL_START_BIT       0
#define IPMUX_LINK_CONTROL_BITS            3
#define IPMUX_LINK_CONTROL_MASK            3



#define IPMUX_LINK0_RSV_WRITE_RDY          0x1
#define IPMUX_IINK1_RSV_WRITE_RDY          0x2

#define IPMUX_LINK_SEND_1_BYTES            0
#define IPMUX_LINK_SEND_2_BYTES            1
#define IPMUX_LINK_SEND_3_BYTES            2
#define IPMUX_LINK_SEND_4_BYTES            3



#define IPMUX_LINK_EOP                     0x4
#define IPMUX_LINK_SOP                     0x8



#define IPMUX_ARL_AGE_ENABLE_BIT IPMUX_REG_BIT0
#define IPMUX_BRIDGE_ENABLE_BIT   IPMUX_REG_BIT0


#define IPMUX_UPPT_AGE_ENABLE_BIT IPMUX_REG_BIT0


extern OPL_STATUS TableAddrCal16(UINT8 *pucValue,UINT32 ulValueLen,UINT16 usAddrMask,UINT16 *pusAddress);
extern OPL_STATUS TableAddrCal32(UINT8 *pucValue,UINT32 ulValueLegnth,UINT32 ulAddrMask,UINT32 *pulAddress);


#endif
