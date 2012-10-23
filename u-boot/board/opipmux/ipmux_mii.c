/******************************************************************************/
/**  COPYRIGHT (C) 2003- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/
/*******************************************************************************
 **    MODULE     : IPMux mii, including MII read, write function.
 **
 **    FILE       : ipmux_mii.c
 **
 **    DESCRIPTION: This file contains IPMux MII configuration driver utility function.
 **
 **
 **    NOTES      :
 **
 *******************************************************************************/
/*
 **    MODIFICATION HISTORY:
 **    Author            			Date				Description of Changes
 **    ----------------  	------------  	----------------------------------------------
 **    Ran zhaoyang            	10/09/2006      	Initial Version
 **
 */

/* includes */
//#include <stdio.h> /* for printf, yzhang 20070521 */
#include <common.h>
#include "opl_driver.h"
#include "ipmux_misc.h"
#include "opl_debug.h"
#include "ipmux_mii.h"
//#include "ipmux_gmac.h"

/* imports */

/* Global */

/* for debug system */

/* defines */

/* for debug system */
#ifdef opl_log
#undef opl_log
#endif
#define opl_log(fmt)		printf fmt



/*******************************************************************************
 *
 * ipmux_MII_read
 *
 * DESCRIPTION:
 * 	this function reads MII register.
 *
 * INPUTS:
 *	port		- port number.
 *	phy		- phy number.
 *	phyregid	- phy register id
 *
 * OUTPUT:
 *	UINT32	- the content of register id
 *
 * RETURNS:
 *	UINT32	- the content of register id
 *
 * SEE ALSO:
 */

UINT16 ipmux_MII_read( UINT8 port, UINT8 phy, UINT8 phyregid)
{
	UINT32 badr;
	UINT32 regid;
	UINT32 val, tmpval;

	switch (port)
	{
		case	 IPMUX_MII_PORT_GE0:
			badr = 0xbf008000;
			regid = 0x8000;
			//		opl_log(("GE0 selected\n"));
			break;
		case IPMUX_MII_PORT_GE1:
			badr = 0xbf00a000;
			regid = 0xa000;
			opl_log(("GE1 selected\n"));
			break;
		case IPMUX_MII_PORT_GEL:
			badr = 0xbf00c000;
			regid = 0xc000;
			opl_log(("GEL selected\n"));
			break;
		default:
			opl_log(("wrong port selected\n"));
			return 0;
	}

	/* enable tx rx
	 * GMAC Configuration 1 Register = regid + 0x200
	 */
	ipMuxRegWrite(regid + 0x200, 0x3f);

	/* full-duplex crc/pad enable
	 * word [expr $badr +0x204]
	 * bit		 FEDC BA98 7654 3210
	 *0x7127 = 0b 0111 0001 0010 0111;
	 * GMAC Configuration 2 Register = regid + 0x204
	 */
	ipMuxRegWrite(regid + 0x204, 0x7127);

	//opl_log(("ethernet interface configure completed\n"));

	/* MII Management Indicators = regid + 0x234
	 * wait for register ready to read
	 */
	ipMuxRegRead(regid + 0x234, &tmpval);
	while ( 0x1 == (tmpval & 0x1)  || 0x2 == (tmpval & 0x2) )
	{
		/* zDelay (100); *//* since now the timer of linux is not very accurate, it is about 1s */
		/* sleep(0); */
		ipMuxRegRead(regid + 0x234, &tmpval);
	}

	do
	{
		/*# 32bit preamble, MDC = sysclk/28
		 * word [expr $badr + 0x220] 0x17
		 * MII Management Configuration = regid + 0x220
		 * 0x17 = 0b 0001 01111 to suppress preamble(32 clocks), slowest speed
		 */
		ipMuxRegWrite(regid + 0x220, 0x17);

		/* # phy addr, register
		 * word [expr $badr + 0x228] [expr [expr $phy << 8] + $i]
		 * MII Management Address = regid + 0x228
		 * phy is located at bits 12-8, regaddr is located at bits 4-0;
		 */
		ipMuxRegWrite(regid + 0x228, ((phy & 0x1f)  << 8) + phyregid);

		ipMuxRegRead(regid + 0x228, &tmpval);
		/* printf("phyaddr = 0x%08x\n", tmpval); */

		/* # start read
		 * word [expr $badr + 0x224] 0x1
		 * MII Management Command = regid + 0x224
		 */
		ipMuxRegWrite(regid + 0x224, 0x1);

		/* test if Read Data is valid or not. */
		ipMuxRegRead(regid + 0x234, &tmpval);
	}while (0x100 == (tmpval & 0x100) );

	/* # return the data
	 * MII Management Status = regid + 0x230
	 */
	ipMuxRegRead(regid + 0x230, &val);

	//opl_log(("reg%d = 0x%08x\n", phyregid, val));

	/* # end read
	 * word [expr $badr + 0x224] 0x0
	 * MII Management Command = regid + 0x224
	 */
	ipMuxRegWrite(regid + 0x224, 0x0);

	return (UINT16)val;
}

