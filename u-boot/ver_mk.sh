export CROSS_COMPILE=mips_4KC-
svn_ver=`svn info|grep Rev:|awk -F' ' '{print $4}'`
echo "version=$svn_ver"
make clean
make BUILDNUMBER=$svn_ver DEFINES="-DBUILDNUMBER=$svn_ver -DCONFIG_LZMA"
