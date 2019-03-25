/**/
#include <common.h>
#include <command.h>
#include "cnc1800l_spi.h"

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_CMD_SF)

#define CONFIG_SPI_DMA_EN

#define CFG_SPI_FREQ 94500000

#define MAX_RCV_SZ   0x1000
#define DMA_RD_BAUD  0x4
#define DMA_WR_BAUD  0x2
#define IO_RD_BAUD   0x8
#define IO_WR_BAUD   0x8

#ifdef CONFIG_SPI_DMA_EN
#define RD_BAUD DMA_RD_BAUD
#define WR_BAUD DMA_WR_BAUD
#else
#define RD_BAUD IO_RD_BAUD
#define WR_BAUD IO_WR_BAUD
#endif


unsigned char alen = 3;

static void inline spin_wheel (void)
{
	static int p = 0;
	static char w[] = "\\/-";

	printf ("\010%c", w[p]);
	(++p == 3) ? (p = 0) : 0;
}

void spi__init(void)
{
	unsigned int ctrl0 = 0;

    /* enable spi pinmux */
	spi_write(SPI_PINMUX, 0x1);

	/* Disable the ssi by write 0 to SSIEN,tx and rx buffer was cleared */
	spi_write(rSSIENR, 0x0);

	/* Config the CTRLR0 register */
    ctrl0 = 0x0 << 12  | /* CFS (Control Frame Size). 4bit length */
        0x0 << 11  |     /* SRL 0:nomormod;1:test mode            */
        0x0 << 10  |     /* SLV_OE                                */
        0x0 << 8   |     /* TMOD master  TODO 0                   */
        0x1 << 7   |     /* SCPOL clock inactive state            */
        0x1 << 6   |     /* SCPH                                  */
        0x0 << 4   |     /* frame format select  SPI              */
        0x7 << 0;        /* DFS data frame size                   */

	/*  rCTRLR0=0x2B00; receive and transmit enable */

	spi_write(rCTRLR0, ctrl0);

	/* ctrl1 */
	spi_write(rCTRLR1, (MAX_RCV_SZ-1));

	/* SSI_CLK baud out */
	spi_write(rBAUDR, IO_WR_BAUD);

	/* rSER, select the device */
	spi_write(rSER, 0x1);

	/* rSSIENR, enable the ssi bus */
	spi_write(rSSIENR, 0x1);

/*    alen = spi_read(BOOTCFG)>>5 & 0x3;*/
}

#if 0
void spi_baud(unsigned short baud)
{
	spi_write(rSSIENR, 0x0);
	spi_write(rBAUDR, baud);
	spi_write(rSSIENR, 0x1);
}

static void master_set_mode(TMOD_E tmod, TIMING_E tm)
{
	unsigned short ctrl0 = spi_read(rCTRLR0);

	ctrl0 &= ~(0xf<<6);
	ctrl0 |= (tmod<<8 | tm<<6);

	spi_write(rSSIENR, 0x0);
	spi_write(rCTRLR0, ctrl0);
	spi_write(rSSIENR, 0x1);
}
static unsigned char master_check_mode(TMOD_E tmod, TIMING_E tm)
{
	unsigned short ctrl0 = spi_read(rCTRLR0);

	if (ctrl0 & (tmod<<8 | tm<<6))
		return 1;

	return 0;
}

/*===================================================================
 * 	flash oepration
 */
static inline void read_out_data(volatile unsigned char *buf, int size)
{
	int i = 0, j = 0;

	while (i < size) {
		while (!(j = spi_read(rRXFLR)));
		while (j > 0) {
			if (buf) buf[i] = spi_read(rDR);
			else     spi_read(rDR);

			i++; j--;
		}
	}
}

static inline unsigned char flash_status(void)
{
	unsigned char buf[2];

	/* If check fail, return 0xff, also satisfy while's condition */
	if (! master_check_mode(TMOD_TXRX, TIMING3))
		return -1;

	spi_write(rDR, CMD_RDSR); spi_write(rDR, 0x0);
	read_out_data(buf, 2);
/*    printf("status: %x\n", buf[1]);*/
	return buf[1];
}

static void wait_for_finish(void)
{
	int tmp = 0;

	while (flash_status() & 1) {
		udelay(1000);
		if (!(tmp % 100))
			spin_wheel();
		tmp++;
	}
}