/*******************************************************************************
 *
 * ipmux_MII_write
 *
 * DESCRIPTION:
 * 	this function writes MII register.
 *
 * INPUTS:
 *	port		- port number.
 *	phy		- phy number.
 *	phyregid	- phy register id
 *	val		- value to be written
 *
 * OUTPUT:
 *	n.a
 *
 * RETURNS:
 *	OPL_OK.
 *
 * SEE ALSO:
 */

OPL_STATUS ipmux_MII_write1( UINT8 port, UINT8 phy, UINT8 phyregid, UINT16 valw)
{
	UINT32 badr;
	UINT32 regid;
	UINT32 tmpval;

	switch (port)
	{
		case	 IPMUX_MII_PORT_GE0:
			badr = 0xbf008000;
			regid = 0x8000;
			opl_log(("GE0 selected\n"));
			break;
		case IPMUX_MII_PORT_GE1:
			badr = 0xbf00a000;
			regid = 0xa000;
			opl_log(("GE1 selected\n"));
			break;
		case IPMUX_MII_PORT_GEL:
			badr = 0xbf00c000;
			regid = 0xc000;
			opl_log(("GEL selected\n"));
			break;
		default:
			opl_log(("wrong port selected\n"));
			return 0;
	}

	/* enable tx rx
	 * GMAC Configuration 1 Register = regid + 0x200
	 */
	ipMuxRegWrite(regid + 0x200, 0x3f);

	/* full-duplex crc/pad enable
	 * word [expr $badr +0x204]
	 * 	bit		 FEDC BA98 7654 3210
	 * 0x7127 = 0b 0111 0001 0010 0111;
	 * GMAC Configuration 2 Register = regid + 0x204
	 */
	ipMuxRegWrite(regid + 0x204, 0x7127);

	//        opl_log(("ethernet interface configure completed\n"));

	/* MII Management Indicators = regid + 0x234
	 * wait for register ready to read
	 */
	ipMuxRegRead(regid + 0x234, &tmpval);
	while ( 0x1 == (tmpval & 0x1)  || 0x2 == (tmpval & 0x2) )
	{
		/* zDelay (100); *//* since now the timer of linux is not very accurate, it is about 1s */
		/* sleep(0); */
		ipMuxRegRead(regid + 0x234, &tmpval);
	}

	/* # 32bit preamble, MDC = sysclk/28
	 * word [expr $badr + 0x220] 0x17
	 * MII Management Configuration = regid + 0x220
	 * 0x17 = 0b 0001 01111 to suppress preamble(32 clocks), slowest speed
	 */
	ipMuxRegWrite(regid + 0x220, 0x17);

	/* # phy addr, register
	 * word [expr $badr + 0x228] [expr [expr $phy << 8] + $i]
	 * MII Management Address = regid + 0x228
	 * phy is located at bits 12-8, regaddr is located at bits 4-0;
	 */
	ipMuxRegWrite(regid + 0x228, ((phy & 0x1f) << 8) + phyregid);

	/*# start write
	 * MII Management Control = regid + 0x22C
	 */
	ipMuxRegWrite(regid + 0x22C, valw);

	return OPL_OK;
}


