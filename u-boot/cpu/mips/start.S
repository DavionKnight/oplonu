/*
 *  Startup Code for MIPS32 CPU-core
 *
 *  Copyright (c) 2003  Wolfgang Denk <wd@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <config.h>
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <configs/opipmux.h>



#define RVECENT(f,n) \
   b f; nop
#define XVECENT(f,bev) \
   b f     ;           \
   li k0,bev

        .set noreorder

        .globl _start
        .text
_start: 
        RVECENT(reset,0)        /* U-boot entry point */
        RVECENT(reset,1)        /* software reboot */
#if defined(CONFIG_INCA_IP)
        .word INFINEON_EBU_BOOTCFG /* EBU init code, fetched during booting */
        .word 0x00000000           /* phase of the flash                    */
#elif defined(CONFIG_PURPLE)
        .word INFINEON_EBU_BOOTCFG /* EBU init code, fetched during booting */
        .word INFINEON_EBU_BOOTCFG /* EBU init code, fetched during booting */
#else
        RVECENT(romReserved,2)
#endif
        RVECENT(romReserved,3)
        RVECENT(romReserved,4)
        RVECENT(romReserved,5)
        RVECENT(romReserved,6)
        RVECENT(romReserved,7)
        RVECENT(romReserved,8)
        RVECENT(romReserved,9)
        RVECENT(romReserved,10)
        RVECENT(romReserved,11)
        RVECENT(romReserved,12)
        RVECENT(romReserved,13)
        RVECENT(romReserved,14)
        RVECENT(romReserved,15)
        RVECENT(romReserved,16)
        RVECENT(romReserved,17)
        RVECENT(romReserved,18)
        RVECENT(romReserved,19)
        RVECENT(romReserved,20)
        RVECENT(romReserved,21)
        RVECENT(romReserved,22)
        RVECENT(romReserved,23)
        RVECENT(romReserved,24)
        RVECENT(romReserved,25)
        RVECENT(romReserved,26)
        RVECENT(romReserved,27)
        RVECENT(romReserved,28)
        RVECENT(romReserved,29)
        RVECENT(romReserved,30)
        RVECENT(romReserved,31)
        RVECENT(romReserved,32)
        RVECENT(romReserved,33)
        RVECENT(romReserved,34)
        RVECENT(romReserved,35)
        RVECENT(romReserved,36)
        RVECENT(romReserved,37)
        RVECENT(romReserved,38)
        RVECENT(romReserved,39)
        RVECENT(romReserved,40)
        RVECENT(romReserved,41)
        RVECENT(romReserved,42)
        RVECENT(romReserved,43)
        RVECENT(romReserved,44)
        RVECENT(romReserved,45)
        RVECENT(romReserved,46)
        RVECENT(romReserved,47)
        RVECENT(romReserved,48)
        RVECENT(romReserved,49)
        RVECENT(romReserved,50)
        RVECENT(romReserved,51)
        RVECENT(romReserved,52)
        RVECENT(romReserved,53)
        RVECENT(romReserved,54)
        RVECENT(romReserved,55)
        RVECENT(romReserved,56)
        RVECENT(romReserved,57)
        RVECENT(romReserved,58)
        RVECENT(romReserved,59)
        RVECENT(romReserved,60)
        RVECENT(romReserved,61)
        RVECENT(romReserved,62)
        RVECENT(romReserved,63)
        XVECENT(romExcHandle,0x200)     /* bfc00200: R4000 tlbmiss vector */
        RVECENT(romReserved,65)
        RVECENT(romReserved,66)
        RVECENT(romReserved,67)
        RVECENT(romReserved,68)
        RVECENT(romReserved,69)
        RVECENT(romReserved,70)
        RVECENT(romReserved,71)
        RVECENT(romReserved,72)
        RVECENT(romReserved,73)
        RVECENT(romReserved,74)
        RVECENT(romReserved,75)
        RVECENT(romReserved,76)
        RVECENT(romReserved,77)
        RVECENT(romReserved,78)
        RVECENT(romReserved,79)
        XVECENT(romExcHandle,0x280)     /* bfc00280: R4000 xtlbmiss vector */
        RVECENT(romReserved,81)
        RVECENT(romReserved,82)
        RVECENT(romReserved,83)
        RVECENT(romReserved,84)
        RVECENT(romReserved,85)
        RVECENT(romReserved,86)
        RVECENT(romReserved,87)
        RVECENT(romReserved,88)
        RVECENT(romReserved,89)
        RVECENT(romReserved,90)
        RVECENT(romReserved,91)
        RVECENT(romReserved,92)
        RVECENT(romReserved,93)
        RVECENT(romReserved,94)
        RVECENT(romReserved,95)
        XVECENT(romExcHandle,0x300)     /* bfc00300: R4000 cache vector */
        RVECENT(romReserved,97)
        RVECENT(romReserved,98)
        RVECENT(romReserved,99)
        RVECENT(romReserved,100)
        RVECENT(romReserved,101)
        RVECENT(romReserved,102)
        RVECENT(romReserved,103)
        RVECENT(romReserved,104)
        RVECENT(romReserved,105)
        RVECENT(romReserved,106)
        RVECENT(romReserved,107)
        RVECENT(romReserved,108)
        RVECENT(romReserved,109)
        RVECENT(romReserved,110)
        RVECENT(romReserved,111)
        XVECENT(romExcHandle,0x380)     /* bfc00380: R4000 general vector */
        RVECENT(romReserved,113)
        RVECENT(romReserved,114)
        RVECENT(romReserved,115)
        RVECENT(romReserved,116)
        RVECENT(romReserved,116)
        RVECENT(romReserved,118)
        RVECENT(romReserved,119)
        RVECENT(romReserved,120)
        RVECENT(romReserved,121)
        RVECENT(romReserved,122)
        RVECENT(romReserved,123)
        RVECENT(romReserved,124)
        RVECENT(romReserved,125)
        RVECENT(romReserved,126)
        RVECENT(romReserved,127)

        /* We hope there are no more reserved vectors!
         * 128 * 8 == 1024 == 0x400
         * so this is address R_VEC+0x400 == 0xbfc00400
         */
