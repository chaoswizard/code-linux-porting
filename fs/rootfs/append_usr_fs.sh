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

def_tools_dir=/usr/local/arm/4.4.3/lib

echo "install ${def_tools_dir} to ${my_rootfs}"
cp -d  ${def_tools_dir}/*.so* ${my_rootfs}/lib
