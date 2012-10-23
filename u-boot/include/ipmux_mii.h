/*=============================================================================
     Header Name: ipmux_mii.h

     General Description:
general definition for the basic data type in Opulan driver.
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		Date              		Description of Changes
----------------   	------------  	----------------------------------------------
Ran zhaoyang          		10/10/06           	Initial Version
----------------   	------------  	----------------------------------------------*/



#ifndef IPMUX_IPMUXMII_H
#define IPMUX_IPMUXMII_H

#define IPMUX_MII_SPEED_RESERVED			3
#define IPMUX_MII_SPEED_1000M				2
#define IPMUX_MII_SPEED_100M				1
#define IPMUX_MII_SPEED_10M					0

#define IPMUX_AN_ENABLE						1
#define IPMUX_AN_DISABLE						0

#define IPMUX_MII_DUPLEX_FULL				1
#define IPMUX_MII_DUPLEX_HALF				0

#define IPMUX_MII_LOOPBACK_ENABLE			1
#define IPMUX_MII_LOOPBACK_DISABLE			0

#define IPMUX_MII_REG_PHY_CONTROL			0
#define IPMUX_MII_REG_PHY_STATUS			1
#define IPMUX_MII_REG_PHY_AN_ADS			4
#define IPMUX_MII_REG_PHY_AN_EX				6
#define IPMUX_MII_REG_PHY_1000M_CONTROL	9
#define IPMUX_MII_REG_PHY_SPEC_CONTROL		16
#define IPMUX_MII_REG_PHY_SPEC_STATUS		17

#define IPMUX_MII_PORT_GE0		0
#define IPMUX_MII_PORT_GE1		1
#define IPMUX_MII_PORT_GEL		2

#define IPMUX_MII_PHY0			0


#define IPMUX_MII_REG_PHY_SPEC_STATUS_MDI_BIT		6

#define IPMUX_MII_REG_PHY_SPEC_CONTROL_MDI_BIT		5

#define IPMUX_MII_REG_CONTROL_AN_BIT		12

#define IPMUX_MII_REG_PHY_SPEC_STATUS_LINK_BIT			10
#define IPMUX_MII_REG_PHY_SPEC_STATUS_SPEEDDUPLEX_BIT		11
#define IPMUX_MII_REG_PHY_SPEC_STATUS_DUPLEX_BIT			13
#define IPMUX_MII_REG_PHY_SPEC_STATUS_SPEED_BIT			14

#endif