/************************************************************************
 *
 * This is the next part if the initialization sequence: we are now
 * running from RAM and have a "normal" C environment, i. e. global
 * data can be written, BSS has been cleared, the stack size in not
 * that critical any more, etc.
 *
 ************************************************************************
 */

void ipmux_MII_write(int port, unsigned char phy, unsigned char phyregid, unsigned short valw)

{

	unsigned int badr;

	unsigned int regid;

	unsigned int tmpval;


	/* enable tx rx

	 * GMAC Configuration 1 Register = regid + 0x200

*/

	ipMuxRegWrite(0x8200, 0x3f);


	/* full-duplex crc/pad enable      

	 * word [expr $badr +0x204] 

	 *       bit              FEDC BA98 7654 3210

	 * 0x7127 = 0b 0111 0001 0010 0111;

	 * GMAC Configuration 2 Register = regid + 0x204

*/

	ipMuxRegWrite(0x8204, 0x7127); 


	/* MII Management Indicators = regid + 0x234

	 * wait for register ready to read

*/

	ipMuxRegRead(0x8234, &tmpval);

	//while ( 0x1 == (tmpval & 0x1)  || 0x2 == (tmpval & 0x2) )


	while ( 0x1 == (tmpval & 0x1) )

	{

		/* zDelay (100); *//* since now the timer of linux is not very accurate, it is about 1s */

		/* sleep(0); */

		ipMuxRegRead(0x8234, &tmpval);              

	}

	/* # 32bit preamble, MDC = sysclk/28

	 * word [expr $badr + 0x220] 0x17

	 * MII Management Configuration = regid + 0x220

	 * 0x17 = 0b 0001 01111 to suppress preamble(32 clocks), slowest speed

*/

	ipMuxRegWrite(0x8220, 0x17);

	/* # phy addr, register

	 * word [expr $badr + 0x228] [expr [expr $phy << 8] + $i]

	 * MII Management Address = regid + 0x228

	 * phy is located at bits 12-8, regaddr is located at bits 4-0;

*/

	ipMuxRegWrite(0x8228, ((phy & 0x1f) << 8) + phyregid);

	tmpval = 0;
	ipMuxRegRead(0x8228, &tmpval);
	printf("tmpval: %d, val: %d\n", tmpval,valw);

	/*# start write

	 * MII Management Control = regid + 0x22C

*/

	ipMuxRegWrite(0x822C, valw); 

}    



/*******************************************************************************
 *
 * ipmux_AN_enable
 *
 * DESCRIPTION:
 * 	this function writes MII register.
 *
 * INPUTS:
 *	port		- port number.
 *	phy		- phy number.
 *	yes		- enable or disable
 *
 * OUTPUT:
 *	n.a
 *
 * RETURNS:
 *	OPL_OK.
 *
 * SEE ALSO:
 */

