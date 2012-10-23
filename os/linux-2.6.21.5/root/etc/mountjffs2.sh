#mkdir /jffs2
mknod /dev/mtdcfg c 90 0
mknod /dev/mtdk0 c 90 2
mknod /dev/mtdk1 c 90 4
#mount -t jffs2 /dev/mtdk1 /jffs2
#mkdir /jffs2
#mknod /dev/mtdcfg b 31 0
#mknod /dev/mtdk0  b 31 1
#mknod /dev/mtdk1  b 31 2
#mount -t jffs2 /dev/mtdk1 /jffs2
