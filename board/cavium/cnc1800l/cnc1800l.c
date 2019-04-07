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
#include "irqs.h"

DECLARE_GLOBAL_DATA_PTR;

#define PINMUX_I2C_ENABLE 0xb2110004
#define PINMUX_UART_ENABLE 0xb2110008
#define PINMUX_SPI_ENABLE 0xb211000c
#define PINMUX_SMI_ENABLE 0xb2110010
#define PINMUX_DISPLAY_ENABLE 0xb2110014
#define PINMUX_ETHER_ENABLE 0xb2110020

#define CE0_CFG_0 (CNC1800L_NAND_CFG_BASE)
#define CE1_CFG_0 (CNC1800L_NAND_CFG_BASE+0xc)
#define CE2_CFG_0 (CNC1800L_NAND_CFG_BASE+0x18)
#define CE3_CFG_0 (CNC1800L_NAND_CFG_BASE+0x24)

#define CE0_CFG_1 (CNC1800L_NAND_CFG_BASE+0x4)
#define CE1_CFG_1 (CNC1800L_NAND_CFG_BASE+0x10)
#define CE2_CFG_1 (CNC1800L_NAND_CFG_BASE+0x1c)
#define CE3_CFG_1 (CNC1800L_NAND_CFG_BASE+0x28)

#define CE0_CFG_2 (CNC1800L_NAND_CFG_BASE+0x8)
#define CE1_CFG_2 (CNC1800L_NAND_CFG_BASE+0x14)
#define CE2_CFG_2 (CNC1800L_NAND_CFG_BASE+0x20)
#define CE3_CFG_2 (CNC1800L_NAND_CFG_BASE+0x2c)

#define CFG_32M_MODE (CNC1800L_NAND_CFG_BASE+0x38)

#define CS0_RWLENGTH 0x04
#define CS0_CE_SETUP 0x00
#define CS0_CE_HOLD  0x04

#define CS0_OE_SETUP 0x01
#define CS0_OE_HOLD  0x04

#define CS0_WE_SETUP 0x01
#define CS0_WE_HOLD  0x04

#define CS0_BE_SETUP 0x01
#define CS0_BE_HOLD  0x04

//#define TEST_IRQ

#ifdef TEST_IRQ
#define CNC1800L_PCTL_BASE           0x80172000
#define CNC1800L_PCTL_INT_STA 	(0x000)
#define CNC1800L_PCTL_INT_MSK 	(0x004)
#define CNC1800L_PCTL_CTL_LO		(0x008)
#define CNC1800L_PCTL_CTL_HI		(0x00A)
#define CNC1800L_PCTL_RC5_LO  	(0x100)
#define CNC1800L_PCTL_RC5_HI  	(0x102)
#define CNC1800L_PCTL_NEC_LO  	(0x104)
#define CNC1800L_PCTL_NEC_HI		(0x106)
#define CNC1800L_PCTL_KEY_DATA_L 	(0x300) /* Key Scan Data Register */
#define CNC1800L_PCTL_KEY_CNTL_L		(0x304) /* Key Scan Control Register */
#define CNC1800L_PCTL_KEY_CNTL_H 	(0x306) /* Key Scan Control Register */

#define PCLK_PERIOD	1818    /* in 10 picosecond */
#endif


