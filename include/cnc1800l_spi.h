#ifndef __CNC1800L_SPI_H__
#define __CNC1800L_SPI_H__
#define SREG_BASE   	0x80150000

#define rCTRLR0	    (SREG_BASE + 0x00)
#define rCTRLR1	    (SREG_BASE + 0x04)
#define rSSIENR	    (SREG_BASE + 0x08)
#define rMWCR  	    (SREG_BASE + 0x0C)
#define rSER   	    (SREG_BASE + 0x10)
#define rBAUDR    	(SREG_BASE + 0x14)
#define rTXFTLR 	(SREG_BASE + 0x18)
#define rRXFTLR 	(SREG_BASE + 0x1C)
#define rTXFLR  	(SREG_BASE + 0x20)
#define rRXFLR  	(SREG_BASE + 0x24)
#define rSR    		(SREG_BASE + 0x28)
#define rIMR   		(SREG_BASE + 0x2C)
#define rISR   		(SREG_BASE + 0x30)
#define rRISR  		(SREG_BASE + 0x34)
#define rTXOICR 	(SREG_BASE + 0x38)
#define rRXOICR 	(SREG_BASE + 0x3C)
#define rRXUICR		(SREG_BASE + 0x40)
#define rMSTICR		(SREG_BASE + 0x44)
#define rICR   		(SREG_BASE + 0x48)
#define rDR    		(SREG_BASE + 0x60)

#define SPI_PINMUX  (0xb211000c)

#define BOOTCFG		0x80100034

#define spi_write(a, v) 	(*((volatile unsigned short *)(a)) = (v))
#define spi_read(a) 		(*((volatile unsigned short *)(a)))

#define mdelay(s) 	({ int _tmp = 0; while (_tmp++ < (s)) udelay(1000); })
#define mdelay_wheel(s)	({ int _tmp = 0; while (_tmp++ < (s)) { udelay(1000); if (!(_tmp % 100)) spin_wheel(); } })

#define CMD_RDSR 	0x05
#define CMD_WRSR 	0x01
#define CMD_READ 	0x03
#define CMD_WREN 	0x06
#define CMD_PP 		0x02
#define CMD_SE 		0x20
#define CMD_BE_32K	0x52
#define CMD_BE 		0xd8
#define CMD_CE 		0xc7
#define CMD_RDID 	0x9f

#define PP_SZ        256
#define PP_TIME_US 	1500
//#define FL_SEC_SZ 	0x1000
//#define FL_BLK_SZ 	0x10000

/****************************************************************************/

//#define SPI_DEBUG
#ifdef SPI_DEBUG
#define SDEBUG(args...)                     \
	do {                                    \
		printf(args);                       \
	} while(0)
#else
#define SDEBUG(args...) do { } while(0)
#endif

/****************************************************************************/

typedef enum {
	ERASE_SEC,
	ERASE_BLK,
	ERASE_CHIP
} flasherase_t;

typedef enum {
	TMOD_TXRX = 0x0,
	TMOD_TX   = 0x1,
	TMOD_RX   = 0x2,
	TMOD_EE   = 0x3 } TMOD_E;
typedef enum {
	TIMING0   = 0,
	TIMING1   = 1,
	TIMING2   = 2,
	TIMING3   = 3   } TIMING_E;

typedef enum {
	BOOT_NOR  = 0,
	BOOT_SPI  = 1,
	BOOT_NAND = 2   } BOOT_TYPE_E;

typedef struct sflash_info {
	char	*name;
	u32		jedec_id;
	u16     ext_id;

	unsigned sector_size;
	u16		n_sectors;

	u16		flags;
#define	SECT_4K		0x01
} sfinf_t;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof((x)[0]))
#endif

/**********************************************************************************/

void spi__init(void);

int seeprom_read(unsigned caddr, uchar *maddr, unsigned size);
int seeprom_write(unsigned caddr, uchar *maddr, unsigned size);
int seeprom_erase(unsigned cmd, unsigned saddr, unsigned size);
int seeprom_probe(void);

/**********************************************************************************/

#endif
