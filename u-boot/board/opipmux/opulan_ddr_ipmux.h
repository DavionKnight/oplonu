#ifdef DDR2_UP_1G
/*====================== UP 1G======================*/
#ifndef CONFIG_IPMUX_E2_C
	#ifdef CONFIG_IPMUX_E2_10G
		#define DENALI_CTL_00_DATA_UP 0x01000000
		#define DENALI_CTL_01_DATA_UP 0x01000001
		#define DENALI_CTL_02_DATA_UP 0x00000101
		#define DENALI_CTL_03_DATA_UP 0x01000000
		
		#define DENALI_CTL_05_DATA_UP 0x00000101
		#define DENALI_CTL_06_DATA_UP 0x01020000
		#define DENALI_CTL_07_DATA_UP 0x04030402
		
		#define DENALI_CTL_08_DATA_UP 0x03040302
		#define DENALI_CTL_09_DATA_UP 0x0A030404
		#define DENALI_CTL_10_DATA_UP 0x00020707
		#define DENALI_CTL_11_DATA_UP 0x05080000
		
		#define DENALI_CTL_12_DATA_UP 0x00000000
		#define DENALI_CTL_13_DATA_UP 0x03100210
		#define DENALI_CTL_14_DATA_UP 0x1D1D0C00
		#define DENALI_CTL_15_DATA_UP 0x100f0404
		
		#define DENALI_CTL_16_DATA_UP 0x6a0c5f00 
		#define DENALI_CTL_17_DATA_UP 0x0032040f
		#define DENALI_CTL_18_DATA_UP 0x000007ff
		#define DENALI_CTL_19_DATA_UP 0x00000000
		
		#define DENALI_CTL_20_DATA_UP 0x445000C8
		#define DENALI_CTL_21_DATA_UP 0x00Cf0025
		#define DENALI_CTL_22_DATA_UP 0x00000000
		#define DENALI_CTL_23_DATA_UP 0x00000035
		
		#define DENALI_CTL_24_DATA_UP 0x00000000
	#else
	    /*1G UP*/
	    #define DENALI_CTL_00_DATA_UP 0x01000000
		#define DENALI_CTL_01_DATA_UP 0x01000001
		#define DENALI_CTL_02_DATA_UP 0x00000101
		#define DENALI_CTL_03_DATA_UP 0x01000000
		#define DENALI_CTL_05_DATA_UP 0x00000101
		#define DENALI_CTL_06_DATA_UP 0x01020000
		#define DENALI_CTL_07_DATA_UP 0x03030402
		#define DENALI_CTL_08_DATA_UP 0x02030202
		#define DENALI_CTL_09_DATA_UP 0x0A030204
		#define DENALI_CTL_10_DATA_UP 0x00020707
		#define DENALI_CTL_11_DATA_UP 0x04080000
		#define DENALI_CTL_12_DATA_UP 0x00000000
		#define DENALI_CTL_13_DATA_UP 0x030F020E
		#define DENALI_CTL_17_DATA_UP 0x0022040B
		#define DENALI_CTL_18_DATA_UP 0x00000694
		#define DENALI_CTL_19_DATA_UP 0x00000000
		#define DENALI_CTL_20_DATA_UP 0x3B0C00C8
		#define DENALI_CTL_21_DATA_UP 0x00C80025
		#define DENALI_CTL_22_DATA_UP 0x00000000
		#define DENALI_CTL_23_DATA_UP 0x00000006
		#define DENALI_CTL_24_DATA_UP 0x00000000
	
		#define DENALI_CTL_14_DATA_UP 0x0D0D0C00
		/*南亚0x10680404 0x6a0c2c00*/
		//#define           DENALI_CTL_15_DATA_UP 0x100F0404 /*0_0010000 dqs_out_shift_bypass 0_1111111 dqs_out_shift 0_0000100 dll_dqs_delay_bypass_1 0_0000100 dll_dqs_delay_bypass_0 */
		//#define           DENALI_CTL_16_DATA_UP 0x6A0C5F00 /*01101010 tcpd 0_0001100 wr_dqs_shift_bypass 0_1011111 wr_dqs_shift 0_0000000 out_of_range_length */
		//#define DENALI_CTL_15_DATA_UP 0x10680404 /*0_0010000 dqs_out_shift_bypass 0_1111111 dqs_out_shift 0_0000100 dll_dqs_delay_bypass_1 0_0000100 dll_dqs_delay_bypass_0 */
		#define DENALI_CTL_15_DATA_UP 0x107f0303
		#define DENALI_CTL_16_DATA_UP 0x6a0c2c00 /*01101010 tcpd 0_0001100 wr_dqs_shift_bypass 0_1011111 wr_dqs_shift 0_0000000 out_of_range_length */
	#endif
