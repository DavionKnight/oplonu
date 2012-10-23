/******************************************************************************/
/**  COPYRIGHT (C) 2005- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**  Module name: opulangmac.h                                                                        **/
/******************************************************************************/

#ifndef __gmac_H__
#define __gmac_H__
#include <configs/opipmux.h>

#if defined(CONFIG_IPMUX) || defined(CONFIG_IPMUX_A)
#ifdef __cplusplus
extern "C" {
#endif

/***add by thu for base address for GMAC and GEL***********************************/
//please move following to op_ipmux.h
#define OP_IPMUX_GEL_ADDR   0xbf00C200
#define OP_IPMUX_GEL_DMA_ADDR   0xbf002BC0

#define OP_IPMUX_GEW0_ADDR  0xbf002000
#define OP_IPMUX_GEW1_ADDR  0xbf002800

/* prototype declare */
static void write_bd_rx(UINT32 data,UINT16 bd_index);
static void write_bd_tx(UINT32 data,UINT16 bd_index);
static UINT32 read_bd_rx(UINT16 bd_index);
static UINT32 read_bd_tx(UINT16 bd_index);
static UINT16 find_valid_rx(void);
static void print_config(void);
static void test_gel_sendpack(void);
static void unable_dma_fe(void);
static void init_dma_fe(void);
static void print_bd_data(UINT32 bd_data, UINT16 bd_index);
static void init_bd_rx(void);
static void init_bd_tx(void);
static void inc_bdid_tx(void);
static void inc_bdid_rx(void);
#ifdef DEBUGDMA
void print_bd_current_rx(void);
void testBD(void);
void testgel(void);
#endif

void print_net(void);
void print_bd_one_rx(UINT16 bd_index,int check_valid);
void ntoh_range(UINT32 *addr,UINT32 len);
void test_gel_debug(void);
void opipmux_gel_reset(void);
void opipmux_gel_tx_loopbackto_rx(void);
void opipmux_gel_set_loopback(void);
int opipmux_gel_init(void);
int debug_fe_loopback(void);
int opipmux_gel_initialize(bd_t *bis);

extern ulong get_timer_usec(ulong base);
extern void debug_pck_send(UINT8 *pck,int len,UINT16 bd_id);
extern void debug_pck_Rcv(unsigned char *pck,int len,unsigned short bd_id);

typedef struct gmac_ctrl_registers
{
    volatile u32  mac_config1;      /* 0x00 */
    volatile u32  mac_config2;      /* 0x04 */
    volatile u32  ipg_ifg;          /* 0x08 */
    volatile u32  half_duplex;      /* 0x0c */
    volatile u32  maxframelen;      /* 0x10 */
    volatile u32  reserved1[2];     /* 0x14-0x18 */
    volatile u32  test;         /* 0x1c */
    volatile u32  mii_config;       /* 0x20 */
    volatile u32  mii_command;      /* 0x24 */
    volatile u32  mii_addr;         /* 0x28 */
    volatile u32  mii_ctrl;         /* 0x2c */
    volatile u32  mii_stat;         /* 0x30 */
    volatile u32  mii_indicator;        /* 0x34 */
    volatile u32  interface_ctrl;       /* 0x38 */
    volatile u32  interface_stat;       /* 0x3c */
    volatile u32  sta_addr0;        /* 0x40 */
    volatile u32  sta_addr1;        /* 0x44 */
    volatile u32  reserved2[2];     /* 0x48-0x4c */
} gmac_ctrl_reg;


#define GMAC_CONFIG1_SOFT_RESET_SHIFT      31
#define GMAC_CONFIG1_SIM_RESET_SHIFT       30
#define GMAC_CONFIG1_RESET_RX_CNTL_SHIFT   19
#define GMAC_CONFIG1_RESET_TX_CNTL_SHIFT   18
#define GMAC_CONFIG1_RESET_RX_FUNC_SHIFT   17
#define GMAC_CONFIG1_RESET_TX_FUNC_SHIFT   16
#define GMAC_CONFIG1_LOOPBACK_SHIFT         8
#define GMAC_CONFIG1_RX_FLOW_CNTL_SHIFT     5
#define GMAC_CONFIG1_TX_FLOW_CNTL_SHIFT     4
#define GMAC_RXRD_ENABLE_SHIFT              3
#define GMAC_RX_ENABLE_SHIFT                2
#define GMAC_TXRD_ENABLE_SHIFT              1
#define GMAC_TX_ENABLE_SHIFT                0

#define GMAC_RX_ENABLE                      0x4
#define GMAC_TX_ENABLE                      0x1

#define GMAC_CONFIG_1(soft_reset,      \
             sim_reset,       \
             reset_rx_cntl,   \
             reset_tx_cntl,   \
                         reset_rx_func,   \
                         reset_tx_func,   \
                         loopback,        \
                         rx_flow_cntl,    \
                         tx_flow_cntl,    \
                         rx_enable,       \
                         tx_enable)       \
 ((soft_reset << GMAC_CONFIG1_SOFT_RESET_SHIFT) | \
  (sim_reset << GMAC_CONFIG1_SIM_RESET_SHIFT) | \
  (reset_rx_cntl << GMAC_CONFIG1_RESET_RX_CNTL_SHIFT) | \
  (reset_tx_cntl << GMAC_CONFIG1_RESET_TX_CNTL_SHIFT) | \
  (reset_rx_func << GMAC_CONFIG1_RESET_RX_FUNC_SHIFT) | \
  (reset_tx_func << GMAC_CONFIG1_RESET_TX_FUNC_SHIFT) | \
  (loopback << GMAC_CONFIG1_LOOPBACK_SHIFT) | \
  (rx_flow_cntl << GMAC_CONFIG1_RX_FLOW_CNTL_SHIFT) | \
  (tx_flow_cntl << GMAC_CONFIG1_TX_FLOW_CNTL_SHIFT) | \
  (rx_enable << GMAC_RX_ENABLE_SHIFT) | \
  (tx_enable << GMAC_TX_ENABLE_SHIFT))

