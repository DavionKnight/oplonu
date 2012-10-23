export CROSS_COMPILE=mips_4KC-
svn_ver=100
echo "version=$svn_ver"
make clean
make BUILDNUMBER=$svn_ver DEFINES="-DBUILDNUMBER=$svn_ver -DCONFIG_LZMA"
mips-linux-objdump -dtSs u-boot > u-boot.dis

