/*
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
#include <command.h>
#include <malloc.h>
#include <image.h>
#include <asm/byteorder.h>
#include <usb.h>

#include <jffs2/jffs2.h>
#include <nand.h>
char nand_already_erase = 0;
#include <cnc1800l_spi.h>

#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif


#ifdef CONFIG_AUTO_UPDATE

#ifndef CONFIG_USB_OHCI_NEW
#error "must define CONFIG_USB_OHCI"
#endif

#ifndef CONFIG_USB_STORAGE
#error "must define CONFIG_USB_STORAGE"
#endif

#ifndef CONFIG_CMD_FAT
#error "must define CONFIG_CMD_FAT"
#endif

#undef AU_DEBUG

#undef debug
#ifdef	AU_DEBUG
#define debug(fmt,args...)	printf (fmt ,##args)
#else
#define debug(fmt,args...)
#endif	/* AU_DEBUG */

/* possible names of files on the USB stick. */
#define AU_FIRMWARE	"u-boot.img"
#define AU_KERNEL	"uimage"
#define AU_RAMDISK	"ramdisk.img"
static  char *filename = "misc.img";
//#define RESET_AFTER_USB_UPGRADE

struct flash_layout {
	long start;
	long end;
};

char bootargs_len = 0;
char bootcmd_len = 0;

/* layout of the FLASH. ST = start address, ND = end address. */
#define AU_FL_FIRMWARE_ST	0x00000000
#define AU_FL_FIRMWARE_ND	0x0007FFFF
#define AU_FL_KERNEL_ST		0x34800000
#define AU_FL_KERNEL_ND		0x34AFFFFF
#define AU_FL_RAMDISK_ST	0x34B00000
#define AU_FL_RAMDISK_ND	0x387FFFFF


static int au_usb_stor_curr_dev; /* current device */

/* index of each file in the following arrays */
#define IDX_FIRMWARE	0
#define IDX_KERNEL	1
#define IDX_RAMDISK	2

/* max. number of files which could interest us */
#define AU_MAXFILES 3

#define BOOT_START	0x34400000

/* pointers to file names */
char *aufile[AU_MAXFILES];

/* sizes of flash areas for each file */
long ausize[AU_MAXFILES];

/* the index table of the file type */
int autype[AU_MAXFILES] = { \
	IH_TYPE_FIRMWARE, IH_TYPE_KERNEL, IH_TYPE_RAMDISK};

/* array of flash areas start and end addresses */
struct flash_layout aufl_layout[AU_MAXFILES] = { \
	{AU_FL_FIRMWARE_ST, AU_FL_FIRMWARE_ND,}, \
	{AU_FL_KERNEL_ST, AU_FL_KERNEL_ND,}, \
	{AU_FL_RAMDISK_ST, AU_FL_RAMDISK_ND,}, \
};

/* where to load files into memory */
#define LOAD_ADDR 	((unsigned char *)0x8000000)	//Modify By River 04.08.2010 0x00200000
#define VALID_ADDR 	(LOAD_ADDR + 2 + bootargs_len + bootcmd_len + 48)

/* the app is the largest image */
#define MAX_LOADSZ ausize[IDX_RAMDISK]

/* externals */
extern int fat_register_device(block_dev_desc_t *, int);
extern int file_fat_detectfs(void);
extern long file_fat_read(const char *, void *, unsigned long);
//extern int i2c_read (unsigned char, unsigned int, int , unsigned char* , int);
extern int flash_sect_erase(ulong, ulong);
extern int flash_sect_protect (int, ulong, ulong);
extern int flash_write (char *, ulong, ulong);
/* change char* to void* to shutup the compiler */
extern block_dev_desc_t *get_dev (char*, int);
extern int u_boot_hush_start(void);

int au_check_cksum_valid(long nbytes)
{
	image_header_t *hdr;
	unsigned long checksum;

	hdr = (image_header_t *)(VALID_ADDR);

	/* check the data CRC */
	checksum = ntohl(hdr->ih_dcrc);

	if (crc32 (0, (uchar *)(VALID_ADDR + sizeof(*hdr)), ntohl(hdr->ih_size)) != checksum) {
		printf ("usb upgrade file bad data checksum\n");
		return -1;
	}
	return 0;
}

