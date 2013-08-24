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

echo "install  glibc to $(my_rootfs)"
cp /usr/local/arm/4.4.3/lib/*.so* $(my_rootfs)/lib