#else
/*
    #define DENALI_CTL_00_DATA_UP 0x01000000
    #define DENALI_CTL_01_DATA_UP 0x01000001
    #define DENALI_CTL_02_DATA_UP 0x00000101
    #define DENALI_CTL_03_DATA_UP 0x01000000
    #define DENALI_CTL_05_DATA_UP 0x00000101
    #define DENALI_CTL_06_DATA_UP 0x01020000
    #define DENALI_CTL_07_DATA_UP 0x04030402  
    #define DENALI_CTL_08_DATA_UP 0x03040302
    #define DENALI_CTL_09_DATA_UP 0x0A030404
    #define DENALI_CTL_10_DATA_UP 0x00020707
    #define DENALI_CTL_11_DATA_UP 0x05080000
    #define DENALI_CTL_12_DATA_UP 0x00000000
    #define DENALI_CTL_13_DATA_UP 0x03100210
    #define DENALI_CTL_17_DATA_UP 0x0032040f
    #define DENALI_CTL_18_DATA_UP 0x000007ff
    #define DENALI_CTL_19_DATA_UP 0x00000000
    #define DENALI_CTL_20_DATA_UP 0x445000c8
    #define DENALI_CTL_21_DATA_UP 0x00Cf0025
    #define DENALI_CTL_22_DATA_UP 0x00000000
    #define DENALI_CTL_23_DATA_UP 0x00000035
    #define DENALI_CTL_24_DATA_UP 0x00000000   
    
    #define DENALI_CTL_14_DATA_UP 0x18180C00
    #define DENALI_CTL_15_DATA_UP 0x107f0303
    #define DENALI_CTL_16_DATA_UP 0x6a0c5f00
    #endif
    #define DENALI_CTL_14_DATA_UP 0x16160C00
    #define DENALI_CTL_15_DATA_UP 0x100f0404
    #define DENALI_CTL_16_DATA_UP 0x6a0c5f00
*/
#endif 

        
#else
    /*512M UP*/
#endif


#ifdef DDR2_DN_1G
    /*1G DN*/                                          
    #define DENALI_CTL_00_DATA_DN 0x01000000
    #define DENALI_CTL_01_DATA_DN 0x01000001
    #define DENALI_CTL_02_DATA_DN 0x00000101
    #define DENALI_CTL_03_DATA_DN 0x01000000
    #define DENALI_CTL_05_DATA_DN 0x00000101
    #define DENALI_CTL_06_DATA_DN 0x01020000
    #define DENALI_CTL_07_DATA_DN 0x03030402
    #define DENALI_CTL_08_DATA_DN 0x02030202
    #define DENALI_CTL_09_DATA_DN 0x0A030204
    #define DENALI_CTL_10_DATA_DN 0x00020707
    #define DENALI_CTL_11_DATA_DN 0x04080000
    #define DENALI_CTL_12_DATA_DN 0x00000000
    #define DENALI_CTL_13_DATA_DN 0x030F020E
    #define DENALI_CTL_17_DATA_DN 0x0022040B
    #define DENALI_CTL_18_DATA_DN 0x00000694
    #define DENALI_CTL_19_DATA_DN 0x00000000
    #define DENALI_CTL_20_DATA_DN 0x3B0C00C8
    #define DENALI_CTL_21_DATA_DN 0x00C80025
    #define DENALI_CTL_22_DATA_DN 0x00000000
    #define DENALI_CTL_23_DATA_DN 0x00000006
    #define DENALI_CTL_24_DATA_DN 0x00000000

    #if 0
    #define DENALI_CTL_14_DATA_DN 0x0D0D0C00


  /*南亚0x10680404 0x6a0c2c00*/
        #define DENALI_CTL_15_DATA_DN 0x10680404
        #define DENALI_CTL_16_DATA_DN 0x6a0c2c00
    #endif
    #define DENALI_CTL_14_DATA_DN 0x16160C00
    #define DENALI_CTL_15_DATA_DN 0x100f0404
    #define DENALI_CTL_16_DATA_DN 0x6a0c5f00