#ifdef CONFIG_PURPLE
/* 0xbfc00400 */
        .word   0xdc870000
        .word   0xfca70000
        .word   0x20840008
        .word   0x20a50008
        .word   0x20c6ffff
        .word   0x14c0fffa
        .word   0x00000000
        .word   0x03e00008
        .word   0x00000000
        .word   0x00000000
/* 0xbfc00428 */
        .word   0xdc870000
        .word   0xfca70000
        .word   0x20840008
        .word   0x20a50008
        .word   0x20c6ffff
        .word   0x14c0fffa
        .word   0x00000000
        .word   0x03e00008
        .word   0x00000000
        .word   0x00000000
#endif /* CONFIG_PURPLE */
        .align 4
reset:
	/* init chip PLL config.*/
#ifndef DEBUGMODE
#ifdef CONFIG_PLL_PARA
       #define SYS_PLL_REG  0xbf000214
       #define CPU_PLL_REG  0xbf000218
       #define MAB_PLL_REG  0xbf00021c
       #define PLL_CTRL_REG 0xbf000224

     /*check whether the pll was configed */
	   la t0, PLL_CTRL_REG
	   lw t1, 0(t0)
	   li t2, 0x80000000
	   and t1, t1, t2
	   beq t1, t2, 11f

	 /*config sys clk */
       la t0, SYS_PLL_REG
       li t1, 0x2BB03   /*275M*/
 //      li t1, 0x27B03   /*250M*/   
 //    li t1, 0x1FB03   /*200M*/
//     li t1, 0x1BB03   /*175M*/
//     li t1, 0x21303   /*209.375M*/
 //    li t1, 0x23B03   /*225M*/
 //    li t1, 0x29303   /*259.375M*/
//       li t1, 0x26b03   /*243.75M*/
//       li t1, 0x26303   /*240.625M*/
//       li t1, 0x24B03   /*231.25M*/
//       li t1, 0x22B03   /*218.75M*/
       sw t1, 0(t0)
#if defined(CONFIG_10G_IPMUX_C3) || defined(CONFIG_10G_IPMUX_C3_NJZTE) || defined(CONFIG_10G_IPMUX_T3) 
     /*config cpu clk 300M */
	   la t0, CPU_PLL_REG
       //li t1, 0x2FA03
       li t1, 0x17a03  /**300m clk**/
//	   li t1, 0x17b03    /**150m clk**/
//	   li t1, 0x1fd03    /**50m clk**/
//	   li t1, 0x19403    /**80m clk**/
       sw t1, 0(t0)

	 /*config mab clk  */
       la t0, MAB_PLL_REG
    //   li t1, 0x29B03    /**1066m clk**/
       li t1, 0x1FB03    /**800m clk**/
#elif defined(CONFIG_10G_IPMUX_C2)
     /*config cpu clk 600M */
	   la t0, CPU_PLL_REG   
       li t1, 0x17A03   /*300M*/
//	   li t1, 0x1FC03   /*100M*/
       sw t1, 0(t0)
       
	 /*config mab clk 200M */
       la t0, MAB_PLL_REG 
// #ifdef 800M_DDR
       li t1, 0x1fb03     /*200M*/
// #else
//       li t1, 0x1a303     /*166M*/
// #endif

