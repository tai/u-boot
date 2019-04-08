export CROSS_COMPILE=arm-none-linux-gnueabi-
export MY_WORKING_DIR=$(PWD)

ifndef SVN_REVISION
export SVN_REVISION=`svn info | grep '^Revision:' | sed -e 's/^Revision: //'`
endif

export DEFAULT_CUSTOMER_NAME=cavium
export HDMIDIR=$(MY_WORKING_DIR)/../apps/hdmi
export CUIDDIR=$(MY_WORKING_DIR)/../apps/cuid
export DVUDIR=$(MY_WORKING_DIR)/../apps/dvu
export UBOOT_CONFIG=cnc1800l_config
export USER_SUPPLIED_RESOURCE_DIR=0

ifndef RESOURCEDIR
export RESOURCEDIR=$(MY_WORKING_DIR)/../../../resources/$(DEFAULT_CUSTOMER_NAME)
else
export USER_SUPPLIED_RESOURCE_DIR=1
endif

ifndef DDR_SIZE
export DDR_SIZE=256
endif

ifeq ($(USER_SUPPLIED_RESOURCE_DIR), 1)
ifeq ($(CUSTOMER_NAME),)
$(error ERROR: please specify CUSTOMER_NAME")
endif
else
ifeq ($(CUSTOMER_NAME),)
export CUSTOMER_NAME=$(DEFAULT_CUSTOMER_NAME)
endif
export RESOURCEDIR=$(MY_WORKING_DIR)/../../../resources/$(CUSTOMER_NAME)
endif
ifeq ($(DDR_SIZE),512)
export UBOOT_CONFIG=cnc1800l_512m_config
endif

export UBOOT_UNSIGNED_IAMGE_NAME=$(CUSTOMER_NAME)_unsigned_uboot_r$(SVN_REVISION).bin
export UBOOT_SIGNED_IAMGE_NAME=$(CUSTOMER_NAME)_signed_uboot_r$(SVN_REVISION).sbin

$(info U-Boot Build Started, please wait...)

all:
	@echo "********** Building U-Boot with followin parameters ***************"
	@echo "SVN_REVISION=$(SVN_REVISION)"
	@echo "MY_WORKING_DIR=$(MY_WORKING_DIR)"
	@echo "CUSTOMER_NAME=$(CUSTOMER_NAME)"
	@echo "RESOURCEDIR=$(RESOURCEDIR)"
	@echo "DDR_SIZE=$(DDR_SIZE)"
	@echo "UBOOT_CONFIG=$(UBOOT_CONFIG)"
	@echo "HDMIDIR=$(HDMIDIR)"
	@echo "CUIDDIR=$(CUIDDIR)"
	@echo "DVUDIR=$(DVUDIR)"
	@echo "*******************************************************************"
	@sleep 1

	@if test -f $(RESOURCEDIR)/$(CUSTOMER_NAME)_pkey.c; then echo "public key found OK..."; else (echo "ERROR: $(RESOURCEDIR)/$(CUSTOMER_NAME)_pkey.c  file doesn't exist, Make sure resources folder exist and containes all required files. For help please consult Cavium";exit 2;) ; fi
	@if test -f $(RESOURCEDIR)/$(CUSTOMER_NAME)_blob_pkey.c; then echo "public blob key found OK..."; else (echo "ERROR: $(RESOURCEDIR)/$(CUSTOMER_NAME)_blob_pkey.c  file doesn't exist, Make sure resources folder exist and containes all required files. For help please consult Cavium";exit 2;) ; fi
	@if test -f $(RESOURCEDIR)/bootloader/customer_specific_definitions.h; then echo "customer_specific_definitions found OK..."; else (echo "ERROR: $(RESOURCEDIR)/bootloader/customer_specific_definitions.h  file doesn't exist, Make sure resources folder exist and containes all required files. For help please consult Cavium";exit 2;) ; fi

	@cp $(RESOURCEDIR)/$(CUSTOMER_NAME)_pkey.c                        common
	@cp $(RESOURCEDIR)/$(CUSTOMER_NAME)_blob_pkey.c                   common
	@cp -v $(RESOURCEDIR)/bootloader/customer_specific_definitions.h  include

	@if test -f $(RESOURCEDIR)/$(CUSTOMER_NAME)_dvu.bin; then cp $(RESOURCEDIR)/$(CUSTOMER_NAME)_dvu.bin $(DVUDIR)/dvu.bin; fi
	
ifeq ($(HDMIDIR),)
	@if test -f $(CUIDDIR)/cuid.bin; then echo "cuid.bin file found @ $(CUIDDIR)"; else (echo "cuid.bin file not found, please contact support@cavium"; exit 2;) ; fi
endif

ifeq ($(CUIDDIR),)
	@if test -f $(HDMIDIR)/hdmi.bin; then echo "hdmi.bin file found @ $(HDMIDIR)"; else (echo "hdmi.bin file not found, please contact support@cavium"; exit 2;) ; fi
endif
		
ifeq ($(DVUDIR),)
	@if test -f $(DVUDIR)/dvu.bin; then echo "dvu.bin file found @ $(DVUDIR)"; else (echo "dvu.bin file not found, please contact support@cavium"; exit 2;) ; fi
endif

	@rm -f *.bin *.sbin
	@make -s -f Makefile.uboot clean
	@make -s -f Makefile.uboot distclean
	@make -s -f Makefile.uboot mrproper
	@make -s -f Makefile.uboot $(UBOOT_CONFIG)
	@make -s -f Makefile.uboot CUSTOMER_NAME=$(CUSTOMER_NAME)
	@echo "packaging dvu.bin, hdmi.bin and cuid.bin into u-boot.bin, please wait..."
	@cat u-boot.bin $(DVUDIR)/dvu.bin $(HDMIDIR)/hdmi.bin $(CUIDDIR)/cuid.bin > u-boot.bin+dvu.bin+hdmi.bin+cuid.bin
	@cp u-boot.bin+dvu.bin+hdmi.bin+cuid.bin $(UBOOT_UNSIGNED_IAMGE_NAME)
	@echo "Removing unwanted files..."
	@rm -f cavm18_bootloader.bin  su-boot.bin  u-boot.bin  u-boot.bin+dvu.bin+hdmi.bin+cuid.bin
	@echo "Done. Final unsigned image - $(UBOOT_UNSIGNED_IAMGE_NAME)  is ready to use"

#developer - please uncomment following line in order to build secure image.
#Make sure you have tools to create secure image and copied at $(PWD)/CustomerTools folder for a specific customer.
#./CustomerTools/$(CUSTOMER_NAME)_sign_uboot -i $(UBOOT_UNSIGNED_IAMGE_NAME) -o $(UBOOT_SIGNED_IAMGE_NAME)
#echo "Done. Final signed image - $(UBOOT_SIGNED_IAMGE_NAME)  is ready to use"
	
	
clean:
	@rm -f *.bin *.sbin
	@make -s -f Makefile.uboot clean
	@make -s -f Makefile.uboot distclean
	@make -s -f Makefile.uboot mrproper
	@make -s -f Makefile.uboot $(UBOOT_CONFIG)
	@rm -rf .boards.depend
	@rm -rf sldr/mk_image
	@rm -rf arch/arm/include/asm/proc
	@rm -rf arch/arm/include/asm/arch
	@rm -rf include/config.h
	@rm -rf include/config.mk
	@rm -rf include/asm

#don't remove following three files, it used for creating patch, else build will failed.
#@rm -rf include/customer_specific_definitions.h
#@rm -rf common/cavium_blob_pkey.c
#@rm -rf common/cavium_pkey.c



	
