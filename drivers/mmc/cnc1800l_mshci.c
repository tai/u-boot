/*
 * (C) Copyright 2011 Cavium Inc.
 * Scott Shu <sshu@caviumnetworks.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <asm/io.h>

#include <asm/arch/cnc1800l.h>
#include <asm/arch/mshci.h>

DECLARE_GLOBAL_DATA_PTR;

#define DEBUG_CNC1800L_MSHCI
 #undef DEBUG_CNC1800L_MSHCI
#ifdef DEBUG_CNC1800L_MSHCI
#define dbg(x...)       printf(x)
#else
#define dbg(x...)       do { } while (0)
#endif

#ifndef mdelay
#define mdelay(x)	udelay(1000*x)
#endif

#define MMC_MAX_CHANNEL		1

struct mmc mmc_channel[MMC_MAX_CHANNEL];

struct mshci_host mmc_host[MMC_MAX_CHANNEL];

static struct mshci_idmac idmac_desc[0x10000]; /* it can cover to transfer 256MB at a time */

static void mshci_reset_all(struct mshci_host *host);

static void mshci_reset_ciu(struct mshci_host *host)
{
	u32 timeout = 100;
	u32 ier;

	ier = readl(host->ioaddr + MSHCI_CTRL);
	ier |= CTRL_RESET;

	writel(ier, host->ioaddr + MSHCI_CTRL);

	while (readl(host->ioaddr + MSHCI_CTRL) & CTRL_RESET) {
		if (timeout == 0) {
			printf("Reset CTRL never completed.\n");
			return;
		}
		timeout--;
		mdelay(1);
	}
}

static void mshci_reset_fifo(struct mshci_host *host)
{
	u32 timeout = 100;
	u32 ier;

	ier = readl(host->ioaddr + MSHCI_CTRL);
	ier |= FIFO_RESET;

	writel(ier, host->ioaddr + MSHCI_CTRL);
	while (readl(host->ioaddr + MSHCI_CTRL) & FIFO_RESET) {
		if (timeout == 0) {
			printf("Reset FIFO never completed.\n");
			return;
		}
		timeout--;
		mdelay(1);
	}
}

static void mshci_reset_dma(struct mshci_host *host)
{
	u32 timeout = 100;
	u32 ier;

	ier = readl(host->ioaddr + MSHCI_CTRL);
	ier |= DMA_RESET;

	writel(ier, host->ioaddr + MSHCI_CTRL);
	while (readl(host->ioaddr + MSHCI_CTRL) & DMA_RESET) {
		if (timeout == 0) {
			printf("Reset DMA never completed.\n");
			return;
		}
		timeout--;
		mdelay(1);
	}
}

static void mshci_reset_all(struct mshci_host *host)
{
	mshci_reset_ciu(host);
	mshci_reset_dma(host);
	mshci_reset_fifo(host);
}

static void mshci_set_mdma_desc(u8 *desc_vir, u8 *desc_phy,
				u32 des0, u32 des1, u32 des2)
{
	((struct mshci_idmac *)(desc_vir))->des0 = des0;
	((struct mshci_idmac *)(desc_vir))->des1 = des1;
	((struct mshci_idmac *)(desc_vir))->des2 = des2;
	((struct mshci_idmac *)(desc_vir))->des3 = (u32)desc_phy + sizeof(struct mshci_idmac);
}

static void mshci_prepare_data(struct mshci_host *host, struct mmc_data *data)
{
	u32 i;
	u32 data_cnt;
	u32 des_flag;
	u32 blksz;

	struct mshci_idmac *pdesc_dmac;

	mshci_reset_fifo(host);

	pdesc_dmac = idmac_desc;

 	dbg("data->blocks: %d data->blocksize: %d desc 0x%x\n",
		(u32)data->blocks, (u32)data->blocksize, idmac_desc);
	blksz = data->blocksize;
	data_cnt = data->blocks;

	for ( i = 0;; i++) {
		des_flag = (MSHCI_IDMAC_OWN|MSHCI_IDMAC_CH);
		des_flag |= (i==0) ? MSHCI_IDMAC_FS:0;
		if (data_cnt <= 8) {
			des_flag |= MSHCI_IDMAC_LD;
			mshci_set_mdma_desc((u8 *)pdesc_dmac, (u8 *)virt_to_phys((u32)pdesc_dmac),
					    des_flag, blksz * data_cnt,
					    (u32)(virt_to_phys((u32)data->dest))+(u32)(i*0x1000));
			break;
		}
		/* max transfer size is 4KB per a description. */
		mshci_set_mdma_desc((u8 *)pdesc_dmac, (u8 *)virt_to_phys((u32)pdesc_dmac),
				    des_flag, blksz * 8,
				    virt_to_phys((u32)data->dest)+(u32)(i*0x1000));

		data_cnt-= 8;
		pdesc_dmac++;
	}

	writel((u32)virt_to_phys((u32)idmac_desc), host->ioaddr + MSHCI_DBADDR);

	/* enable DMA, IDMAC */
	writel( (readl(host->ioaddr + MSHCI_CTRL) | ENABLE_IDMAC|DMA_ENABLE), host->ioaddr + MSHCI_CTRL);
	writel( (readl(host->ioaddr + MSHCI_BMOD) | (BMOD_IDMAC_ENABLE|BMOD_IDMAC_FB)), host->ioaddr + MSHCI_BMOD);

	writel(data->blocksize, host->ioaddr + MSHCI_BLKSIZ);
	writel((data->blocksize * data->blocks),host->ioaddr + MSHCI_BYTCNT);
}

