#
# Makefile for the INCA specific kernel interface routines under Linux.
#
TOPDIR=..
#include $(TOPDIR)/rules.mk
all: uImage

VERSION = $(shell date)
#SVN_VERSION = $(shell svn info | grep Revision | awk 'BEGIN{getline rev; print(substr(rev,11));}'))
SVN_VERSION = 648
PATCHLEVEL = 1
SUBLEVEL = 1
EXTRAVERSION = 1
SOCID = 
CPUID ?=

ifeq ($(CPUID), onu)
	SOCID = 63511
	SVN_VERSION = $(shell cat ./version)
endif
ifeq ($(CPUID), onu1e)
	SOCID = 63512
	SVN_VERSION = $(shell cat ./version)
endif
ifeq ($(CPUID), onu2)
	SOCID = 63511
	SVN_VERSION = $(shell cat ./version)
endif

ifeq ($(CPUID), optrann)
	SOCID = 66801
	SVN_VERSION = $(shell cat ./version)
endif

LX = ../vmlinux
LX_MAP = ../System.map
EXTRA_AFLAGS := $(CFLAGS)
piggy.gz: $(LX)
	@echo "------------- make piggy.gz -----------------"
	ls -lh $(LX)
#	mips_4KC-strip $(LX)
	ls -lh $(LX)
	mips_4KC-objcopy --gap-fill=0xff -O binary $(LX) piggy
	gzip -9 < piggy > piggy.gz
	echo "piggy.gz ok"
piggy.lzma: $(LX)
	@echo "------------- make piggy.gz -----------------"
	ls -lh $(LX)
#	mips_4KC-strip $(LX)
	ls -lh $(LX)
	mips_4KC-objcopy --gap-fill=0xff -O binary $(LX) piggy
#	gzip -9 < piggy > piggy.gz
#	lzma e piggy piggy.lzma
	lzma -f piggy
	echo "piggy.lzma ok"

uImage \
pImage: piggy.gz
	@echo $(VERSION)
	@echo "make uImage using file $(LX) ... "
	@echo $(BUILD_TIME)
	@echo "svn=" $(SVN_VERSION)
	@echo "cpuid=" $(SOCID)
	./mkimage-new -i $(SOCID) -r $(SVN_VERSION) -A mips -O linux -T kernel -C gzip -a 80041000 -e \
	0x$(shell grep kernel_entry $(LX_MAP) | awk '{ printf "%s", $$1 }') \
	-n 'MIPS Linux-$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)' \
	-d piggy.gz $@

uImageLzma: piggy.lzma
	@echo $(VERSION)
	@echo "make uImage using file $(LX) ... "
	@echo $(BUILD_TIME)
	@echo "svn=" $(SVN_VERSION)
	@echo "cpuid=" $(SOCID)
	./mkimage-new -i $(SOCID) -r $(SVN_VERSION) -A mips -O linux -T kernel -C lzma -a 80041000 -e \
	0x$(shell grep kernel_entry $(LX_MAP) | awk '{ printf "%s", $$1 }') \
	-n 'MIPS Linux-$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)' \
	-d piggy.lzma $@

clean:
	rm -f uImage pImage piggy piggy.gz