#endif

       sw t1, 0(t0)

	 /*enable pll software config*/
       la t0, PLL_CTRL_REG
       li t1, 0x1
       sw t1, 0(t0)

11:
       nop;nop;nop;nop;nop;nop;nop;nop
	   la t0, PLL_CTRL_REG  
	   lw t1, 0(t0)
	   li t2, 0x00000e00
	   and t1, t1, t2
	   bne t1, t2, 11b
 #endif
#endif

        /* Clear watch registers. */
        mtc0    zero, CP0_WATCHLO
        mtc0    zero, CP0_WATCHHI
#if defined(CONFIG_IPMUX_ONU)||defined(CONFIG_ONU_2)
        chipInit;
        linklinkEN;
#endif
        inituart38400 ;

        /* STATUS register */
#ifdef  CONFIG_TB0229
        li      k0, ST0_CU0
#else
        mfc0    k0, CP0_STATUS
#endif
        li      k1, ~ST0_IE
        and     k0, k1
        mtc0    k0, CP0_STATUS

        /*disable all interrupte mask, addbywx*/
        //la k0,0x10400006
        //mtc0  k0, CP0_STATUS

        /* CAUSE register */
        mtc0    zero, CP0_CAUSE

        /* Init Timer */
        mtc0    zero, CP0_COUNT
        mtc0    zero, CP0_COMPARE

        /* CONFIG0 register */
        li      t0, CONF_CM_UNCACHED
        mtc0    t0, CP0_CONFIG

#ifdef CONFIG_INCA_IP
        /* Disable INCA-IP Watchdog.
         */
        bal     disable_incaip_wdt
        nop
#endif
#if 0   /* don't do memory init in FPGA board */
        /* Initialize any external memory.
         */
        bal     lowlevel_init
        nop
#endif
//#if 0   /***add by zw in 1217**/
/*kxu 20110511*/
#if defined(FPGA_VERSION)
    la t0, 0xbf0000c0
    la t1, 0x00001002
    sw t1, 0(t0)
#endif
#ifndef DEBUGMODE
  #ifdef CONFIG_TUNE_DDR
        /* tune ddr */
        printl('d');printl('d');printl('r');printl(' ');printl('i');printl('n');printl('i');printl('t');printl('\n');
        /*tune up ddr*/
   #if  defined(CONFIG_10G_IPMUX_C2) 
	   /* tune ddr2 */
	   printl('d');printl('d');printl('r');printl('2');printl('i');printl('n');printl('i');printl('t');printl('\n');
	   
        bal optrann_ddr2_tune
        
   #elif defined(CONFIG_10G_IPMUX_C3)||defined(CONFIG_10G_IPMUX_C3_NJZTE)|| defined(CONFIG_10G_IPMUX_T3) 
	   /* tune ddr3 */
	   printl('d');printl('d');printl('r');printl('3');printl('i');printl('n');printl('i');printl('t');printl('\n');
	   
   //bal optrann_ddr3_tune16bit
        bal optrann_ddr3_tune
        
   #else
        bal ipmux_ddr_tune
   #endif
        nop
  #endif

#endif
/* begin added by jiangmingli */
#ifdef CONFIG_IPMUX_ONU_DDR_TUNE
		bal onu_ddr_tune
		nop
#endif
        /* end added by jiangmingli */
        printl('1');
        /* Initialize caches...
         */
#ifdef ENABLE_CACHE
        bal   sys_init_cache
        //bal   mips_cache_reset
        nop
#endif
        printl('2');
        /* ... and enable them.
         */
        li      t0, CONF_CM_CACHABLE_NONCOHERENT
#ifndef ENABLE_CACHE
        li      t0, CONF_CM_UNCACHED
#endif
        mtc0    t0, CP0_CONFIG

#ifdef TEST_MEM_IN_EARLY_INIT
        la s4,0
//      bal optrann_test_reg_acc
        nop
        bal sdram_test_asm
        nop
#endif
        printl('3');

        /* Set up temporary stack.
         */
        li      a0, CFG_INIT_SP_OFFSET
#ifdef LOCK_CACHE_SP
        bal     mips_cache_lock
        nop
#endif
        li      t0, CFG_SDRAM_BASE + CFG_INIT_SP_OFFSET
        la      sp, 0(t0)

        printl('4');
        /* Initialize GOT pointer.
         */
        bal     1f
        nop
        .word   _GLOBAL_OFFSET_TABLE_ - 1f + 4
1:

        printl('f');
        move    gp, ra
        lw      t1, 0(ra)
        add     gp, t1
        la      t9, board_init_f
        j       t9
        nop
.globl  copy_relocate_code_to_ram
.ent    copy_relocate_code_to_ram

