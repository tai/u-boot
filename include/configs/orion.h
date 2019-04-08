/*
 * (C) Copyright 2005
 * STMicroelectronics.
 * Configuration settings for the "Nomadik Hardware Kit" NHK-8815,
 * the evaluation board for the Nomadik 8815 System on Chip.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_ARM926EJS
#define CONFIG_ARCH_CSM1800
#define CONFIG_256M_DDR

#define CONFIG_SKIP_LOWLEVEL_INIT /* we have already been loaded to RAM */

/* commands */
#include <config_cmd_default.h>

#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_NFS
/* There is no NOR flash, so undefine these commands */
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
#define CONFIG_SYS_NO_FLASH
/* There is NAND storage */
//#define CONFIG_NAND_NOMADIK
#define CONFIG_CMD_JFFS2
/* There is EEPROM storage */
#define CONFIG_CMD_EEPROM

/* user interface */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT		"CSM1800h> "
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE \
					+ sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE /* Boot Arg Buffer Size */
#define CONFIG_SYS_MAXARGS	16
#define CONFIG_SYS_LOAD_ADDR	0x800000	/* default load address */
#define CONFIG_SYS_LOADS_BAUD_CHANGE

/* boot config */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_TAG
#define CONFIG_BOOTDELAY	1
#define CONFIG_ETHADDR	00:12:57:5b:4c:ce
#define CONFIG_SERVERIP	192.168.18.74
#define CONFIG_IPADDR   192.168.18.117
#define CONFIG_BOOTARGS	"root=/dev/ram0 console=ttyAMA1,115200n8 init=linuxrc"
/*
#define CONFIG_BOOTCOMMAND	"fsload 0x100000 kernel.uimg;" \
				" fsload 0x800000 initrd.gz.uimg;" \
				" bootm 0x100000 0x800000"
*/

/* memory-related information */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		0x00000000	/* DDR-SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x10000000	/* 256 MB */
//#define PHYS_SDRAM_2		0x08000000	/* SDR-SDRAM BANK #2*/
//#define PHYS_SDRAM_2_SIZE	0x04000000	/* 64 MB */
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1
#define CONFIG_SYS_SDRAM_SIZE	PHYS_SDRAM_1_SIZE
#define CONFIG_SYS_TEXT_BASE        0x2000000
#define CONFIG_SYS_PHY_UBOOT_BASE   CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_INIT_SP_ADDR (CONFIG_SYS_TEXT_BASE - CONFIG_SYS_MALLOC_LEN - 512)

#define CONFIG_STACKSIZE	(512 * 1024)	/* regular stack */

#define CONFIG_USE_IRQ
#ifdef CONFIG_USE_IRQ
#  define CONFIG_STACKSIZE_IRQ	(4 * 1024)	/* IRQ stack */
#  define CONFIG_STACKSIZE_FIQ	(4 * 1024)	/* FIQ stack */
#endif

#define CONFIG_SYS_MEMTEST_START	0x00000000
#define CONFIG_SYS_MEMTEST_END		0x0FFFFFFF
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 12 * 1024 * 1024)

/* timing informazion */
#define CONFIG_SYS_CPU_CLK              378000000
#define CONFIG_SYS_PCLK_FREQ            (CONFIG_SYS_CPU_CLK / 8)
#define CONFIG_SYS_HZ		        1000
#define CONFIG_SYS_TIMERBASE	        0x101E2000
#define CONFIG_SYS_TIMER_INTERVAL       10000
#define CONFIG_SYS_TIMER_RELOAD         (CONFIG_SYS_TIMER_INTERVAL >> 4)  /* Divide by 16 */
#define CONFIG_SYS_TIMER_CTRL          0x84                              /* Enable, Clock / 16 */

/* usb */
#define CONFIG_CMD_FAT
#define CONFIG_CMD_USB

#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_REGS_BASE        0x80210000
#define CONFIG_SYS_USB_OHCI_SLOT_NAME        "CSM1800H"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS   1
#define CONFIG_DOS_PARTITION
#define CONFIG_USB_CSM
#define CONFIG_USB_STORAGE

#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_CLK          CONFIG_SYS_PCLK_FREQ
#define CONFIG_CONS_INDEX	        1
#define CONFIG_SYS_NS16550_REG_SIZE     (-4)
#define CONFIG_BAUDRATE		        115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_NS16550_COM1         0x101F1000
#define CONFIG_SYS_NS16550_COM2         0x101F2000

/* i2c */
#define CONFIG_ORION_I2C
#define CONFIG_CMD_I2C
#define CONFIG_HARD_I2C
#define CONFIG_SYS_I2C_SPEED	100000
#define CONFIG_SYS_I2C_SLAVE    0x7f
#define CONFIG_SYS_I2C_BASE     0x10170000
#define CONFIG_SYS_I2C_CLK      CONFIG_SYS_PCLK_FREQ

#define CONFIG_CMD_GPIO
#define CONFIG_ORION_GPIO
#define ORION_GPIO_BASE      0x10260000
#define REG_GPIO_SWPORTA_DDR 0x24
#define REG_GPIO_SWPORTB_DDR 0x38
#define REG_GPIO_EXT_PORTA   0x60
#define REG_GPIO_EXT_PORTB   0x64
#define REG_GPIO_SWPORTA_DR  0x20
#define REG_GPIO_SWPORTB_DR  0x34

/* eeprom */
#if defined(CONFIG_CMD_EEPROM)
#define CONFIG_SYS_I2C_EEPROM_ADDR            0x50
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN        2
#define CONFIG_ORION_ETHADDR_EEPROM_OFFSET    0x114
#endif

#define CONFIG_DRIVER_CN100
#define CN100_BASEADDRESS			0x41400000
#define CONFIG_BOOTFILE		"uImage"

#define CONFIG_IP_DEFRAG	/* Allows faster download, TFTP and NFS */
//#define CONFIG_TFTP_BLOCKSIZE	4096
#define CONFIG_NFS_READ_SIZE	4096

//#define CONFIG_CMD_NAND
//#define CONFIG_SYS_MAX_NAND_DEVICE	1
//#define CONFIG_SYS_NAND_BASE		0x40000000 /* SMPS0n */

#define CONFIG_CMD_SAVEENV

#if defined (CONFIG_ARCH_CSM_NAND_BOOT)

#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE          0x00020000 /* Total Size of Environment Sector 128KB(for 2k pagesize of csm1800) */
#define CONFIG_ENV_OFFSET        0x00060000 /* environment starts here/aligned with 128KB                         */
#define CONFIG_ENV_SECT_SIZE     0x00020000 /* 64 KB sectors                                                      */

#else

#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_SIZE          0x00010000 /* Total Size of Environment Sector 64KB */
#define CONFIG_ENV_OFFSET        0x00050000 /* environment starts here               */
#define CONFIG_ENV_SECT_SIZE     0x00010000 /* 64 KB sectors                         */

#endif


//#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_ORION
#define CONFIG_CMD_SF

//#define CONFIG_CMD_SPI
//#define CONFIG_ORION_SPI
//#define CONFIG_SYS_SPI_BASE             0x10150000

/* this is needed to make hello_world.c and other stuff happy */
#define CONFIG_SYS_MAX_FLASH_SECT	512
#define CONFIG_SYS_MAX_FLASH_BANKS	1

#define CONFIG_CSM1800H_XPORT
#define CONFIG_XPORT_BASE (0x41400000) 
#define CONFIG_CSM1800H_TUNER
#endif /* __CONFIG_H */
