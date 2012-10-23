#COPY_TO_FILE="/home/xwang/smb/share/release/wrt_uImage"
#COPY_TO_FILE="/tftpboot/xwang/uImage.onu.wrt.test"
COPY_TO_FILE="../wrt_uImage"

echo "copyed to $COPY_TO_FILE"
cp -f uImage $COPY_TO_FILE
ls -lh $COPY_TO_FILE

echo "==============================================================="
echo "    output files:                           "
echo "         wrt_uImage : the kernel image      "
echo "===============================================================" 
