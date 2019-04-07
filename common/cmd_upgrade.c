#include <common.h>
#include <command.h>
#include <asm/errno.h>

DECLARE_GLOBAL_DATA_PTR;

//upgrade spiuboot force cavm18_bootloader.bin
//upgrade nethd // filename defaults to cavm18_netHD.sbin


//usb start
//fatload usb 0 0x2000000 su-boot.bin
//sfer 0 +${filesize}
//sfwr 0x2000000 0 ${filesize}

//usb start
//fatload usb 0x0 0x02100000 ubootpImage
//saveenv
//nand erase 0x02400000 ${filesize}
//nand write 0x02100000 0x02400000 ${filesize}
//reset

//#define MAX_UBOOT_SIZE (0x80000)
#define MAX_SPI_UBOOT_SIZE_STRING ("0x60000")
#define MAX_UBOOT_SIZE_STRING ("0x80000")

#define FILE_MEM_ADDR_STRING          ("0x02100000")
#define FILE_MEM_ADDR                 (0x02100000)
#define TMP_UBOOT_ADDR_STRING         ("0x0200000c")
#define TMP_UBOOT_ADDR                (0x0200000c)
#define SECURE_IMAGE_FILENAME         "cavm18_netHD.sbin"
#define SECURE_IMAGE_EXTENSION        "sbin"
#define INSECURE_UBOOT_IMAGE_FILENAME "cavm18_bootloader.bin"
#define INSECURE_IMAGE_FILENAME       "cavm18_nethd.bin"
#define UPGRADE_SPIUBOOT              0
#define UPGRADE_UBOOT       	      1
#define UPGRADE_NETHD	              2

