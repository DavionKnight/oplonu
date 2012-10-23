################################################################################
#
# Copyright (C) 2008 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: dvr/mv6046/src module
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
TARGETDIR = $(shell cd ../../../..;/bin/pwd)

# Include common and TARGET specific defines
include $(TARGETDIR)/Makefile.conf


# Library to be built
LIB = libdvrs.a

# Objects to build and included into library

OBJECTS = hsl_acl.o hsl_dev.o hsl_port_prop.o shiva_init.o shiva_acl.o shiva_reg_access.o\
	shiva_mib.o shiva_port_ctrl.o shiva_portvlan.o shiva_vlan.o shiva_fdb.o shiva_qos.o\
	shiva_stp.o shiva_mirror.o shiva_rate.o shiva_misc.o shiva_leaky.o shiva_igmp.o\
	shiva_led.o f1_phy.o f2_phy.o shiva_reduced_acl.o hsl_lock.o

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
