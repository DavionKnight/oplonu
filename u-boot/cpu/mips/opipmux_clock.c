/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
#include <asm/op_ipmux.h>



/*******************************************************************************
*
* get_cpuclk - returns the frequency of the CPU.
*
* Gets the value directly from the INCA-IP hardware.
*
* RETURNS:
*          50.000.000 for 50 MHz
*          100.000.000 for 100 Mhz (= 400MHz/4)
* NOTE:
*   This functions should be used by the hardware driver to get the correct
*   frequency of the CPU. Don't use the macros, which are set to init the CPU
*   frequency in the ROM code.
*/
uint opipmux_get_cpuclk (void)
{
	/* cpu main clock is 555550 MHz */			
	return CPU_CLOCK_RATE;
	
}

/*******************************************************************************
*
* get_fpiclk - returns the frequency of the FPI bus.
*
* Gets the value directly from the OP_IPMUX hardware.
*
* RETURNS: Frquency in Hz
*
* NOTE:
*   This functions should be used by the hardware driver to get the correct
*   frequency of the CPU. Don't use the macros, which are set to init the CPU
*   frequency in the ROM code.
*   The calculation for the
*/
uint opipmux_get_fpiclk (void)
{
	uint clkCPU;

	clkCPU = opipmux_get_cpuclk ();

	switch (*((volatile ulong *) OP_IPMUX_CGU_CGU_DIVCR) & 0xC) {
	case 0x4:
		return clkCPU >> 1;	/* devided by 2 */
		break;
	case 0x8:
		return clkCPU >> 2;	/* devided by 4 */
		break;
	default:
		return clkCPU;
		break;
	}
}

int opipmux_set_cpuclk (void)
{
	extern void ebu_init(long);
	extern void cgu_init(long);
	extern void sdram_init(long);
	uchar tmp[64];
	ulong cpuclk;
	
	
	if (getenv_r ("cpuclk", tmp, sizeof (tmp)) > 0) {
		cpuclk = simple_strtoul (tmp, NULL, 10) * 1000000;
		cgu_init (cpuclk);
		ebu_init (cpuclk);
		sdram_init (cpuclk);
	}
	
	return 0;
}
