#!/bin/sh

#print the parameters string
echo "param num: $# in:"

for arg in "$*"
do
echo $arg
done

#print the parameters array
echo "parameter array:"

for arg in "$@"
do
echo $arg
done


#cmd with  param
if [ x$1 != x ]
then
	my_rootfs=`echo $1 | tr -d "/"`
else
	my_rootfs="myfs_top_dir"
fi




echo "------Create ${my_rootfs} directons start...--------"
mkdir ${my_rootfs}
cd ${my_rootfs}

echo "--------Create root,dev....----------"
mkdir root dev etc boot tmp var sys proc lib mnt home usr
mkdir etc/init.d etc/rc.d etc/sysconfig
mkdir usr/sbin usr/bin usr/lib usr/modules
echo "make node in dev/console dev/null"
mknod -m 600 dev/console c 5 1
mknod -m 600 dev/null c 1 3
mkdir mnt/etc mnt/jffs2 mnt/yaffs mnt/data mnt/temp
mkdir var/lib var/lock var/run var/tmp
chmod 1777 tmp
chmod 1777 var/tmp
echo "-------make direction done---------"

