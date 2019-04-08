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


extern int StopUsbService(void);
extern int StartUsbService(void);


extern int do_fat_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_sflashwr(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_sflasher(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_sflashre(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

extern int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[]);
extern int do_esp1800l_flash_util (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

extern int setenv(char *varname, char *varvalue);


int ReadSplashFile(char *filename)
{
	int status;
	char* cmd1[] = { "fatload" , "usb", "0" , "0x04000000", filename  };
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

int EraseSplashRegion(void)
{
	int status;
	char* cmd1[] = { "nand" , "erase", "0x001c0000" , "0x00200000" };
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

int ProgramSplashImageToNandFlash(void)
{
	int status = 0;
	int retries = 3;

	do 
	{		
		char filesize[16]={0};
		char* cmd1[] = { "nand" , "write" , "0x04000000" , "0x001c0000",  & filesize[0] };
		strncpy(filesize , getenv("filesize") , sizeof(filesize) ) ;

		status = EraseSplashRegion();
		
		if (0 != status ) 
		{
			printf("ERROR: fail to erase NAND flash nethd region\n");
		} 
		else 
		{
			//nand write 0x4000000 0x1c0000 ${filesize}
			status = do_nand(NULL, 0, 5 , cmd1);
			if ( 0 != status)
			{
				printf("ERROR: unable to write to NAND flash\n");
				status = -EIO;
			}
		}
	}while((--retries > 0) && (status == -EIO));

	return status;
}


//--------------------------------------------------------------------------------------------------
int ConfigureLogoCommand( 	const char* apMode   , 
							const char* apWidth  , const char* apHeight, 
							const char* apHOffset, const char* apVOffset)
{
	int Status=0;
	char TempBuf[256]={0};;
	
	strcpy( TempBuf, "nand read 4000000 1C0000 1C2010; logo " );
	
	char DefWidth[8]={0};
	char DefHeight[8]={0};
	char DefHOffset[8]={0};
	char DefVOffset[8]={0};

	if ( NULL != apMode )
	{
		if ( 0 == strcmp(apMode , "1080P" ) )
		{
			strcat( TempBuf, "7 2 ");
			strcpy( DefWidth, " 1920 "); strcpy(DefHeight, " 1080 "); strcpy(DefHOffset , " 0 ");  strcpy(DefVOffset, " 0 ");
		}
		else if  ( 0 == strcmp(apMode , "720P" ) )
		{
			strcat( TempBuf, "5 2 ");
			strcpy( DefWidth, " 1280 "); strcpy(DefHeight, " 720 "); strcpy(DefHOffset , " 0 ");  strcpy(DefVOffset, " 0 ");
		}
		else if  ( 0 == strcmp(apMode , "480P" ) )
		{
			strcat( TempBuf, "3 2 ");
			strcpy( DefWidth, " 720 "); strcpy(DefHeight, " 480 "); strcpy(DefHOffset , " 0 ");  strcpy(DefVOffset, " 0 ");
		}
	}
	strcat( TempBuf, "4000000 ");
	
	if (NULL == apWidth)
	{
		strcat(TempBuf, DefWidth);
	}
	else
	{
		strcat(TempBuf, apWidth);
	}
	

	if (NULL == apHeight)
	{
		strcat(TempBuf, DefHeight);
	}
	else
	{
		strcat(TempBuf, apHeight);
	}

	if (NULL == apHOffset)
	{
		strcat(TempBuf, DefHOffset);
	}
	else
	{
		strcat(TempBuf, apHOffset);
	}

	if (NULL == apVOffset)
	{
		strcat(TempBuf, DefVOffset);
	}
	else
	{
		strcat(TempBuf, apVOffset);
	}
	
	Status =  setenv("show_logo" , TempBuf);
	
	return Status;
}
//--------------------------------------------------------------------------------------------------
int do_burnsplash(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int status;

	status = StartUsbService();
	
	if (status != 0 ) 
	{
		printf("ERROR: fail to start usb service\n");
		goto exit2;
	}
	
	status = ReadSplashFile(argv[1]);				if (status != 0 ) goto exit2;
	status = StopUsbService();						if (status != 0 ) goto exit2;
	status = ProgramSplashImageToNandFlash();		if (status != 0 ) goto exit2;
	switch (argc)
	{
		case 3:	status = ConfigureLogoCommand(argv[2] , NULL , NULL , NULL , NULL ); break;
		case 4:	status = ConfigureLogoCommand(argv[2] , argv[3] , NULL , NULL , NULL ); break;
		case 5:	status = ConfigureLogoCommand(argv[2] , argv[3] , argv[4] , NULL , NULL ); break;
		case 6:	status = ConfigureLogoCommand(argv[2] , argv[3] , argv[4] , argv[5] , NULL ); break;
		case 7:	status = ConfigureLogoCommand(argv[2] , argv[3] , argv[4] , argv[5] , argv[6] ); break;
	}
	
exit2:
	return status;
}


U_BOOT_CMD(
	burnsplash,	7,	0,	do_burnsplash,
	"Utility to burn splash image\n",
	"\tprogram u-boot splash screen to NAND and set corresponding env. parameter\n"
	"\t  burnsplash filename.565 720P/1080P/480P Width Height HOffset VOffset\n"
	);