static int mshci_set_transfer_mode(struct mshci_host *host, struct mmc_data *data)
{
	int mode = 0;

	/* this cmd has data to transmit */
	mode |= CMD_DATA_EXP_BIT;
	if (data->blocks > 1)
		mode |= CMD_SENT_AUTO_STOP_BIT;
	if (data->flags & MMC_DATA_WRITE)
		mode |= CMD_RW_BIT;

	return mode;
}

#define COMMAND_TIMEOUT (0x200000)

/*
 * Sends a command out on the bus.  Takes the mmc pointer,
 * a command pointer, and an optional data pointer.
 */
static int mshci_send_command(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	struct mshci_host *host = mmc->priv;

	int flags = 0,i;
	u32 mask;
	uint timeout = 0x1000000;

	/* check SDMMC card is busy */
	while (mshci_readl(host, MSHCI_STATUS) & (1<<9)) {
		if (timeout == 0) {
			printf("MSHC: Controller never released "
				"data0.\n");
			return TIMEOUT;
		}
		timeout--;
	}
	/* check there are any interrupts on RINTSTS */
	if ( mshci_readl(host,MSHCI_RINTSTS) &&
			(mshci_readl(host,MSHCI_RINTSTS) != 0x4000))
		printf("there are pending interrupts 0x%x\n",
				mshci_readl(host,MSHCI_RINTSTS));
	/* It clears all pending interrupts before sending a command*/
	mshci_writel(host, INTMSK_ALL, MSHCI_RINTSTS);

	if (data)
		mshci_prepare_data(host, data);

	dbg("cmd->cmdidx %2d cmd->arg: 0x%08x\n", cmd->cmdidx, cmd->cmdarg);
	writel(cmd->cmdarg, host->ioaddr + MSHCI_CMDARG);

	if (data)
		flags = mshci_set_transfer_mode(host, data);

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY)) {
		/* this is out of SD spec */
		return -1;
	}

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		flags |= CMD_RESP_EXP_BIT;
		if (cmd->resp_type & MMC_RSP_136)
			flags |= CMD_RESP_LENGTH_BIT;
	}

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= CMD_CHECK_CRC_BIT;

	flags |= (cmd->cmdidx | CMD_STRT_BIT | CMD_WAIT_PRV_DAT_BIT);

	mask = mshci_readl(host, MSHCI_CMD);
	if (mask & CMD_STRT_BIT)
		printf("CMD busy. current cmd %d. last cmd reg 0x%x\n",
			cmd->cmdidx, mask);

	mshci_writel(host, flags, MSHCI_CMD);

	/* wait for command complete by busy waiting. */
	for (i=0; i<COMMAND_TIMEOUT; i++) {
		mask = readl(host->ioaddr + MSHCI_RINTSTS);
		if (mask & INTMSK_CDONE) {
			if (!data)
				writel(mask, host->ioaddr + MSHCI_RINTSTS);
			break;
		}
	}

	/* timeout for command complete. */
	if (COMMAND_TIMEOUT == i) {
		printf("FAIL: waiting for status update.\n");
		return TIMEOUT;
	}

	if (mask & INTMSK_RTO) {
		printf("response timeout error : %08x cmd %d\n", mask, cmd->cmdidx);
		return TIMEOUT;
	}
	else if (mask & INTMSK_RE) {
		printf("response error : %08x cmd %d\n", mask, cmd->cmdidx);
	}

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136) {
			/* CRC is stripped so we need to do some shifting. */
				cmd->response[0] = mshci_readl(host, MSHCI_RESP3);
				cmd->response[1] = mshci_readl(host, MSHCI_RESP2);
				cmd->response[2] = mshci_readl(host, MSHCI_RESP1);
				cmd->response[3] = mshci_readl(host, MSHCI_RESP0);
		} else {
			cmd->response[0] = readl(host->ioaddr + MSHCI_RESP0);
		}
	}

	if (data) {
		while (!(mask & (DATA_ERR | DATA_TOUT | INTMSK_DTO ))) {
			mask = readl(host->ioaddr + MSHCI_RINTSTS);
		}
		writel(mask, host->ioaddr + MSHCI_RINTSTS);
		if (mask & (DATA_ERR | DATA_TOUT)) {
			printf("cmd %d arg 0x%x error during transfer : "
				"0x%08x\n", cmd->cmdidx, cmd->cmdarg, mask);

			/* make sure disable IDMAC and IDMAC_Interrupts */
			mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) &
					~(DMA_ENABLE|ENABLE_IDMAC)), MSHCI_CTRL);
			/* mask all interrupt source of IDMAC */
			mshci_writel(host, 0x0, MSHCI_IDINTEN);

			return -1;
		} else if (mask & INTMSK_DTO) {
			dbg("SDHCI_INT_DMA_END\n");
		} else {
			printf("unexpected condition 0x%x\n",mask);
		}
		/* make sure disable IDMAC and IDMAC_Interrupts */
		mshci_writel(host, (mshci_readl(host, MSHCI_CTRL) &
				~(DMA_ENABLE|ENABLE_IDMAC)), MSHCI_CTRL);
		/* mask all interrupt source of IDMAC */
		mshci_writel(host, 0x0, MSHCI_IDINTEN);
	}

	return 0;
}