/* Only Used For Erase Read Write */
static void flash_send_cmd(unsigned char cmd, unsigned int caddr)
{
	int i;
	unsigned char *byte_addr = (unsigned char *) &caddr;

	if (/*CMD_PP == cmd || */ CMD_SE == cmd ||
	    CMD_BE == cmd || CMD_CE == cmd) {

		master_set_mode(TMOD_TXRX, TIMING3);
		if (CMD_CE == cmd) {
			spi_write(rDR, cmd);
			read_out_data(NULL, 1);
		} else {
			spi_write(rDR, cmd);
			for (i = alen - 1; i >= 0; i--)
				spi_write(rDR, byte_addr[i]);

			/*if (CMD_PP != cmd)*/
				read_out_data(NULL, alen+1);
		}

	} else if (CMD_WREN == cmd) {

/*                master_set_mode(TMOD_TXRX, TIMING3);*/
		master_set_mode(TMOD_TXRX, TIMING0);
		spi_write(rDR, cmd);
		read_out_data(NULL, 1);

	} else if (CMD_PP == cmd) {

		master_set_mode(TMOD_TX, TIMING3);
		spi_write(rDR, cmd);
		for (i = alen - 1; i >= 0; i--)
			spi_write(rDR, byte_addr[i]);

	} else if (CMD_READ == cmd || CMD_RDID == cmd) {

		master_set_mode(TMOD_EE, TIMING3);
		if (CMD_RDID == cmd) {
			spi_write(rDR, cmd);
		} else {
			spi_write(rDR, cmd);
			for (i = alen - 1; i >= 0; i--)
				spi_write(rDR, byte_addr[i]);
		}

	} else {
		printf("flash_send_cmd: Not Support this command!\n");
	}

	return;
}
#endif

static inline void wait_for_finish(void)
{
	unsigned int status;

	spi_write(rSSIENR,  0x0     );
	spi_write(rCTRLR0,  0x3c7   );
	spi_write(rCTRLR1,  0x0     );
	spi_write(rSSIENR,  0x1     );

	do {
		spi_write(rDR,  CMD_RDSR);
		while(spi_read(rRXFLR) != 1);
		status = spi_read(rDR);
/*        printf("status : %x\n", status);*/
		//spin_wheel();
	} while(status & 0x1);
}

static int _flash_read(unsigned int caddr, unsigned char *buf, int size)
{
	unsigned char *addr = (unsigned char *)&caddr;

	spi_write(rSSIENR, 0x0);
	spi_write(rCTRLR0, 0x3c7);
	spi_write(rCTRLR1, (MAX_RCV_SZ-1));
	spi_write(rBAUDR,  IO_RD_BAUD);
	spi_write(rSSIENR, 0x1);

	spi_write(rDR, CMD_READ);
	spi_write(rDR, addr[2]);
	spi_write(rDR, addr[1]);
	spi_write(rDR, addr[0]);

	while (size > 0) {
		if (size >= 16) {
			while (spi_read(rRXFLR) < 16);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			size -= 16;
		} else if (size >= 8) {
			while (spi_read(rRXFLR) < 8);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			size -= 8;
		} else if (size >= 4) {
			while (spi_read(rRXFLR) < 4);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			size -= 4;
		} else if (size >= 2) {
			while (spi_read(rRXFLR) < 2);
			*buf++ = spi_read(rDR);
			*buf++ = spi_read(rDR);
			size -= 2;
		} else if (size > 0) {
			while (spi_read(rRXFLR) == 0);
			*buf++ = spi_read(rDR);
			size -= 1;
		}
	}

	while ((spi_read(rSR) & 0x5) != 0x4);
	udelay (100);

#if 0
	if (spi_read(rSR) & (1<<5))
		printf("Error   T error! 5 \n");

	if (spi_read(rSR) & (1<<6))
		printf("Error   T data error! 6 \n");
#endif

	return 0;
}

