export CROSS_COMPILE=mips_4KC-
#svn_ver=`svn info|grep Rev:|awk -F' ' '{print $4}'`
echo "version=1"
make clean
make BUILDNUMBER=1 DEFINES="-DBUILDNUMBER=1 -DCONFIG_LZMA"
