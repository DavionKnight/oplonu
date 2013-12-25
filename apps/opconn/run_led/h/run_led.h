#ifndef __RUN_LED__
#define __RUN_LED__

extern unsigned int test_led_flag;


char BoardReset();
static void led_change_status();
void run_led();
void powerStatus_check();
int cpldRead(int offset,unsigned char *setData);
int cpldWrite(int offset,unsigned char setData);
int SetAlarmEnable(int enable);
char get_serialcard_status();
unsigned char cpld_register_read(unsigned char regAddr,unsigned char *uData);
extern int loopdetect_en;


#define CS1_CPLD_VERSION 0x00
#define CS1_SRIALCARD_STATUS 0x01
#define CS1_INT_SOURCE 0x02
#define CS1_LIGHT_SIGNAL 0x03
#define CS1_POWER_STATUS 0x04
#define CS1_PROTECT_REG 0x05
#define CS1_VOIP_STATUS 0x06
#define CS1_RESET_REG 0x07
#define CS1_INT_CONTROL 0x08
#define CS1_WATCHDOG_ENABLE 0x09
#define CS1_CLEARINT_REG 0x0A
#define CS1_PON_CONTROL 0x0B
#define CS1_IIC_CONTROL 0x0C
#define CS1_RUN_LED 0x0D
#define CS1_RACK_WARNING 0x0E
#endif