static int _flash_write(unsigned int caddr, volatile unsigned char *buf, int size)
{
	unsigned char *addr = (unsigned char *)&caddr;

	/* Unprotect flash */
	spi_write(rSSIENR, 0x0);
	spi_write(rCTRLR0, 0x107);
	spi_write(rSSIENR, 0x1);
	spi_write(rDR, CMD_WREN);
	while ((spi_read(rSR) & 0x5) != 0x4);

	spi_write(rSSIENR, 0x0);
	spi_write(rCTRLR0, 0x1c7);
	spi_write(rBAUDR,  IO_WR_BAUD);
	spi_write(rSSIENR, 0x1);

	spi_write(rDR, CMD_PP);
	spi_write(rDR, addr[2]);
	spi_write(rDR, addr[1]);
	spi_write(rDR, addr[0]);

	while (size) {
		if (size >= 16) {
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			while (spi_read(rTXFLR) > 8);
			size -= 16;
		} else if (size >= 8) {
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			while (spi_read(rTXFLR) > 8);
			size -= 8;
		} else if (size >= 4) {
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			while (spi_read(rTXFLR) > 8);
			size -= 4;
		} else if (size >= 2) {
			spi_write(rDR, *buf++);
			spi_write(rDR, *buf++);
			while (spi_read(rTXFLR) > 8);
			size -= 2;
		} else if (size > 0) {
			spi_write(rDR, *buf++);
			while (spi_read(rTXFLR) > 8);
			size -= 1;
		}
	}

	while ((spi_read(rSR) & 0x5) != 0x4);

	wait_for_finish();

	return 0;
}

static int _flash_erase(unsigned int caddr, unsigned short cmd)
{
	unsigned char *addr = (unsigned char *)&caddr;

	/* Unprotect flash */
	spi_write(rSSIENR, 0x0);
	spi_write(rCTRLR0, 0x107);
	spi_write(rSSIENR, 0x1);
	spi_write(rDR, CMD_WREN);
	while ((spi_read(rSR) & 0x5) != 0x4);

	spi_write(rSSIENR, 0x0);
	spi_write(rBAUDR, IO_WR_BAUD);
	spi_write(rCTRLR0, 0x1c7);
	spi_write(rSSIENR, 0x1);

	spi_write(rDR, cmd);
	spi_write(rDR, addr[2]);
	spi_write(rDR, addr[1]);
	spi_write(rDR, addr[0]);
	while ((spi_read(rSR) & 0x5) != 0x4);

	wait_for_finish();

	return 0;
}


#define  WriteReg(a, v) 		(*(volatile unsigned int *)(a) = (v))
#define  WriteReg8(a, v) 		(*(volatile unsigned char *)(a) = (v))
#define  WriteReg32(a, v) 		(*(volatile unsigned int *)(a) = (v))
#define  WriteReg16(a, v) 		(*(volatile unsigned short *)(a) = (v))

#define  ReadReg(a) 			(*(volatile unsigned int *)(a))
#define  ReadReg8(a) 			(*(volatile unsigned char *)(a))
#define  ReadReg16(a) 			(*(volatile unsigned short *)(a))
#define  ReadReg32(a) 			(*(volatile unsigned int *)(a))

enum {
    SPI_CTRLR0      = 0x80150000,
    SPI_CTRLR1      = 0x80150004,
    SPI_SSIENR      = 0x80150008,
    SPI_SER         = 0x80150010,
    SPI_BAUDR       = 0x80150014,
    SPI_TXFLR       = 0x80150020,
    SPI_RXFLR       = 0x80150024,
    SPI_SR          = 0x80150028,
    SPI_IMR         = 0x8015002c,
    SPI_DMACR       = 0x8015004c,
    SPI_DMATDLR     = 0x80150050,
    SPI_DMARDLR     = 0x80150054,
    SPI_DR          = 0x80150060,

    PINMUX_SPI      = 0xB211000c,

    SAR0            = 0x80154000,
    DAR0            = 0x80154008,
    LLP0            = 0x80154010,
    CTL0            = 0x80154018,
    SSTAT0          = 0x80154020,
    DSTAT0          = 0x80154028,
    SSTATAR0        = 0x80154030,
    DSTATAR0        = 0x80154038,
    CFG0            = 0x80154040,
    SGR0            = 0x80154048,
    DSR0            = 0x80154050,

	RawTfr			= 0x801542c0,
	RawBlock		= 0x801542c8,
	RawDstTran		= 0x801542d0,
	RawSrcTran		= 0x801542d8,
	RawErr			= 0x801542e0,

	StatusTfr		= 0x801542e8,
	StatusBlock		= 0x801542f0,
	StatusSrcTran	= 0x801540f8,
	StatusDstTran	= 0x80154300,
	StatusErr		= 0x80154308,

	StatusInt		= 0x80154360,

    MASK_TFR        = 0x80154310,
    CLEAR_TFR       = 0x80154338,
    CHEN            = 0x801543a0,
    DMACFG          = 0x80154398,
    MaskTfr         = 0x80154310,
    ClearTfr        = 0x80154338
};

#if defined(CONFIG_SPI_DMA_EN)

