/*
 * (C) Copyright 2002
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com
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

/*
 * SPI Read/Write Utilities
 */

#include <common.h>
#include <command.h>
#include <spi.h>

#if (CONFIG_COMMANDS & CFG_CMD_SPI)

/*-----------------------------------------------------------------------
 * Definitions
 */

#ifndef MAX_SPI_BYTES
#   define MAX_SPI_BYTES 32	/* Maximum number of bytes we can handle */
#endif

/*
 * External table of chip select functions (see the appropriate board
 * support for the actual definition of the table).
 */
extern spi_chipsel_type spi_chipsel[];
extern int spi_chipsel_cnt;

/*
 * Values from last command.
 */
static int   device;
static int   bitlen;
static uchar dout[MAX_SPI_BYTES];
static uchar din[MAX_SPI_BYTES];

/*
 * SPI read/write
 *
 * Syntax:
 *   spi {dev} {num_bits} {dout}
 *     {dev} is the device number for controlling chip select (see TBD)
 *     {num_bits} is the number of bits to send & receive (base 10)
 *     {dout} is a hexadecimal string of data to send
 * The command prints out the hexadecimal string received via SPI.
 */

int do_spi (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char  *cp = 0;
	uchar tmp;
	int   j;
	int   rcode = 0;

	/*
	 * We use the last specified parameters, unless new ones are
	 * entered.
	 */

	if ((flag & CMD_FLAG_REPEAT) == 0)
	{
		if (argc >= 2)
			device = simple_strtoul(argv[1], NULL, 10);
		if (argc >= 3)
			bitlen = simple_strtoul(argv[2], NULL, 10);
		if (argc >= 4) {
			cp = argv[3];
			for(j = 0; *cp; j++, cp++) {
				tmp = *cp - '0';
				if(tmp > 9)
					tmp -= ('A' - '0') - 10;
				if(tmp > 15)
					tmp -= ('a' - 'A');
				if(tmp > 15) {
					printf("Hex conversion error on %c, giving up.\n", *cp);
					return 1;
				}
				if((j % 2) == 0)
					dout[j / 2] = (tmp << 4);
				else
					dout[j / 2] |= tmp;
			}
		}
	}

	if ((device < 0) || (device >=  spi_chipsel_cnt)) {
		printf("Invalid device %d, giving up.\n", device);
		return 1;
	}
	if ((bitlen < 0) || (bitlen >  (MAX_SPI_BYTES * 8))) {
		printf("Invalid bitlen %d, giving up.\n", bitlen);
		return 1;
	}

	debug ("spi_chipsel[%d] = %08X\n",
		device, (uint)spi_chipsel[device]);

	if(spi_xfer(spi_chipsel[device], bitlen, dout, din) != 0) {
		printf("Error with the SPI transaction.\n");
		rcode = 1;
	} else {
		cp = din;
		for(j = 0; j < ((bitlen + 7) / 8); j++) {
			printf("%02X", *cp++);
		}
		printf("\n");
	}

	return rcode;
}

/***************************************************/

U_BOOT_CMD(
	sspi,	5,	1,	do_spi,
	"sspi    - SPI utility commands\n",
	"<device> <bit_len> <dout> - Send <bit_len> bits from <dout> out the SPI\n"
	"<device>  - Identifies the chip select of the device\n"
	"<bit_len> - Number of bits to send (base 10)\n"
	"<dout>    - Hexadecimal string that gets sent\n"
);

#endif	/* CFG_CMD_SPI */

#if defined(DRV_NEW_SPI)||defined(CFG_SPI_FLASH) //defined(CFG_NEW_SPI_FLASH)||defined(CFG_SPI_FLASH) 
void do_spi_init(){
#if defined(CFG_NEW_SPI_FLASH)||defined(DRV_NEW_SPI)
	spiInit();
#else
	spi_init();
#endif
}

void do_spi_status(){
	unsigned long result;

	result = spi_read_status();

	printf("spi status:0x%x\n",result);

}

void do_spi_man(){
	unsigned long result;

	result = spi_read_man();

	printf("spi id:0x%x\n",result);

}

void do_spi_pro_boot(){
    spiProgramUb();
}


void do_spi_read_word(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]){
	unsigned long result;
	unsigned long ofs;

	ofs = simple_strtoul(argv[1], NULL, 16);
	result = spi_read_one_word(ofs);

	printf("0x%x:0x%x\n",ofs,result);
}

void do_spi_burn(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]){
	
	unsigned long *src;
	int count;

	src = simple_strtoul(argv[1], NULL, 16);
	count = simple_strtoul(argv[2], NULL, 16);
	
	burn_spi_bootloader(src,count);

}

void do_spi_burn_os(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]){
	
	unsigned long *src;
	int count,count1;

	src = simple_strtoul(argv[1], NULL, 16);
	count = simple_strtoul(argv[2], NULL, 16);
	
	//printf("src:%x,count:%x",src,count);
	burn_spi_os(src,count);

}

void do_spi_write_word(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]){
	unsigned long ofs;
	unsigned long val;

	ofs = simple_strtoul(argv[1], NULL, 16);
	val = simple_strtoul(argv[2], NULL, 16);
	//spiWordProgram(&val, ofs);
#if defined(CFG_NEW_SPI_FLASH)||defined(DRV_NEW_SPI)
	spiWordProgram(&val,ofs);
#else
	spi_write_one_word_directly(ofs,val);
#endif
}

void do_spi_erase_sector(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]){
	unsigned long ofs;

	ofs = simple_strtoul(argv[1], NULL, 16);

	spi_erase_one_sector(ofs);

}

void do_spi_erase_chip(){
	spi_erase_chip();
}

void do_write_spi_status(){
	spi_write_status(0);
}

U_BOOT_CMD(
	spi_burn,	3,	1,	do_spi_burn,
	"spi_burn    - SPI status register\n",
	"help\n"
);

U_BOOT_CMD(
	spi_burn_os,	3,	1,	do_spi_burn_os,
	"spi_burn_os    - SPI status register\n",
	"help\n"
);

U_BOOT_CMD(
	spi_erase_chip,	1,	1,	do_spi_erase_chip,
	"spi_erase_chip    - erase whole chip\n",
	"help\n"
);
U_BOOT_CMD(
	spi_erase_sector,	2,	1,	do_spi_erase_sector,
	"spi_erase_sector    - SPI erase 64K sector.  \n arg1: sector number\n",
	"help\n"
);

U_BOOT_CMD(
	spi_status,	1,	1,	do_spi_status,
	"spi_status    - SPI status register\n",
	"help\n"
);

U_BOOT_CMD(
	spi_read,	2,	1,	do_spi_read_word,
	"spi_read    - SPI status register\n",
	"help\n"
);

U_BOOT_CMD(
	spi_write,	3,	1,	do_spi_write_word,
	"spi_write    - SPI status register\n",
	"help\n"
);

U_BOOT_CMD(
	spi_status_write,	1,	1,	do_write_spi_status,
	"spi_status_write    - SPI status register\n",
	"help\n"
);

U_BOOT_CMD(
	spi_man,	1,	1,	do_spi_man,
	"spi_man    - SPI status register\n",
	"help\n"
);

U_BOOT_CMD(
	spi_pro_boot,	1,	1,	do_spi_pro_boot,
	"spi_pro_boot    - SPI program Uboot\n",
	"help\n"
);
U_BOOT_CMD(
	spi_init,	1,	1,	do_spi_init,
	"spi_init    - init spi interface : div:3   cs:0   \n",
	"help\n"
);


#endif

