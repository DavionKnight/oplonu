#include <common.h>

#ifdef FPGA_ONU_16B
#define INT_CTRL_ADDR 0xbf002e00     /* wwei mark */
#else
#define INT_CTRL_ADDR 0xbf002a00
#endif

typedef struct S_INT_CTRL
{
	volatile u32 SOURCE1;	//0x2A00
	volatile u32 SOURCE2;	//0x2A04
	volatile u32 ENABLE1;	//0x2A08
	volatile u32 ENABLE2;	//0x2A0c
	volatile u32 CLEAR1;	//0x2A10
	volatile u32 CLEAR2;	//0x2A14
	volatile u32 CFG;		//0x2A18
} INT_CTRL;