OPL_STATUS ipmux_AN_enable( UINT8 port, UINT8 phy, UINT8 yes)
{
	UINT16 valw, valstatus;

	switch (port)
	{
		case	 IPMUX_MII_PORT_GE0:
			opl_log(("GE0 selected\n"));
			break;
		case IPMUX_MII_PORT_GE1:
			opl_log(("GE1 selected\n"));
			break;
		case IPMUX_MII_PORT_GEL:
			opl_log(("GEL selected\n"));
			break;
		default:
			opl_log(("wrong port selected\n"));
			return 0;
	}

	if ( IPMUX_AN_ENABLE == yes )
	{
		/* enable automatic crossover: register 16 */
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_SPEC_CONTROL, 0x68);

		/* update register 9: 1000BASE-T control */
		valw = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_1000M_CONTROL);
		valw |= 0x0300; /* advertise 1000BASE-T Full/Half duplex */
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_1000M_CONTROL, valw);

		/* read register 0 */
		valw = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_CONTROL);

		/* 0.12 AN enable/disable */
		/* ipmux_MII_write(port, phy, 0, valw | (0x1 << 12)); */
		valw = 0x1140;
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw);

		/* 0.9 AN restart, 0.15 Reset */
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw | (0x1 << 15) | (0x1 << 9));
	}
	else
	{
		/* disable automatic crossover: register 16; */
		/* fisrt to get MDI/MDIX status in register 17 */
		valstatus = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_SPEC_STATUS);
		valw = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_SPEC_CONTROL);
		if ( 0x1 == (valstatus >> IPMUX_MII_REG_PHY_SPEC_STATUS_MDI_BIT))
		{
			/* set "01" for manual MDIX mode: 16.6:5 bit. */
			valw &= ~(0x3 << IPMUX_MII_REG_PHY_SPEC_CONTROL_MDI_BIT);
			valw |= (0x01 << IPMUX_MII_REG_PHY_SPEC_CONTROL_MDI_BIT);
		}
		else
		{
			/* set "00" for manual MDI mode: 16.6:5 bit. */
			valw &= ~(0x3 << IPMUX_MII_REG_PHY_SPEC_CONTROL_MDI_BIT);
		}
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_SPEC_CONTROL, valw);

		/* read register 0 */
		valw = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_CONTROL);

		/* 0.12 AN enable/disable */
		valw &= (UINT16)(~(0x1 << 12));
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw);

		/* 0.9 AN restart, 0.15 Reset */
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw | (0x1 << 15) | (0x1 << 9));
	}

	valw = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_CONTROL);

	return OPL_OK;
}








/*******************************************************************************
 *
 * ipmux_AN_set_duplex
 *
 * DESCRIPTION:
 * 	this function writes MII register.
 *
 * INPUTS:
 *	port		- port number.
 *	phy		- phy number.
 *	yes		- enable or disable
 *
 * OUTPUT:
 *	n.a
 *
 * RETURNS:
 *	OPL_OK.
 *
 * SEE ALSO:
 */

OPL_STATUS ipmux_AN_set_duplex( UINT8 port, UINT8 phy, UINT8 type)
{
	UINT16 valw;

	switch (port)
	{
		case	 IPMUX_MII_PORT_GE0:
			opl_log(("GE0 selected\n"));
			break;
		case IPMUX_MII_PORT_GE1:
			opl_log(("GE1 selected\n"));
			break;
		case IPMUX_MII_PORT_GEL:
			opl_log(("GEL selected\n"));
			break;
		default:
			opl_log(("wrong port selected\n"));
			return 0;
	}

	/* read register 0 */
	valw = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_CONTROL);

	/* check if AN is disabled */
	if ( 0x1 == (valw >> IPMUX_MII_REG_CONTROL_AN_BIT))
	{
		opl_log(("AN is enabled now\n"));
		return OPL_ERROR;
	}

	if ( IPMUX_MII_DUPLEX_FULL == type )
	{
		/* 0.8 determines duplex = 1 */
		valw |= (UINT16)(0x1 << 8);
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw);
	}
	else /* half duplex */
	{
		/* 0.8 determines duplex = 0 */
		valw &= (UINT16)(~(0x1 << 8));
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw);
	}

	/* 0.9 AN restart, 0.15 Reset */
	ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw | (0x1 << 15) | (0x1 << 9));

	valw = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_CONTROL);

	return OPL_OK;
}






/*******************************************************************************
 *
 * ipmux_AN_set_speed
 *
 * DESCRIPTION:
 * 	this function writes MII register.
 *
 * INPUTS:
 *	port		- port number.
 *	phy		- phy number.
 *	yes		- enable or disable
 *
 * OUTPUT:
 *	n.a
 *
 * RETURNS:
 *	OPL_OK.
 *
 * SEE ALSO:
 */

