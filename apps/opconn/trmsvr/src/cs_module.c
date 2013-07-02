#include "osal_common.h"
#include "cs_module.h"

sys_mod_desc_t sys_mod_desc[] = {
    {IROS_MID_OSAL             ,"OSAL"},
    {IROS_MID_TIMER            ,"TIMER"},
    {IROS_MID_INTR             ,"INTR"},
    {IROS_MID_CPULOAD          ,"CPULD"},
    {IROS_MID_MALLOC           ,"MALLOC"},
    {IROS_MID_IFETH            ,"IFETH"},
    {IROS_MID_EXCEPT           ,"EXCEPT"},
    {IROS_MID_LOGGER           ,"LOGGER"},
    {IROS_MID_AAL              ,"AAL"},
    {IROS_MID_APP              ,"APP"},
    {IROS_MID_CLI              ,"CLI"},
    {IROS_MID_SC               ,"SC"},
    {IROS_MID_OAM              ,"OAM"},
    {IROS_MID_1X               ,"8021X"},
    {IROS_MID_HELLO            ,"HELLO"},
    {IROS_MID_IGMP             ,"IGMP"},
    {IROS_MID_OOBCMD           ,"OOBCMD"},
    {IROS_MID_L2FTP            ,"L2FTP"},
    {IROS_MID_EVENT            ,"EVENT"},
    {IROS_MID_ETHDRV           ,"ETHDRV"},
    {IROS_MID_STP              ,"STP"},
    {IROS_MID_SNMP             ,"SNMP"},
    {IROS_MID_UPGRADE          ,"UPGRADE"},
    {IROS_MID_PKT_HDR          ,"PKTHDR"},
    {IROS_MID_PACKET           ,"PKT"},
    {IROS_MID_SYS              ,"SYS"},
    {IROS_MID_SAL              ,"SAL"},
    {IROS_MID_WEB              ,"WEB"},
    {IROS_MID_SLIC             ,"SLIC"},
    {IROS_MID_DMA              ,"DMA"},
    {IROS_MID_ESDMA            ,"ESDMA"},
    {IROS_MID_VOIP             ,"VOIP"},
    {IROS_MID_ORG_OAM          ,"ORGOAM"},
    {IROS_MID_DHCP             ,"DHCP"},
    {IROS_MID_VLAN             ,"VLAN"},
    {IROS_MID_PWRS             ,"PWRS"},
    {IROS_MID_PTP             ,"PTP"},
};
cs_int32 sys_mod_number = 0;

cs_int32 cs_sys_mod_init()
{
    sys_mod_number = sizeof(sys_mod_desc)/sizeof(sys_mod_desc[0]);
    return CS_E_OSAL_OK;
}

cs_int8 *cs_sys_mod_desc_get(cs_int32 mod_id)
{
    cs_int32 i;

    for(i = 0 ; i < sys_mod_number ; i++)
    {
        if(sys_mod_desc[i].mod_id == mod_id)
            return (cs_int8 *)sys_mod_desc[i].mod_name;
    }

    return NULL;
}



