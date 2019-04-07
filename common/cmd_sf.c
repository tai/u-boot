/*
 * (C) Copyright 2002
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com
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

/*
 * SPI Read/Write Utilities
 */

#include <common.h>
#include <command.h>

DECLARE_GLOBAL_DATA_PTR;

int do_sflashre(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
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

	SDEBUG("caddr: 0x%x, maddr: 0x%x, size: 0x%x\n",
	      caddr, maddr, size);

	seeprom_read(caddr, maddr, size);

	return 0;
}

U_BOOT_CMD(
	sfre,	CONFIG_SYS_MAXARGS/*5*/,	0,	do_sflashre,
	"sfre    - spi flash read byte.\n",
	"\nsfre <maddr> <offset> <len>\n"
	);

int do_sflashwr(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned char *maddr;
	unsigned int caddr;
	int size;

	if (argc != 4) {
		printf("Usage: %s\n", cmdtp->help);
		return -1;
	}

	maddr = (unsigned char *)simple_strtoul(argv[1], NULL, 16);
	caddr = simple_strtoul(argv[2], NULL, 16);
	size = simple_strtoul(argv[3], NULL, 16);

	SDEBUG("caddr: 0x%x, maddr: 0x%x, size: 0x%x\n",
	      caddr, maddr, size);

	seeprom_write(caddr, maddr, size);

	return 0;
}

U_BOOT_CMD(
	sfwr,	CONFIG_SYS_MAXARGS/*5*/,	0,	do_sflashwr,
	"sfwr    - spi flash write byte\n",
	"\nsfwr <maddr> <offset> <len>\n"
	);

int do_sflasher(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	//volatile unsigned char *maddr;
	unsigned int caddr, eaddr;
	unsigned char *arg = (unsigned char *) argv[2];
	int size;

	if (argc < 2) {
		printf("Usage: %s\n", cmdtp->help);
		return -1;
	}

	if (strcmp(argv[1], "all") == 0) {
		seeprom_erase(CMD_CE, 0, 0);
		return 0;
	}

	caddr = simple_strtoul(argv[1], NULL, 16);

	if (*argv[2] == '+'){
		size = simple_strtoul((char*) ++arg, NULL, 16);
	} else {
		eaddr = simple_strtoul(argv[2], NULL, 16);
		size = eaddr - caddr;
	}

	//SDEBUG("caddr: 0x%x, maddr: 0x%x, size: 0x%x\n",  caddr, maddr, size);
    SDEBUG("caddr: 0x%x, size: 0x%x\n",  caddr, size);

	seeprom_erase(CMD_SE, caddr, size);

	return 0;
}

U_BOOT_CMD(
	sfer,	CONFIG_SYS_MAXARGS/*5*/,	0,	do_sflasher,
	"sfer    - spi flash erase\n",
	"\nsfer <from> <to>\n"
	"sfer <from> <+size>\n"
	);

static void print_num(const char *name, ulong value)
{
	printf ("%-12s= 0x%08lX\n", name, value);
}
static void print_str(const char *name, const char *str)
{
	printf ("%-12s= %6s\n", name, str);
}

int do_sfinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	print_str ("chipname",	    gd->sf_inf->name	   );
	print_num ("chipid",	    gd->sf_inf->jedec_id   );
	print_num ("extid",	    	gd->sf_inf->ext_id	   );
	print_num ("secsize",       gd->sf_inf->sector_size);
	print_num ("secnum",	    gd->sf_inf->n_sectors  );
	print_num ("chipsize",	    gd->sf_size 	   );

	return 0;
}

U_BOOT_CMD(
	sfinfo,	CONFIG_SYS_MAXARGS/*5*/,	1,	do_sfinfo,
	"sfinfo  - display spi flash informations.\n",
	NULL
	);

#if 0
void spi_rx_flash(void)
{
	/* write data */
	spi_write(rDR, 0x3);
	/* write addr */
	spi_write(rDR, 0x0);
	spi_write(rDR, 0x0);
	spi_write(rDR, 0x0);
	/* read 3 byte */
	spi_write(rDR, 0x0);	/* for receive only */
	spi_write(rDR, 0x0);	/* for reveive only */
	spi_write(rDR, 0x0);	/* for reveive only */

	/* polling for finish */
	while (spi_read(rSR) & 0x1);
	while (spi_read(rTXFLR) & 0x1);
	while (! (spi_read(rSR) & 0x8));
	udelay(10000);	/* important */

	/* read dummy data */
	spi_read(rDR);
	spi_read(rDR);
	spi_read(rDR);
	spi_read(rDR);

	printf("read data 1: 	0x%x\n", spi_read(rDR));
	printf("read data 2: 	0x%x\n", spi_read(rDR));
	printf("read data 3: 	0x%x\n", spi_read(rDR));
}