OPL_STATUS ipmux_AN_set_speed( UINT8 port, UINT8 phy, UINT8 type)
{
	UINT16 valw;

	/* check if AN is enabled */

	switch (port)
	{
		case	 IPMUX_MII_PORT_GE0:
			opl_log(("GE0 selected\n"));
			break;
		case IPMUX_MII_PORT_GE1:
			opl_log(("GE1 selected\n"));
			break;
		case IPMUX_MII_PORT_GEL:
			opl_log(("GEL selected\n"));
			break;
		default:
			opl_log(("wrong port selected\n"));
			return 0;
	}

	/* read register 0 */
	valw = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_CONTROL);

	/* check if AN is disabled */
	if ( 0x1 == (valw >> IPMUX_MII_REG_CONTROL_AN_BIT))
	{
		opl_log(("AN is enabled now\n"));
		return OPL_ERROR;
	}

	if ( IPMUX_MII_SPEED_1000M == type )
	{
		/* 0.6 and 0.13 determine speed = 10 */
		valw &= (UINT16)(~(0x1 << 13));
		valw |= (UINT16)(0x1 << 6) | (UINT16)(0x1 << 8);   /* also set duplex when speed = 1000M */
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw);
	}
	else 	if ( IPMUX_MII_SPEED_100M == type ) /* 100M */
	{
		/* 0.6 and 0.13 determine speed = 01 */
		valw &= (UINT16)(~(0x1 << 6));
		valw |= (UINT16)(0x1 << 13);
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw);
	}
	else /* 10M */
	{
		/* 0.6 and 0.13 determine speed = 00 */
		valw &= (UINT16)(~(0x1 << 6));
		valw &= (UINT16)(~(0x1 << 13));
		ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw);
	}

	/* 0.9 AN restart, 0.15 Reset */
	ipmux_MII_write(port, phy, IPMUX_MII_REG_PHY_CONTROL, valw | (0x1 << 15) | (0x1 << 9));

	valw = ipmux_MII_read(port, phy, IPMUX_MII_REG_PHY_CONTROL);

	return OPL_OK;
}






/*******************************************************************************
 *
 * ipmux_AN_loopback_enable
 *
 * DESCRIPTION:
 * 	this function writes MII register.
 *
 * INPUTS:
 *	port		- port number.
 *	phy		- phy number.
 *	yes		- enable or disable
 *
 * OUTPUT:
 *	n.a
 *
 * RETURNS:
 *	OPL_OK.
 *
 * SEE ALSO:
 */

OPL_STATUS ipmux_AN_loopback_enable( UINT8 port, UINT16 phy, UINT8 type)
{
	UINT16 valw;

	/* check if AN is enabled */

	switch (port)
	{
		case	 IPMUX_MII_PORT_GE0:
			opl_log(("GE0 selected\n"));
			break;
		case IPMUX_MII_PORT_GE1:
			opl_log(("GE1 selected\n"));
			break;
		case IPMUX_MII_PORT_GEL:
			opl_log(("GEL selected\n"));
			break;
		default:
			opl_log(("wrong port selected\n"));
			return 0;
	}

	/* read register 0 */
	valw = ipmux_MII_read(port, phy, 0);

	if ( IPMUX_MII_LOOPBACK_ENABLE == type )
	{
		/* 0.14 determines loopback = 1 */
		valw |= (UINT16)(0x1 << 14);
		ipmux_MII_write(port, phy, 0, valw);
	}
	else /* disable loopback */
	{
		/* 0.14 determines loopback = 0 */
		valw &= (UINT16)(~(0x1 << 14));
		ipmux_MII_write(port, phy, 0, valw);
	}

	/* 0.9 AN restart, 0.15 Reset */
	ipmux_MII_write(port, phy, 0, valw | (0x1 << 15) | (0x1 << 9));

	valw = ipmux_MII_read(port, phy, 0);

	return OPL_OK;
}

