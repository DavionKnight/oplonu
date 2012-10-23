/*=============================================================================
     Header Name: ipmux_mdio.h

     General Description:
general definition for the basic data type in Opulan driver.
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                		Date              		Description of Changes
----------------   	------------  	----------------------------------------------
Jiang Mingli          		07/06/09           	Initial Version
----------------   	------------  	----------------------------------------------*/



#ifndef IPMUX_MDIO_H
#define IPMUX_MDIO_H


/* opconn Table dimension:width,in records */
#define MDIO_READ				0X02
#define MDIO_WRITE				0X01

#define MDIO_BUSY				0X1
#define MDIO_TIMEOUT			1000

/*mdio register*/
#define OPCONN_BASE_ADDR		0x00000000

#define MDIO_BA_B				 (OPCONN_BASE_ADDR + 0x0500)
#define MDIO_BA_E				 (OPCONN_BASE_ADDR + 0x0506)

#define REG_MDIO_DIV_FACTOR     		((MDIO_BA_B + 0)*4)
#define REG_MDIO_OP_PULSE				((MDIO_BA_B + 1)*4)
#define REG_MDIO_PHYAD  				((MDIO_BA_B + 2)*4)
#define REG_MDIO_REGAD					((MDIO_BA_B + 3)*4)
#define REG_MDIO_WRITE_DATA   			((MDIO_BA_B + 4)*4)
#define REG_MDIO_READ_DATA     			((MDIO_BA_B + 5)*4)
#define REG_MDIO_BUSY     				((MDIO_BA_B + 6)*4)

#endif