static inline void SetupDMA_LLP_SG_RX(const u32 caddr, u32 maddr, u32 size)
{
    unsigned int llp = CONFIG_SPI_DMA_BUF_BASE;
    unsigned int ctl_lo =  0   /* CTL[31:0]                    */
              | ( 0x1  << 28)  /* LLP_SRC_EN                   */
              | ( 0x1  << 27)  /* LLP_DST_EN                   */
              | ( 0x0  << 25)  /* SRC Master                   */
              | ( 0x1  << 23)  /* DST Master                   */
              | ( 0x2  << 20)  /* P -> M, M as flow controller */
              | ( 0x0  << 18)  /* DSCATTER_EN                  */
              | ( 0x0  << 17)  /* SGATHER_EN                   */
              | ( 0x1  << 14)  /* SMSIZE:  4 * 1B              */
              | ( 0x3  << 11)  /* DMSIZE: 16 * 4B              */
              | ( 0x2  <<  9)  /* SINC: no change              */
              | ( 0x0  <<  7)  /* DINC: increment              */
              | ( 0x0  <<  4)  /* SRC_TR_WIDTH:  8b            */
              | ( 0x2  <<  1)  /* DST_TR_WIDTH: 32b            */
              | ( 0x0  <<  0); /* INT_EN                       */


    /* 4B align */
    if (size & 0x3) {
        printf("%s: size not 4B align!\n", __func__);
        return;
    }

    /* Controller init */
    WriteReg16(SPI_SSIENR , 0x0              );
    WriteReg16(SPI_CTRLR0 , 0x3c7            );
    WriteReg16(SPI_CTRLR1 , (size-1)         );
    WriteReg16(SPI_BAUDR  , DMA_RD_BAUD          );
    WriteReg16(SPI_DMARDLR, 3                ); /* When 4bytes got, send to DMA controller immediately */
    WriteReg16(SPI_DMACR  , 0x1              ); /* Receive FIFO DMA request enabled */
    WriteReg16(SPI_SSIENR , 0x1              );

    /* DMA enable */
    WriteReg(DMACFG,    0x1                  ); /* Gloabl Enable */

    WriteReg(llp +  0, SPI_DR                ); /* SAR           */
    WriteReg(llp +  4, maddr                 ); /* DAR           */
    WriteReg(llp +  8, (llp + 32) | 0x1      ); /* Next LLP from previous llp */
    WriteReg(llp + 12, ctl_lo                );
    WriteReg(llp + 16, (size>2048)?2048:size );

    if (size > 2048) {
    WriteReg(llp + 32, SPI_DR                ); /* SAR      */
    WriteReg(llp + 36, maddr+2048            ); /* DAR      */
    WriteReg(llp + 40, 0                     ); /* Next LLP */
    WriteReg(llp + 44, ctl_lo                );
    WriteReg(llp + 48, size-2048             );
    } else
    WriteReg(llp + 8, 0                      );

#if 0
	{
		int i;
		for (i = 0; i <= 48; i+=4) {
			printf("%d: -------------- 0x%x\n",i, ReadReg(llp+i));
		}
		printf("\n");
		printf("\n");
	}
#endif

    WriteReg(CTL0,      0x3 << 27            ); /* LLP_SRC_EN/LLP_DST_EN                           */
    WriteReg(CFG0 + 4,  0x1 <<  7            ); /* use RX handshaking signals for source periphral */
    WriteReg(CFG0,      0x1 << 10            ); /* HS_SEL_DST = 0 (HW), HS_SEL_DST = 1 (SW)        */
    WriteReg(LLP0,      llp | 0x1            ); /* LMS = 1, external DRAM                          */
    WriteReg(CHEN,      (0x1 << 8) | 0x1     ); /* Enable the only channel                         */

    WriteReg(ClearTfr,  0x1                  );
    WriteReg(MaskTfr,   (0x1 << 8) | 0x1     );

    /* Send Read command */
    WriteReg16(SPI_SSIENR , 0x3              );
    WriteReg16(SPI_DR, CMD_READ           );
    WriteReg16(SPI_DR, (caddr>>16) & 0xff );
    WriteReg16(SPI_DR, (caddr>>8)  & 0xff );
    WriteReg16(SPI_DR, (caddr)     & 0xff );
    WriteReg16(SPI_SSIENR , 0x1              );

    /* Wait dma finished  */
    while (ReadReg(CHEN));
}