OPL_STATUS ipmux_MII_dump(UINT8 port, UINT8 phy)
{
	int i;
	UINT16 val;

	printf("port = %d\tphy = %d\n", port, phy);
	printf("================================");
	printf("================================\n");

	for ( i = 1; i < 32; i++)
	{
		val = ipmux_MII_read(port, phy, i);
		printf("reg%02d = 0x%04x\t", i-1, val);
		if (0 == (i+1) % 4 )
			printf("\n");
	}

	printf("\n");

	return OPL_OK;
}

OPL_STATUS ipmux_MII_print_phy_spec_status_reg(UINT16 val)
{
	UINT8 index;
	char*       speed[] = {"10M/s","100M/s","1000M/s","Reserved",OPL_NULL};
	char*       duplex[] = {"Half duplex","Full duplex", OPL_NULL};
	char*       pgrecv[] = {"Page not received","Page received", OPL_NULL};
	char*       resolved[] = {"Speed/Duplex not resolved","Speed/Duplex resolved", OPL_NULL};
	char*       link[] = {"Link is down","Link is up", OPL_NULL};
	char*       mdi[] = {"MDI","MDIX", OPL_NULL};

	printf("PHY Specific Status register =============\n");

	index = (val >> 14) & 0x3;
	printf("%s\t", speed[index]);

	index = (val >> 13) & 0x1;
	printf("%s\t", duplex[index]);

	index = (val >> 12) & 0x1;
	printf("%s\t", pgrecv[index]);

	index = (val >> 11) & 0x1;
	printf("%s\t", resolved[index]);

	index = (val >> 10) & 0x1;
	printf("%s\t", link[index]);

	index = (val >> 6) & 0x1;
	printf("%s\t", mdi[index]);

	printf("\n");

	return OPL_OK;
}



OPL_STATUS ipmux_MII_print_phy_control_reg(UINT16 val)
{
	UINT8 index;
	char*       loopback[] = {"loopback disable", "loopback enable",OPL_NULL};
	char*       speed[] = {"10M/s","100M/s","1000M/s","Reserved",OPL_NULL};
	char*       an[] = {"auto-nego disable", "auto-nego enable",OPL_NULL};
	char*       power[] = {"Normal operation", "Power down",OPL_NULL};
	char*       duplex[] = {"Half duplex","Full duplex", OPL_NULL};

	printf("PHY Control register =============\n");

	index = (val >> 14) & 0x1;
	printf("%s\t", loopback[index]);

	index = ((val >> 13) & 0x1) | ((val >> 5) & 0x2); /* yzhang change to be such from: (val >> 13) & 0x1 | (val >> 5) & 0x2 */
	printf("%s\t", speed[index]);

	index = (val >> 12) & 0x1;
	printf("%s\t", an[index]);

	index = (val >> 11) & 0x1;
	printf("%s\t", power[index]);

	index = (val >> 8) & 0x1;
	printf("%s\t", duplex[index]);

	printf("\n");

	return OPL_OK;
}





OPL_STATUS ipmux_MII_print_phy_status_reg(UINT16 val)
{
	UINT8 index;
	char*       an[] = {"auto-nego not completed", "auto-nego completed",OPL_NULL};
	char*       link[] = {"Link is down", "Link is up",OPL_NULL};

	printf("PHY Status register =============\n");

	printf("PHY not able to perform 100BASE-T4, able to perform Full/Half duplex 100BASE-X\n");
	printf("able to perform Full/Half duplex 10BASE-T, able to perform Full/Half duplex 100BASE-T2, able to perform AN\n");

	printf("Not able to perform Full/Half duplex 1000BASE-X, able to perform Full/Half duplex 1000BASE-T\n");

	index = (val >> 5) & 0x1;
	printf("%s\t", an[index]);

	index = (val >> 2) & 0x1;
	printf("%s\t", link[index]);

	printf("\n");

	return OPL_OK;
}