extern int do_fat_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_sflashwr(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_sflasher(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_sflashre(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_usb(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[]);
extern int do_esp1800l_flash_util (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

/* return the extension of a filename. returns filename if no extension */
char *get_extension(char *filename)
{
	char *ext = filename;
	
	if (ext == NULL)
		return ext;

	// Work backwards from end of the name to the '.'
	ext = filename + strlen(filename);

	while ((*(ext-1) != '.') && (ext != filename)) {
		ext--;
	}

	return ext;
}


int ReadNetHdBinFile(char *filename)
{
	int status;
	char* cmd1[] = { "fatload" , "usb", "0" , FILE_MEM_ADDR_STRING, filename  };
	status = do_fat_fsload(NULL, 0, 5 , cmd1);
	if ( 0 == status)
	{		
		saveenv();
	}
	else
	{
		printf("ERROR: failed to load %s\n", filename);
	}
	return status;
}

int EraseNetHdRegion(void)
{
	 int status;
	//nand erase 0x02400000 ${filesize}
	//char filesize[16]={0};
	//char* cmd1[] = { "nand" , "erase", "0x02400000" , & filesize[0] };
	//strncpy(filesize , getenv("filesize") , sizeof(filesize) ) ;
	char* cmd1[] = { "nand" , "erase", "0x02400000" , "0x02000000" };
	status = do_nand(NULL, 0, 4 , cmd1);
	if ( 0 == status)
	{		
		
	}
	else
	{
		printf("ERROR: unable to erase NAND flash for cavm18_nethd.bin\n");
	}
	return status;
}

int ProgramNetHdToNandFlash(char *filename)
{
	int status = 0;;
	char *ext = get_extension(filename);
	int retries = 3;

	do {
		if (0 == strcmp(ext, SECURE_IMAGE_EXTENSION)) {
			// esp1800l_flash_util
			char* cmd1[] = { "esp1800l_flash_util" , "--kernel", "--force", FILE_MEM_ADDR_STRING};

			status = do_esp1800l_flash_util(NULL, 0 , 4 , cmd1 ) ;
			// status will be -EIO on nand error
		} else {
			char filesize[16]={0};
			char* cmd1[] = { "nand" , "write" , FILE_MEM_ADDR_STRING , "0x02400000",  & filesize[0] };
			strncpy(filesize , getenv("filesize") , sizeof(filesize) ) ;

			status = EraseNetHdRegion();
			if (0 != status ) {
				printf("ERROR: fail to erase NAND flash nethd region\n");
			} else {
				//nand write 0x02100000 0x02400000 ${filesize}
				status = do_nand(NULL, 0, 5 , cmd1);
				if ( 0 != status)
				{
					printf("ERROR: unable to write %s to NAND flash\n", filename);
					status = -EIO;
				}
			}
		}
	} while ((--retries > 0) && (status == -EIO));

	return status;
}

// returns 0 if it found valid boot addresses in SPI
int ubootBootAddrs(int *bootaddr1, int *bootaddr2)
{
	int status;
	char spi_addr[16]={0};
	char* cmd1[] = { "sfre" , TMP_UBOOT_ADDR_STRING, &spi_addr[0] ,  "4"  };
	*bootaddr1 = *bootaddr2 = -1;

	strcpy(spi_addr, "0x208");

	// read from 0x0208 from SPI and if it is 0x2c00 it boots from SPI (insecure board). 
	status = do_sflashre(NULL, 0 , 4 , cmd1 ) ;
	if (0 != status) {
		printf("ERROR: fail to read SPI flash\n");
	} else {
		*bootaddr1 = (int)*(int *)(TMP_UBOOT_ADDR);

		strcpy(spi_addr, "0xb504");

		// read from 0x0b500 from SPI and to get primary NAND uboot address
		status = do_sflashre(NULL, 0 , 4 , cmd1 ) ;
		if (0 != status) {
			printf("ERROR: fail to read SPI flash\n");
		} else {
			*bootaddr2 = (int)*(int *)(TMP_UBOOT_ADDR);
		}
	}
	
	return status;
}

int ReadBootLoaderBinFile(char *filename)
{
	int status;
	char* cmd1[] = { "fatload" , "usb", "0" , FILE_MEM_ADDR_STRING, filename  };

	memset((void*) FILE_MEM_ADDR , 0xFF , simple_strtoul ( MAX_UBOOT_SIZE_STRING , NULL, 16 ));
	status = do_fat_fsload(NULL, 0, 5 , cmd1);
	if ( 0 == status)
	{		
	}
	else
	{
		printf("ERROR: failed to load %s\n", filename);
	}

	return status;
}

int EraseSPIUBootRegion(void)
{
	int status;
	char* cmd2[] = { "sfer" , "0", MAX_SPI_UBOOT_SIZE_STRING }; //erase 512K.

	status =  do_sflasher(NULL, 0,3,  cmd2) ;	
	printf("status = %d\n" , status );

	return status;
}

int EraseNANDUBootRegion(char *addr)
{
	int status;
	char* cmd1[] = { "nand" , "erase.spread", addr, MAX_UBOOT_SIZE_STRING }; //erase 512K.
	status = do_nand(NULL, 0, 4 , cmd1);
	if ( 0 != status)
	{
		printf("ERROR: unable to erase NAND flash at address %s\n", addr);
	}

	return status;
}

int ProgramUBootToSPIFlash(void)
{
	int status;
	char* cmd1[] = { "sfwr" , FILE_MEM_ADDR_STRING, "0" ,  MAX_SPI_UBOOT_SIZE_STRING  };
	int bootaddr1, bootaddr2;

	status = ubootBootAddrs(&bootaddr1, &bootaddr2);

	if (status != 0)  {
	} else if (bootaddr1 != 0x2c00) {
		printf("ERROR: miniloader in SPI flash does not boot uboot from SPI flash!\n");
		status = -1;
	} else {
		status = EraseSPIUBootRegion();
		if (status != 0 ) {
			printf("ERROR: fail to erase SPI flash\n");
		} else {
			status = do_sflashwr(NULL, 0 , 4 , cmd1 ) ;
		}
	}
	return status;
}

int ProgramUBootToNANDFlash(char *filename)
{
	int status = 0;
	int bootaddr1, bootaddr2;
	char *ext = get_extension(filename);
	int retries = 3;

	status = ubootBootAddrs(&bootaddr1, &bootaddr2);

	do {
		if (status != 0)  {
		} else if (bootaddr1 == 0x2c00) {
			printf("ERROR: miniloader in SPI flash boots from SPI not NAND!\n");
			status = -1;
		} else if (0 == strcmp(ext, SECURE_IMAGE_EXTENSION)) {
			// esp1800l_flash_util
			char* cmd1[] = { "esp1800l_flash_util" , "--uboot", "--force", FILE_MEM_ADDR_STRING};

			status = do_esp1800l_flash_util(NULL, 0 , 4 , cmd1 ) ;
			// status will be -EIO on nand error

		} else {
			char nand_addr[16]={0};
			char* cmd1[] = { "nand" , "write", FILE_MEM_ADDR_STRING, &nand_addr[0] ,  MAX_UBOOT_SIZE_STRING  };
			sprintf(nand_addr, "0x%x", bootaddr2);

			status = EraseNANDUBootRegion(nand_addr);
			if (status == 0 ) {
				status = do_nand(NULL, 0 , 5 , cmd1 ) ;
				if ( 0 != status )
				{
					status = -EIO;
					printf("ERROR: fail to write to nand\n");
				}
			}
		}
	} while ((--retries > 0) && (status == -EIO));

	return status;
}

int StartUsbService(void)
{
	extern int usb_stor_curr_dev;
	int status;
	char* usb_start[] = { "usb" , "start" };

	//printf("Going for UpgradeAllIfRequired\n");
	status = do_usb(NULL, 0, 2, usb_start );
	if ( usb_stor_curr_dev < 0)
	{
		status = -1;
	}

	return status;
}
//--------------------------------------------------------------------------------------------------
int StopUsbService(void)
{
	int status;
	char* usb_stop[] = { "usb" , "stop" };

	printf("Usb service is stopped now\n");
	status = do_usb(NULL, 0, 2, usb_stop );

	return status;
}
//--------------------------------------------------------------------------------------------------

void stoupper(char *s)
{
	for(; *s; s++)
	{
		if(('a' <= *s) && (*s <= 'z'))
		{
			*s = 'A' + (*s - 'a');
		}
	}
}

void stolower(char *s)
{
	for(; *s; s++)
	{
		if(('A' <= *s) && (*s <= 'Z'))
		{
			*s = 'a' + (*s - 'A');
		}
	}
}

int do_upgrade(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int status;
	int what_to_upgrade = -1 ;
	int force = 0;
	char *filename = SECURE_IMAGE_FILENAME;

	if ((argc < 2) || (argc > 4))
	{
		return cmd_usage(cmdtp);
	}

	stolower((char*) argv[1] );
	stolower((char*) argv[2] );

	if( 0 == strcmp( argv[1] , "spiuboot" ) )
	{
		what_to_upgrade=UPGRADE_SPIUBOOT;
		filename = INSECURE_UBOOT_IMAGE_FILENAME;
	}
	else if( 0 == strcmp( argv[1] , "uboot" ) )
	{
		what_to_upgrade=UPGRADE_UBOOT;
	}
	else if( 0 == strcmp( argv[1] , "nethd" ) )
	{
		what_to_upgrade=UPGRADE_NETHD;
	}
	else
	{
		return cmd_usage(cmdtp);
	}

	if( 0 == strcmp( argv[2] , "force" ) )
	{
		force=1;
	}

	if (((force)  && (argc == 4)) ||
	   ((!force) && (argc == 3)))
		filename = argv[argc-1];

	status = StartUsbService();
	if (status != 0 ) {
		printf("ERROR: fail to start usb service\n");
		goto exit2;
	}

	if ( UPGRADE_SPIUBOOT == what_to_upgrade )
	{
		if ( 1 == force ) {
			status = ReadBootLoaderBinFile(filename);
			if (status == 0 ) {
				status = ProgramUBootToSPIFlash();
				if (status != 0 ) {
					printf("ERROR: fail to program SPI flash\n");
				}
			}
		} else {
			printf("Expected force option when updating SPI uboot\n");
		}
	}
	else if ( UPGRADE_UBOOT == what_to_upgrade )
	{
		if ( 1 == force ) {
			status = ReadNetHdBinFile(filename);
			if (status == 0 ) {
				status = ProgramUBootToNANDFlash(filename);
				if (status != 0 ) {
					printf("ERROR: fail to program SPI flash\n");
				}
			}
		} else {
			printf("Expected force option when updating uboot\n");
		}
	}
	else // kernel image
	{
		status = ReadNetHdBinFile(filename);
		if (status != 0 ) {
			printf("WARNING: failed to read %s file, trying %s\n",
					filename, INSECURE_IMAGE_FILENAME);
			filename = INSECURE_IMAGE_FILENAME;
			status = ReadNetHdBinFile(filename);
			if (status != 0 ) {
				printf("ERROR: failed to read %s.\n", filename);
			}
		}
		if (status == 0) {
			status = ProgramNetHdToNandFlash(filename);
			if (status != 0 ) {
				printf("ERROR: fail to program NAND flash\n");
			}
		}
	}

	status = StopUsbService();
	if (status != 0 ) {
		printf("ERROR: fail to stop usb service\n");
	}

exit2:	
	return status;
}



int AutoFirmwareUpgrade(void)
{
	printf("Performing USB_UPDATES2=1\n");
	return 0;
}

U_BOOT_CMD(
	upgrade,	4,	0,	do_upgrade,
	"Utility to upgrade uboot or nethd image\n",
	"\tupgrade spiuboot|uboot|nethd [force] [filename]\n"
	"\t  spiuboot indicates that uboot in SPI flash should be updated with cavm18_bootloader.bin\n"
	"\t  uboot indicates that uboot image in NAND flash should be updated with cavm18_netHD.sbin or file specified\n"
	"\t  nethd indicates that nethd image in NAND flash should be updated with cavm18_netHD.sbin or file specified\n"
	"\n"
	"\t  force indicates that the upgrade should be done regardless of version or whether uboot was specified\n"
	);