static void mshci_clock_onoff(struct mshci_host *host, int val)
{
	volatile u32 loop_count = 0x100000;

	if (val) {
		mshci_writel(host, (0x1<<0), MSHCI_CLKENA);
		mshci_writel(host, 0, MSHCI_CMD);
		mshci_writel(host, CMD_ONLY_CLK, MSHCI_CMD);
		do {
			if (!(mshci_readl(host, MSHCI_CMD) & CMD_STRT_BIT))
				break;
			loop_count--;
		} while (loop_count);
	} else {
		mshci_writel(host, (0x0<<0), MSHCI_CLKENA);
		mshci_writel(host, 0, MSHCI_CMD);
		mshci_writel(host, CMD_ONLY_CLK, MSHCI_CMD);
		do {
			if (!(mshci_readl(host, MSHCI_CMD) & CMD_STRT_BIT))
				break;
			loop_count--;
		} while (loop_count);
	}
	if (loop_count == 0) {
		printf("Clock %s has been failed.\n ",val ? "ON":"OFF");
	}
}

#define MAX_EMMC_CLOCK	(40000000) /* max clock 40Mhz */

static void mshci_change_clock(struct mshci_host *host, uint clock)
{
	int div;
	volatile u32 loop_count;

	if (clock == host->clock)
		return;

	/* befor changing clock. clock needs to be off. */
	mshci_clock_onoff(host, CLK_DISABLE);

	if (clock == 0)
		goto out;

	if (clock >= MAX_EMMC_CLOCK) {
		div = 1;
	} else {
		for (div = 1;div < 255;div++) {
			if ((MAX_EMMC_CLOCK / (div<<1)) <= clock)
				break;
		}
	}

	mshci_writel(host, div, MSHCI_CLKDIV);

	mshci_writel(host, 0, MSHCI_CMD);
	mshci_writel(host, CMD_ONLY_CLK, MSHCI_CMD);
	loop_count = 0x10000000;

	do {
		if (!(mshci_readl(host, MSHCI_CMD) & CMD_STRT_BIT))
			break;
		loop_count--;
	} while(loop_count);

	if (loop_count == 0)
		printf("Changing clock has been failed.\n ");

	mshci_writel(host, mshci_readl(host, MSHCI_CMD)&(~CMD_SEND_CLK_ONLY), MSHCI_CMD);

	mshci_clock_onoff(host, CLK_ENABLE);
out:
	host->clock = clock;

	return;
}