void flash_init(void)
{
    unsigned int *ce_cfg;
    unsigned int ce_cfg_val;
    /* configuration for CS0 NAND flash for cnc1800*/
    ce_cfg = (unsigned int *)CE0_CFG_0;
    ce_cfg_val = ((*ce_cfg) & (~(0x10000ff))) | (CS0_RWLENGTH);
    *ce_cfg = ce_cfg_val ;
    ce_cfg = (unsigned int *)CE1_CFG_0;
    *ce_cfg = ce_cfg_val ;
    ce_cfg = (unsigned int *)CE2_CFG_0;
    *ce_cfg = ce_cfg_val ;
     
    ce_cfg = (unsigned int *)CE0_CFG_1;
    *ce_cfg = (CS0_OE_HOLD << 24) | (CS0_OE_SETUP << 16) | (CS0_CE_HOLD << 8) | CS0_CE_SETUP;    /* configure OE and CE Timing */
    ce_cfg = (unsigned int *)CE1_CFG_1;
    *ce_cfg = (CS0_OE_HOLD << 24) | (CS0_OE_SETUP << 16) | (CS0_CE_HOLD << 8) | CS0_CE_SETUP;    /* configure OE and CE Timing */;
    ce_cfg = (unsigned int *)CE2_CFG_1;
    *ce_cfg = (CS0_OE_HOLD << 24) | (CS0_OE_SETUP << 16) | (CS0_CE_HOLD << 8) | CS0_CE_SETUP;    /* configure OE and CE Timing */;


    ce_cfg = (unsigned int *)CE0_CFG_2;
    *ce_cfg = (CS0_BE_HOLD << 24) | (CS0_BE_SETUP << 16) | (CS0_WE_HOLD << 8) | CS0_WE_SETUP;    /* configure BE and WE Timing */
    ce_cfg = (unsigned int *)CE1_CFG_2;
    *ce_cfg = (CS0_BE_HOLD << 24) | (CS0_BE_SETUP << 16) | (CS0_WE_HOLD << 8) | CS0_WE_SETUP;    /* configure BE and WE Timing */
    ce_cfg = (unsigned int *)CE2_CFG_2;
    *ce_cfg = (CS0_BE_HOLD << 24) | (CS0_BE_SETUP << 16) | (CS0_WE_HOLD << 8) | CS0_WE_SETUP;    /* configure BE and WE Timing */
}

#ifdef CONFIG_USE_IRQ
irq_handle cnc1800l_irq_vector[NR_IRQS+1];
void * cnc1800l_irq_data[NR_IRQS +1];


void cnc1800l_irq_enable(unsigned int irq)
{
    volatile unsigned int *irq_inten_l=(unsigned int *)(PA_VIC_BASE+VIC_IRQ_ENABLE);
    *irq_inten_l|=(1<<irq);
}

void cnc1800l_irq_disable(unsigned int irq)
{
    volatile unsigned int *irq_inten_l=(unsigned int *)(PA_VIC_BASE+VIC_IRQ_ENABLE);
    *irq_inten_l&=~(1<<irq);
}
void cnc1800l_irq_ack(unsigned int irq)
{
    volatile unsigned int *irq_inten_l=(unsigned int *)(PA_VIC_BASE+VIC_IRQ_ENABLE);
    *irq_inten_l&=~(1<<irq);
}

void cnc1800l_register_irq(int irq, irq_handle irq_r, void * irq_data)
{
    if (irq>=0 && irq<= NR_IRQS) {
        cnc1800l_irq_vector[irq] = irq_r;
        cnc1800l_irq_data[irq] = irq_data;
    }
}

void cnc1800l_unregister_irq(int irq)
{
    if (irq>=0 && irq<= NR_IRQS) {
        cnc1800l_irq_vector[irq] = NULL;
        cnc1800l_irq_data[irq] = NULL;
    }
}

#ifdef TEST_IRQ
/*for test only*/
int panel_irq_handle(unsigned int irq, void * data)
{
     int status,keydata;
     printf("in panel irq_handle \n");
     status = *((int *)(CNC1800L_PCTL_BASE+CNC1800L_PCTL_INT_STA));
     printf("status1 = 0x%x\n",status);
     keydata=*((int *)(CNC1800L_PCTL_BASE+CNC1800L_PCTL_KEY_DATA_L));
     printf("key data = 0x%x\n",keydata);

     return 0;
 }
#endif

