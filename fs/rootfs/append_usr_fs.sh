#!/bin/sh

#cmd with  param atleast is 1 and is a valid path.
if [ -d $1 ] && [ $# -ge 1 ]
then
my_rootfs=`echo $1 | tr -d "/"`
else
echo "usage: $0 valid dir which existed"
# Will exit with status of last command.
exit $?
fi

dest_dir=${my_rootfs}

src_files=lib_busybox/*

echo "install ${src_files} to ${dest_dir}"
cp -d -rf  ${src_files} ${dest_dir}
#ls ${dest_dir} -l
