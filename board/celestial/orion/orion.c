/*
 * (C) Copyright 2005
 * STMicrolelctronics, <www.st.com>
 *
 * (C) Copyright 2004
 * ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
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
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>
#include "irqs.h"

DECLARE_GLOBAL_DATA_PTR;

#define CE0_CFG_0 0x10100000
#define CE1_CFG_0 0x1010000C
#define CE2_CFG_0 0x10100018
#define CE3_CFG_0 0x10100024

#define CE0_CFG_1 0x10100004
#define CE1_CFG_1 0x10100010
#define CE2_CFG_1 0x1010001C
#define CE3_CFG_1 0x10100028

#define CE0_CFG_2 0x10100008
#define CE1_CFG_2 0x10100014
#define CE2_CFG_2 0x10100020
#define CE3_CFG_2 0x1010002C

#define CFG_32M_MODE 0x10100038

#define CS0_RWLENGTH 0x04
#define CS0_CE_SETUP 0x00
#define CS0_CE_HOLD  0x04

#define CS0_OE_SETUP 0x01
#define CS0_OE_HOLD  0x03

#define CS0_WE_SETUP 0x01
#define CS0_WE_HOLD  0x03

#define CS0_BE_SETUP 0x01
#define CS0_BE_HOLD  0x03


#define PINMUX_I2C_ENABLE 0x42110004
#define PINMUX_UART_ENABLE 0x42110008
#define PINMUX_SPI_ENABLE 0x4211000c
#define PINMUX_SMI_ENABLE 0x42110010
#define PINMUX_DISPLAY_ENABLE 0x42110014
#define PINMUX_ETHER_ENABLE 0x42110020

#ifdef CONFIG_USE_IRQ
irq_handle orion_irq_vector[NR_IRQS+1];
void * orion_irq_data[NR_IRQS +1];


void orion_irq_enable(unsigned int irq)
{
	volatile unsigned int *irq_inten_l=(unsigned int *)(PA_VIC_BASE+VIC_IRQ_ENABLE);
	*irq_inten_l|=(1<<irq);
}

void orion_irq_disable(unsigned int irq)
{
	volatile unsigned int *irq_inten_l=(unsigned int *)(PA_VIC_BASE+VIC_IRQ_ENABLE);
	*irq_inten_l&=~(1<<irq);
}
void orion_irq_ack(unsigned int irq)
{
	volatile unsigned int *irq_inten_l=(unsigned int *)(PA_VIC_BASE+VIC_IRQ_ENABLE);
	*irq_inten_l&=~(1<<irq);
}

void orion_register_irq(int irq, irq_handle irq_r, void * irq_data)
{
	if (irq>=0 && irq<= NR_IRQS) {
		orion_irq_vector[irq] = irq_r;
		orion_irq_data[irq] = irq_data;
	}
}

void orion_unregister_irq(int irq)
{
	if (irq>=0 && irq<= NR_IRQS) {
		orion_irq_vector[irq] = NULL;
		orion_irq_data[irq] = NULL;
	}
}

void orion_init_irq(void)
{
	int i;

	*((unsigned int *)(PA_VIC_BASE + VIC_IRQ_ENABLE))= 0;
	*((unsigned int *)(PA_VIC_BASE + VIC_IRQ_MASK)) = 0;

	for (i=0; i<= NR_IRQS; i++) {
		orion_irq_vector[i] = NULL;
		orion_irq_data[i] = NULL;
	}

}

int orion_do_interrupt(void)
{
	int vec,ret;
	unsigned int  isr;
	printf("handling interrupt....\n");
	isr = (*(unsigned int *)(PA_VIC_BASE+VIC_IRQ_STATUS));

	for (vec = 0; vec <= NR_IRQS; vec++) {
		if (isr & (1<<vec)) {
			if (NULL != orion_irq_vector[vec]) {
				ret =(orion_irq_vector[vec])(vec,orion_irq_data[vec]);
				return ret;
			}
		}
	}

	return -1;
}
#endif

#ifdef CONFIG_SHOW_BOOT_PROGRESS
void show_boot_progress(int progress)
{
	printf("%i\n", progress);
}
#endif

/***********************************
 * Peripheral device pinmux setting
 * Ethernet, UART, I2C, DF ..
 * *******************************/
void peripheral_pinmux_enable()
{
	unsigned int* reg_addr;
	reg_addr = (unsigned int*)PINMUX_I2C_ENABLE;
	*reg_addr = 0x1; //I2C0 enable; I2C1 disable

	reg_addr = (unsigned int*)PINMUX_UART_ENABLE;
	*reg_addr = 0x1;  //UART0 enable; UART1/2 disable

	reg_addr = (unsigned int*)PINMUX_SMI_ENABLE;
	*reg_addr = 0x1;  // Smart card enable

	reg_addr = (unsigned int*)PINMUX_DISPLAY_ENABLE;
	*reg_addr = 0x0; // TVE0(DAC1)YPbPr; TVE1(DAC0)CVBS

	reg_addr = (unsigned int*)PINMUX_ETHER_ENABLE;
	*reg_addr = 0x0; // Ethernet enable

}

/******************************
 * Routine:
 * Description:
 ******************************/
void flash__init (void)
{
	unsigned int *ce_cfg;
	unsigned int ce_cfg_val;
	unsigned char iobits;
	/* configuration for CS0 NAND flash for csm1800*/
	ce_cfg = (unsigned int *)CE0_CFG_0;
	ce_cfg_val = ((*ce_cfg) & (~(0x10000ff))) | (CS0_RWLENGTH);
	*ce_cfg = ce_cfg_val ;

	ce_cfg = (unsigned int *)CE0_CFG_1;
	*ce_cfg = (CS0_OE_HOLD << 24) | (CS0_OE_SETUP << 16) | (CS0_CE_HOLD << 8) | CS0_CE_SETUP;    /* configure OE and CE Timing */
	ce_cfg = (unsigned int *)CE0_CFG_2;
	*ce_cfg = (CS0_BE_HOLD << 24) | (CS0_BE_SETUP << 16) | (CS0_WE_HOLD << 8) | CS0_WE_SETUP;    /* configure BE and WE Timing */
}


/*
 * Miscellaneous platform dependent initialisations
 */
int board_init(void)
{
	/* arch number of ORION Board */
	gd->bd->bi_arch_number = 1662;
	/* adress of boot parameters */
#ifdef CONFIG_512M_DDR
	gd->bd->bi_boot_params = 0x80000100;
#else
	gd->bd->bi_boot_params = 0x00000100;
#endif
	icache_enable ();
	peripheral_pinmux_enable();
	flash__init ();
	//ether__init ();
	spi__init();
#ifdef CONFIG_USE_IRQ
	orion_init_irq();
#endif
	return 0;
}

int board_late_init(void)
{
	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size(CONFIG_SYS_SDRAM_BASE,
			CONFIG_SYS_SDRAM_SIZE);
	return 0;
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_SMC91111
	rc = smc91111_initialize(0, CONFIG_SMC91111_BASE);
#endif
	return rc;
}
#endif
