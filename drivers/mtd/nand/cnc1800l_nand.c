/*
 * (C) Copyright 2007 STMicroelectronics, <www.st.com>
 * (C) Copyright 2009 Alessandro Rubini <rubini@unipv.it>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <nand.h>
#include <asm/io.h>
#include <linux/err.h>

#define SMC_STATUS (CNC1800L_NAND_CFG_BASE + 0x30)

#define ECC_CODE_OFFSET 0x40
#define ECC_CNT_OFFSET 0x44
#define ECC_A_OFFSET  0x48

static inline unsigned int csm_nand_readl(int offset)
{
	return *((unsigned int *)(CNC1800L_NAND_CFG_BASE + offset));
}

static inline void csm_nand_writel(int offset, unsigned long value)
{
	*((unsigned int *) (CNC1800L_NAND_CFG_BASE + offset)) = value;
}

/*
 * Read hardware ECC value and pack into three bytes
 */
static int cnd_nand_ecc_calculate(struct mtd_info *mtd,
		const u_char *dat, u_char *ecc_code)
{

	unsigned int ecc24 = csm_nand_readl(ECC_CODE_OFFSET);

	ecc_code[0] = (u_char)(ecc24);
	ecc_code[1] = (u_char)(ecc24 >> 8);
	ecc_code[2] = (u_char)(ecc24 >> 16);

	return 0;
}


/* Software ECC to enable Multi Byte reading for accelerating NAND performace */

static inline void make_ecc_512(u_char * ecc_buf, const u_char * data_buf)
{
 
    uint32_t i, ALIGN_FACTOR; 
    uint32_t tmp;
    uint32_t uiparity = 0;
    uint32_t parityCol, ecc = 0;
    uint32_t parityCol4321 = 0, parityCol4343 = 0, parityCol4242 = 0, parityColTot = 0;
    uint32_t *Data;
    uint32_t Xorbit=0;

    ALIGN_FACTOR = (uint32_t)data_buf % 4 ;
    Data = (uint32_t *)(data_buf + ALIGN_FACTOR);

    for( i = 0; i < 16; i++)
        {
            parityCol = *Data++; 
            tmp = *Data++; parityCol ^= tmp; parityCol4242 ^= tmp;
            tmp = *Data++; parityCol ^= tmp; parityCol4343 ^= tmp;
            tmp = *Data++; parityCol ^= tmp; parityCol4343 ^= tmp; parityCol4242 ^= tmp;
            tmp = *Data++; parityCol ^= tmp; parityCol4321 ^= tmp;
            tmp = *Data++; parityCol ^= tmp; parityCol4242 ^= tmp; parityCol4321 ^= tmp;
            tmp = *Data++; parityCol ^= tmp; parityCol4343 ^= tmp; parityCol4321 ^= tmp;
            tmp = *Data++; parityCol ^= tmp; parityCol4242 ^= tmp; parityCol4343 ^= tmp; parityCol4321 ^= tmp;

            parityColTot ^= parityCol;

            tmp = (parityCol >> 16) ^ parityCol;
            tmp = (tmp >> 8) ^ tmp;
            tmp = (tmp >> 4) ^ tmp;
            tmp = ((tmp >> 2) ^ tmp) & 0x03;
            if ((tmp == 0x01) || (tmp == 0x02))
                {
                    uiparity ^= i;
                    Xorbit ^= 0x01;
                }
        }

    tmp = (parityCol4321 >> 16) ^ parityCol4321;
    tmp = (tmp << 8) ^ tmp;
    tmp = (tmp >> 4) ^ tmp;
    tmp = (tmp >> 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x200; // p128
 
    tmp = (parityCol4343 >> 16) ^ parityCol4343;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp << 4) ^ tmp;
    tmp = (tmp << 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x80; // p64

    tmp = (parityCol4242 >> 16) ^ parityCol4242;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp << 4) ^ tmp;
    tmp = (tmp >> 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x20; // p32

    tmp = parityColTot & 0xFFFF0000;
    tmp = tmp >> 16;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp >> 4) ^ tmp;
    tmp = (tmp << 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x08; // p16

    tmp = parityColTot & 0xFF00FF00;
    tmp = (tmp >> 16) ^ tmp;
    tmp = (tmp >> 8);
    tmp = (tmp >> 4) ^ tmp;
    tmp = (tmp >> 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x02; // p8

    tmp = parityColTot & 0xF0F0F0F0 ;
    tmp = (tmp << 16) ^ tmp;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp << 2) ^ tmp;
    ecc |= ((tmp << 1) ^ tmp) & 0x800000; // p4

    tmp = parityColTot & 0xCCCCCCCC ;
    tmp = (tmp << 16) ^ tmp;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp << 4) ^ tmp;
    tmp = (tmp >> 2);
    ecc |= ((tmp << 1) ^ tmp) & 0x200000; // p2

    tmp = parityColTot & 0xAAAAAAAA ;
    tmp = (tmp << 16) ^ tmp;
    tmp = (tmp >> 8) ^ tmp;
    tmp = (tmp >> 4) ^ tmp;
    tmp = (tmp << 2) ^ tmp;
    ecc |= (tmp & 0x80000); // p1

    ecc |= (uiparity & 0x01) <<11; 
    ecc |= (uiparity & 0x02) <<12; 
    ecc |= (uiparity & 0x04) <<13;
    ecc |= (uiparity & 0x08) <<14;

    if (Xorbit)
        {
            ecc |= (ecc ^ 0x00AAAAAA)>>1;
        }
    else
        {
            ecc |= (ecc >> 1);
        }
 
    ecc = ~ecc;
    *(ecc_buf + 2) = (u_char) (ecc >> 16);
    *(ecc_buf + 0) = (u_char) (ecc >> 8);
    *(ecc_buf + 1) = (u_char) (ecc);
}


