#include <asm/regdef.h>
#include <configs/opipmux.h>
/*------------------------------------------------------------------

 Function:    sdram_test

 Description: test sdram memory
              test mode : write the address as value to all the tested
              memory, and read back to check it's right.

-------------------------------------------------------------------*/
#ifdef INCLUDE_MEM_EVALUE_RW
#define OPIPMUX_DRAM_RD_START 0xa0000000
#define OPIPMUX_DRAM_RD_END 0xa1000000

        .globl  sdram_wr_asm
        .ent    sdram_wr_asm
sdram_wr_asm:

        la t0,OPIPMUX_DRAM_RD_START
        la t1,OPIPMUX_DRAM_RD_END

1:      sw t0,0(t0)
        add t0,4
        bne t0,t1,1b
        nop

        /*jump to caller*/
1:      j       ra
        nop

        .end    sdram_wr_asm





#endif
