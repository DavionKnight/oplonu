#include <asm/regdef.h>
#include <configs/opipmux.h>
/*-----------------------------------------------------------------

 Function:    sdram_test

 Description: test sdram memory 
              test mode : write the address as value to all the tested   
              memory, and read back to check it's right.
              input 
                s4 : 0 :  nommal mem test
                     1 :  ddr tune mem test  
              output
                s4 : the result.   1 : passed, 0 : failed.
------------------------------------------------------------------*/

	.globl	sdram_test_asm
	.ent	sdram_test_asm
sdram_test_asm:
 /*--------- write through the tested ram ---------------*/ 
 la s0,OPIPMUX_DRAM_TEST_START
	la s1,OPIPMUX_DRAM_TEST_END
                                           
 beqz s4,1f
 nop 
 
 la s0,OPIPMUX_DDRTUNE_TEST_START
	la s1,OPIPMUX_DDRTUNE_TEST_END

 
1:	sw s0,0(s0)	
	add s0,4
	bne s0,s1,1b
	nop 

 /*--------- compare ---------------*/
 la s0,OPIPMUX_DRAM_TEST_START
	la s1,OPIPMUX_DRAM_TEST_END	
	/*error address*/
	la s3,OPIPMUX_DRAM_TEST_END
 
 beqz s4,1f
 nop 
 
 la s0,OPIPMUX_DDRTUNE_TEST_START
	la s1,OPIPMUX_DDRTUNE_TEST_END
	/*error address*/
	la s3,OPIPMUX_DDRTUNE_TEST_END


1:/* s4 is the error indicator for ddr tune memory test*/
 la s4,0

1:	/*read current int value */
	lw s2,0(s0)
	/*store the current address*/
	sw s0,0(s3)
	
	/*compare address and value*/
	bne s0,s2,1f
	nop 
	
	add s0,4
	
	bne s0,s1,1b
	nop 

 /* indicate that the memory test passed */
 la s4,1
 
	/*jump to caller*/
1:	j	ra
	  nop

	.end	sdram_test_asm


	
/*-----------------------------------------------------------------

 Function:    sdram_test_down

 Description: test sdram memory 
              test mode : write the address as value to all the tested  
              memory, and read back to check it's right.
              input 
                s4 : 0 :  nommal mem test
                     1 :  ddr tune mem test  
              output
                s4 : the result.   1 : passed, 0 : failed.
------------------------------------------------------------------*/

	.globl	sdram_test_asm_down
	.ent	sdram_test_asm_down
sdram_test_asm_down:
 /*--------- write through the tested ram ---------------*/ 
 la s0,OPIPMUX_DRAM_TEST_START
	la s1,OPIPMUX_DRAM_TEST_END
                                           
 beqz s4,1f
 nop 
 
 la s0,OPIPMUX_DDRTUNE_TEST_START
	la s1,OPIPMUX_DDRTUNE_TEST_END

 
1:	sw s0,0(s0)	
	add s0,4
	bne s0,s1,1b
	nop 

 /*--------- compare ---------------*/
 la s0,OPIPMUX_DRAM_TEST_START
	la s1,OPIPMUX_DRAM_TEST_END	
	/*error address*/
	la s3,OPIPMUX_DRAM_TEST_END
 
 beqz s4,1f
 nop 
 
 la s0,OPIPMUX_DDRTUNE_TEST_START
	la s1,OPIPMUX_DDRTUNE_TEST_END
	/*error address*/
	la s3,OPIPMUX_DDRTUNE_TEST_END


1:/* s4 is the error indicator for ddr tune memory test*/
 la s4,0

1:	/*read current int value */
	lw s2,0(s0)
	/*store the current address*/
	sw s0,0(s3)
	
	/*compare address and value*/
	bne s0,s2,1f
	nop 
	
	add s0,4
	
	bne s0,s1,1b
	nop 

 /* indicate that the memory test passed */
 la s4,1
 
	/*jump to caller*/
1:	j	ra
	  nop

	.end	sdram_test_asm

