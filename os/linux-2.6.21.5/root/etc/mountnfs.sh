######################  create local dir #######################
if  test ! -d /nfs; then \
	mkdir /nfs;\
fi;

######################  mount for max times ####################
maxcnt=2
mountcnt=0
while  test ! -d /nfs/gdb  && test $mountcnt -lt $maxcnt ; do 
	let "mountcnt = $mountcnt + 1" 
	echo "mount nfs server 192.168.1.57:/ucnfs on /nfs for $mountcnt times, wait ..."
	mount -o nolock -t nfs 192.168.1.57:/ucnfs /nfs
done

#####################  check if mount ok #######################
if  test -d /nfs/gdb; then \
	export PATH=$PATH:/nfs/gdb ; \
else \
	echo "mount nfs server failed." ; \
fi;\

