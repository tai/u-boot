/*
 * Basic XPORT functions
 *
 * Copyright (c) 2007 Celestial Semiconductor
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Ma Jia jia.ma@celestialsemi.com, Celestial Semiconductor
 *
 * Copyright (c) 2008 Celestial Semiconductor
 *
 */

#include <common.h>
#include "cnc1800l_xport.h"
#include "cnc1800l_xport_regs.h"
#include "xport_fw_text.h"
#include "xport_fw_data.h"

#define xport_writel(a,v)	do{ *(volatile unsigned int *)(a) = (v); udelay(5); }while(0)
#define xport_readl(a)	(*(volatile unsigned int *)(a))
#define unused_variable(a) do { a = a; } while(0);
#define mdelay(n) ({unsigned long msec=(n); while (msec--) udelay(1000);})

#define MAX_FW_DATA_SIZE 2048*4
#define MAX_FW_TEXT_SIZE  3072*4

#if 0
/*****************************DEBUG start***********************************/
int ReadReg32(unsigned int a, unsigned int * v)
{
	*v = xport_readl(a);
}

static void xport_dump_regs(void)
{
	unsigned int chl_id;
	unsigned int reg_val;

	printf("==============> Hardware Register <=====================\n");

	reg_val = xport_readl(0x41400000 + (0x0008 << 2));
	printf("xport_cfg0         = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0009 << 2));
	printf("xport_cfg1         = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0018 << 2));
	printf("tuner enable       = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0011 << 2));
	printf("IRQ0 En            = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0015 << 2));
	printf("IRQ1 En            = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0010 << 2));
	printf("IRQ0 ST            = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0014 << 2));
	printf("IRQ1 ST            = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0040 << 2));
	printf("channel0 base addr = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0041 << 2));
	printf("channel0 cfg       = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0042 << 2));
	printf("channel0 rp        = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0043 << 2));
	printf("channel0 wp        = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0045 << 2));
	printf("channel0 st        = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0060 << 2));
	printf("pid filter0        = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0061 << 2));
	printf("pid filter1        = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0003 << 2));
	printf("Mail Box3          = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0004 << 2));
	printf("Mail Box4          = 0x%08x\n", reg_val);
	reg_val = xport_readl(0x41400000 + (0x0005 << 2));
	printf("Mail Box5          = 0x%08x\n", reg_val);

	printf("\n==============>   Mips Register   <=====================\n");
	for (chl_id = 0; chl_id < 6; chl_id++) {
		printf("==============> Output Channel %d <=====================\n", chl_id);

		xport_mips_read(MIPS_CHL_EN(chl_id), &reg_val);
		printf("channel en           = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_PID0(chl_id), &reg_val);
		printf("channel PID0         = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_PID1(chl_id), &reg_val);
		printf("channel PID1         = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_PID2(chl_id), &reg_val);
		printf("channel PID2         = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_PID3(chl_id), &reg_val);
		printf("channel PID3         = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_DIR_LOW_ADDR(chl_id), &reg_val);
		printf("channel DIR_LOW_ADRR = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_DIR_UP_ADDR(chl_id), &reg_val);
		printf("channel DIR_UP_ADRR  = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_WP(chl_id), &reg_val);
		printf("channel WP           = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_BUF_LOW_ADDR(chl_id), &reg_val);
		printf("channel BUF_LOW_ADRR = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_BUF_UP_ADDR(chl_id), &reg_val);
		printf("channel BUF_UP_ADRR  = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_ERR_PKT_CNT(chl_id), &reg_val);
		printf("channel ERR Packet   = 0x%08x\n", reg_val);

		xport_mips_read(MIPS_CHL_OUT_PKT_CNT(chl_id), &reg_val);
		printf("channel Out Packet   = 0x%08x\n", reg_val);
	}

	return;
}
/*************************************DEBUG end******************************************/
#endif

union firmware_req {

	struct {
		unsigned int req_type:8;
		unsigned int output_idx:8;
		unsigned int reserved:14;
		unsigned int rw:1;
		unsigned int enable:1;
	} bits;

	unsigned int val;
};

static int xport_mips_write(unsigned int cmd, unsigned int req_dat)
{
	unsigned int cnt = 1000;
	unsigned int regs_val;

	cmd |= 0xc0000000;	/* bit31 = 1, indicates that will have a request */
				/* bit30 = 1, indicates that a request will send to firmware */

	do {
		regs_val = xport_readl(MIPS_CMD_REQ);
		udelay(50);
	} while ((cnt-- > 0) && (regs_val >> 31));

	if (regs_val >> 31) {
		printf("mips_write error: timeout\n");
		return (-1);
	}

	xport_writel(MIPS_CMD_DATA, req_dat);	/* write a request to firmware */
	xport_writel(MIPS_CMD_REQ, cmd);

	return (0);
}

static int xport_mips_read(unsigned int cmd, unsigned int *req_dat)
{
	unsigned int cnt = 1000;
	unsigned int regs_val = 0;

	union firmware_req req;

	req.val = cmd;
	req.val |= 0x80000000;	/* bit31 = 1, indicates that will have a request */
				/* bit30 = 0, indicates that get information from firmware */

	do {
		regs_val = xport_readl(MIPS_CMD_REQ);
		udelay(50);
	} while ((cnt-- > 0) && (regs_val >> 31));

	if (regs_val >> 31) {
		printf("mips_read error: timeout\n");
		return (-1);
	}

	xport_writel(MIPS_CMD_REQ, req.val);	/* send read-request to firmware */
	udelay(50);

	cnt = 1000;
	do {
		regs_val = xport_readl(MIPS_CMD_REQ);
		udelay(50);
	} while ((cnt-- > 0) && (regs_val >> 31));

	if (regs_val >> 31) {
		printf(" mips_read error: timeout1 \n");
		return (-1);
	}

	*req_dat = xport_readl(MIPS_CMD_DATA);

	return (0);
}
/*
*Load xport firmware to memory.
*/
int _xport_load_firmware(void)
{
	int i, wr_offset;
	unsigned int wr_data;

	xport_writel(MIPS_FW_EN, 1);

	wr_offset = sizeof(uiXport_MIPS_Data) / sizeof(uiXport_MIPS_Data[0]);
	if (wr_offset > MAX_FW_DATA_SIZE){
		printf("u-boot error:xport firmware data overflow!\n");
		return -1;	/* data overflow. */
	}

	for (i = 0; i < wr_offset; i += 4) {
		wr_data = ((u32)uiXport_MIPS_Data[i+3] << 24) | ((u32)uiXport_MIPS_Data[i+2] << 16) | ((u32)uiXport_MIPS_Data[i+1] << 8) | uiXport_MIPS_Data[i];
        xport_writel(MIPS_FW_WRITE_DATA + i, wr_data);
	}

	for (; wr_offset < 1024; wr_offset++) {
		xport_writel(MIPS_FW_WRITE_DATA + (wr_offset << 2), 0);
	}

	wr_offset = sizeof(uiXport_MIPS_Inst) / sizeof(uiXport_MIPS_Inst[0]);
	if (wr_offset > MAX_FW_TEXT_SIZE){
		printf("u-boot error:xport firmware instruction overflow!\n");
		return (-1);	/* instruction overflow. */
	}

	for (i = 0; i < wr_offset; i += 4) {
		wr_data = ((u32)uiXport_MIPS_Inst[i+3] << 24) | ((u32)uiXport_MIPS_Inst[i+2] << 16) | ((u32)uiXport_MIPS_Inst[i+1] << 8) | uiXport_MIPS_Inst[i];
		xport_writel(MIPS_FW_WRITE_INST + i, wr_data);
	}

	xport_writel(MIPS_FW_EN, 0);

	return 0;
}

/*
*Xport hardware configuration.
*/
int xport_hw_init(void)
{
	xport_writel(XPORT_CHL0_BASE_ADDR, (XPORT_CHL0_BASE_ADDR_DEF >> 3));
	xport_writel(XPORT_CHL0_CFG_ADDR, XPORT_CHL0_CFG_DEF);

	xport_writel(XPORT_CHL1_BASE_ADDR, (XPORT_CHL1_BASE_ADDR_DEF >> 3));
	xport_writel(XPORT_CHL1_CFG_ADDR, XPORT_CHL1_CFG_DEF);

	xport_writel(XPORT_CFG_ADDR1,
		     XPORT_CFG1_OUT_CHL0_LINE_SYNC |
		     XPORT_CFG1_OUT_CHL1_LINE_SYNC | XPORT_CFG1_OUT_CHL2_LINE_SYNC | XPORT_CFG1_OUT_CHL3_LINE_SYNC);

	return 0;
}
/*
*Set wanted channel with wanted status.
*Xport supports two channels. The first parameter "chl_num" should be 0 or 1.
*The second parameter "stauts" should be 0 or 1. 0 means disable. 1 means enable.
*Notice:Xport supports two tuner inputs.We suppose that xport channel0 is binded to tuner0 input ,channel1 is binded to tuner1.
*/
int xport_set_channel_status(int chl_num,int status)
{
	unsigned int tmp_val = 0;

	/*check parameters*/
	if((chl_num > 1)||(chl_num < 0)||(status > 1)||(status < 0)){
		printf("xport_set_channel_status : invalid parameters\n");
		return (-1);
	}

	/*set wanted channel status*/
	if(chl_num == 0){
		xport_writel(XPORT_CHL0_RP_ADDR, /*chl0_wp | */0x40000000);
		memset((unsigned char*)XPORT_CHL0_BASE_ADDR_DEF, 0, XPORT_CHL0_UNIT_SIZE_DEF * XPORT_CHL0_UNIT_NUM_DEF);
	}
	else if(chl_num == 1){
		xport_writel(XPORT_CHL1_RP_ADDR, /*chl1_wp | */0x40000000);	/* xunli: clear CHL_FIFO */
		memset((unsigned char*)XPORT_CHL1_BASE_ADDR_DEF, 0, XPORT_CHL1_UNIT_SIZE_DEF * XPORT_CHL1_UNIT_NUM_DEF);
	}

	tmp_val = xport_readl((chl_num == 0 ? XPORT_CHL0_CFG_ADDR : XPORT_CHL1_CFG_ADDR));
	if (status == 1)
		tmp_val |= 0x80000000;
	else
		tmp_val &= ~0x80000000;
	xport_writel((chl_num == 0 ? XPORT_CHL0_CFG_ADDR : XPORT_CHL1_CFG_ADDR), tmp_val);

	/*bind channelx to tunerx input*/
	tmp_val = xport_readl(XPORT_TUNER_EN);
	tmp_val &= ~0x00000003;
	if (chl_num == 1)
		tmp_val |= 0x00000002;
	else
		tmp_val |= 0x00000001;
	xport_writel(XPORT_TUNER_EN, tmp_val);

	return (0);
}

/*
*Xport supports three input modes. In u-boot we only support tuner mode. So the second parameter is reserved now.
If you sets xport channel input mode, you will must disable this channel before.
*/
int xport_set_channel_inputmode(int chl_num,int input_mode)
{
	unsigned int tmp_val = 0;
	unsigned int mask_val = 0;

	/*check parameters*/
	if((chl_num > 1)||(chl_num < 0)){
		printf("xport_set_channel_inputmode : invalid parameters\n");
		return (-1);
	}

	unused_variable(input_mode);

	tmp_val = xport_readl(XPORT_CFG_ADDR0);

	if (chl_num == 0) mask_val = 0x00000004;
	else mask_val = 0x00000020;

	tmp_val &= (~mask_val);
	xport_writel(XPORT_CFG_ADDR0, tmp_val);

	tmp_val = xport_readl((chl_num == 0 ? XPORT_CHL0_CFG_ADDR : XPORT_CHL1_CFG_ADDR));
	tmp_val &= ~0x60000000;
	xport_writel((chl_num == 0 ? XPORT_CHL0_CFG_ADDR : XPORT_CHL1_CFG_ADDR), tmp_val);

	/*
	 * The following code segment is copy from kernel xport driver. I'm not sure that still work in u-boot.
	 */
	/*
	 * enable interrupt, according the following flow:
	 * INT0_ENB_ADDR | 0x41, for DEMUX_INPUT_MOD_TUNER.
	 * INT0_ENB_ADDR | 0x41, for DMA_MOD / CHL0.
	 * INT0_ENB_ADDR | 0x82, for DMA_MOD / CHL1.
	 */
	tmp_val = xport_readl(XPORT_INT_ENB_ADDR0);
	if (chl_num == 0)
		tmp_val &= ~0x41;	/* WARNING: this parameters are undocumented, pls don't change it. */
	else
		tmp_val &= ~0x82;	/* WARNING: this parameters are undocumented, pls don't change it. */

	xport_writel(XPORT_INT_ENB_ADDR0, tmp_val);

	return (0);
}

/*
 *  tuner input mode : 0 for parallel, 1 for serial
 */
int xport_set_tuner_input_mode(int chl_num, int tuner_mode)
{
	unsigned int tmp_val = 0;
	unsigned int mask_val = 0;

	/*check parameters*/
	if((chl_num > 1)||(chl_num < 0)){
		printf("xport_set_tuner_input_mode : invalid parameters\n");
		return (-1);
	}

    // set pinmux
    tmp_val = xport_readl(XPORT_PINMUX_TUNER_INPUT_MODE);
    mask_val = (1 << 14);
	tmp_val = (tuner_mode == 0 ? tmp_val & ~mask_val : tmp_val | mask_val);
	xport_writel(XPORT_PINMUX_TUNER_INPUT_MODE, tmp_val);

    // set config reg
	tmp_val = xport_readl(XPORT_CFG_ADDR0);
    mask_val = 0x00000004 << (chl_num * 8);
	tmp_val = (tuner_mode == 0 ? tmp_val & ~mask_val : tmp_val | mask_val);
	xport_writel(XPORT_CFG_ADDR0, tmp_val);

	return (0);
}

/*
*Xport supports 64 PID filter. So the range of second parameter "pidft_index" is between 0 and 63.
*/
int xport_pidft_bind_chl(int chl_num,int pidft_index)
{
	unsigned int tmp_val = 0;

	if((chl_num > 1)||(chl_num < 0)||(pidft_index > 63)||(pidft_index < 0)){
		printf("xport_pidft_bind_chl : invalid parameters\n");
		return (-1);
	}
	tmp_val = xport_readl(__PID_FILTER__(pidft_index));
	if (chl_num == 1)
		tmp_val |= 0x40000000;
	else
		tmp_val &= ~0x40000000;
	xport_writel(__PID_FILTER__(pidft_index), tmp_val);

	return (0);
}

/*
*The second parameter "pid" is pid value which you want to filter TS data.
*/
int xport_pidft_set_pid(int pidft_index,short int pid)
{
	unsigned int tmp_val = 0;

	if((pidft_index > 63)||(pidft_index < 0)){
		printf("xport_pidft_set_pid : invalid parameters\n");
		return (-1);
	}

	tmp_val = xport_readl(__PID_FILTER__(pidft_index));
	tmp_val &= ~0x1fff;
	tmp_val |= pid;
	xport_writel(__PID_FILTER__(pidft_index), tmp_val);

	return (0);
}

/*
*The second parameter "status" should be 0 or 1. 0 means disable this PID filter. 1 means enable.
*/
int xport_pidft_set_status(int pidft_index,int status)
{
	unsigned int tmp_val = 0;

	if((status > 1)||(status < 0)||(pidft_index > 63)||(pidft_index < 0)){
		printf("xport_pidft_set_status : invalid parameters\n");
		return (-1);
	}

	tmp_val = xport_readl(__PID_FILTER__(pidft_index));
	if (status == 1)
		tmp_val |= 0x80000000;
	else
		tmp_val &= ~0x80000000;
	xport_writel(__PID_FILTER__(pidft_index), tmp_val);

	return (0);
}

inline unsigned int FILTER_BUF_LOW(unsigned int filter_index)
{
	switch (filter_index) {
	case 0:
		return FILTER0_BUF_BASE_ADDR;
	case 1:
		return FILTER1_BUF_BASE_ADDR;
	case 2:
		return FILTER2_BUF_BASE_ADDR;
	case 3:
		return FILTER3_BUF_BASE_ADDR;
	case 4:
		return FILTER4_BUF_BASE_ADDR;
	case 5:
		return FILTER5_BUF_BASE_ADDR;
	case 6:
		return FILTER6_BUF_BASE_ADDR;
	case 7:
		return FILTER7_BUF_BASE_ADDR;
	case 8:
		return FILTER8_BUF_BASE_ADDR;
	case 9:
		return FILTER9_BUF_BASE_ADDR;
	case 10:
		return FILTER10_BUF_BASE_ADDR;
	case 11:
		return FILTER11_BUF_BASE_ADDR;
	case 12:
		return FILTER12_BUF_BASE_ADDR;
	case 13:
		return FILTER13_BUF_BASE_ADDR;
	case 14:
		return FILTER14_BUF_BASE_ADDR;
	case 15:
		return FILTER15_BUF_BASE_ADDR;
	case 16:
		return FILTER16_BUF_BASE_ADDR;
	case 17:
		return FILTER17_BUF_BASE_ADDR;
	case 18:
		return FILTER18_BUF_BASE_ADDR;
	case 19:
		return FILTER19_BUF_BASE_ADDR;
	case 20:
		return FILTER20_BUF_BASE_ADDR;
	case 21:
		return FILTER21_BUF_BASE_ADDR;
	case 22:
		return FILTER22_BUF_BASE_ADDR;
	case 23:
		return FILTER23_BUF_BASE_ADDR;
	case 24:
		return FILTER24_BUF_BASE_ADDR;
	case 25:
		return FILTER25_BUF_BASE_ADDR;
	case 26:
		return FILTER26_BUF_BASE_ADDR;
	case 27:
		return FILTER27_BUF_BASE_ADDR;
	case 28:
		return FILTER28_BUF_BASE_ADDR;
	case 29:
		return FILTER29_BUF_BASE_ADDR;
	case 30:
		return FILTER30_BUF_BASE_ADDR;
	case 31:
		return FILTER31_BUF_BASE_ADDR;
	}

	return -1;
}
inline unsigned int FILTER_BUF_UP(unsigned int filter_index)
{
	switch (filter_index) {
	case 0:
		return FILTER0_BUF_BASE_ADDR + FILTER0_BUF_SIZE;
	case 1:
		return FILTER1_BUF_BASE_ADDR + FILTER1_BUF_SIZE;
	case 2:
		return FILTER2_BUF_BASE_ADDR + FILTER2_BUF_SIZE;
	case 3:
		return FILTER3_BUF_BASE_ADDR + FILTER3_BUF_SIZE;
	case 4:
		return FILTER4_BUF_BASE_ADDR + FILTER4_BUF_SIZE;
	case 5:
		return FILTER5_BUF_BASE_ADDR + FILTER5_BUF_SIZE;
	case 6:
		return FILTER6_BUF_BASE_ADDR + FILTER6_BUF_SIZE;
	case 7:
		return FILTER7_BUF_BASE_ADDR + FILTER7_BUF_SIZE;
	case 8:
		return FILTER8_BUF_BASE_ADDR + FILTER8_BUF_SIZE;
	case 9:
		return FILTER9_BUF_BASE_ADDR + FILTER9_BUF_SIZE;
	case 10:
		return FILTER10_BUF_BASE_ADDR + FILTER10_BUF_SIZE;
	case 11:
		return FILTER11_BUF_BASE_ADDR + FILTER11_BUF_SIZE;
	case 12:
		return FILTER12_BUF_BASE_ADDR + FILTER12_BUF_SIZE;
	case 13:
		return FILTER13_BUF_BASE_ADDR + FILTER13_BUF_SIZE;
	case 14:
		return FILTER14_BUF_BASE_ADDR + FILTER14_BUF_SIZE;
	case 15:
		return FILTER15_BUF_BASE_ADDR + FILTER15_BUF_SIZE;
	case 16:
		return FILTER16_BUF_BASE_ADDR + FILTER16_BUF_SIZE;
	case 17:
		return FILTER17_BUF_BASE_ADDR + FILTER17_BUF_SIZE;
	case 18:
		return FILTER18_BUF_BASE_ADDR + FILTER18_BUF_SIZE;
	case 19:
		return FILTER19_BUF_BASE_ADDR + FILTER19_BUF_SIZE;
	case 20:
		return FILTER20_BUF_BASE_ADDR + FILTER20_BUF_SIZE;
	case 21:
		return FILTER21_BUF_BASE_ADDR + FILTER21_BUF_SIZE;
	case 22:
		return FILTER22_BUF_BASE_ADDR + FILTER22_BUF_SIZE;
	case 23:
		return FILTER23_BUF_BASE_ADDR + FILTER23_BUF_SIZE;
	case 24:
		return FILTER24_BUF_BASE_ADDR + FILTER24_BUF_SIZE;
	case 25:
		return FILTER25_BUF_BASE_ADDR + FILTER25_BUF_SIZE;
	case 26:
		return FILTER26_BUF_BASE_ADDR + FILTER26_BUF_SIZE;
	case 27:
		return FILTER27_BUF_BASE_ADDR + FILTER27_BUF_SIZE;
	case 28:
		return FILTER28_BUF_BASE_ADDR + FILTER28_BUF_SIZE;
	case 29:
		return FILTER29_BUF_BASE_ADDR + FILTER29_BUF_SIZE;
	case 30:
		return FILTER30_BUF_BASE_ADDR + FILTER30_BUF_SIZE;
	case 31:
		return FILTER31_BUF_BASE_ADDR + FILTER31_BUF_SIZE;
	}

	return -1;
}

int xport_filter_clear_buffer(unsigned int filter_index)
{
	xport_mips_write(FILTER_BUF_LOW_ADDR(filter_index), FILTER_BUF_LOW(filter_index));
	xport_mips_write(FILTER_BUF_UP_ADDR(filter_index), FILTER_BUF_UP(filter_index));

	xport_writel(FILTER_BUF_RP_ADDR(filter_index), FILTER_BUF_LOW(filter_index));
	xport_writel(FILTER_BUF_WP_ADDR(filter_index), FILTER_BUF_LOW(filter_index));

	return (0);
}

int xport_filter_check_section_number(unsigned int filter_index)
{
	unsigned int len;
	unsigned int wp, wp_tog;
	unsigned int rp, rp_tog;
	unsigned int up_addr, low_addr;
	unsigned int totalcnt = 0, counter = 0, i = 0;

	up_addr = FILTER_BUF_UP(filter_index);
	low_addr = FILTER_BUF_LOW(filter_index);

	wp = xport_readl(FILTER_BUF_WP_ADDR(filter_index));
	rp = xport_readl(FILTER_BUF_RP_ADDR(filter_index));

	wp_tog = wp & 0x80000000;
	rp_tog = rp & 0x80000000;

	wp = wp & 0x7fffffff;
	rp = rp & 0x7fffffff;

	if (wp_tog == rp_tog) {
		len = wp - rp;
	}
	else {
		len = (up_addr - low_addr) + wp - rp;
	}

	len = len & 0xfffff000;

	counter = len / 4096;

	for(i = 0; i < counter; i ++){
		len = ((unsigned char *) rp)[1];
		len <<= 8;
		len += ((unsigned char *) rp)[2];
		len &= 0xfff;
		len += 3;
		totalcnt +=len;

		rp += 0x1000;

		if (rp >= FILTER_BUF_UP(filter_index)) {
			rp = FILTER_BUF_LOW(filter_index);
		}
	}

	return totalcnt;
}

int xport_filter_check_section_count(unsigned int filter_index)
{
	unsigned int len;
	unsigned int wp, wp_tog;
	unsigned int rp, rp_tog;
	unsigned int up_addr, low_addr;

	up_addr = FILTER_BUF_UP(filter_index);
	low_addr = FILTER_BUF_LOW(filter_index);

	wp = xport_readl(FILTER_BUF_WP_ADDR(filter_index));
	rp = xport_readl(FILTER_BUF_RP_ADDR(filter_index));

	wp_tog = wp & 0x80000000;
	rp_tog = rp & 0x80000000;

	wp = wp & 0x7fffffff;
	rp = rp & 0x7fffffff;

	if (wp_tog == rp_tog) {
		len = wp - rp;
	}
	else {
		len = (up_addr - low_addr) + wp - rp;
	}

	len = len/4096;

	return len;
}

int xport_filter_set_filter(int filter_index, unsigned char *filter, unsigned char *mask)
{
	xport_mips_write(FILTER_FILTER0_ADDR(filter_index), *(unsigned int *) filter);
	xport_mips_write(FILTER_FILTER1_ADDR(filter_index), *(unsigned int *) (filter + 4));
	xport_mips_write(FILTER_FILTER2_ADDR(filter_index), *(unsigned int *) (filter + 8));
	xport_mips_write(FILTER_MASK0_ADDR(filter_index), *(unsigned int *) mask);
	xport_mips_write(FILTER_MASK1_ADDR(filter_index), *(unsigned int *) (mask + 4));
	xport_mips_write(FILTER_MASK2_ADDR(filter_index), *(unsigned int *) (mask + 8));

	return (0);
}

int xport_filter_set_pid(unsigned int filter_index, unsigned int pid)
{
	return xport_mips_write(FILTER_PID_ADDR(filter_index), pid);
}

int xport_filter_set_type(int filter_index, int filter_type)
{
	unsigned int en;
	int rt;

	rt = xport_mips_read(FILTER_EN_ADDR(filter_index), &en);

	en >>= 31;		/* bit31: enable flag: 1 - output channel enabled */

	if ((rt == 0) && (en == 0)) {
		if (filter_type == 0)
			en = 3;	/* section output */
		else if (filter_type == 1)
			en = 1;	/* TS output */
		else if (filter_type == 2)
			en = 4;	/* PES output */
		else if (filter_type == 3)
			en = 2;	/* ES output */
		else
			en = 0;	/* reserved, no output type */

		xport_mips_write(FILTER_EN_ADDR(filter_index), en);

		return 0;
	}

	return (-1);
}

/*
*The second parameter "status" should be 0 or 1. 0 means disable this filter. 1 means enable.
*/
int xport_filter_set_status(int filter_index, int status)
{
	int rt;
	unsigned int en;
	unsigned long irq1_en;

	if(status == 1){/*enable*/
		rt = xport_mips_read(FILTER_EN_ADDR(filter_index), &en);

		if (rt != 0)
			return (-1);

		xport_writel(FILTER_BUF_WP_ADDR(filter_index), FILTER_BUF_LOW(filter_index));
		xport_writel(FILTER_BUF_RP_ADDR(filter_index), FILTER_BUF_LOW(filter_index));

		irq1_en = xport_readl(XPORT_INT_ENB_ADDR1);
		irq1_en |= (1 << filter_index);

		xport_writel(XPORT_INT_CLS_ADDR1, (1 << filter_index));
		xport_writel(XPORT_INT_ENB_ADDR1, irq1_en);

		if(xport_mips_write(FILTER_EN_ADDR(filter_index), 0x80000000 | en))
			return (-1);
	}
	else{/*disable*/
		irq1_en = xport_readl(XPORT_INT_ENB_ADDR1);
		irq1_en &= (~(1 << filter_index));
		xport_writel(XPORT_INT_ENB_ADDR1, irq1_en);

		if(xport_mips_write(FILTER_DISABLE_ADDR(filter_index), 0))
			return (-1);
	}

	xport_filter_clear_buffer(filter_index);

	return (0);
}

int xport_filter_wait_data(int filter_index, int ms_timeout)
{
	unsigned int sec_num = 0;
	unsigned int timeout = 0;

	for(timeout =0 ; timeout < ms_timeout;){
		sec_num = xport_filter_check_section_number(filter_index);
		if(sec_num == 0){
			mdelay(1);
			timeout +=1;
		}
		else{
			return (0);
		}
	}

	return (-1);
}

int xport_fliter_read_data(int filter_index, unsigned char *buf, unsigned int size)
{
	unsigned int rp, rp_tog;
	unsigned int section_len;

	if((buf == NULL)||(size > 4096)||(size < 1)){
		printf("xport_fliter_read_data: invalid parameters!\n");
		return (-1);
	}

	if (xport_filter_check_section_number(filter_index) == 0) {
		return (-1);
	}

	rp = xport_readl(FILTER_BUF_RP_ADDR(filter_index));
	rp_tog = rp & 0x80000000;
	rp = rp & 0x7fffffff;

	section_len = ((unsigned char *) rp)[1];
	section_len <<= 8;
	section_len += ((unsigned char *) rp)[2];
	section_len &= 0xfff;
	section_len += 3;

	if (section_len > 4096 || section_len > size) {
		return (-1);
	}

	memcpy(buf, (unsigned char *)rp, section_len);

	rp += 0x1000;

	if (rp >= FILTER_BUF_UP(filter_index)) {
		rp = FILTER_BUF_LOW(filter_index);
		rp_tog = rp_tog ^ 0x80000000;
	}
	xport_writel(FILTER_BUF_RP_ADDR(filter_index), rp_tog | rp);

	return section_len;
}

int xport_init(void)
{
	if(_xport_load_firmware())
		return (-1);

	xport_hw_init();

#if 0
{
	unsigned char filter[12];
	unsigned char mask[12];
	unsigned int return_len = 0;
	unsigned char buf[4096];

	memset(filter, 0 ,12);
	memset(mask, 0 ,12);

	filter[0] = 0x00;
	mask[0]   = 0xff;

	if(xport_set_channel_status(0, 0))
		return (-1);

	if(xport_set_channel_inputmode(0, 0))
		return (-1);

	if(xport_set_channel_status(0, 1))
		return (-1);

	xport_pidft_bind_chl(0, 1);
	xport_pidft_set_pid(1, 0x00);
	xport_pidft_set_status(1, 1);

	xport_filter_set_filter(0, filter, mask);
	xport_filter_set_pid(0, 0x00);
	xport_filter_set_type(0, 0);
	xport_filter_set_status(0, 1);

	while(1){
		if(xport_filter_wait_data(0, 5*1000) == 0){
			return_len = xport_fliter_read_data(0, buf, 4096);
			printf("readed length %d\n",return_len);
			printf("filter0:section  = 0x%02x, 0x%02x, 0x%02x,  0x%02x, 0x%02x, 0x%02x,0x%02x, 0x%02x, 0x%02x\n", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8]);
		}
		else{
			printf("read wait\n");
		}
	}
}
#endif
	return (0);
}
