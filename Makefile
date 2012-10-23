#!/bin/sh
################################################################################
#
# Copyright (C) 2008 Opulan Technologies Corp., All Rights Reserved
#
# FILENAME: Makefile
#
# DESCRIPTION: ipc library module
#
# Date Created (optional): 9/20/2009
#
# Authors (optional): 
#
# Reviewed by (optional):
#
# Edit History:
#
#
################################################################################

TARGET = apps
TARGETDIR = $(shell /bin/pwd)

# Include common and TARGET specific defines
include $(TARGETDIR)/Makefile.conf

# Define a prefix for this Makefile, so that the common targets 
# (COMMONTARGS defined in Makefile.conf), such as clean, etc. can be 
# made in the subdirectories. Use the name of the current directory
# for COMMONPREFIX.
COMMONPREFIX = top

# Final target. Define at least one rule before commonrules,
# otherwise the first commonrule will be the default.
# Default is to:
#	1) create output directory if not exists. Directory name is the same as TARGET
#	2) build all COMPONENTS


all: $(COMPONENTS)

# Rule to make a directory component
$(COMPONENTS): 
	@echo "=== make COMPONENT ./$@"
	@$(MAKE) -rw -C $@

# All the COMPONENTS are nonexistent targets. 
.PHONY: $(COMPONENTS) $(COMMONTARGS)

# Rule to make common targets, such as clean, etc.
# This loop will propogate any of COMMONTARGS to the directories listed in COMPONENTS. COMPONENTS is defined in the Makefile.
# This rule must live in the parent level Makefile, as opposed to the common rules, so that the leaf node make doesn't confuse the
# rules $(COMMONPREFIX)clean with clean, etc.

$(COMMONTARGS):
	@for d in $(COMPONENTS); do \
		echo "=== make $@ directory '$(shell /bin/pwd)/$$d'"; \
		$(MAKE) -r $@ -C $$d; \
	done
	@if [ "$@" = "cleanall" ]; then \
		cd $(TARGETDIR)/apps/opconn/linux/opl_xmodem;make clean; \
	fi
	@if [ "$@" = "cleanall" ] && [ -d $(KERNEL_DIR)/linux-2.6.21.5 ]; then \
		cd $(KERNEL_DIR)/linux-2.6.21.5;make clean; \
	fi
	@if [ "$@" = "cleanall" ] && [ -d $(KERNEL_DIR)/modules/build ]; then \
		cd $(KERNEL_DIR)/modules/build;make clean; \
	fi
	@if [ "$@" = "cleanall" ] && [ -f $(TARGETDIR)/u-boot/Makefile ]; then \
		cd $(TARGETDIR)/u-boot;make distclean; \
	fi

# Include other make rules.
include $(TARGETDIR)/make.rules
