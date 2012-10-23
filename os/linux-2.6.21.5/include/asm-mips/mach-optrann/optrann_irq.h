
#include <linux/types.h>


#define IPMUX_GEL_INT 5
/*--------------------------Macro definition------------------------- */
#define IPMUX_REG_SIZE         32           /* 32 bits register for IPMux */
#define REG_ADDR_ALLIGNED       4           /* 32bits word */
#define FIELD_BITS(n)               (0xffffffff >> (IPMUX_REG_SIZE - (n)))

#define IPMUX_REG_BASE  0xbf000000
#define IPMUX_INT_REG_BASE (IPMUX_REG_BASE + PPH_BASE)

#define IPMUX_REG_ID2ADDR(regId)   ((u32)IPMUX_REG_BASE + PPH_BASE + (u32)(regId))

#define ipMuxRegRead(regId, pval)  *(u32*)(pval) = *(volatile u32 *)IPMUX_REG_ID2ADDR(regId)
#define ipMuxRegWrite(regId, val)  *((volatile u32 *)IPMUX_REG_ID2ADDR(regId)) = (u32)(val)

#define REG_BIT_INTCFG_IRQ0MD_OFF      15
#define REG_BIT_INTCFG_IRQ0MD          (FIELD_BITS(1) << REG_BIT_INTCFG_IRQ0MD_OFF)
#define REG_BIT_INTCFG_MINTE_OFF       0
#define REG_BIT_INTCFG_MINTE           (FIELD_BITS(1) << REG_BIT_INTCFG_MINTE_OFF)

#define REG_BIT_SYSCFG1_MINTE_OFF       0
#define REG_BIT_SYSCFG1_MINTE           (FIELD_BITS(1) << REG_BIT_SYSCFG1_MINTE_OFF)
#define IPMUX_MAX_NUM_OF_INT_LEVEL2 32

/** irq  number assignment   */
#define OPL_ONU_GPIO_0  0
#define OPL_ONU_GPIO_1  1
#define OPL_ONU_GPIO_2  2
#define OPL_ONU_GPIO_3  3
#define OPL_ONU_GPIO_4  4
#define OPL_ONU_GPIO_5  5
#define OPL_ONU_GPIO_6  6
#define OPL_ONU_GPIO_7  7
#define OPL_ONU_GPIO_8  8
#define OPL_ONU_GPIO_9  9
#define OPL_ONU_GPIO_10 10 
#define OPL_ONU_GPIO_11 11

#define OPL_ONU_PCM_IRQ  19
#define OPL_ONU_PMAC_IRQ 20

#define OPL_UART0_IRQ    23
#define OPL_UART1_IRQ    24
#define OPL_HOSTDMA0_IRQ 25
#define OPL_FEDMA1_IRQ   26
#define OPL_IPMUX_IRQ    29



/*-----------------global varible/function declaration--------------- */
extern void malta_hw0_irqdispatch(void);
extern void corehi_irqdispatch(void);
extern void __init init_IRQ(void);

extern void ipmux_irq_disable(unsigned int irq);
extern void ipmux_irq_enable(unsigned int irq);