static int cnd_nand_ecc_soft_calculate(struct mtd_info *mtd,
		const u_char *dat, u_char *ecc_code)
{
    //    printf("soft ecc calc 512 data=0x%x\n",dat);
    //    dcache_enable();
    make_ecc_512(ecc_code, dat);
    // dcache_disable();
	return 0;
}



static int csm_nand_ecc_correct(struct mtd_info *mtd, u_char *dat,
		u_char *read_ecc, u_char *calc_ecc)
{

	uint32_t eccNand = read_ecc[0] | (read_ecc[1] << 8) |
		(read_ecc[2] << 16);
	uint32_t eccCalc = calc_ecc[0] | (calc_ecc[1] << 8) |
		(calc_ecc[2] << 16);
	uint32_t diff = eccCalc ^ eccNand;
	unsigned int bit, byte;

    //    printf("ecc correct 512 dat=0x%x\n",dat);
	if (diff) {
		if ((((diff >> 1) ^ diff) & 0x555555) == 0x555555) {
			/* Correctable error */
			printf("diff=0x%x:1bit error , correction\n",diff);
			/* calculate the bit position of the error */
			bit  = ((diff >> 19) & 1) |
				((diff >> 20) & 2) |
				((diff >> 21) & 4);
			/* calculate the byte position of the error */

			byte = ((diff >> 9) & 0x100) |
				(diff  & 0x80)  |
				((diff << 1) & 0x40)  |
				((diff << 2) & 0x20)  |
				((diff << 3) & 0x10)  |
				((diff >> 12) & 0x08)  |
				((diff >> 11) & 0x04)  |
				((diff >> 10) & 0x02)  |
				((diff >> 9) & 0x01);

			printf("byte=%d, bit=%d, dat[byte]=0x%x\n",byte,bit,dat[byte]);
			dat[byte] ^= (1 << bit);
			printf("corrected dat[byte]=0x%x\n",dat[byte]);
			return 1;
		} else if (!(diff & (diff - 1))) {
			/* Single bit ECC error in the ECC itself,
			 * nothing to fix */
			printf("ECC error, not correction needed\n");
			return 1;
		} else {
			/* Uncorrectable error */
			printf("more than 1bit error\n");
			return -1;
		}
	}
	return 0;
}

