/******************************************************************************/
/**  COPYRIGHT (C) 2005- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/

#include "ipmux_gpio.h"

volatile IPMUX_GPIO_REGISTERS_S *pGpio = (volatile  IPMUX_GPIO_REGISTERS_S *)IPMUX_GPIO_BASEADDR;

/******************************************************************************
*	Name: gpioInit
*
*	Description: This function will initilize gpio. 
*				 
*				 
*
*	Input parameter:  none
*
*	Output parameter: none
*
*/
void ipmuxGpioInit(void)
{
	pGpio->clr = 0xFFF;
}


/******************************************************************************
*	Name: gpioDirInputSet(UINT8 pin)
*
*	Description: 	
*
*	  This function set the pin as input.
*
*	Input parameter:  pin,  which pin to be set as input
*
*	Output parameter: none
*
*/
void ipmuxGpioDirInputSet(UINT8 pin)
{
	pGpio->dir &=(~(1<<pin)) ;
}


/******************************************************************************
*	Name: gpioDirOutputSet(UINT8 pin)
*
*	Description: 
*
*	Input parameter:  pin,  which pin to be set as output
*
*	Output parameter: none
*
*/
void ipmuxGpioDirOutputSet(UINT8 pin)
{
	pGpio->dir |= 1<<pin;
}

/******************************************************************************
*	Name: gpioIntModeLevelSet(UINT8 pin)
*
*	Description: This function set the interrupt triggered by level mode.
*
*	Input parameter:  pin,  which pin to be set the interrupt triggered by level mode.
*
*	Output parameter: none
*
*/
void ipmuxGpioIntModeLevelSet(UINT8 pin)
{
	pGpio->intMode |=(1<<pin) ;
}


/******************************************************************************
*	Name: gpioIntModeEdgeSet(UINT8 pin)
*
*	Description: This function set the interrupt to be set as edge mode.
*
*	Input parameter:  pin,  which pin's interrupt to be set as edge mode
*
*	Output parameter: none
*
*/
void ipmuxGpioIntModeEdgeSet(UINT8 pin)
{
	pGpio->intMode &=(~(1<<pin)) ;
}



/******************************************************************************
*	Name: gpioIntLevelHighSet(UINT8 pin)
*
*	Description: This function set the interrupt to be triggered by active high signal.
*
*	Input parameter:  pin,  which pin to be set the interrupt to be triggered by active high signal.
*
*	Output parameter: none
*
*/
void ipmuxGpioIntLevelHighSet(UINT8 pin)
{
	pGpio->intLevel |= 1<<pin;	
}

/******************************************************************************
*	Name: gpioIntModeLevelSet(UINT8 pin)
*
*	Description: This function set the interrupt to be triggered by active low signal.
*
*	Input parameter:  pin,  which pin to be set the interrupt to be triggered by active low signal.
*
*	Output parameter: none
*
*/
void ipmuxGpioIntLevelLowSet(UINT8 pin)
{
	pGpio->intLevel &=~(1<<pin);
}

/******************************************************************************
*	Name: gpioIntEdgeSet(UINT8 pin)
*
*	Description: 
*		This function set the interrupt to be triggered on rising or falling edge of signal the pin controls
*
*	Input parameter:  
*		pin	-	which pin to be set the interrupt to be triggered on rising or falling edge of signal the pin controls
*		type-	0x00: No edge programmed
*				0x01: Interrupt to be triggered on rising edge of the signal
*				0x10: Interrupt to be triggered on falling edge of signal
*				0x11: Interrupt to be triggered on both edge of signal
*
*	Output parameter: none
*
*/
void ipmuxGpioIntEdgeSet(UINT8 pin,UINT8 type)
{
	UINT32 data;
	
	/* clear 2 bit to zero  */ 
	data = (pGpio->intEdge & ~(0x11<<(pin*2)));
	
	/* write 2 bit */ 
	data |=type<<(pin*2);

	pGpio->intEdge = data;
}

/******************************************************************************
*	Name: gpioRead(UINT8 pin)
*
*	Description: 
*		
*
*	Input parameter:  
*		 	pin: which pin to be read.
*
*	Output parameter: 
*			the character that is received
*/
UINT8 ipmuxGpioValGet(UINT8 pin)
{
	UINT8 data;
	data=(pGpio->val & (1<<pin))>>pin;
	return data;
}


/******************************************************************************
*	Name: void ipmuxGpioOutputSet(UINT8 pin,UINT8 val)
*
*	Description:  
*		This function write 1 byte data to pin.
*
*	Input parameter:  none
*		pin: which pin to be read.
*		val: data to be write to pin.
*/
void ipmuxGpioOutputSet(UINT8 pin,UINT8 val)
{
	if (val) 
	{
		pGpio->set |= 1<<pin;
	}
	else
	{
		pGpio->set &= ~((UINT32)1<<pin);
		pGpio->clr |= 1<<pin;
	}
}


/******************************************************************************
*	Name: void ipmuxGpioOutputGet(UINT8 pin)
*
*	Description:  
*		This function get 1 byte data from pin.
*
*	Input parameter:  none
*		pin: which pin to be read.
*		val: data to be write to pin.
*/
UINT8 ipmuxGpioOutputGet(UINT8 pin)
{
	return (pGpio->set & (1<<pin))>>pin;
}


void ipmuxGpioTest(void)
{
#define IPMUX_GPIO_NUM 12
	int i;
	UINT8 data;
	/* ------ test input mode ------ */
	printf("input mode:\n");
	for(i=0; i< IPMUX_GPIO_NUM ; i++)
	{
		data=ipmuxGpioValGet(i);
		printf(" <%d:%d>",i,data);
	}
	printf("\n");

	/* ------ output mode ------ */ 
	printf("output mode:\n");
	for(i=0; i< IPMUX_GPIO_NUM ; i++)
	{
		ipmuxGpioDirOutputSet(i);
	}
	
	/* write output value 1 */ 
	printf("write output value 1 :\n");
	for(i=0; i< IPMUX_GPIO_NUM ; i++)
	{
		ipmuxGpioOutputSet(i,1);
	}
	for(i=0; i< IPMUX_GPIO_NUM ; i++)
	{
		data=ipmuxGpioOutputGet(i);
		printf(" <%d:%d>",i,data);
	}
	printf("\n");
	
	/* write output value 0 */ 
	printf("write output value 0 :\n");
	for(i=0; i< IPMUX_GPIO_NUM ; i++)
	{
		ipmuxGpioOutputSet(i,1);
	}
	for(i=0; i< IPMUX_GPIO_NUM ; i++)
	{
		data=ipmuxGpioOutputGet(i);
		printf(" <%d:%d>",i,data);
	}
	printf("\n");
	
}