int au_check_header_valid(int idx, long nbytes)
{
	image_header_t *hdr;
	unsigned long checksum;

	hdr = (image_header_t *)VALID_ADDR;
	/* check the easy ones first */
#undef CHECK_VALID_DEBUG
#ifdef CHECK_VALID_DEBUG
	printf("magic %#x %#x ", ntohl(hdr->ih_magic), IH_MAGIC);
	printf("arch %#x %#x ", hdr->ih_arch, IH_ARCH_ARM);
	printf("size %#x %#lx ", ntohl(hdr->ih_size), nbytes);
	printf("type %#x %#x ", hdr->ih_type, IH_TYPE_KERNEL);
#endif
	if (nbytes < sizeof(*hdr)) {
		printf ("Image %s bad header SIZE\n", aufile[idx]);
		return -1;
	}
	if (ntohl(hdr->ih_magic) != IH_MAGIC || hdr->ih_arch != IH_ARCH_ARM) {
		printf ("Image %s bad MAGIC or ARCH\n", aufile[idx]);
		return -1;
	}
	/* check the hdr CRC */
	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;

	if (crc32 (0, (uchar *)hdr, sizeof(*hdr)) != checksum) {
		printf ("Image %s bad header checksum\n", aufile[idx]);
		return -1;
	}
	hdr->ih_hcrc = htonl(checksum);
	/* check the type - could do this all in one gigantic if() */
	if ((idx == IDX_FIRMWARE) && (hdr->ih_type != IH_TYPE_FIRMWARE)) {
		printf ("Image %s wrong type\n", aufile[idx]);
		return -1;
	}
	if ((idx == IDX_KERNEL) && (hdr->ih_type != IH_TYPE_KERNEL)) {
		printf ("Image %s wrong type\n", aufile[idx]);
		return -1;
	}

	/* recycle checksum */
	checksum = ntohl(hdr->ih_size);
	/* for kernel and app the image header must also fit into flash */
	if (idx != IDX_FIRMWARE)
		checksum += sizeof(*hdr);
	/* check the size does not exceed space in flash. HUSH scripts */
	/* all have ausize[] set to 0 */
	if ((ausize[idx] != 0) && (ausize[idx] < checksum)) {
		printf ("Image %s is bigger than FLASH\n", aufile[idx]);
		return -1;
	}
	return 0;
}

void decrypt(char * addr, int nbytes)
{
	int i;

	for (i = 0; i < nbytes; i++) {
		*addr++ ^= 0xE3;
	}
}

