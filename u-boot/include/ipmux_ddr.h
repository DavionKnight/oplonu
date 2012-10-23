/*
=============================================================================
     Header Name: ipmux_ddr.h

     General Description:
IPMux DDR Controller module head file.
===============================================================================
                         Opulan Confidential Proprietary
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved


Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 yzhang            08/21/06      Initial Version
 xwang             11/21/06      tune the parameter values of ddr controls 
----------------   ------------  ----------------------------------------------
*/
#ifndef DDRC_CONFIG_INCLUDE
#define DDRC_CONFIG_INCLUDE

/* includes */
/* choose the DDR Memory type : 1G or 512M */

/*
#define DDR2_1G 
*/

/* #define LOG_DDR_CONFIG_DELAY_VAL */

#ifdef LOG_DDR_CONFIG_DELAY_VAL
#define DDR_CONFIG_DELAY_MIN     0xa1000010
#define DDR_CONFIG_DELAY_MAX     0xa1000014
#define DDR_CONFIG_DELAY_AVERAGE 0xa1000018
#endif        

/* ddr tune config */           
#define DDRTUNE_ADDR_TEST_REST 0xa0000004
#define DDRTUNE_VAL_TEST_REST 0xFFFFFFFF

#define DDRTUNE_ADDR_TEST 0xa0000000
#define DDRTUNE_VAL_TEST 0xaaaaaaaa

#define DDRTUNE_ADDR_TEST1 0xa0000004
#define DDRTUNE_VAL_TEST1 0x55555555

#define DDR_CONFIG_START_UP 0xbf004914
#define DDR_CONFIG_START_DN 0xbf004950

/* Registers address */
//#ifndef IPMUX_REG_BASE_DDR
#define IPMUX_REG_BASE_DDR  0xbf004800
//#endif

/* Denali Control registers */
#define REG_DDR_UP_CTLX(x)       (IPMUX_REG_BASE_DDR + (x) * 4)
#define REG_DDR_DN_CTLX(x)       (IPMUX_REG_BASE_DDR + 0x80 + (x) * 4)

/* DDR Upstream Arbitration registers */
#define REG_DDRUPAE                 (IPMUX_REG_BASE_DDR + 0x100)
#define REG_DDRUPACFG               (IPMUX_REG_BASE_DDR + 0x104)

/* DDR Downstream Arbitration registers */
#define REG_DDRDNAE                 (IPMUX_REG_BASE_DDR + 0x140)
#define REG_DDRDNACFG               (IPMUX_REG_BASE_DDR + 0x144)

#define IPMUX_DDR_MAX_DELAY_VAL 0x7F
#define IPMUX_DDR_DELAY0_SHIFT 24
#define IPMUX_DDR_DELAY1_SHIFT 16

