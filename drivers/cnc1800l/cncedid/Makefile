include $(TOPDIR)/config.mk

LIB	:= $(obj)libedid.o

COBJS-$(CONFIG_CNC1800L_DF) += cnc_edid_ops.o cnc_edid_parse.o cnc_edid_assemble.o cnc_edid_print.o cnc_edid_util.o      

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
all:	$(LIB)

$(LIB): $(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS) $(HDMIOBJ))
#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