int au_do_update(int idx, char * address)
{
	image_header_t *hdr;
	long start, end;
	char *addr;
	int off;
    int rc=-1;
	uint nbytes;
	char strcmdline[100];
	char argv0[20];
	char argv1[20];
	char argv2[20];
	char argv3[20];
	char argv4[20];

	hdr = (image_header_t *)address;

	start = aufl_layout[idx].start;
	end = aufl_layout[idx].end;

	/* unprotect the address range */
	/* this assumes that ONLY the firmware is protected!
	   if (idx == IDX_FIRMWARE) {
	   flash_sect_protect(0, AU_FL_FIRMWARE_ST, aufl_layout[idx+1].start-1);
	   }
	   */

	/*
	 * erase the address range.
	 */
	/* Add By River 12.16.2008 */
	switch( hdr->ih_type )
	{
		case IH_TYPE_FIRMWARE:
			fpc_init_show("E.Ub");
			break;

		case IH_TYPE_KERNEL:
			fpc_init_show("E.uI");
			break;

		case IH_TYPE_RAMDISK:
			fpc_init_show("E.FS");
			break;

		default:
			break;
	}
	debug ("flash_sect_erase(%lx, %lx);\n", start, end);

	/*--------------------------------------------------------------------*/
	/* 20091021 River's suggestion                                        */
	/*--------------------------------------------------------------------*/

	if(( hdr->ih_type == IH_TYPE_KERNEL )||( hdr->ih_type == IH_TYPE_RAMDISK ))
	{
#if 1
		if( nand_already_erase == 0 )
		{
			sprintf( strcmdline, "nand erase.chip");
			printf (strcmdline);
			printf ("\n");
			sprintf( argv0, "%s", "nand" );
			sprintf( argv1, "%s", "erase.chip" );
			//sprintf( argv2, "%x", ((start - AU_FL_RAMDISK_ST)/0x10000)*0x10000 );
			//sprintf( argv3, "%x", ((end - start + 1)/0x10000+((end - start + 1)%0x10000?1:0))*0x10000 );
			//sprintf( argv2, "%s", "clean" );
			//sprintf( argv3, "%x", 0x0 );
			//sprintf( argv4, "%x", 0x4000000 );

			char *argv[] = {
				argv0,
				argv1
			};
			rc = do_nand( NULL, 0, sizeof(argv)/sizeof(char *), argv );
			printf ("nand erase finished!\n");
			nand_already_erase = 1;
		}
#else
		flash_sect_erase(start, 0x34800000);
		printf ("flash_sect_erase(%lx, %lx);\n", start, 0x34800000);
#endif
	}
	/* Modify By River 27.04.2009 */
	else if( hdr->ih_type == IH_TYPE_FIRMWARE )
	{
#if 1
		sprintf( strcmdline, "sfer %x %x", AU_FL_FIRMWARE_ST,
				AU_FL_FIRMWARE_ND );
		printf (strcmdline);
		printf ("\n");
		sprintf( argv0, "%s", "sfer" );
		sprintf( argv1, "%x", AU_FL_FIRMWARE_ST );
		sprintf( argv2, "%x", AU_FL_FIRMWARE_ND );

		char *argv[] = {
			argv0,
			argv1,
			argv2
		};
		rc = do_sflasher( NULL, 0, sizeof(argv)/sizeof(char *), argv );
		printf ("spi flash erase finished!\n");
#else
		flash_sect_erase(AU_FL_FIRMWARE_ST, end);
		printf ("flash_sect_erase(%lx, %lx);\n", AU_FL_FIRMWARE_ST, aufl_layout[idx+1].start-1);
#endif
	}
#if 0
	else
	{
		flash_sect_erase(start, end);
		printf ("flash_sect_erase(%lx, %lx);\n", start, end);
	}
#endif
	/*--------------------------------------------------------------------*/


	wait_ms(100);
	/* strip the header for the U-boot */
	if (hdr->ih_type != IH_TYPE_KERNEL) {
		addr = (char *)((char *)hdr + sizeof(*hdr));
		off = 0;
		nbytes = ntohl(hdr->ih_size);
	} else {
		addr = (char *)hdr;
		off = sizeof(*hdr);
		nbytes = sizeof(*hdr) + ntohl(hdr->ih_size);
	}

	/* copy the data from RAM to FLASH */
	/* Add By River 12.16.2008 */
	switch( hdr->ih_type )
	{
		case IH_TYPE_FIRMWARE:
			fpc_init_show("P.Ub");
			break;

		case IH_TYPE_KERNEL:
			fpc_init_show("P.uI");
			break;

		case IH_TYPE_RAMDISK:
			fpc_init_show("P.FS");
			break;

		default:
			break;
	}


	if(( hdr->ih_type == IH_TYPE_KERNEL )||( hdr->ih_type == IH_TYPE_RAMDISK ))
	{
		sprintf( strcmdline, "nand write.jffs2 %x %x %x", addr, start - AU_FL_KERNEL_ST, nbytes);
		printf( strcmdline );
		printf("\n");
		sprintf( argv0, "%s", "nand" );
		sprintf( argv1, "%s", "write.jffs2" );
		sprintf( argv2, "%x", addr );
		sprintf( argv3, "%x", start - AU_FL_KERNEL_ST );
		sprintf( argv4, "%x", nbytes );
		char *argv[] = {
			argv0,
			argv1,
			argv2,
			argv3,
			argv4
		};
		rc = do_nand( NULL, 0, sizeof(argv)/sizeof(char *), argv );
	}
	if(hdr->ih_type == IH_TYPE_FIRMWARE)
	{
		sprintf( strcmdline, "sfwr %x %x %x", addr, start, nbytes );
		printf( strcmdline );
		printf("\n");
		sprintf( argv0, "%s", "sfwr" );
		sprintf( argv1, "%x", addr );
		sprintf( argv2, "%x", start );
		sprintf( argv3, "%x", nbytes );
		char *argv[] = {
			argv0,
			argv1,
			argv2,
			argv3
		};
		rc = do_sflashwr( NULL, 0, sizeof(argv)/sizeof(char *), argv );
	}
	else
	{
		printf("flash_write(%p, %lx %x)\n", addr, start, nbytes);
		debug ("flash_write(%p, %lx %x)\n", addr, start, nbytes);
		/* rc = flash_write(addr, start, nbytes);*/
	}
	printf("flash write finished!\n");
	if (rc != 0) {
		printf("Flashing failed due to error %d\n", rc);
		return -1;
	}

	/* check the dcrc of the copy
	   if (crc32 (0, (uchar *)(start + off), ntohl(hdr->ih_size)) != ntohl(hdr->ih_dcrc)) {
	   printf ("Image %s Bad Data Checksum After COPY\n", aufile[idx]);
	   return -1;
	   }
	   */

	/* protect the address range */
	/* this assumes that ONLY the firmware is protected!
	   if (idx == IDX_FIRMWARE)
	   flash_sect_protect(1, AU_FL_FIRMWARE_ST, aufl_layout[idx+1].start-1);
	   */
	return 0;
}

