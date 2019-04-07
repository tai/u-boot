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
#include <cnc1800l_spi.h>

#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif

#include <u-boot/md5.h>

#ifdef CONFIG_AUTO_UPDATE

#if 0
#ifndef CONFIG_USB_OHCI_NEW
#error "must define CONFIG_USB_OHCI"
#endif

#ifndef CONFIG_USB_STORAGE
#error "must define CONFIG_USB_STORAGE"
#endif

#ifndef CONFIG_CMD_FAT
#error "must define CONFIG_CMD_FAT"
#endif
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

unsigned int bootargs_len = 0;
unsigned int bootcmd_len = 0;

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

/* the index table of the OS type */
int ostype[AU_MAXFILES] = { \
	IH_OS_LINUX, IH_OS_LINUX, IH_OS_LINUX}; //Reduce the probability of ramdom while we only upgrade uboot. --leiyu,2011.12.13

/* array of flash areas start and end addresses */
struct flash_layout aufl_layout[AU_MAXFILES]={{0}};

#define VALID_ADDR 	(CONFIG_USB_UPGRADE_DATA_ADDR + 2 + bootargs_len + bootcmd_len + 48)

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
	unsigned int start, end;
	char *addr;
	int off=0;
	int rc=0;
	unsigned int nbytes;
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
		sprintf( argv0, "%s", "nand" );
		sprintf( argv1, "%s", "erase" );
		sprintf( argv2, "%X", start );
		sprintf( argv3, "%X", end-start );

		char *argv[] = {
			argv0,
			argv1,
			argv2,
			argv3
		};

		printf("running:%s %s %s %s\n", argv0, argv1, argv2, argv3);
		rc = do_nand( NULL, 0, sizeof(argv)/sizeof(char *), argv );
		printf ("nand erase %X %X finished!\n",start,end-start);
	}
	/* Modify By River 27.04.2009 */
	else if( hdr->ih_type == IH_TYPE_FIRMWARE )
	{
		sprintf( argv0, "%s", "sfer" );
		sprintf( argv1, "%x", start );
		sprintf( argv2, "%x", end-start );

		char *argv[] = {
			argv0,
			argv1,
			argv2
		};
		printf("running:%s %s %s\n", argv0, argv1, argv2);
		rc = do_sflasher( NULL, 0, sizeof(argv)/sizeof(char *), argv );
		printf ("spi flash erase finished!\n");
	}
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
		sprintf( argv0, "%s", "nand" );
		sprintf( argv1, "%s", "write.jffs2" );
		sprintf( argv2, "%x", (unsigned int)addr );
		sprintf( argv3, "%x", start );
		sprintf( argv4, "%x", nbytes );
		char *argv[] = {
			argv0,
			argv1,
			argv2,
			argv3,
			argv4
		};
		printf("running:%s %s %s %s %s\n", argv0, argv1, argv2, argv3, argv4);
		rc = do_nand( NULL, 0, sizeof(argv)/sizeof(char *), argv );
	}
	if(hdr->ih_type == IH_TYPE_FIRMWARE)
	{
		sprintf( argv0, "%s", "sfwr" );
		sprintf( argv1, "%x", (unsigned int)addr );
		sprintf( argv2, "%x", start );
		sprintf( argv3, "%x", nbytes );
		char *argv[] = {
			argv0,
			argv1,
			argv2,
			argv3
		};
		printf("running:%s %s %s %s\n", argv0, argv1, argv2, argv3);
		rc = do_sflashwr( NULL, 0, sizeof(argv)/sizeof(char *), argv );
	}
	else
	{
		printf("flash_write(%p, %x %x)\n", addr, start, nbytes);
		debug ("flash_write(%p, %x %x)\n", addr, start, nbytes);
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


int hex2string(unsigned char *in , unsigned char *out, int len)
{
	unsigned char a ;
	int i = 0 ;
	unsigned char *index = out ;
	for(i = 0 ; i<len*2 ; i++)
	{
		if(i%2 == 0)
			a = (in[i/2] & 0xf0)>>4 ;   
		else 
			a = in[i/2]&0x0f ;

		if(a >= 0 && a<= 9 )
			a = a - 0 + 48 ;
		else if(a>= 10 && a<= 15)
			a = a - 10 + 'a' ;
		else 
			return -1;

		*(index++) = a ;
	}   
	return 0;
}   





/*
 * this is called from board_init() after the hardware has been set up
 * and is usable. That seems like a good time to do this.
 * Right now the return value is ignored.
 */
int do_auto_update(int size)
{
	block_dev_desc_t *stor_dev;
	long sz;
	int i, j, res, bitmap_first, old_ctrlc;
	char *env;
	long start, end;
	long actual_size;
	image_header_t *hdr;
	char * addr;
	char st[9], len[9];
	char *ptr = (char *)CONFIG_USB_UPGRADE_DATA_ADDR;
	char *ep;
	char *bootargs = NULL;
	char *bootcmd = NULL;

	/* initialize the array of file names */
	memset(aufile, 0, sizeof(aufile));
	aufile[IDX_FIRMWARE] = AU_FIRMWARE;
	aufile[IDX_KERNEL] = AU_KERNEL;
	aufile[IDX_RAMDISK] = AU_RAMDISK;
	/* initialize the array of flash sizes */
	memset(ausize, 0, sizeof(ausize));
	/* make sure that we see CTRL-C and save the old state */
	old_ctrlc = disable_ctrlc(0);

	if(!size){
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
#ifdef CONFIG_UPDATE_WITH_EXT
		if(ext_fs_read("usb" , 0,&sz) == 0 )
			goto update_with_ext;
#endif
		if (fat_register_device(stor_dev, 1) != 0) {
			debug ("Unable to use USB %d:%d for fatls\n",
					au_usb_stor_curr_dev, 1);
			return -1;
		}
		if (file_fat_detectfs() != 0) {
			debug ("file_fat_detectfs failed\n");
			printf ("file_fat_detectfs failed\n");
		}

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

		bitmap_first = 0;
		// the first byte is the string length of bootargs

		fpc_init_show("Read");	/* Add By River 12.16.2008 */

		sz = file_fat_read(filename, CONFIG_USB_UPGRADE_DATA_ADDR, 0);
		if (sz <= 0) {
			debug ("file %s not found\n", filename);
			return -1;
		}

	}else{
		size -= 256;
		sz = size;
		printf("Upgrade directly from RAM: 0x%08x, size = 0x%08x\n", CONFIG_USB_UPGRADE_DATA_ADDR, sz);
	}

#ifdef CONFIG_UPDATE_WITH_EXT
update_with_ext:
#endif	

	dcache_enable();
#ifdef CONFIG_UPDATE_MD5_CHECK
	char *md5string= NULL ;
	md5string = (char *)malloc(32 + 1);
	uint32_t md5_offset = sz;
	unsigned char *temp_md5buf = (unsigned char *)CONFIG_USB_UPGRADE_DATA_ADDR ;
	md5(temp_md5buf, md5_offset, md5string);
	md5string[16]='\0';
	char *out=(char *)malloc(33);
	hex2string(md5string,out,16);
	out[32]='\0';
	printf("md5 string is %s\n",out);
	memcpy(md5string,out,32);
	md5string[32]='\0';
	free(out);
	env = getenv("md5");
	if (env != NULL )
	{	
		printf("env md5 is %s\n",env);
		if (strncmp(env,md5string,32) == 0 )
		{
			dcache_disable();
			goto auto_up_end;
		}	
	}
#endif

	decrypt((char *)CONFIG_USB_UPGRADE_DATA_ADDR, sz);
	bootargs_len = *ptr++;
	bootcmd_len = *ptr;
	bootargs = malloc(bootargs_len + 1);
	memcpy(bootargs, CONFIG_USB_UPGRADE_DATA_ADDR + 2, bootargs_len);
	bootargs[bootargs_len] = '\0';
	printf("[bootargs]: %s\n", bootargs);
	bootcmd = malloc(bootcmd_len + 1);
	memcpy(bootcmd, CONFIG_USB_UPGRADE_DATA_ADDR + 2 + bootargs_len, bootcmd_len);
	bootcmd[bootcmd_len] = '\0';
	printf("[bootcmd]: %s\n", bootcmd);

	ptr = CONFIG_USB_UPGRADE_DATA_ADDR + 2 + bootargs_len + bootcmd_len;

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

		end = simple_strtoul(len, &ep, 16);
		if (ep == st || *ep != '\0') 
		{
			printf("simple_strtoul return error!\n");
			return -1;
		}
		if (end == 0)
			continue;

		if (start >= end) {
			printf("Start address beyond end address\n");
			return -1;
		}
		ausize[i] = (end + 1) - start;
		aufl_layout[i].start = start;
		aufl_layout[i].end = end;
		printf("%s: 0x%lX--0x%lX\n", aufile[i], aufl_layout[i].start, aufl_layout[i].end);
	}
	hdr = (image_header_t *)(VALID_ADDR);
	actual_size = ntohl(hdr->ih_size);
	if (au_check_cksum_valid(sz) < 0) {
		debug ("%s checksum not valid\n", filename);
		return -1;
	}

	dcache_disable();

	addr = VALID_ADDR + sizeof(image_header_t);
	/* just loop thru all the possible files */
	for (i = 0; i < AU_MAXFILES; i++) {
		hdr = (image_header_t *)addr;
		if (hdr->ih_type == autype[i] && hdr->ih_os == ostype[i]) {

			res = au_do_update(i, addr);
			addr += (sizeof(image_header_t) + ntohl(hdr->ih_size));
		}
	}

#ifdef CONFIG_UPDATE_MD5_CHECK
	if(md5string != NULL )
		setenv("md5",md5string);
#endif	
	if (bootargs_len != 0)
		setenv("bootargs", bootargs);
	if (bootcmd_len != 0)
		setenv("bootcmd", bootcmd);
	if ((bootargs_len != 0) || (bootcmd_len != 0))
		saveenv();
	free(bootargs);
	free(bootcmd);

auto_up_end:	
#ifdef CONFIG_UPDATE_MD5_CHECK
	if(md5string)
		free(md5string);
#endif	
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