static inline void SetupDMA_LLP_SG_TX(u32 caddr, const u32 maddr, u32 size)
{
    unsigned int llp = CONFIG_SPI_DMA_BUF_BASE;
    unsigned int ctl_lo =  0     /* CTL[31:0]                    */
                | ( 0x1  << 28)  /* LLP_SRC_EN                   */
                | ( 0x1  << 27)  /* LLP_DST_EN                   */
                | ( 0x1  << 25)  /* SRC Master                   */
                | ( 0x0  << 23)  /* DST Master                   */
                | ( 0x1  << 20)  /* M -> P, M as flow controller */
                | ( 0x0  << 18)  /* DSCATTER_EN                  */
                | ( 0x0  << 17)  /* SGATHER_EN                   */
                | ( 0x3  << 14)  /* SMSIZE: 16 * 4B              */
                | ( 0x1  << 11)  /* DMSIZE:  4 * 1B              */
                | ( 0x0  <<  9)  /* SINC: increment              */
                | ( 0x2  <<  7)  /* DINC: no charge              */
                | ( 0x2  <<  4)  /* SRC_TR_WIDTH: 32b            */
                | ( 0x0  <<  1)  /* DST_TR_WIDTH:  8b            */
                | ( 0x1  <<  0); /* INT_EN                       */

    /* 4 Byte align */
    if (size & 0x3) {
        printf("%s: size not 4B align!\n", __func__);
        return;
    }

    /* Unprotect flash */
    spi_write(rSSIENR, 0x0               );
    spi_write(rCTRLR0, 0x107             );
    spi_write(rSSIENR, 0x1               );
    spi_write(rDR    , CMD_WREN          );
    while ((spi_read(rSR) & 0x5) != 0x4  );

    /* Init Controller */
    WriteReg16(SPI_SSIENR , 0x0          );
    WriteReg16(SPI_BAUDR  , DMA_WR_BAUD      );
    WriteReg16(SPI_CTRLR0 , 0x1c7        ); /* Transmit Only                                                */
    WriteReg16(SPI_DMATDLR, 16           ); /* When FIFO half full, receive from DMA controller immediately */
    WriteReg16(SPI_DMACR  , 0x2          ); /* Transmit FIFO DMA request enabled                            */
    WriteReg16(SPI_SSIENR , 0x1          );

    /* Enable DMA */
    WriteReg(DMACFG,    0x1              ); /* Gloabl Enable                                   */

    WriteReg(llp +  0, maddr             ); /* SAR                                             */
    WriteReg(llp +  4, SPI_DR            ); /* DAR                                             */
    WriteReg(llp +  8, 0                 ); /* Next LLP                                        */
    WriteReg(llp + 12, ctl_lo            );
    WriteReg(llp + 16, (size + 4) / 4    ); /* 256B data + 4B command                          */

    WriteReg(CTL0,      0x3 << 27        ); /* LLP_SRC_EN/LLP_DST_EN                           */
    WriteReg(CFG0 + 4,  0x0 <<  7        ); /* use TX handshaking signals for source periphral */
    WriteReg(CFG0,      0x2 << 10        ); /* HS_SEL_DST = 0 (HW), HS_SEL_DST = 1 (SW)        */
    WriteReg(LLP0,      llp | 0x1        ); /* LMS = 1, external DRAM                          */
    WriteReg(CHEN,      (0x1 << 8) | 0x1 ); /* Enable the only channel                         */

    WriteReg(ClearTfr,  0x1              );
    WriteReg(MaskTfr,   (0x1 << 8) | 0x1 );

    /* Wait dma finish  */
    while (ReadReg(CHEN));
    /* Wait spi transfer finish  */
    while((ReadReg16(SPI_SR) & 0x5) != 0x4);
    /* Wait flash program finish */
    wait_for_finish();
}


#if 0
int do_sflashre_dma(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned char *maddr;
	unsigned int caddr;
	unsigned int size;

	if (argc != 4) {
		printf("Usage: %s\n", cmdtp->help);
		return -1;
	}

	maddr = (unsigned char *)simple_strtoul(argv[1], NULL, 16);
	caddr = simple_strtoul(argv[2], NULL, 16);
	size = simple_strtoul(argv[3], NULL, 16);

	printf("caddr: 0x%x, maddr: 0x%x, size: 0x%x\n",
	      caddr, maddr, size);

	seeprom_read(caddr, maddr, size);

	return 0;
}

U_BOOT_CMD(
	sfdre,	CFG_MAXARGS/*5*/,	0,	do_sflashre_dma,
	"sfre    - spi flash read byte.\n",
	"\nsfre <maddr> <offset> <len>\n"
	);
#endif

#endif /* CONFIG_SPI_DMA */

/*--------------------------------------------------------------------------------*/