#define GMAC_CONFIG2_FULL_DUPLEX_SHIFT          0
#define GMAC_CONFIG2_CRC_EN_SHIFT               1
#define GMAC_CONFIG2_PAD_CRC_EN_SHIFT           2
#define GMAC_CONFIG2_LEN_CHECK_EN_SHIFT         4
#define GMAC_CONFIG2_HUGE_FRAME_EN_SHIFT        5
#define GMAC_CONFIG2_INTERFACE_MODE_SHIFT       8
#define GMAC_CONFIG2_PREAMBLE_LEN_SHIFT         12

#define GMAC_CONFIG_2(full_duplex,  \
             crc_en,                   \
             pad_and_crc_en,           \
             len_field_check_en,       \
             huge_frame_en,            \
             interface_mode,           \
             preamble_length)          \
( ((full_duplex) << GMAC_CONFIG2_FULL_DUPLEX_SHIFT) | \
  ((crc_en)      << GMAC_CONFIG2_CRC_EN_SHIFT)      | \
  ((pad_and_crc_en) << GMAC_CONFIG2_PAD_CRC_EN_SHIFT) | \
  ((len_field_check_en) << GMAC_CONFIG2_LEN_CHECK_EN_SHIFT) | \
  ((huge_frame_en) << GMAC_CONFIG2_HUGE_FRAME_EN_SHIFT) | \
  ((interface_mode) << GMAC_CONFIG2_INTERFACE_MODE_SHIFT) | \
  ((preamble_length) << GMAC_CONFIG2_PREAMBLE_LEN_SHIFT))


#define GMAC_NON_B2B_IPG1_SHIFT         24
#define GMAC_NON_B2B_IPG2_SHIFT         16
#define GMAC_MIN_IFG_SHIFT              8
#define GMAC_B2B_IPG_SHIFT              0

#define DEFAULT_IPG_IFG                 0x4060_5060

#define GMAC_IPG_IFG(b2b_ipg, \
            min_ifg,          \
            non_b2b_ipg2,     \
            non_b2b_ipg1)     \
 ((b2b_ipg << GMAC_B2B_IPG_SHIFT) |    \
  (min_ifg << GMAC_MIN_IFG_SHIFT) |    \
  (non_b2b_ipg2 << GMAC_NON_B2B_IPG2_SHIFT) | \
  (non_b2b_ipg1 << GMAC_NON_B2B_IPG1_SHIFT))
int ge_reg;
#define GE_WRITE_REG(base_addr,offset,val)  \
    addr=base_addr+offset;  \
    *ge_reg=base_adde+offset;\
    ge_reg=val;

/* test register */
#define TEST_MAXIMUM_BACKOFF_SHIFT          3
#define TEST_REGISTER_TX_FLOW_EN_SHIFT      2
#define TEST_PAUSE_SHIFT                    1
#define TEST_SHORTCUT_SLOT_TIME_SHIFT       0

/*typedef long long          int64;*/
//typedef unsigned long long eth_stat_t;

typedef struct gmac_dmal_registers
{
    volatile u32  SHADOW_SET;       /*indirected access data*/
    volatile u32  MEM_INDIR_ACC_CTRL;   /*indirected access control*/
    volatile u32  RxBADDR;          /*Receive buffer base address*/
    volatile u32  TxBADDR;          /*Transmit Buffer base address*/
    volatile u32  DMAEN;            /*DMA enable*/
    volatile u32  BDINT;            /*Buffer DMA interrupt*/
    volatile u32  BDIE;         /*Buffer DMA interrupt enable*/
} gmac_dmal_reg;

#ifdef __cplusplus
    }
#endif

#endif //#if defined(CONFIG_IPMUX) || defined(CONFIG_IPMUX_A)

#endif /* __gmac_H__ */


