#!/bin/sh
export CROSS_COMPILE=arm-none-linux-gnueabi-

SVN_REVISION=`svn info | grep '^Revision:' | sed -e 's/^Revision: //'`
echo "SVN_REVISION=${SVN_REVISION}"


if [ $# -lt 1 ];then
    export CUSTOMER_NAME=cavium
else 
    export CUSTOMER_NAME=$1
fi

export HDMIDIR=../apps/hdmi
export CUIDDIR=../apps/cuid
export DVUDIR=../apps/dvu
export RESOURCEDIR=../../../resources

export UBOOT_UNSIGNED_IAMGE_NAME=${CUSTOMER_NAME}_unsigned_uboot_r${SVN_REVISION}.bin
export UBOOT_SIGNED_IAMGE_NAME=${CUSTOMER_NAME}_signed_uboot_r${SVN_REVISION}.sbin

cp ${RESOURCEDIR}/${CUSTOMER_NAME}/${CUSTOMER_NAME}_pkey.c common
cp ${RESOURCEDIR}/${CUSTOMER_NAME}/${CUSTOMER_NAME}_blob_pkey.c common
cp -v ${RESOURCEDIR}/${CUSTOMER_NAME}/bootloader/customer_specific_definitions.h include

if [ -f ${RESOURCEDIR}/${CUSTOMER_NAME}/${CUSTOMER_NAME}_dvu.bin ];then 
	cp ${RESOURCEDIR}/${CUSTOMER_NAME}/${CUSTOMER_NAME}_dvu.bin ${DVUDIR}/dvu.bin
fi

if [ -z "$HDMIDIR" ]; then
	if [ -f ${HDMIDIR}/hdmi.bin ];then 
		echo "hdmi.bin file found @ ${HDMIDIR}" 
	else 
		echo "hdmi.bin file not found, please contact support@cavium" 
		exit
	fi
fi

if [ -z "$CUIDDIR" ]; then
	if [ -f ${CUIDDIR}/cuid.bin ];then 
		echo "cuid.bin file found @ ${CUIDDIR}" 
	else 
		echo "cuid.bin file not found, please contact support@cavium" 
		exit
	fi
fi

if [ -z "$DVUDIR" ]; then
	if [ -f ${DVUDIR}/dvu.bin ];then 
		echo "dvu.bin file found @ ${DVUDIR}" 
	else 
		echo "dvu.bin file not found, please contact support@cavium" 
		exit
	fi
fi

echo "CUSTOMER_NAME=${CUSTOMER_NAME}"
echo "CUIDDIR=${CUIDDIR}"
echo "HDMIDIR=${HDMIDIR}"
echo "DVUDIR=${DVUDIR}"

rm -f *.bin *.sbin
make -s -f Makefile.uboot clean
make -s -f Makefile.uboot distclean
make -s -f Makefile.uboot mrproper
make -s -f Makefile.uboot cnc1800l_config
make -f Makefile.uboot CUSTOMER_NAME=${CUSTOMER_NAME}
echo "packaging dvu.bin, hdmi.bin and cuid.bin into u-boot.bin, please wait..."
cat u-boot.bin ${DVUDIR}/dvu.bin ${HDMIDIR}/hdmi.bin ${CUIDDIR}/cuid.bin > u-boot.bin+dvu.bin+hdmi.bin+cuid.bin
cp u-boot.bin+dvu.bin+hdmi.bin+cuid.bin ${UBOOT_UNSIGNED_IAMGE_NAME}
echo "Removing unwanted files..."
rm -f cavm18_bootloader.bin  su-boot.bin  u-boot.bin  u-boot.bin+dvu.bin+hdmi.bin+cuid.bin
echo "Done. Final unsigned image - ${UBOOT_UNSIGNED_IAMGE_NAME}  is ready to use"

#developer - please uncomment following line in order to build secure image.
#Make sure you have tools to create secure image and copied at ${PWD}/CustomerTools folder for a specific customer.
#./CustomerTools/${CUSTOMER_NAME}_sign_uboot -i ${UBOOT_UNSIGNED_IAMGE_NAME} -o ${UBOOT_SIGNED_IAMGE_NAME}
#echo "Done. Final signed image - ${UBOOT_SIGNED_IAMGE_NAME}  is ready to use"

