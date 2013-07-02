#include <stdio.h>
#include "opl_cs1_base.h"
#include "warning_check.h"
#include "run_led.h"

void warning_check()
{
	char led_status,light_warning,rack_warning,voice_port_warning;
	
	cpldRead(CS1_RUN_LED,&led_status);
	cpldRead(CS1_LIGHT_SIGNAL,&light_warning);
	cpldRead(CS1_VOIP_STATUS,&voice_port_warning);
	cpldRead(CS1_RACK_WARNING,&rack_warning);
#if 0
	if(0x01 == ((0x0 == (light_warning & 0x01)) && (0x0 == ((light_warning & 0x02)>>1)) || (voice_port_warning & 0x01)| 
			((voice_port_warning & 0x02)>>1)	|| (rack_warning & 0x01)))
#else
	if(((0x0 == (light_warning & 0x01)) && (0x0 == (light_warning & 0x02))) || (rack_warning & 0x01))
#endif
	{
		led_status = (led_status & 0xfd);	//warning_led  on
	}
	else
		led_status = (led_status | 0x02);	//warning_led off
	cpldWrite(CS1_RUN_LED,led_status);
}
void alarm_led_enable(int test_led_flag)
{
	char led_status;

	cpldRead(CS1_RUN_LED,&led_status);
	if(1 == test_led_flag)								//warning_led on
	{
		led_status &= 0xfd;
	}
	else if(2 == test_led_flag)
	{
		led_status |= 0x02;
	}
	else
	{
		printf("test_led_flag error..\r\n");
		return;
	}
	cpldWrite(CS1_RUN_LED,led_status);	
}