/* DDR2 controller register config default vaule */
#ifdef DDR2_1G
#define               DENALI_CTL_00_DATA 0x01000000 /*0000000_1 concurrentap 0000000_0 auto_refresh_mode 0000000_0 arefresh 0000000_0 ap */
#define               DENALI_CTL_01_DATA 0x01000001 /*0000000_1 dqs_n_en 0000000_0 dll_bypass_mode 0000000_0 dlllockreg 0000000_1 ddrii_sdram_mode */
#define               DENALI_CTL_02_DATA 0x00000101 /*0000000_0 intrptreada 0000000_0 intrptapburst 0000000_1 eight_bank_mode 0000000_1 drive_dq_dqs */
#define               DENALI_CTL_03_DATA 0x01000000 /*0000000_1 odt_add_turn_clk_en 0000000_0 no_cmd_init 0000000_0 max_cs_reg 0000000_0 intrptwritea */
#define               DENALI_CTL_04_DATA 0x00000000 /*0000000_0 start 0000000_0 srefresh 0000000_0 reg_dimm_enable 0000000_0 power_down */
#define               DENALI_START_DATA  0xff000000 /* 1111111_1 start, after initialization, these bits should be set to start DDRC */
#define               DENALI_CTL_05_DATA 0x00000101 /*000000_00 fast_write 0000000_0 write_modereg 0000000_1 writeinterp 0000000_1 tras_lockout */
#define               DENALI_CTL_06_DATA 0x01020000 /*000000_01 rtt_pad_termination 000000_10 rtt_0 000000_00 q_fullness 000000_00 out_of_range_type */
#define               DENALI_CTL_07_DATA 0x03030402 /*00000_011 tcke 00000_011 column_size 00000_100 caslat 00000_010 addr_pins */
#define               DENALI_CTL_08_DATA 0x02030202 /*00000_010 trtp 00000_011 trrd 00000_010 tpdex 00000_010 temrs */
#define               DENALI_CTL_09_DATA 0x0A030204 /*0000_1010 aprebit 00000_011 wrlat 00000_010 twtr 00000_100 twr_int */
#define               DENALI_CTL_10_DATA 0x00020707 /*0000_0000 int_ack 0000_0010 initaref 0000_1000 caslat_lin_gate 0000_0111 caslat_lin */
#define               DENALI_CTL_11_DATA 0x04080000 /*0000_0100 trp 0000_1000 tdal 0000_0000 max_row_reg 0000_0000 max_col_reg */
#define               DENALI_CTL_12_DATA 0x00000000 /*000_00000 ocd_adjust_pup_cs_0 000_00000 ocd_adjust_pdn_cs_0 000_00000 int_status 000_00000 int_mask */
#define               DENALI_CTL_13_DATA 0x030F020E /*00_000011 dll_increment 000_01111 trc 000_00010 tmrd 000_01110 tfaw */
#define               DENALI_CTL_14_DATA 0x1D1D0C00 /*0_0011101 dll_dqs_delay_1 0_0011101 dll_dqs_delay_0 00_001100 dll_start_point 00_000000 dll_lock */
#define               DENALI_CTL_15_DATA 0x100F0404 /*0_0010000 dqs_out_shift_bypass 0_1111111 dqs_out_shift 0_0000100 dll_dqs_delay_bypass_1 0_0000100 dll_dqs_delay_bypass_0 */
#define               DENALI_CTL_16_DATA 0x6A0C5F00 /*01101010 tcpd 0_0001100 wr_dqs_shift_bypass 0_1011111 wr_dqs_shift 0_0000000 out_of_range_length */
#define               DENALI_CTL_17_DATA 0x0022040B /*0000000000100010 trfc 00000100 trcd_int 00001011 tras_min */
#define               DENALI_CTL_18_DATA 0x000007FF /*0_000000000000000 emrs2_data 00_00011111111111 tref */
#define               DENALI_CTL_19_DATA 0x00000000 /*0_000000000000000 emrs_data 0_000000000000000 emrs3_data */
#define               DENALI_CTL_20_DATA 0x47EC00C8 /*0100011111101100 tras_max 0000000011001000 tdll */
#define               DENALI_CTL_21_DATA 0x00C80025 /*0000000011001000 txsr 0000000000100101 txsnr */
#define               DENALI_CTL_22_DATA 0x00000000 /*00000000000000000000000000000000 version */
#define               DENALI_CTL_23_DATA 0x00000006 /*00000000000000000000000000000110 tinit */
#define               DENALI_CTL_24_DATA 0x00000000 /*00000000000000000000000000000000 out_of_range_addr */
#else                                               
#define               DENALI_CTL_00_DATA 0x01000000 /*0000000_1 concurrentap 0000000_0 auto_refresh_mode 0000000_0 arefresh 0000000_0 ap */
#define               DENALI_CTL_01_DATA 0x01000001 /*0000000_1 dqs_n_en 0000000_0 dll_bypass_mode 0000000_0 dlllockreg 0000000_1 ddrii_sdram_mode */
#define               DENALI_CTL_02_DATA 0x00000001 /*0000000_0 intrptreada 0000000_0 intrptapburst 0000000_0 eight_bank_mode 0000000_1 drive_dq_dqs */
#define               DENALI_CTL_03_DATA 0x01000000 /*0000000_1 odt_add_turn_clk_en 0000000_0 no_cmd_init 0000000_0 max_cs_reg 0000000_0 intrptwritea */
#define               DENALI_CTL_04_DATA 0x00000000 /*0000000_0 start 0000000_0 srefresh 0000000_0 reg_dimm_enable 0000000_0 power_down */
#define               DENALI_START_DATA  0xff000000 /* 1111111_1 start, after initialization, these bits should be set to start DDRC */
#define               DENALI_CTL_05_DATA 0x00000101 /*000000_00 fast_write 0000000_0 write_modereg 0000000_1 writeinterp 0000000_1 tras_lockout */
#define               DENALI_CTL_06_DATA 0x01020000 /*000000_01 rtt_pad_termination 000000_10 rtt_0 000000_00 q_fullness 000000_00 out_of_range_type */
#define               DENALI_CTL_07_DATA 0x03030402 /*00000_011 tcke 00000_011 column_size 00000_100 caslat 00000_010 addr_pins */
#define               DENALI_CTL_08_DATA 0x02020202 /*00000_010 trtp 00000_011 trrd 00000_010 tpdex 00000_010 temrs */
#define               DENALI_CTL_09_DATA 0x0A030204 /*0000_1010 aprebit 00000_011 wrlat 00000_010 twtr 00000_100 twr_int */
#define               DENALI_CTL_10_DATA 0x00020707 /*0000_0000 int_ack 0000_0010 initaref 0000_1000 caslat_lin_gate 0000_0111 caslat_lin */
#define               DENALI_CTL_11_DATA 0x04080000 /*0000_0100 trp 0000_1000 tdal 0000_0000 max_row_reg 0000_0000 max_col_reg */
#define               DENALI_CTL_12_DATA 0x00000000 /*000_00000 ocd_adjust_pup_cs_0 000_00000 ocd_adjust_pdn_cs_0 000_00000 int_status 000_00000 int_mask */
#define               DENALI_CTL_13_DATA 0x03100200 /*00_000011 dll_increment 000_10000 trc 000_00010 tmrd 000_00000 tfaw */
#define               DENALI_CTL_14_DATA 0x05050C00 /*0_0011011 dll_dqs_delay_1 0_0011011 dll_dqs_delay_0 00_001100 dll_start_point 00_000000 dll_lock */
#define               DENALI_CTL_15_DATA 0x100f0303 /* 0_0010000 dqs_out_shift_bypass*/
#define               DENALI_CTL_16_DATA 0x6A2C5f00 /*01101010 tcpd 0_0001100 wr_dqs_shift_bypass 0_1011111 wr_dqs_shift 0_0000000 out_of_range_length */
#define               DENALI_CTL_17_DATA 0x001C040C /*0000000000011100 trfc 00000100 trcd_int 00001100 tras_min */
#define               DENALI_CTL_18_DATA 0x000007FF /*0_000000000000000 emrs2_data 00_00011111111111 tref */
#define               DENALI_CTL_19_DATA 0x00000000 /*0_000000000000000 emrs_data 0_000000000000000 emrs3_data */
#define               DENALI_CTL_20_DATA 0x47EC00C8 /*0100011111101100 tras_max 0000000011001000 tdll */
#define               DENALI_CTL_21_DATA 0x00C8001F /*0000000011001000 txsr 0000000000011111 txsnr */
#define               DENALI_CTL_22_DATA 0x00000000 /*00000000000000000000000000000000 version */
#define               DENALI_CTL_23_DATA 0x00000035 /*00000000000000000000000000110101 tinit */
#define               DENALI_CTL_24_DATA 0x00000000 /*00000000000000000000000000000000 out_of_range_addr */
#endif