#else
  /*512M DN*/   
   #define DENALI_CTL_00_DATA_DN 0x01000000
    #define DENALI_CTL_01_DATA_DN 0x01000001
    #define DENALI_CTL_02_DATA_DN 0x00000001
    #define DENALI_CTL_03_DATA_DN 0x01000000
    #define DENALI_CTL_05_DATA_DN 0x00000101
    #define DENALI_CTL_06_DATA_DN 0x01020000
    #define DENALI_CTL_07_DATA_DN 0x03030402
    #define DENALI_CTL_08_DATA_DN 0x02030202
    #define DENALI_CTL_09_DATA_DN 0x0A030204
    #define DENALI_CTL_10_DATA_DN 0x00020707
    #define DENALI_CTL_11_DATA_DN 0x04080000
    #define DENALI_CTL_12_DATA_DN 0x00000000
    #define DENALI_CTL_13_DATA_DN 0x0310020E
    #define DENALI_CTL_17_DATA_DN 0x001C040C
    #define DENALI_CTL_18_DATA_DN 0x00000694
    #define DENALI_CTL_19_DATA_DN 0x00000000
    #define DENALI_CTL_20_DATA_DN 0x3B0C00C8
    #define DENALI_CTL_21_DATA_DN 0x00C8001F
    #define DENALI_CTL_22_DATA_DN 0x00000000
    #define DENALI_CTL_23_DATA_DN 0x00000035
    #define DENALI_CTL_24_DATA_DN 0x00000000

    #define DENALI_CTL_14_DATA_DN 0x0D0D0C00
    #define DENALI_CTL_15_DATA_DN 0x10680404
    #define DENALI_CTL_16_DATA_DN 0x6a0c2c00
#endif
#ifdef CONFIG_IPMUX_E2_C   /*PARMER_FROM_DXD*/
#ifdef DDR2_UP_1G
#define               DENALI_CTL_00_DATA_UP 0x01000001 
#define               DENALI_CTL_01_DATA_UP 0x01000101 
#define               DENALI_CTL_02_DATA_UP 0x00000101 
#define               DENALI_CTL_03_DATA_UP 0x01000100 
#define               DENALI_CTL_05_DATA_UP 0x00000101 
#define               DENALI_CTL_06_DATA_UP 0x01020000 
#define               DENALI_CTL_07_DATA_UP 0x03030402 
#define               DENALI_CTL_08_DATA_UP 0x02030202
#define               DENALI_CTL_09_DATA_UP 0x0a030204 
#define               DENALI_CTL_10_DATA_UP 0x00020707 
#define               DENALI_CTL_11_DATA_UP 0x04080f0d 
#define               DENALI_CTL_12_DATA_UP 0x00001400
#define               DENALI_CTL_13_DATA_UP 0x030f020e 
#define               DENALI_CTL_17_DATA_UP 0x00220f0b 
#define               DENALI_CTL_18_DATA_UP 0x00000694 
#define               DENALI_CTL_19_DATA_UP 0x00000000 
#define               DENALI_CTL_20_DATA_UP 0x3b0c00c8
#define               DENALI_CTL_21_DATA_UP 0x00c80025 
#define               DENALI_CTL_22_DATA_UP 0x00002040 
#define               DENALI_CTL_23_DATA_UP 0x00000006 
#define               DENALI_CTL_24_DATA_UP 0x00000000 
#define               DENALI_CTL_14_DATA_UP 0x00000c00 
#ifdef CONFIG_IPMUX_E2
#define               DENALI_CTL_15_DATA_UP 0x10080404 
#else
#define               DENALI_CTL_15_DATA_UP 0x107f0404 
#endif
#define               DENALI_CTL_16_DATA_UP 0x6a0c5800 
#else
#define               DENALI_CTL_00_DATA 0x01000000 
#define               DENALI_CTL_01_DATA 0x01000001 
#define               DENALI_CTL_02_DATA 0x00000001 
#define               DENALI_CTL_03_DATA 0x01000000 
#define               DENALI_CTL_05_DATA 0x00000101
#define               DENALI_CTL_06_DATA 0x01010000 
#define               DENALI_CTL_07_DATA 0x04030402  
#define               DENALI_CTL_08_DATA 0x03040302  
#define               DENALI_CTL_09_DATA 0x0a030404 
#define               DENALI_CTL_10_DATA 0x00020707 
#define               DENALI_CTL_11_DATA 0x05080000
#define               DENALI_CTL_12_DATA 0x00000000 
#define               DENALI_CTL_13_DATA 0x03100210
#define               DENALI_CTL_14_DATA 0x00000c00 
#ifdef CONFIG_IPMUX_E2
#define               DENALI_CTL_15_DATA 0x100f0303 
#else
#define               DENALI_CTL_15_DATA 0x107f0303 
#endif
#define               DENALI_CTL_16_DATA 0x6a0c5f00 
#define               DENALI_CTL_17_DATA 0x0032040f  
#define               DENALI_CTL_18_DATA 0x000007ff 
#define               DENALI_CTL_19_DATA 0x00000000 
#define               DENALI_CTL_20_DATA 0x445000c8 
#define               DENALI_CTL_21_DATA 0x00cf0025 
#define               DENALI_CTL_22_DATA 0x00000000 
#define               DENALI_CTL_23_DATA 0x00000035
#define               DENALI_CTL_24_DATA 0x00000000 
#endif
#endif