void spi_flash_erase(void)
{
	/* write data */
	spi_write(rDR, 0xd8);
	/* write addr */
	spi_write(rDR, 0x0);
	spi_write(rDR, 0x0);
	spi_write(rDR, 0x0);

	/* polling for finish */
	while (spi_read(rSR) & 0x1);
	while (spi_read(rTXFLR) & 0x1);
	while (! (spi_read(rSR) & 0x8));
	udelay(10000);	/* important */
}
void spi_tx_flash(void)
{
	//spi_flash_erase();

	/* write data */
	spi_write(rDR, 0x2);
	/* write addr */
	spi_write(rDR, 0x0);
	spi_write(rDR, 0x0);
	spi_write(rDR, 0x0);
	/* read 3 byte */
#if 0
	spi_write(rDR, 0x1);	/* for receive only */
	spi_write(rDR, 0x2);	/* for reveive only */
	spi_write(rDR, 0x3);	/* for reveive only */
#else
	spi_write(rDR, 0x3);	/* for receive only */
	spi_write(rDR, 0x2);	/* for reveive only */
	spi_write(rDR, 0x1);	/* for reveive only */
#endif

	/* polling for finish */
	while (spi_read(rSR) & 0x1);
	while (spi_read(rTXFLR) & 0x1);
	while (! (spi_read(rSR) & 0x8));
	udelay(10000);	/* important */
}

int do_sflashr(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
/*    spi_rxtx_mode();*/
/*    spi_timing_mode3();*/
	*(volatile unsigned short *)(0x10150008) = 0;
	*(volatile unsigned short *)(0x10150000) = 0x0c7;
	*(volatile unsigned short *)(0x10150008) = 1;

	spi_rx_flash();
}

U_BOOT_CMD(
	sfr,	CONFIG_SYS_MAXARGS/*5*/,	1,	do_sflashr,
	"sfr     - flash read test\n",
	NULL
	);

int do_sflashw(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
/*    spi_rxtx_mode();*/
/*    spi_timing_mode3();*/

/*    spi_timing_mode0();*/
	*(volatile unsigned short *)(0x10150008) = 0;
	*(volatile unsigned short *)(0x10150000) = 0x007;
	*(volatile unsigned short *)(0x10150008) = 1;
	spi_write(rDR, 0x06);
	mdelay(1);
/*    spi_timing_mode3();*/
	*(volatile unsigned short *)(0x10150008) = 0;
	*(volatile unsigned short *)(0x10150000) = 0x0c7;
	*(volatile unsigned short *)(0x10150008) = 1;

	spi_tx_flash();

	return 0;
}

U_BOOT_CMD(
	sfw,	CONFIG_SYS_MAXARGS/*5*/,	1,	do_sflashw,
	"sfw     - flash write test\n",
	NULL
	);

int do_sflashe(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
/*    spi_rxtx_mode();*/
/*    spi_timing_mode0();*/
	*(volatile unsigned short *)(0x10150008) = 0;
	*(volatile unsigned short *)(0x10150000) = 0x007;
	*(volatile unsigned short *)(0x10150008) = 1;
	spi_write(rDR, 0x06);
	mdelay(1);
/*    spi_timing_mode3();*/
	*(volatile unsigned short *)(0x10150008) = 0;
	*(volatile unsigned short *)(0x10150000) = 0x0c7;
	*(volatile unsigned short *)(0x10150008) = 1;

	spi_flash_erase();


	return 0;
}

U_BOOT_CMD(
	sfe,	CONFIG_SYS_MAXARGS/*5*/,	1,	do_sflashe,
	"sfe     - flash erase test\n",
	NULL
	);

#endif

#if 0

/*===================================================================
 * Two interface to write data bus.
 */

int do_spiw(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}
	spi_write(rDR, simple_strtoul(argv[1], NULL, 16));

	return 0;
}

int do_spir(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc > 1) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	printf("%x\n", spi_read(rDR));

	return 0;
}

U_BOOT_CMD(
	spiw,	CONFIG_SYS_MAXARGS/*5*/,	1,	do_spiw,
	"spiw    - wirte to spi bus(32)\n",
	NULL
	);
U_BOOT_CMD(
	spir,	CONFIG_SYS_MAXARGS/*5*/,	1,	do_spir,
	"spir    - read to spi bus(32)\n",
	NULL
	);
#endif

/* Do test */
#if 0
int do_spi_read1K(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	unsigned saddr, maddr, size;
	int i;


	saddr = simple_strtoul(argv[1], NULL, 16);
	maddr = simple_strtoul(argv[2], NULL, 16);
	size = simple_strtoul(argv[3], NULL, 16);

	for (i = 0; i < size; i++) {
		asm_spi_read_1k(saddr, maddr);
		saddr += 1024;
		maddr += 1024;
	}

	return 0;
}
U_BOOT_CMD(
	spir1k,	CONFIG_SYS_MAXARGS/*5*/,	1,	do_spi_read1K,
	"spir1k    - read 1k use asmbly method\n",
	NULL
	);

int do_nand_read(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	unsigned caddr, maddr, size;
	int i;


	caddr = simple_strtoul(argv[1], NULL, 16);
	maddr = simple_strtoul(argv[2], NULL, 16);
	size = simple_strtoul(argv[3], NULL, 16);

	NANDOpen();
	NANDRead_1K(maddr,caddr);
	for (i = 0; i <= size/1024; i++) {
		NANDRead_1K(maddr,caddr);
		caddr += 1024;
		maddr += 1024;
	}
	NANDClose();

	return 0;
}
U_BOOT_CMD(
	nandr,	CONFIG_SYS_MAXARGS/*5*/,	1,	do_nand_read,
	"nandr    - read 1k from nand\n",
	NULL
	);
#endif