void cnc1800l_init_irq(void)
{
    int i;

    *((unsigned int *)(PA_VIC_BASE + VIC_IRQ_ENABLE))= 0;
    *((unsigned int *)(PA_VIC_BASE + VIC_IRQ_MASK)) = 0;

    for (i=0; i<= NR_IRQS; i++) {
        cnc1800l_irq_vector[i] = NULL;
        cnc1800l_irq_data[i] = NULL;
    }

#ifdef TEST_IRQ
/*for test only*/
    int panel_sel;
    int rc_mode;
    int bit_time_cnt;
   *((int *)(0x80172004)) = 0;
   //*((int *)(0x80172008)) = 1<<31;
	
    /* Test Interrupt handle */
 	panel_sel    = 11;	
 	//rc_mode	     = 0x1;
 	bit_time_cnt = 112000000 / PCLK_PERIOD;

         *((int *)(CNC1800L_PCTL_BASE+CNC1800L_PCTL_CTL_LO))  = ((0xc000 | (bit_time_cnt >> 8)) << 16 )| (rc_mode | (panel_sel << 2) | ((bit_time_cnt & 0xff) << 8));

	//*((short *)(CNC1800L_PCTL_BASE+CNC1800L_PCTL_KEY_CNTL_H)) = 0x000f ;
     	*((int *)(CNC1800L_PCTL_BASE+CNC1800L_PCTL_KEY_CNTL_L)) = 0x000f << 16 |0x0001;
	

      *((int *)(CNC1800L_PCTL_BASE+CNC1800L_PCTL_INT_MSK)) &= ~0x3;                         /* enable IRQ */
     cnc1800l_register_irq(8, panel_irq_handle, (void *) NULL);
      cnc1800l_irq_enable(8);
#endif

}




int cnc1800l_do_interrupt(void)
{
    int vec,ret;
    unsigned int  isr;
    printf("handling interrupt....\n");
    isr = (*(unsigned int *)(PA_VIC_BASE+VIC_IRQ_STATUS));

    for (vec = 0; vec <= NR_IRQS; vec++) {
        if (isr & (1<<vec)) {
            if (NULL != cnc1800l_irq_vector[vec]) {
                ret =(cnc1800l_irq_vector[vec])(vec,cnc1800l_irq_data[vec]);
                return ret;
            }
        }
    }


    return -1;
}
#endif

/***********************************
 * Peripheral device pinmux setting
 * Ethernet, UART, I2C, DF ..
 * *******************************/
void peripheral_pinmux_enable(void)
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

#define CNC_POWER_VDAC  0xb2100210
#define CNC_POWER_ADAC  0xb210021c
#define CNC_POWER_USBPHY 0xb2100218

#define CNC_CLK_TVE0 (1<<4)
#define CNC_CLK_TVE1 (1<<5)
#define CNC_CLK_SDIO (1<<7))

#define CNC_CLK_AUD (1<<22)
#define CNC_CLK_AUD_PLL (1<<9)
#define CNC_CLK_AUD_DACEN (1<<8)

#define CNC_CLK_DECODER (1<<16)
#define CNC_CLK_VIDEO   (1<<23)

#define CNC_CLK_XPORT  (1<<21)
#define CNC_CLK_CRYPTO (1<<20)
#define CNC_CLK_GFX    (1<<18)
#define CNC_CLK_VIB    (1<<28)

#define CNC_CLOCK_CTL  (0xb2100140)

/** 
 * Power and Clock control
 * 
 */