#ifndef UINT32
#define UINT32  unsigned int
#endif

/* define a magic number for terminator */
#define DDRC_MAGIC_NUM  0x123123ee

/* initialization process for DDR Controler.
 * user should put this MACRO into the initialization process before the Memory is accessed
 *
 * please redefine MACROs UINT32,
 */
#define IPMUX_DDR_INIT      {\
                            int ni = 0; \
                            UINT32 denaliDefVal[] = {   DENALI_CTL_00_DATA, \
                                                        DENALI_CTL_01_DATA, \
                                                        DENALI_CTL_02_DATA, \
                                                        DENALI_CTL_03_DATA, \
                                                        DENALI_CTL_04_DATA, \
                                                        DENALI_CTL_05_DATA, \
                                                        DENALI_CTL_06_DATA, \
                                                        DENALI_CTL_07_DATA, \
                                                        DENALI_CTL_08_DATA, \
                                                        DENALI_CTL_09_DATA, \
                                                        DENALI_CTL_10_DATA, \
                                                        DENALI_CTL_11_DATA, \
                                                        DENALI_CTL_12_DATA, \
                                                        DENALI_CTL_13_DATA, \
                                                        DENALI_CTL_14_DATA, \
                                                        DENALI_CTL_15_DATA, \
                                                        DENALI_CTL_16_DATA, \
                                                        DENALI_CTL_17_DATA, \
                                                        DENALI_CTL_18_DATA, \
                                                        DENALI_CTL_19_DATA, \
                                                        DENALI_CTL_20_DATA, \
                                                        DENALI_CTL_21_DATA, \
                                                        DENALI_CTL_22_DATA, \
                                                        DENALI_CTL_23_DATA, \
                                                        DENALI_CTL_24_DATA, \
                                                        DDRC_MAGIC_NUM}; \
                            while (DDRC_MAGIC_NUM != denaliDefVal[ni]) {ipMuxRegWrite(REG_DDR_UP_CTLX(ni), denaliDefVal[ni]); ni++;}; \
                            ni = 0; \
                            while (DDRC_MAGIC_NUM != denaliDefVal[ni]) {ipMuxRegWrite(REG_DDR_DN_CTLX(ni), denaliDefVal[ni]); ni++;}; \
                            ipMuxRegWrite(REG_DDR_UP_CTLX(4), DENALI_START_DATA); /*start controller upstream */ \
                            ipMuxRegWrite(REG_DDR_DN_CTLX(4), DENALI_START_DATA); /*start controller downstream */ \
                            udelay(20000);/*delay for 20 ms*/ \
                            ipMuxRegWrite(REG_DDRUPAE, 0x1); /* enable upstream arbitration */ \
                            ipMuxRegWrite(REG_DDRDNAE, 0x1); /* enable downstream arbitration */ \
                    }

#endif /* DDRC_CONFIG_INCLUDE */