int seeprom_read(unsigned caddr, uchar *maddr, unsigned size)
{
	unsigned cnt;

    /* Make sure 4B align */
    if (size & 0x3)
        size = ((size + 4) >> 2) << 2;

	while (size > 0) {
		cnt = (size >= MAX_RCV_SZ) ? MAX_RCV_SZ : size;

#if defined(CONFIG_SPI_DMA_EN)
		/* DMA controller */
		SetupDMA_LLP_SG_RX((u32)caddr, (u32)maddr, cnt );
#else
		_flash_read(caddr, maddr, cnt);
#endif

		caddr += cnt;
		maddr += cnt;
		size  -= cnt;
	}

    /* fix dma bug, reload with 4byte offset */
    WriteReg16(SPI_SSIENR , 0x0 );
    WriteReg16(SPI_DMARDLR, 0x0 );
    WriteReg16(SPI_DMACR  , 0x0 );
    WriteReg16(SPI_SSIENR , 0x1 );

	return 0;
}

int seeprom_write(unsigned caddr, uchar *maddr, unsigned size)
{
	unsigned cnt;
#if defined(CONFIG_SPI_DMA_EN)
	unsigned int v_bak;
#endif

	printf("Page Write from 0x%x to 0x%x ...  ", (unsigned int)maddr, caddr);

	while (size > 0) {
		cnt = (size >= PP_SZ) ? PP_SZ : size;

#if defined(CONFIG_SPI_DMA_EN)
        /* Backup Address */
		v_bak = *(unsigned int *)(maddr - 4);

        /* Initialize Write Command */
        WriteReg8((maddr-4),  CMD_PP           );
        WriteReg8((maddr-3),  (caddr>>16)&0xff );
        WriteReg8((maddr-2),  (caddr>>8)&0xff  );
        WriteReg8((maddr-1),  (caddr)&0xff     );

        if (cnt % 0x3)
            _flash_write(caddr, maddr, cnt);
        else
            SetupDMA_LLP_SG_TX((u32)caddr, (u32)(maddr-4), cnt);

        /* Restore Address */
		*(unsigned int *)(maddr - 4) = v_bak;
#else
		_flash_write(caddr, maddr, cnt);
#endif

		caddr += cnt;
		maddr += cnt;
		size  -= cnt;
	}
	printf(" ok!\n");

	return 0;
}

int seeprom_erase(unsigned cmd, unsigned saddr, unsigned size)
{
	unsigned eaddr;
	unsigned sector_size = gd->sf_inf->sector_size;

	eaddr = saddr + size;
	if (eaddr > gd->sf_size)
		eaddr = gd->sf_size;

	switch (cmd) {
		case CMD_CE: /* chip */
			printf("Chip Erase ...  ");
			_flash_erase(saddr, cmd);
			break;

		case CMD_BE: /* block */
			printf("Block Erase from 0x%x to 0x%x ...  ", saddr, eaddr);
			if (gd->sf_inf->jedec_id == 0x1c2017)
				if (saddr < 0x10000) {
					_flash_erase(0x0000, cmd);
					_flash_erase(0x1000, cmd);
					_flash_erase(0x2000, cmd);
					_flash_erase(0x4000, cmd);
					_flash_erase(0x8000, cmd);

					if (eaddr >= 0x10000)
						saddr = 0x10000;
				}

			while (saddr < eaddr) {
				_flash_erase(saddr, cmd);
				saddr+=sector_size;
			}
			break;

		case CMD_SE:	/* sector */
			printf("Sector Erase from 0x%x to 0x%x ...  ", saddr, eaddr);
			while (saddr < eaddr) {
				_flash_erase(saddr, cmd);
				saddr+=0x1000;
			}
			break;

		default:
			printf("Error! No such chioce.\n");
			return -1;
	}
	printf(" ok!\n");

	return 0;
}

