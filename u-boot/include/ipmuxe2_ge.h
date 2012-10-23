#ifndef _IPMUX_E2_GE_H_
#define _IPMUX_E2_GE_H_
/** THE FORMAT is FOR DOXYGEN to Generate the document automatically

  *  @defgroup 
  *  @copyright 
  *  @author 
  *  @version 
  *  @date 
  *  @histroy 
  *  author		 date		 description
  *  -------------------------------------------------------------------

  *  @{
  */

/** @name Macro definition
 *  @{
 */
/*--------------------------Macro definition------------------------- */

#define	IPMUX_DMA0 	1
#define	IPMUX_DMA1 	2
#define	IPMUX_DMA2 	4
#define	IPMUX_DMA3 	8

#define IPMUX_DMA_HOST	IPMUX_DMA0 
#define IPMUX_DMA_FE	IPMUX_DMA1 
#define IPMUX_DMA_GE0   IPMUX_DMA2
#define IPMUX_DMA_GE1   IPMUX_DMA3

#define IPMUX_DMA_BUF_LEN   2048
#define IPMUX_DMA_INT_RX   	0x1
#define IPMUX_DMA_INT_TX   	0x2
#define IPMUX_DMA_INT_BUSY 	0x4

#define IPMUX_REG_BASE 0xbf000000

#define IPMUX_REG_ID2ADDR(regId) 	((unsigned long)IPMUX_REG_BASE + (unsigned long)(regId))
#define IPMUX_REG_ADDR2ID(addr) 	((unsigned long)(addr) - (unsigned long)IPMUX_REG_BASE)

#define ipMuxRegRead(regId, pval)	*(unsigned long*)(pval) = *(volatile unsigned long *)IPMUX_REG_ID2ADDR(regId)
#define ipMuxRegWrite(regId, val)	*((volatile unsigned long *)IPMUX_REG_ID2ADDR(regId)) = (unsigned long)(val)

static inline int ipMuxRegBitTest (int regId, unsigned long bits)
{
        unsigned long nTemp = 0;
	if (regId % 4 != 0)
		return -1;

        ipMuxRegRead (regId, &nTemp);
        return (nTemp & bits);
}

/** @}*/

/** @name type definition
 *  @{
 */
/*--------------------------type definition------------------------- */





/** @}*/

/** @name global variable and function imported
 *  @{
 */
/*-----------------global varible/function declaration--------------- */





/** @}*/
   
/** @name local function declaration
 *  @{
 */
/*-------------------local  function declaration---------------------- */





/** @}*/
 

/** @name local variable declaration and definition
 *  @{
 */
/*--------------local  variable declaration and definition------------- */





/** @}*/
 

/** @name global variable and function exported
 *  @{
 */
/*------------------global variable and function exported-------------- */
extern int ipmuxDmaInit(int dmas);
extern int ipmuxDmaRx(int dmas, volatile char **packet, int pckCopied);
extern int ipmuxDmaTx(int dmas, void *packet, int length);

extern void ipmuxDmaEnable(int dmas);

/** @}*/


/** @defgroup

 *  @{
 */
/*----------------------local  function definition--------------------- */






/** @}*/
/** @}*/
#endif /* _IPMUX_E2_GE_H_ */
