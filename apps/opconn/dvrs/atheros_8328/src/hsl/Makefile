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

OBJECTS = hsl_acl.o hsl_api.o hsl_dev.o hsl_lock.o hsl_port_prop.o isis_acl.o \
	isis_acl_parse.o isis_cosmap.o isis_fdb.o isis_igmp.o isis_init.o isis_ip.o\
	isis_leaky.o isis_led.o isis_mib.o isis_mirror.o isis_misc.o isis_nat.o\
	isis_port_ctrl.o isis_portvlan.o isis_qos.o isis_rate.o isis_reg_access.o isis_sec.o\
	isis_stp.o isis_trunk.o isis_vlan.o f1_phy.o f2_phy.o

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