/*
 * this is called from board_init() after the hardware has been set up
 * and is usable. That seems like a good time to do this.
 * Right now the return value is ignored.
 */
int do_auto_update(void)
{
	block_dev_desc_t *stor_dev;
	long sz;
	int i, j, res, bitmap_first, old_ctrlc;
	//char *env;
	long start, end;
	long actual_size;
	image_header_t *hdr;
	char * addr;
	char st[9], len[9];
	char *ptr = LOAD_ADDR;
	char *ep;
	char *bootargs = NULL;
	char *bootcmd = NULL;

	au_usb_stor_curr_dev = -1;
	/* start USB */
	if (usb_stop() < 0) {
		debug ("usb_stop failed\n");
		return -1;
	}
	if (usb_init() < 0) {
		debug ("usb_init failed\n");
		return -1;
	}

	/*
	 * check whether a storage device is attached (assume that it's
	 * a USB memory stick, since nothing else should be attached).
	 */
	au_usb_stor_curr_dev = usb_stor_scan(0);
	if (au_usb_stor_curr_dev == -1) {
		debug ("No device found. Not initialized?\n");
		return -1;
	}

	fpc_init_show("Upda");	/* Add By River 12.16.2008 */

	/* check whether it has a partition table */
	stor_dev = get_dev("usb", 0);
	if (stor_dev == NULL) {
		debug ("uknown device type\n");
		return -1;
	}
	if (fat_register_device(stor_dev, 1) != 0) {
		debug ("Unable to use USB %d:%d for fatls\n",
				au_usb_stor_curr_dev, 1);
		return -1;
	}
	if (file_fat_detectfs() != 0) {
		debug ("file_fat_detectfs failed\n");
	}

	/* initialize the array of file names */
	memset(aufile, 0, sizeof(aufile));
	aufile[IDX_FIRMWARE] = AU_FIRMWARE;
	aufile[IDX_KERNEL] = AU_KERNEL;
	aufile[IDX_RAMDISK] = AU_RAMDISK;
	/* initialize the array of flash sizes */
	memset(ausize, 0, sizeof(ausize));
	ausize[IDX_FIRMWARE] = (AU_FL_FIRMWARE_ND + 1) - AU_FL_FIRMWARE_ST;
	ausize[IDX_KERNEL] = (AU_FL_KERNEL_ND + 1) - AU_FL_KERNEL_ST;
	ausize[IDX_RAMDISK] = (AU_FL_RAMDISK_ND + 1) - AU_FL_RAMDISK_ST;
	/*
	 * now check whether start and end are defined using environment
	 * variables.
	 */
	/*start = -1;
	  end = 0;
	  env = getenv("firmware_st");
	  if (env != NULL)
	  start = simple_strtoul(env, NULL, 16);
	  env = getenv("firmware_nd");
	  if (env != NULL)
	  end = simple_strtoul(env, NULL, 16);
	  if (start >= 0 && end && end > start) {
	  ausize[IDX_FIRMWARE] = (end + 1) - start;
	  aufl_layout[0].start = start;
	  aufl_layout[0].end = end;
	  }
	  start = -1;
	  end = 0;
	  env = getenv("kernel_st");
	  if (env != NULL)
	  start = simple_strtoul(env, NULL, 16);
	  env = getenv("kernel_nd");
	  if (env != NULL)
	  end = simple_strtoul(env, NULL, 16);
	  if (start >= 0 && end && end > start) {
	  ausize[IDX_KERNEL] = (end + 1) - start;
	  aufl_layout[1].start = start;
	  aufl_layout[1].end = end;
	  }
	  start = -1;
	  end = 0;
	  env = getenv("ramdisk_st");
	  if (env != NULL)
	  start = simple_strtoul(env, NULL, 16);
	  env = getenv("ramdisk_nd");
	  if (env != NULL)
	  end = simple_strtoul(env, NULL, 16);
	  if (start >= 0 && end && end > start) {
	  ausize[IDX_KERNEL] = (end + 1) - start;
	  aufl_layout[2].start = start;
	  aufl_layout[2].end = end;
	  }*/
	/* make certain that HUSH is runnable */
	u_boot_hush_start();
	/* make sure that we see CTRL-C and save the old state */
	old_ctrlc = disable_ctrlc(0);

	bitmap_first = 0;
	// the first byte is the string length of bootargs

	fpc_init_show("Read");	/* Add By River 12.16.2008 */

	sz = file_fat_read(filename, LOAD_ADDR, 2);
	if (sz <= 0) {
		debug ("file %s not found\n", filename);
		return -1;
	}
	decrypt(LOAD_ADDR, 2);
	bootargs_len = *ptr++;
	bootcmd_len = *ptr;

	sz = file_fat_read(filename, LOAD_ADDR,
			2 + bootargs_len + bootcmd_len + 48 + sizeof(image_header_t));
	if (sz <= 0 || sz < 2 + bootargs_len + bootcmd_len + 48 + sizeof(image_header_t)) {
		debug ("file %s not found\n", filename);
		return -1;
	}
	decrypt(LOAD_ADDR, 2 + bootargs_len + bootcmd_len + 48 + sizeof(image_header_t));
	bootargs = malloc(bootargs_len + 1);
	memcpy(bootargs, LOAD_ADDR + 2, bootargs_len);
	bootargs[bootargs_len] = '\0';
	printf("[bootargs]: %s\n", bootargs);
	bootcmd = malloc(bootcmd_len + 1);
	memcpy(bootcmd, LOAD_ADDR + 2 + bootargs_len, bootcmd_len);
	bootcmd[bootcmd_len] = '\0';
	printf("[bootcmd]: %s\n", bootcmd);

	ptr = LOAD_ADDR + 2 + bootargs_len + bootcmd_len;
	for (i = 0; i < 3; i++) {
		for(j = 0; j < 8; j++) {
			st[j] = *ptr++;
		}
		st[j] = '\0';
		for(j = 0; j < 8; j++) {
			len[j] = *ptr++;
		}
		len[j] = '\0';

		start = simple_strtoul(st, &ep, 16);
		if (ep == st || *ep != '\0') {
			printf("simple_strtoul return error!\n");
			return -1;
		}
		if (start == 0)
			continue;

		printf("start address: %x len:%x\n", start, len );
		/*
		   if ( i < 2 )
		   {
		   if (do_addr_spec(st, len, &start, &end) < 0) {
		   printf ("Bad address format\n");
		   return -1;
		   }
		   }
		   else*/
		{
			end = start + len;
		}

		if (start >= end) {
			printf("Start address beyond end address\n");
			return -1;
		}
		ausize[i] = (end + 1) - start;
		aufl_layout[i].start = start;
		aufl_layout[i].end = end;
		printf("%s: 0x%x--0x%x\n", aufile[i], (unsigned int)start, (unsigned int)end);
	}
	hdr = (image_header_t *)(VALID_ADDR);
	actual_size = ntohl(hdr->ih_size);
	sz = file_fat_read(filename, LOAD_ADDR,
			2 + bootargs_len + bootcmd_len + 48 + sizeof(image_header_t) + actual_size);
	decrypt(LOAD_ADDR, sz);
	if (au_check_cksum_valid(sz) < 0) {
		debug ("%s checksum not valid\n", filename);
		return -1;
	}

	addr = VALID_ADDR + sizeof(image_header_t);
	nand_already_erase = 0;
	/* just loop thru all the possible files */
	for (i = 0; i < AU_MAXFILES; i++) {
		hdr = (image_header_t *)addr;
		if (hdr->ih_type == autype[i]) {
			res = au_do_update(i, addr);
			addr += (sizeof(image_header_t) + ntohl(hdr->ih_size));
		}
	}

	if (bootargs_len != 0)
		setenv("bootargs", bootargs);
	if (bootcmd_len != 0)
		setenv("bootcmd", bootcmd);
	if ((bootargs_len != 0) || (bootcmd_len != 0))
		saveenv();

	free(bootargs);
	free(bootcmd);

	usb_stop();
	/* restore the old state */
	disable_ctrlc(old_ctrlc);

	fpc_init_show("rebt");	/* Add By River 12.16.2008 */
	//while(1){};
#ifdef RESET_AFTER_USB_UPGRADE
	do_reset( NULL, 0, 0, NULL );
#endif
	return 0;
}
#endif /* CONFIG_AUTO_UPDATE */