static void csm_nand_ecc_hwctl(struct mtd_info *mtd , int mode)
{
	/* Restart ECC hardware */

	struct nand_chip *this = mtd->priv;
	if (mode == NAND_ECC_READ)
		csm_nand_writel(ECC_A_OFFSET, (unsigned long)this->IO_ADDR_R);
	else if (mode == NAND_ECC_WRITE)
		csm_nand_writel(ECC_A_OFFSET, (unsigned long)this->IO_ADDR_W);
}

#define MASK_ALE	(1 << 3)
#define MASK_CLE	(1 << 2)

static void cnc1800l_nand_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct          nand_chip *this = mtd->priv;
	u_int32_t       IO_ADDR_W = (u_int32_t)this->IO_ADDR_W;
	unsigned int    value;

	if (ctrl & NAND_CTRL_CHANGE) {
		IO_ADDR_W &= ~(MASK_ALE|MASK_CLE);

		if (ctrl & NAND_CLE) {
			IO_ADDR_W |= MASK_CLE;
		}

		if (ctrl & NAND_ALE) {
			IO_ADDR_W |= MASK_ALE;
		}

		if (ctrl & NAND_NCE) {
			value = *((unsigned int *)CNC1800L_NAND_CFG_BASE);
			*((unsigned int *)(CNC1800L_NAND_CFG_BASE)) = value | 0x00000200;
		} else {
			value = *((unsigned int *)CNC1800L_NAND_CFG_BASE);
			*((unsigned int *)(CNC1800L_NAND_CFG_BASE)) = value & (~0x00000200);
		}

		this->IO_ADDR_W = (void __iomem *) IO_ADDR_W;
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, IO_ADDR_W);
}

/* return 1: ready  0: not ready*/
static int cnc1800l_device_ready(struct mtd_info *mtd)
{
    return ((*((int *)(SMC_STATUS))) & 0x1);
    
}

/* Copy a buffer 32bits at a time: faster than default method which is 8bit */
static void cnc1800l_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	int i;
	struct nand_chip *chip = mtd->priv;
	u32 *p = (u32 *) buf;
	void * io_r = chip->IO_ADDR_R;
    //    printf("readbuf_len=%d\n", len);

	len >>= 2;
    for (i = 0; i < len; i++)
        p[i] = readl(io_r);
}


/**
 * nand_verify_buf - Verify chip data against buffer
 * @mtd:	MTD device structure
 * @buf:	buffer containing the data to compare
 * @len:	number of bytes to compare
 *
 * cnc1800l fast verify function for 8bit buswith
 */
static int cnc1800l_nand_verify_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	int i;
	struct nand_chip *chip = mtd->priv;
	u32 *p = (u32 *) buf;
	void * io_r = chip->IO_ADDR_R;

	len >>= 2;

	for (i = 0; i < len; i++)
        if (p[i] != readl(io_r))
			return -EFAULT;
	return 0;
}

int board_nand_init(struct nand_chip *chip)
{
	chip->options = NAND_USE_FLASH_BBT | NAND_BBT_LASTBLOCK | NAND_BBT_CREATE | NAND_BBT_VERSION | NAND_BBT_WRITE;
	chip->chip_delay = 20;
	chip->IO_ADDR_R = (unsigned char *)CONFIG_SYS_NAND_BASE;
	chip->IO_ADDR_W = (unsigned char *)CONFIG_SYS_NAND_BASE;
	chip->cmd_ctrl = cnc1800l_nand_hwcontrol;
   	chip->dev_ready = cnc1800l_device_ready;

	/* The chip allows 32bit reads, so avoid the default 8bit copy */
	chip->read_buf = cnc1800l_nand_read_buf;
    chip->verify_buf = cnc1800l_nand_verify_buf;

	/* ECC: follow the hardware-defined rulse, but do it in sw */
	chip->ecc.mode = NAND_ECC_HW;
	chip->ecc.bytes = 3;
	chip->ecc.size = 512;
	chip->ecc.calculate = cnd_nand_ecc_soft_calculate; //cnd_nand_ecc_calculate;
	chip->ecc.hwctl = csm_nand_ecc_hwctl;
	chip->ecc.correct = csm_nand_ecc_correct;

	return 0;
}

