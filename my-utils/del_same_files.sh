#!/bin/sh

#cmd with  param atleast is 1 and is a valid path.
if  [ $# -ge 2 ] && [ -d $1 ] && [ -d $2 ]
then
src_files=`echo $2 | tr -d "/"`
dest_dir=`echo $1 | tr -d "/"`
else
echo "usage: $0 valid dir which existed"
# Will exit with status of last command.
exit $?
fi

#diff the dirs
diff -r -s -q ${src_files} ${dest_dir} | grep "identical" | sed -n 's/^Files \(.*\) and \(.*\) are identical/\1 \2/p' |\
xargs -t rm -rf 

#remove the dirs
find ${src_files}  ${dest_dir} -type d -empty | xargs -exec rmdir -p
