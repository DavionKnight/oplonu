#ifndef DAL_VLAN_H
#define DAL_VLAN_H


#include <opl_driver.h>
#include <opconn_api.h>


typedef enum VLAN_EGRESS_TYPE_S
{
    UNMODIFY = 0,
    UNTAG    = 1,
    TAG      = 2 
}VLAN_EGRESS_TYPE_E;

typedef enum
{
    UNMODIFY_EGRESS = 0,
    UNTAGGED_EGRESS,
    TAGGED_EGRESS,
    ADD_TAG
} PORT_EGRESS_MODE;
typedef enum
{
	DISABLE = 0,
	FALLBACK,
	CHECK,
	SECURE
} SW_DOT1Q_MODE;





#endif 