copy_relocate_code_to_ram:

 //   addr&=0xFFFFFFF;
 //   addr|=0xa0000000;
//andi	t1,a2,0x0FFFFFFF
//ori	t1,t1,0xa0000000
        printl('r');

lui	v0,0xfff
ori	v0,v0,0xffff
and	t1,a2,v0
lui	v0,0xa000
or	t1,t1,v0

la      t0, relocate_code
// la      t3, in_ram
la      t2, in_ram-4
//      lw      t2, -12(t3)     /* t2 <-- uboot_end_data        */
addu    t1, t1,-200
move    t4, t1
/*
 * t0 = source address
 * t1 = target address
 * t2 = source end address
 */
/* On the purple board we copy the code earlier in a special way
 * in order to solve flash problems
 */
#ifndef DEBUGMODE
1:
lw      t3, 0(t0)
sw      t3, 0(t1)
addu    t0, 4
ble     t0, t2, 1b
addu    t1, 4                   /* delay slot */

jr      t4
nop
#endif

jr      t0
nop

//        .end   copy_relocate_code_to_ram


/*
 * void relocate_code (addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 * a0 = addr_sp
 * a1 = gd
 * a2 = destination address
 */
.globl  relocate_code
.ent    relocate_code
relocate_code:
/* unlock cache */
#if UNLOCK_CACHE_SP
li      a0, CFG_INIT_SP_OFFSET
bal     mips_cache_unlock
nop
#endif
printl('0')
move    sp, a0          /* Set new stack pointer                */

printl('1')

li      t0, CFG_MONITOR_BASE
la      t3, in_ram
lw      t2, -12(t3)     /* t2 <-- uboot_end_data        */
move    t1, a2

printl('2')
/*
 * Fix GOT pointer:
 *
 * New GOT-PTR = (old GOT-PTR - CFG_MONITOR_BASE) + Destination Address
 */
move    t6, gp
sub     gp, CFG_MONITOR_BASE
add     gp, a2                  /* gp now adjusted              */
sub     t6, gp, t6              /* t6 <-- relocation offset     */

printl('3')
/*
 * t0 = source address
 * t1 = target address
 * t2 = source end address
 */
/* On the purple board we copy the code earlier in a special way
 * in order to solve flash problems
 */
#ifndef DEBUGMODE
#ifndef CONFIG_PURPLE
1:
lw      t3, 0(t0)
sw      t3, 0(t1)
        addu    t0, 4
//ble     t0, t2, 1b
#ifndef NO_CP_RELO_CODE_TO_RAM
beq     t0, t2, jump_in_ram
#else
ble     t0, t2, 1b
#endif
addu    t1, 4                   /* delay slot                   */
#ifndef NO_CP_RELO_CODE_TO_RAM
/*kxu*/
addiu	t5,t4,0x30  /*relocate_code+0x30 */ /*if jump 1: , the code will run back to Flash*/
jr      t5
nop
#endif
#endif
#endif
printl('4')
/* If caches were enabled, we would have to flush them here.
 */
 
#ifndef NO_CP_RELO_CODE_TO_RAM
.globl  jump_in_ram
.ent    jump_in_ram
jump_in_ram:
#endif
/* Jump to where we've relocated ourselves.
 */
addi    t0, a2, in_ram - _start
j       t0
nop
#ifndef NO_CP_RELO_CODE_TO_RAM
.end    jump_in_ram
#endif

        .word   uboot_end_data
	.word   uboot_end
	.word   num_got_entries

in_ram:
        printl('5')
        /* Now we want to update GOT.
         */
        lw      t3, -4(t0)      /* t3 <-- num_got_entries       */
        addi    t4, gp, 8       /* Skipping first two entries.  */
        li      t2, 2
1:
        lw      t1, 0(t4)
        beqz    t1, 2f
        add     t1, t6
        sw      t1, 0(t4)
2:
        addi    t2, 1
        blt     t2, t3, 1b
        addi    t4, 4           /* delay slot                   */

        printl('6')
        /* Clear BSS.
         */
        lw      t1, -12(t0)     /* t1 <-- uboot_end_data        */
        lw      t2, -8(t0)      /* t2 <-- uboot_end             */
        add     t1, t6          /* adjust pointers              */
        add     t2, t6

        sub     t1, 4
1:      addi    t1, 4
        bltl    t1, t2, 1b
        sw      zero, 0(t1)     /* delay slot                   */

        printl('r')
        printl('\n')

        move    a0, a1
        la      t9, board_init_r
        j       t9
        move    a1, a2          /*      delay slot                   */

        .end    relocate_code

        /* Exception handlers.
         */
romReserved:
        b romReserved

romExcHandle:
        b romExcHandle