OPL_STATUS ipmux_MII_print_AN_AD_reg(UINT16 val)
{
	UINT8 index;
	char*       np[] = {"Next page Not advertised", "Next page Advertised",OPL_NULL};
	char*       rf[] = {"Do not set remote fault bit", "Set remote fault bit",OPL_NULL};
	char*       ap[] = {"No asymmetic Pause", "Asymmetic pause",OPL_NULL};
	char*       pause[] = {"MAC PAUSE not implemented", "MAC PAUSE implemented",OPL_NULL};
	char*       TX100FDUP[] = {"100BASE-TX FULL Duplex not advertised", "100BASE-TX FULL Duplex advertised",OPL_NULL};
	char*       TX100HDUP[] = {"100BASE-TX HALF Duplex not advertised", "100BASE-TX HALF Duplex advertised",OPL_NULL};
	char*       TX10FDUP[] = {"10BASE-TX FULL Duplex not advertised", "10BASE-TX FULL Duplex advertised",OPL_NULL};
	char*       TX10HDUP[] = {"10BASE-TX HALF Duplex not advertised", "10BASE-TX HALF Duplex advertised",OPL_NULL};

	printf("AN advertisement register =============\n");

	index = (val >> 15) & 0x1;
	printf("%s\t", np[index]);

	index = (val >> 13) & 0x1;
	printf("%s\t", rf[index]);

	index = (val >> 11) & 0x1;
	printf("%s\t", ap[index]);

	index = (val >> 10) & 0x1;
	printf("%s\n", pause[index]);

	printf("Not cable of 100BASE-T4\n");

	index = (val >> 8) & 0x1;
	printf("%s\n", TX100FDUP[index]);

	index = (val >> 7) & 0x1;
	printf("%s\n", TX100HDUP[index]);

	index = (val >> 6) & 0x1;
	printf("%s\n", TX10FDUP[index]);

	index = (val >> 5) & 0x1;
	printf("%s\n", TX10HDUP[index]);

	printf("\n");

	return OPL_OK;
}





OPL_STATUS ipmux_MII_print_AN_EXP_reg(UINT16 val)
{
	UINT8 index;
	char*       pd[] = {"A fault has not been detected via the parallel detection", "A fault has been detected via the parallel detection",OPL_NULL};
	char*       lpnp[] = {"Link partner is not Next Page able", "Link partner is Next Page able",OPL_NULL};
	char*       pr[] = {"A new page has not been received", "A new page has been received",OPL_NULL};
	char*       lpan[] = {"Link partner is not AN able", "Link partner is AN able",OPL_NULL};

	printf("AN Expension register =============\n");

	index = (val >> 4) & 0x1;
	printf("%s\n", pd[index]);

	index = (val >> 3) & 0x1;
	printf("%s\n", lpnp[index]);

	printf("Local device is Next Page able\n");

	index = (val >> 1) & 0x1;
	printf("%s\n", pr[index]);

	index = (val >> 0) & 0x1;
	printf("%s\n", lpan[index]);

	printf("\n");

	return OPL_OK;
}


OPL_STATUS ipmux_MII_print_reg(UINT8 phyregid, UINT16 val)
{
	switch (phyregid)
	{
		case IPMUX_MII_REG_PHY_SPEC_STATUS:
			ipmux_MII_print_phy_spec_status_reg(val);
			break;
		case IPMUX_MII_REG_PHY_CONTROL:
			ipmux_MII_print_phy_control_reg(val);
			break;
		case IPMUX_MII_REG_PHY_STATUS:
			ipmux_MII_print_phy_status_reg(val);
			break;
		case IPMUX_MII_REG_PHY_AN_ADS:
			ipmux_MII_print_AN_AD_reg(val);
			break;
		case IPMUX_MII_REG_PHY_AN_EX:
			ipmux_MII_print_AN_EXP_reg(val);
			break;
		default:
			opl_log(("invalid phy reg id\n"));
			return OPL_ERROR;
	}

	return OPL_OK;
}

OPL_STATUS ipmux_MII_show_reg(UINT8 port, UINT8 phy, UINT8 phyregid)
{
	UINT16 val;

	val = ipmux_MII_read(port, phy, phyregid);

	ipmux_MII_print_reg(phyregid, val);

	return OPL_OK;
}


