/*
 * (C) Copyright 2001
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

/*
 * RTC, Date & Time support: get and set date & time
 */
#include <common.h>
#include <command.h>

#if (CONFIG_COMMANDS & CFG_CMD_UU)


int do_utx (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern void spa(char *p,unsigned short length);
	ulong *addr ;
	int len;

	switch (argc) {
	case 3:		
		addr=(ulong *)simple_strtoul((char *)argv[1],NULL,16);
		len=simple_strtoul(argv[2],NULL,16);	

		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return -1;
	}

	printf("send packet addr = 0x%x, len=%d\n",addr,len);
	
	spa((char *)addr,len);

	return 1;
}

/*

int do_uloop(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	DECLARE_GLOBAL_DATA_PTR;
	ulong *addr ;
	int len;

	char cmd[50]="u-boot.bin";

	switch (argc) {
	case 3:		
		addr=(int *)simple_strtoul(argv[1],NULL,16);
		len=simple_strtoul(argv[2],NULL,16);	

		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return;
	}

	printf("send packet addr = 0x%x, len=%d\n",addr,len);
	spa(addr,len);

	return 1;
}
*/


/***************************************************/

U_BOOT_CMD(
	utx,	3,	1,	do_utx,
	"utx     - send packet to network\n",
	"addr len	 - send packet to network\n"
);

#endif	/* CFG_CMD_UU */
