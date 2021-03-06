#ifndef __IPMUX_ERRORS__H__
#define __IPMUX_ERRORS__H__

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

#define FLOW_LATENCY_TYPE_ERROR                 0x80000097

#define REMARK_PARAMETER_OUTOF_RANGE            0x800000AC
#define REMARK_MAPTYPE_OUTOF_RANGE              0x800000AD

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
#define BRIDGE_VLAN_ENTRY_TABLE_FULL            0x8000010A
#define BRIDGE_VLAN_ENTRY_TABLE_ERROR           0x8000010B
#define BRIDGE_VLAN_ENTRY_TABLE_GT16            0x8000010C
#define BRIDGE_VLAN_ENTRY_OUTOF_RANGE           0x8000010D
#define BRIDGE_VCC_POINTER_OUTOF_RANGE          0x8000010E
#define BRIDGE_VCC_ENTRY_OUTOF_RANGE            0x8000010F
#define BRIDGE_VCC_ENTRY_TABLE_FULL             0x80000110
#define BRIDGE_ARL_MCID_OUTOF_RANGE             0x80000111
#define BRIDGE_ARL_MC_MAC_INVALID	            0x80000112
#define BRIDGE_ARL_SVID_OUTOF_RANGE             0x80000113

#define BRIDGE_PVC_BUNDLE_OUT_OF_RANGE			0x80000201
#define BRIDGE_PVC_BUNDLE_PORT_MISMATCH		0x80000202

#define BRIDGE_VTT_TABLE_FULL		0x80000210
#define BRIDGE_VTT_PORT_MISMATCH	0x80000211
#define BRIDGE_VTT_OUTOF_RANGE		0x80000212
#define BRIDGE_VTT_RANGE_FULL		0x80000213
#define BRIDGE_VTT_RECORD_NOT_FOUND		0x80000214

#define BRIDGE_L2S_TABLE_FULL		0x80000310
#define BRIDGE_L2S_PORT_MISMATCH	0x80000311
#define BRIDGE_L2S_OUTOF_RANGE		0x80000312
#define BRIDGE_L2S_RANGE_FULL		0x80000313

#define BRIDGE_L3S_TABLE_FULL		0x80000410
#define BRIDGE_L3S_PORT_MISMATCH	0x80000411
#define BRIDGE_L3S_OUTOF_RANGE		0x80000412
#define BRIDGE_L3S_RANGE_FULL		0x80000413
#define BRIDGE_L3S_NO_INACTIVE_ENTRY		0x80000414
#define BRIDGE_L3S_NO_NUM_SET		0x80000415

#define BRIDGE_MTT_TABLE_FULL		0x80000510
#define BRIDGE_MTT_TYPE_MISMATCH	0x80000511
#define BRIDGE_MTT_OUTOF_RANGE		0x80000512
#define BRIDGE_MTT_RANGE_FULL		0x80000513
#define BRIDGE_MTT_NO_UNUSED_ENTRY		0x80000514
#define BRIDGE_MTT_NO_NUM_SET		0x80000515

#define BRIDGE_TPID_OUTOF_RANGE                 0x80000611
#define BRIDGE_REVMAC_NUM_OUTOF_RANGE           0x80000612
#define BRIDGE_ETHERMAP_OUTOF_RANGE             0x80000613

#define IWF_UPPT_ENTRY_SAME                     0x80002004
#define IWF_UPPT_ENTRY_NULL                     0x80002005
#define IWF_PARAMETER_OUTOF_RANGE               0x80002006
#define IWF_GET_INVALID_ENTRY                   0x80002007
#define IWF_SAME_PORT_ID                        0x80002008
#define IWF_PORT_PHY_MISMATCH                   0x80002009
#define IWF_UPPT_LATENCY_TYPE_ERROR             0x80002010

/*List operation*/
#define LIST_NULL                               0x80001000
#define LIST_POINTER_NULL                       0x80001001
#define PARAMETER_INPUT_POINTER_NULL            0x80001002
#define TABLE_INIT_MEMORY_NULL                  0x80001003

#define IPMUX_LINK_NUMBER_ERROR                 0x80004003
#define IPMUX_LINK_REV_SEND_TIMEOUT             0x80004004
#define PPPOE_INVALID_STEP                      0x80004000
#define PPPOE_PACKET_ERR_TAG                    0x80004001

#define IPMUX_AGGREGATION_INIT_TASK_ERROR       0x80010000

#define IPMUX_ATM_BONDING_OUTOF_RANGE           0x80030000

#define IPMUX_VERSION_INVALID					0x80040000

#endif