/*
   OPL_STATUS ipmux_MII_update (void)
   {
   UINT16	valw;
   UINT8	speed;
   UINT8	duplex;

   valw = ipmux_MII_read(IPMUX_MII_PORT_GE0, IPMUX_MII_PHY0, IPMUX_MII_REG_PHY_SPEC_STATUS);
   if ( (0x0 == (0x01 & (valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_LINK_BIT)))
   || (0x0 == (0x1 &(valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_SPEEDDUPLEX_BIT))))
   {
   opl_log(("Link GE0 is down or Speed/Duplex is not resolved\n"));
   }
   else
   {
   speed = 0x3 &  (valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_SPEED_BIT);
   duplex= 0x1 &  (valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_DUPLEX_BIT);

   ipmux_gmac_set_speed(0, speed);
   ipmux_gmac_set_duplex(0, duplex);
   }


   valw = ipmux_MII_read(IPMUX_MII_PORT_GE1, IPMUX_MII_PHY0, IPMUX_MII_REG_PHY_SPEC_STATUS);
   if ( (0x0 == (0x01 & (valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_LINK_BIT)))
   || (0x0 == (0x1 &(valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_SPEEDDUPLEX_BIT))))
   {
   opl_log(("Link GE1 is down or Speed/Duplex is not resolved\n"));
   }
   else
   {
   speed = 0x3 &  (valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_SPEED_BIT);
   duplex= 0x1 &  (valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_DUPLEX_BIT);

   ipmux_gmac_set_speed(1, speed);
   ipmux_gmac_set_duplex(1, duplex);
   }

   return OPL_OK;
   }
   */




OPL_STATUS ipmux_MII_Reset_Counter (UINT32 Port, UINT32 Phy)
{
	UINT16	valw;
	UINT32	regVal, regVal2;

	if (0 == Port)
	{
		/* read MII register and set MAC register accordingly */
		valw = ipmux_MII_read(IPMUX_MII_PORT_GE0, IPMUX_MII_PHY0, IPMUX_MII_REG_PHY_SPEC_STATUS);
		if ( 0x0 == (0x01 & (valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_LINK_BIT)))
		{
			opl_log(("Link GE0 is down or Speed/Duplex is not resolved\n"));
		}
		else
		{
			/* link is up */

			/* Rx etherStatsUndersizePkts Count */
			ipMuxRegRead(0x8118, (UINT32 *)&regVal);

			/* Rx etherStatsOversizePkts Count */
			ipMuxRegRead(0x811C, (UINT32 *)&regVal2);

			printf("GE0 undersize count = %d, oversize count = %d\n", regVal, regVal2);

			/* reset PHY Mac */
			valw = 0x8000;
			ipmux_MII_write(Port, Phy, IPMUX_MII_REG_PHY_CONTROL, valw);
		}
	}
	else if (1 == Port)
	{
		valw = ipmux_MII_read(IPMUX_MII_PORT_GE1, IPMUX_MII_PHY0, IPMUX_MII_REG_PHY_SPEC_STATUS);
		if ( (0x0 == (0x01 & (valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_LINK_BIT)))
				|| (0x0 == (0x1 &(valw >> IPMUX_MII_REG_PHY_SPEC_STATUS_SPEEDDUPLEX_BIT))))
		{
			opl_log(("Link GE1 is down or Speed/Duplex is not resolved\n"));
		}
		else
		{
			/* link is up */

			/* Rx etherStatsUndersizePkts Count */
			ipMuxRegRead(0xa118, (UINT32 *)&regVal);

			/* Rx etherStatsOversizePkts Count */
			ipMuxRegRead(0xa11C, (UINT32 *)&regVal2);

			printf("GE1 undersize count = %d, oversize count = %d\n", regVal, regVal2);

			/* reset PHY Mac */
			valw = 0x8000;
			ipmux_MII_write(Port, Phy, IPMUX_MII_REG_PHY_CONTROL, valw);
		}
	}

	return OPL_OK;
}

