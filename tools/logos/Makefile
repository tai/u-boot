include $(TOPDIR)/config.mk

TOOLS_LOGO_DIR=$(SRCTREE)/tools/logos
$(CONFIG_TARGET_NAME).bin.gz:$(obj)gen_logo_bin$(SFX)
	if [ -f $(TOOLS_LOGO_DIR)/$(CONFIG_TARGET_NAME).bmp ]; then \
		$(TOOLS_LOGO_DIR)/gen_logo_bin $(CONFIG_LOGO_FORMAT) $(TOOLS_LOGO_DIR)/$(CONFIG_TARGET_NAME).bmp $(TOOLS_LOGO_DIR)/$(CONFIG_TARGET_NAME).bin $(CONFIG_LOGO_WIDTH) $(CONFIG_LOGO_HEIGHT);\
		gzip -f -9 $(TOOLS_LOGO_DIR)/$(CONFIG_TARGET_NAME).bin;\
		chmod u+x $(TOOLS_LOGO_DIR)/padding_file.sh;\
		$(TOOLS_LOGO_DIR)/padding_file.sh $(TOOLS_LOGO_DIR)/$@;\
	fi

$(obj)gen_logo_bin$(SFX):	$(obj)gen_logo_bin.c
	$(HOSTCC) $(HOSTCFLAGS) $(HOSTLDFLAGS) -o $@ $^
	$(HOSTSTRIP) $@
	
clean:
	rm -f *.bin *.gz

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################