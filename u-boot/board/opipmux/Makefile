#
# (C) Copyright 2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= lib$(BOARD).a

OBJS	= $(BOARD).o ipmux_flash.o ipmux_fe.o ipmuxe_fe.o ipmux_timer.o ipmux_wdt.o  ipmux_rtc.o \
		  commoninfo.o      ipmux_counter.o   ipmux_counter_sdram.o \
		  ipmux_reg_test.o  ipmux_tab_test.o  ipmux_memory_test.o 	\
		  ipmux_uart_test.o ipmux_fe_test.o   mips_timer_test.o  	\
		  ipmux_rtc_test.o  ipmux_hdlc.o      ipmux_uart.o	ipmux_2_uart.o\
		  ipmux_hdlc_test.o ipmux_hw_access.o ipmux_gpio.o      \
		  opulan_bootextend.o ipmuxclt_fe.o ipmuxe2_fe.o ipmuxe2_ge.o ipmux_mii.o ipmux_2_dmac.o\
		  ipmux_flash_onu_8bit.o ipmuxclt_fe_onu.o ipmux_flash_onu_16bit.o \
		  ipmux_spi.o ipmux_mdio.o  opconn_atheros_8228.o ipmux_update.o onu_respin_spi.o onu_njzte_spi.o


SOBJS	= memsetup.o ipmux_ddr_asm.o lowlevel_init.o ipmux_test_ram.o ipmux_memory_rdtest.o ipmux_memory_wrtest.o ipmux_ddr_asm_for_9806_newest.o

$(LIB):	.depend $(OBJS) $(SOBJS)
	$(AR) crv $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
