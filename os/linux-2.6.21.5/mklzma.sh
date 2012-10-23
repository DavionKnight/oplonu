#export CROSS_COMPILE=mips-linux-uclibc-
#export ARCH=mips
#make V=1 CC="mips-linux-uclibc-gcc" 

if [ ! -d root ]
then
	if [ -f rootfs.tgz ]
	then
		tar xvzf rootfs.tgz
	fi
fi


make -j1 CROSS_COMPILE="mips-linux-uclibc-" ARCH="mips" CONFIG_SHELL="/bin/bash" CC="mips-linux-uclibc-gcc" V=1

cd build_uimage
sh mklzma.sh
cd ..