void power_clock_ctl(void)
{
    unsigned int *ctl_cfg;
    unsigned int ctl_cfg_val;    

#ifdef CNC_VIDEO_DAC_ENABLE
    ctl_cfg = (unsigned int *)CNC_POWER_VDAC;
    ctl_cfg_val = (*ctl_cfg);
#if CNC_VIDEO_DAC_ENABLE
    ctl_cfg_val = 0x3F;
#else
    ctl_cfg_val= 0x0;
#endif
    *ctl_cfg = ctl_cfg_val;    
#endif


#ifdef CNC_AUDIO_DAC_ENABLE
    ctl_cfg = (unsigned int *)CNC_POWER_ADAC;
    ctl_cfg_val = (*ctl_cfg);
#if CNC_AUDIO_DAC_ENABLE
    ctl_cfg_val = 0x1;
#else
    ctl_cfg_val= 0x0;
#endif
    *ctl_cfg = ctl_cfg_val;    
#endif



#ifdef CNC_USB_ENABLE
    ctl_cfg = (unsigned int *)CNC_POWER_USBPHY;
    ctl_cfg_val = (*ctl_cfg);
#if CNC_USB_ENABLE
    ctl_cfg_val = ctl_cfg_val & (~(1<<2));
#else
    ctl_cfg_val= ctl_cfg_val | (1<<2);
#endif
    *ctl_cfg = ctl_cfg_val;    
#endif


    ctl_cfg = (unsigned int *)CNC_CLOCK_CTL;
    ctl_cfg_val = (*ctl_cfg);

#ifdef CNC_VIDEO_CLOCK_ENABLE
#if CNC_VIDEO_CLOCK_ENABLE
    ctl_cfg_val |= CNC_CLK_DECODER | CNC_CLK_VIDEO;
#else
    ctl_cfg_val &= (~(CNC_CLK_DECODER | CNC_CLK_VIDEO));
#endif
#endif

#ifdef CNC_AUDIO_CLOCK_ENABLE
#if CNC_AUDIO_CLOCK_ENABLE
    ctl_cfg_val |= CNC_CLK_AUD | CNC_CLK_AUD_PLL | CNC_CLK_AUD_DACEN;
#else
    ctl_cfg_val &= (~(CNC_CLK_AUD | CNC_CLK_AUD_PLL | CNC_CLK_AUD_DACEN));
#endif
#endif

#ifdef CNC_CRYPTO_CLOCK_ENABLE
#if CNC_CRYPTO_CLOCK_ENABLE
    ctl_cfg_val |= CNC_CLK_CRYPTO;
#else
    ctl_cfg_val &= (~(CNC_CLK_CRYPTO));
#endif
#endif

#ifdef CNC_XPORT_CLOCK_ENABLE
#if CNC_XPORT_CLOCK_ENABLE
    ctl_cfg_val |= CNC_CLK_XPORT;
#else
    ctl_cfg_val &= (~(CNC_CLK_XPORT));
#endif
#endif


#ifdef CNC_GFX_CLOCK_ENABLE
#if CNC_GFX_CLOCK_ENABLE
    ctl_cfg_val |= CNC_CLK_GFX;
#else
    ctl_cfg_val &= (~(CNC_CLK_GFX));
#endif
#endif



#ifdef CNC_VIB_CLOCK_ENABLE
#if CNC_VIB_CLOCK_ENABLE
    ctl_cfg_val |= CNC_CLK_VIB;
#else
    ctl_cfg_val &= (~(CNC_CLK_VIB));
#endif
#endif



#ifdef CNC_TVE_CLOCK_ENABLE
#if CNC_TVE_CLOCK_ENABLE
    ctl_cfg_val |= CNC_CLK_TVE0 | CNC_CLK_TVE1;
#else
    ctl_cfg_val &= (~(CNC_CLK_TVE0 | CNC_CLK_TVE1));
#endif
#endif

    *ctl_cfg = ctl_cfg_val;

}

/*
 * Miscellaneous platform dependent initialisations
 */
int board_init(void)
{
    /* arch number of CNC1800L Board */
    gd->bd->bi_arch_number = 1662;
    /* adress of boot parameters */
    gd->bd->bi_boot_params = 0x00000100;
    icache_enable();
    peripheral_pinmux_enable();
    flash_init();
    //ether__init ();
    spi__init();
#ifdef CONFIG_USE_IRQ
    cnc1800l_init_irq();
#endif
    power_clock_ctl();

    /*
     * comment out by xp, 2011.12.16, some project don't use LED but use GPIO 2,
     * so if want to display on LED, use command fpcd
     */
    //fpc_init_show("boot");	/* Add By River 12.16.2008 */

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

