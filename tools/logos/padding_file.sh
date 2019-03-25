#!/bin/bash

padding()
{
	file_size=`du -b $1 | awk '{ print $1 }'`
	pad=`expr $file_size % 4`
	pad=`expr 4 - $pad`
	if [ $pad -ne 0 ]
	then
		echo "Padding file \"$1\" with $pad \"0\""
		while [ $pad -gt 0 ]
		do
			echo -n "0">>$1
			((pad=$pad-1))
		done
	fi
	return
}

padding $1;
echo "padding finished"