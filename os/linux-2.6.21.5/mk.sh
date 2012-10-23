#!/bin/sh

TOP=$(pwd)
export TOP

BUILD_IMAGE=$TOP/build_uimage
ROOTDIR=$TOP/root
ROOTFS_DIR=$TOP/rootfs

export BUILD_IMAGE
export ROOTDIR
export ROOTFS_DIR


#build busybox
rm -rf $ROOTFS_DIR
mkdir $ROOTFS_DIR
#cd $BUILD_IMAGE
#sh build_busybox.sh

#clean .svn
cd $ROOTDIR
find . -name ".svn" | xargs rm -rf

cd $ROOTFS_DIR
find . -name ".svn" | xargs rm -rf

#copy optrann kernel module
if [ "$CPUID" = "optrann" ];then
	echo "...build optrann kernel..."	
	rm $TOP/root/modules/opl_cpuif.ko.lzma
	cp $TOP/root/modules/opl_cpuif_10g.ko.lzma $TOP/root/modules/opl_cpuif.ko.lzma
fi

#build kernel image
cd $TOP
make -j1 CROSS_COMPILE="mips-linux-uclibc-" ARCH="mips" CONFIG_SHELL="/bin/bash" CC="mips-linux-uclibc-gcc" 

cd $BUILD_IMAGE
sh mk.sh


cd $TOP