static void mshci_set_ios(struct mmc *mmc)
{
	struct mshci_host *host = mmc->priv;

	mshci_change_clock(host, mmc->clock);

	if (mmc->bus_width == 8) {
		mshci_writel(host, (0x1<<16), MSHCI_CTYPE);
		mshci_writel(host, (0x0<<0), MSHCI_UHS_REG);
		mshci_writel(host, (0x00010001), MSHCI_CLKSEL);
	} else if (mmc->bus_width == 4) {
		mshci_writel(host, (0x1<<0), MSHCI_CTYPE);
		mshci_writel(host, (0x0<<0), MSHCI_UHS_REG);
		mshci_writel(host, (0x00010001), MSHCI_CLKSEL);
	} else {
		mshci_writel(host, 0, MSHCI_UHS_REG);
		mshci_writel(host, (0x0<<0), MSHCI_CTYPE);
		mshci_writel(host, (0x00010001), MSHCI_CLKSEL);
	}
}

static void mshci_fifo_init(struct mshci_host *host)
{
	int fifo_val, fifo_depth, fifo_threshold;

	fifo_val = mshci_readl(host, MSHCI_FIFOTH);
	fifo_depth = ((fifo_val & RX_WMARK)>>16)+1;
	fifo_threshold = fifo_depth/2;

	printf("FIFO WMARK FOR RX 0x%x WX 0x%x.\n",
		fifo_depth,((fifo_val & TX_WMARK)>>16)+1  );

	fifo_val &= ~(RX_WMARK | TX_WMARK | MSIZE_MASK);

	fifo_val |= (fifo_threshold | (fifo_threshold<<16));
	fifo_val |= MSIZE_8;

	mshci_writel(host, fifo_val, MSHCI_FIFOTH);
}

static int mshci_init(struct mmc *mmc)
{
	struct mshci_host *host = (struct mshci_host *)mmc->priv;
	int count;

	mshci_reset_all(host);

	/* It clears all pending interrupts */
	mshci_writel(host, INTMSK_ALL, MSHCI_RINTSTS);
	/* It dose not use Interrupt. Disable all */
	mshci_writel(host, 0, MSHCI_INTMSK);

	mshci_change_clock(host, 400000);

	/* set debounce filter value*/
	mshci_writel(host, 0xfffff, MSHCI_DEBNCE);

	/* clear card type. set 1bit mode */
	mshci_writel(host, 0x0, MSHCI_CTYPE);

	/* set bus mode register for IDMAC */
	mshci_writel(host, BMOD_IDMAC_RESET, MSHCI_BMOD);
	count = 0x1000;
	while( (mshci_readl(host, MSHCI_BMOD) & BMOD_IDMAC_RESET )
		&& --count ) ; /* nothing to do */

	if (!count)
		printf("It can not reset IDMAC.!\n");

	/* disable all interrupt source of IDMAC */
	mshci_writel(host, 0x0, MSHCI_IDINTEN);

	/* set max timeout */
	writel(0xffffffff, host->ioaddr + MSHCI_TMOUT);

	return 0;
}

static int mshci_initialize(int channel)
{
	struct mmc *mmc;

	mmc = &mmc_channel[channel];

	sprintf(mmc->name, "CNC1800L_MSHCI%d", channel);
	mmc->priv = &mmc_host[channel];
	mmc->send_cmd = mshci_send_command;
	mmc->set_ios = mshci_set_ios;
	mmc->init = mshci_init;

	mmc->voltages = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->host_caps = MMC_MODE_4BIT | MMC_MODE_8BIT | MMC_MODE_HS | MMC_MODE_HS_52MHz;

	mmc->f_min = 400000;
	mmc->f_max = 47250000;

	mmc_host[channel].clock = 0;

	switch(channel) {
	case 0:
		mmc_host[channel].ioaddr = (void *)CNC1800L_SDIO_BASE;
		break;
	default:
		printf("mmc err: not supported channel %d\n", channel);
	}

	return mmc_register(mmc);
}

int cnc1800l_mshci_init(void)
{
	int err;

	writel(0xffffffff, 0xB2100140);
	writel(0x01, 0xB2110024);

	err = mshci_initialize(0);
	if(err)
		return err;

	return 0;
}

int board_mmc_init(bd_t *bis)
{
    return cnc1800l_mshci_init();
}

/* before jumping to kernel, it has to reset fifo reset value */
void mshci_fifo_deinit(struct mmc *mmc)
{
	struct mshci_host *host = (struct mshci_host *)mmc->priv;

	mshci_clock_onoff(host, CLK_DISABLE);
 	mshci_writel(host, 0x0, MSHCI_DBADDR);
}