static sfinf_t sflash_data [] = {

	/* Atmel -- some are (confusingly) marketed as "DataFlash" */
	{ "at25fs010",  0x1f6601, 0, 32 * 1024, 4, SECT_4K, },
	{ "at25fs040",  0x1f6604, 0, 64 * 1024, 8, SECT_4K, },

	{ "at25df041a", 0x1f4401, 0, 64 * 1024, 8, SECT_4K, },
	{ "at25df641",  0x1f4800, 0, 64 * 1024, 128, SECT_4K, },

	{ "at26f004",   0x1f0400, 0, 64 * 1024, 8, SECT_4K, },
	{ "at26df081a", 0x1f4501, 0, 64 * 1024, 16, SECT_4K, },
	{ "at26df161a", 0x1f4601, 0, 64 * 1024, 32, SECT_4K, },
	{ "at26df321",  0x1f4701, 0, 64 * 1024, 64, SECT_4K, },

	/* Macronix */
	{ "mx25l12805d", 0xc22018, 0, 64 * 1024, 256, },
        { "mx25l6405d",  0xc22017, 0, 64 * 1024, 128, 0 }, 
        { "mx25l6445e",  0xc22017, 0, 64 * 1024, 128, SECT_4K },
	{ "mx25l512e",   0xc22010, 0,  4 * 1024,  16, },


	/* Spansion -- single (large) sector size only, at least
	 * for the chips listed here (without boot sectors).
	 */
	{ "s25sl004a", 0x010212, 0, 64 * 1024, 8, },
	{ "s25sl008a", 0x010213, 0, 64 * 1024, 16, },
	{ "s25sl016a", 0x010214, 0, 64 * 1024, 32, },
	{ "s25sl032a", 0x010215, 0, 64 * 1024, 64, },
	{ "s25sl064a", 0x010216, 0, 64 * 1024, 128, },
        { "s25sl12800", 0x012018, 0x0300, 256 * 1024, 64, },
	{ "s25sl12801", 0x012018, 0x0301, 64 * 1024, 256, },

	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{ "sst25vf040b", 0xbf258d, 0, 64 * 1024, 8, SECT_4K, },
	{ "sst25vf080b", 0xbf258e, 0, 64 * 1024, 16, SECT_4K, },
	{ "sst25vf016b", 0xbf2541, 0, 64 * 1024, 32, SECT_4K, },
	{ "sst25vf032b", 0xbf254a, 0, 64 * 1024, 64, SECT_4K, },

	/* ST Microelectronics -- newer production may have feature updates */
	{ "m25p05",  0x202010,  0, 32 * 1024, 2, },
	{ "m25p10",  0x202011,  0, 32 * 1024, 4, },
	{ "m25p20",  0x202012,  0, 64 * 1024, 4, },
	{ "m25p40",  0x202013,  0, 64 * 1024, 8, },
	{ "m25p80",         0,  0, 64 * 1024, 16, },
	{ "m25p16",  0x202015,  0, 64 * 1024, 32, },
	{ "m25p32",  0x202016,  0, 64 * 1024, 64, },
	{ "m25p64",  0x202017,  0, 64 * 1024, 128, },
	{ "m25p128", 0x202018, 0, 256 * 1024, 64, },

	{ "m45pe10", 0x204011,  0, 64 * 1024, 2, },
	{ "m45pe80", 0x204014,  0, 64 * 1024, 16, },
	{ "m45pe16", 0x204015,  0, 64 * 1024, 32, },

	{ "m25pe80", 0x208014,  0, 64 * 1024, 16, },
	{ "m25pe16", 0x208015,  0, 64 * 1024, 32, SECT_4K, },

	/* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
	{ "w25x10", 0xef3011, 0, 64 * 1024, 2, SECT_4K, },
	{ "w25x20", 0xef3012, 0, 64 * 1024, 4, SECT_4K, },
	{ "w25x40", 0xef3013, 0, 64 * 1024, 8, SECT_4K, },
	{ "w25x80", 0xef3014, 0, 64 * 1024, 16, SECT_4K, },
	{ "w25x16", 0xef3015, 0, 64 * 1024, 32, SECT_4K, },
	{ "w25x32", 0xef3016, 0, 64 * 1024, 64, SECT_4K, },
	{ "w25x64", 0xef3017, 0, 64 * 1024, 128, SECT_4K, },
	{ "w25q64bv", 0xef4017,  0, 64 * 1024, 128, SECT_4K, },
	{ "s25fl008a", 0x1c2017, 0, 64 * 1024, 128, },
	{ "q0bb02b", 0x1c3017, 0, 64 * 1024, 128, },
    { "w25q64-DIP8", 0x00ef4014, 0, 64 * 1024 , 128, SECT_4K },

	{ "GD25Q80", 0xc84014, 0, 64 * 1024, 16, },
	{ "GD25Q64B", 0xc84017, 0, 64 * 1024, 128, SECT_4K ,},
	{ "GD25Q512", 0xc84010, 0, 32 * 1024, 2, SECT_4K },
};

static void get_manufacture_id(unsigned char *buf, int size)
{
	spi_write(rSSIENR, 0x0);
	spi_write(rCTRLR0, 0x3c7);
	spi_write(rCTRLR1, 0x4);
	spi_write(rSSIENR, 0x1);

	spi_write(rDR, CMD_RDID);
	while (spi_read(rRXFLR) < 5);

	*buf++ = spi_read(rDR);
	*buf++ = spi_read(rDR);
	*buf++ = spi_read(rDR);
	*buf++ = spi_read(rDR);
	*buf++ = spi_read(rDR);
}

#if 1

#include "atmel.c"
#include "eon.c"
#include "macronix.c"
#include "ramtron.c"
#include "spansion.c"
#include "sst.c"
#include "stmicro.c"
#include "winbond.c"
#define IDCODE_CONT_LEN 0
static const struct {
	const u8 shift;
	const u8 idcode;
	struct spi_flash *(*probe) (struct spi_slave *spi, u8 *idcode);
} flashes[] = {
	{ 0, 0x1f, spi_flash_probe_atmel, },
	{ 0, 0x1c, spi_flash_probe_eon, },
	{ 0, 0xc2, spi_flash_probe_macronix, },
	{ 0, 0x01, spi_flash_probe_spansion, },
	{ 0, 0xbf, spi_flash_probe_sst, },
	{ 0, 0x20, spi_flash_probe_stmicro, },
	{ 0, 0xef, spi_flash_probe_winbond, },
//	{ 6, 0xc2, spi_fram_probe_ramtron, },
	/* Keep it sorted by best detection */
//	{ 0, 0xff, spi_flash_probe_stmicro, },
//	{ 0, 0xff, spi_fram_probe_ramtron, },
};

