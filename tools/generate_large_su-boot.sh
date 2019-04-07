#!/bin/sh

LOGO_OFFSET=640
LARGE_SU_BOOT_BIN="large_su-boot.bin"
SU_BOOT_BIN="su-boot.bin"
BOOT_LOGO_BIN="boot_logo_argb8888_up_down_reverse.bin"

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

rm -f $BOOT_LOGO_BIN.gz
./gen_logo_bin $1 $BOOT_LOGO_BIN 1920 1080

if [ -f $BOOT_LOGO_BIN ]
then
	gzip $BOOT_LOGO_BIN 
else
	echo "$BOOT_LOGO_BIN not generated"
  	exit 1
fi

rm -f $LARGE_SU_BOOT_BIN;
dd if=$SU_BOOT_BIN of=$LARGE_SU_BOOT_BIN;
dd if=$BOOT_LOGO_BIN.gz of=$LARGE_SU_BOOT_BIN bs=1024 seek=$LOGO_OFFSET;
padding $LARGE_SU_BOOT_BIN

rm -f $BOOT_LOGO_BIN.gz
