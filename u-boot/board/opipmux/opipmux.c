/*
 * (C) Copyright 2005
 *  opulan technologies,
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

#include <common.h>
#include <command.h>
#include <asm/addrspace.h>
#include <asm/op_ipmux.h>


#define OP_IPMUX_SDRAM_CFG_CTRL			OP_IPMUX_SDRAM_ADDR
#define OP_IPMUX_SDRAM_CFG_REGID        0x03
#define OP_IPMUX_SDRAM_CFG_ACCTYPE      0x40
#define OP_IPMUX_SDRAM_CFG_DCBUSY       0x80

#define OP_IPMUX_SDRAM_CFG_REG0         0x00
#define OP_IPMUX_SDRAM_CFG_REG1         0x01
#define OP_IPMUX_SDRAM_CFG_REG2         0x02
#define OP_IPMUX_SDRAM_CFG_REG3         0x03

#define OP_IPMUX_SDRAM_CFG_READ         0x00
#define OP_IPMUX_SDRAM_CFG_WRITE        0x40

#define OP_IPMUX_SDRAM_CFG_CTRL_RWBITS      (OP_IPMUX_SDRAM_CFG_REGID   |   \
                                            OP_IPMUX_SDRAM_CFG_ACCTYPE  |   \
                                            OP_IPMUX_SDRAM_CFG_DCBUSY)


typedef struct op_ipmux_sdram_ctrl{
	volatile unsigned int	cfg_ctrl;
	volatile unsigned int	cfg_word_lsb;
	volatile unsigned int	cfg_word_lsb1;
	volatile unsigned int	cfg_word_lsb2;
	volatile unsigned int	cfg_word_msb;
};

long int initdram(int board_type)
{
	/*	asm volatile ("move %0, $25" : "=r" (our_address) :);*/
	volatile struct op_ipmux_sdram_ctrl* pSdrm = (struct op_ipmux_sdram_ctrl*)0xA0000500/*OP_IPMUX_SDRAM_ADDR*/;
	pSdrm->cfg_word_lsb		= 0xAA;
	pSdrm->cfg_word_lsb1	= 0xBB;
	pSdrm->cfg_word_lsb2	= 0xCC;
	pSdrm->cfg_word_msb		= 0xDD;
	pSdrm->cfg_ctrl			=  OP_IPMUX_SDRAM_CFG_WRITE | OP_IPMUX_SDRAM_CFG_DCBUSY;

	/* Can't probe for RAM size unless we are running from Flash.*/
	/*
	if (PHYSADDR(our_address) < PHYSADDR(PHYS_FLASH_1))
	{
		return max_sdram_size();
	}

	for (cols = 0x8; cols <= 0xC; cols++)
	{
		for (rows = 0xB; rows <= 0xD; rows++)
		{
			*OP_IPMUX_SDRAM_MC_CFGPB0 = (0x14 << 8) |
			                           (rows << 4) | cols;
			size = get_ram_size((ulong *)CFG_SDRAM_BASE,
			                                     max_sdram_size());

			if (size > max_size)
			{
				best_val = *OP_IPMUX_SDRAM_MC_CFGPB0;
				max_size = size;
			}
		}
	}

	*OP_IPMUX_SDRAM_MC_CFGPB0 = best_val;

	return max_size;
	*/
	return OPIPMUX_MAX_DRAM_SZ;

}

int checkboard (void)
{
	/*
	unsigned long chipid = *OP_IPMUX_WDT_CHIPID;
	int part_num;

	puts ("Board: OP-IPMUX ");
	part_num = (chipid >> 12) & 0xffff;
	switch (part_num) {
	case 0xc0:
		printf ("Standard Version, ");
		break;
	case 0xc1:
		printf ("Basic Version, ");
		break;
	default:
		printf ("Unknown Part Number 0x%x ", part_num);
		break;
	}

	printf ("Chip V1.%ld, ", (chipid >> 28));

	printf("CPU Speed %d MHz\n", opipmux_get_cpuclk()/1000000);
	*/
	return 0;
}