static struct sflash_info *flash_based_probe_s1(u8 *idp)
{
	int i, shift;
	struct spi_flash *flash = NULL;
	sfinf_t *fdata = NULL;

	for (shift = 0/* , idp = id */;
	     shift < IDCODE_CONT_LEN && *idp == 0x7f;
	     ++shift, ++idp)
		continue;

	/* search the table for matches in shift and id */
	for (i = 0; i < ARRAY_SIZE(flashes); ++i)
		if (flashes[i].shift == shift && flashes[i].idcode == *idp) {
			/* we have a match, call probe */
			flash = flashes[i].probe(NULL, idp);
			if (flash) { break; }
		}

	if (!flash) {
		printf("SF: Unsupported manufacturer %02x\n", *idp);
		return NULL;
	}

	fdata = malloc(sizeof(*fdata));
	if (!fdata) {
		printf(" %s: Malloc Failed!\n", __func__);
		return NULL;
	}

	memset(fdata, 0, sizeof(*fdata));
	fdata->name        = flash->name;
	fdata->sector_size = flash->sector_size;
	fdata->n_sectors   = flash->size / flash->sector_size;

	return fdata;
}
#else

static struct sflash_info *flash_based_probe_s1(u8 *idp) { return NULL; }

#endif


int seeprom_probe(void)
{
	int			tmp;
	u8			id[5];
	u32			jedec;
	u16                     ext_jedec;
	struct sflash_info	*info;

	get_manufacture_id(id, 5);
	jedec = id[0];
	jedec = jedec << 8;
	jedec |= id[1];
	jedec = jedec << 8;
	jedec |= id[2];

	ext_jedec = id[3] << 8 | id[4];

	if (info = flash_based_probe_s1(id)) {
		info->jedec_id    = jedec;
		info->ext_id      = ext_jedec;
		goto found;
	}

	for (tmp = 0, info = sflash_data;
			tmp < ARRAY_SIZE(sflash_data);
			tmp++, info++) {
		if (info->jedec_id == jedec) {
			if (info->ext_id != 0 && info->ext_id != ext_jedec)
				continue;
			goto found;
		}
	}

	printf("SPI Probe Fail! No Such Devices! --- jedec id: 0x%08x\n", jedec);

	return 0;

found:
	gd->sf_inf = info;
	gd->sf_alen = alen;
	if ((spi_read(BOOTCFG) & 0x1) &&
	    (spi_read(BOOTCFG) & 0x10))
		gd->sf_boot = 1;
	else 	gd->sf_boot = 0;

	gd->sf_rspd = CFG_SPI_FREQ/1000000/RD_BAUD;
	gd->sf_wspd = CFG_SPI_FREQ/1000000/WR_BAUD;

	gd->sf_size  = info->sector_size * info->n_sectors;

	return 1;
}

#endif //(CONFIG_COMMANDS & CFG_CMD_SPI)

