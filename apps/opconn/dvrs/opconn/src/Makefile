################################################################################
#
# Copyright (C) 2008 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: dvr/opconn/src module
#
# Date Created (optional): 4/15/2008
#
# Authors (optional): Gan Zhiheng
#
# Reviewed by (optional):
#
# Edit History:
#
#
################################################################################

TARGET = opconn
TARGETDIR = $(shell cd ../../..;/bin/pwd)

# Include common and TARGET specific defines
include $(TARGETDIR)/Makefile.conf


# Library to be built
LIB = libdvrs.a

# Objects to build and included into library
OBJECTS = \
	bridgemac.o \
	bridgeport.o \
	bridgevlan.o \
	classify.o \
	hw_interface.o \
	i2c.o \
	interrupt.o \
	opconn_api.o \
	opconn_lib.o \
	opconnmisc.o \
	opl_debug.o \
	opl_dma.o \
	opl_errno.o \
	opl_host.o \
	opl_utils.o \
	peripheral.o \
	phy.o \
	pon.o \
	remark.o \
	tm.o \
	wdt.o \
	uart.o


# and place objects into LIBDIR
OBJECTS := $(addprefix $(LIBDIR)/, $(OBJECTS))


# One or more directories containig .h files specific to this module. If there are no local include directories, leave blank.
LOCALINCLUDE =

all: $(LIBDIR)/$(LIB)


# Include dependencies
ifeq ($(findstring $(MAKECMDGOALS), clean cleanall ),)
-include $(OBJECTS:.o=.d)
endif

# Include other make rules.
include $(TARGETDIR)/make.rules
