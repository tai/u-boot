/*
 *      @file   cmd_boots.c
 *      @author Santosh Shukla <sshukla@mvista.com>
 *      @date   Jan 2012
 *
 *      @brief  Command for booting secure kernels.
 *
 *      File contains 'boots' command and  'esp1800l_flash_util' tool.
 *
 *      'Boots' does image decryption and authentication for field, factory, and blob images
 *      (reads data from NAND). Within this there is support for normal factory vs field
 *      boot option, as well as pingpong_boot which allows for switching factory vs field
 *      boot as images are upgraded.
 *
 *      esp1800l_flash_util: This is a helper function used to flash images that can be
 *      decrypted/authenticated by 'boots'
 *
 *      Usage info for esp1800l_flash_util:
 *
 *      1. usb start; fatload usb 0 0x2100000 image.sbin
 *
 *      Default Field or Blob Image:
 *      2. esp1800l_flash_util 0x2100000
 *         esp1800l_flash_util --upgrade 0x2100000
 *         esp1800l_flash_util --force   0x2100000
 *
 *      For Factory Image:
 *      3. esp1800l_flash_util --factory 0x2100000
 *         esp1800l_flash_util --factory --upgrade 0x2100000
 *         esp1800l_flash_util --factory --force 0x2100000
 *
 */
//#define DEBUG

#include <common.h>
#include <rsa_verify.h>
#include <sha256.h>
#include <asm/global_data.h>
#include <asm/errno.h>
#include <customer_specific_definitions.h>

int kluge_flag_to_fix_bug3394;

extern unsigned accum_busy_wait_time;
extern unsigned accum_page_read_time;
extern unsigned accum_pgrd;
extern unsigned accum_ecccalc;
extern unsigned accum_correct;

static void filter_bootargs(void);

/*
 * Global data (for the gd->relocaddr)
 */
DECLARE_GLOBAL_DATA_PTR;

// #define DEBUG

/*
   Public key for Secure Kernel::
   */
/* the declaration/definition of pKey must be adjacent to blob_pKey, or else the
   function public_key_does_not_match_mine() will not work */


extern unsigned char pKey[];

/*
   Public key for Secure Blob::
   */
/* the declaration/definition of blob_pKey must be adjacent to pKey, or else the
   function public_key_does_not_match_mine() will not work */
extern unsigned char blob_pKey[];

typedef struct __FlashImageInfo_t
{
	uint32_t mHdrType;                              // Sanity check of image before
#define UBOOT_IDENTIFIER  (0x11223344)    		// fail-safe/factory uboot
#define KERNEL_IDENTIFIER (0x87543223)    		// Fail-safe/factory kernel
#define BLOB_IDENTIFIER   (0x22113344)    		// APP_SIG :: HDCP + DTCP + ...
	// processing!
	uint32_t mSourceAddressInFlash;
	uint32_t mDestinationAddressInDram;
	uint32_t mLength;                               // image len + image_signature len
	uint32_t mSignLen;                              // size of signature len.
	uint32_t mActiveImage;                          // for future use: could be factory
	// or field type
	char mVersionNo[256];                           // svn revision for Linux/u-boot
	uint32_t mCrc32;                                // crc of ImageInfo_t header

}FlashImageInfo_t;	// size of ImageInfo_t : 284 Byte


/*
 * Addresses in NAND
 */

/* Field headers  */
#define CAVM18_FIELD_HDR_LEN			(128*1024)

/* Factory headers */
#define CAVM18_FACT_HDR_KERNEL_OFFSET		(0x1000)
#define CAVM18_FACT_HDR_BLOB_OFFSET		(0x2000)
#define CAVM18_FACT_HDR_LEN			(128*1024)

/* Blob headers */
#define CAVM18_BLOB_HDR_OLD_FLSH_ADDR		(0x003e0000) // 1k: Blob
#define CAVM18_BLOB_HDR_LEN			(0x1000)

#define SECTOR_SIZE	(128*1024) /* 128K sector size */

/*
 * DRAM addresses
 */

#if (CONFIG_TARGET_NAME == CNC1800L_TMS_512_256)
#define CAVM18_FIELD_HDR_DRAM_ADDR			0x10000000
#define CAVM18_FACT_HDR_DRAM_ADDR			0x02000000
#else
#define CAVM18_FIELD_HDR_DRAM_ADDR			0x06000000
#define CAVM18_FACT_HDR_DRAM_ADDR			0x02000000
#endif

/* Crypto Engine Out DDR Address  */
#if (CONFIG_TARGET_NAME == CNC1800L_TMS_512_256)
#define CRYPTO_OUT_KERNEL_DRAM_ADDR			0x03000000 // Size 64MByte range - 0x14080000 to 0x18080000
#define MAX_KERNEL_IMAGE_SIZE           	0x04000000  // 64 MB
#else
#define CRYPTO_OUT_KERNEL_DRAM_ADDR			0x08080000 // Size 32MByte range - 0x8080000 to 0xa080000
#define MAX_KERNEL_IMAGE_SIZE           	0x2000000  // 32 MB
#endif

#if (CONFIG_TARGET_NAME == CNC1800L_TMS_512_256)
/* image header workspace */
#define HEADER_DRAM_ADDR					0x7000000
#define HEADER_DRAM_SIZE					0x2000
#define READ_BACK_IMAGE_RAM_ADDR			0x10000000 // Image copy to ddr - imglen could be < 32Mbyte
#else
#define HEADER_DRAM_ADDR					0x5000000
#define HEADER_DRAM_SIZE					0x2000
#define READ_BACK_IMAGE_RAM_ADDR			0x6000000 // Image copy to ddr - imglen could be < 32Mbyte
#endif

#if (CONFIG_TARGET_NAME == CNC1800L_TMS_512_256)
/* ImageInfo look-up table for all image types */
FlashImageInfo_t FlashImgHdr[5] = {
	/*	Img_Sig	           SrcFlashAddr DstRAMAddr Img_len  sig_len ActImg   SVN Rev CRC Dummy  */
	{ BLOB_IDENTIFIER,                                      /* image type */
		(CAVM18_BLOB_HDR_FLSH_ADDR + CAVM18_BLOB_HDR_LEN),   /* source address in flash */
		0x14000000,                                           /* destination address in RAM */
		0,                                                    /* image length */
		0,                                                    /* signature length */
		1,                                                    /* 1 = field, 0 = factory */
		"",                                                   /* version information */
		0 },                                                  /* CRC32 */

	{ KERNEL_IDENTIFIER,
		FACTORY_KERNEL_FLASH_ADDR,
		0x10020000,
		0,
		0,
		0,
		"",
		0 }, /* For Fact Kernel: Active Img */

	{ KERNEL_IDENTIFIER,
		FIELD_KERNEL_FLASH_ADDR,
		0x10020000,
		0,
		0,
		1,
		"",
		0 }, /* For Field kernel : Active Img */

	{ UBOOT_IDENTIFIER,
		FACTORY_UBOOT_FLASH_ADDR,
		0x10020000,
		0,
		0,
		0,
		"",
		0 }, /* For Fact Uboot */

	{ UBOOT_IDENTIFIER,
		FIELD_UBOOT_FLASH_ADDR,
		0x10020000,
		0,
		0,
		1,
		"",
		0 } /* For Field uboot */

};
#else
/* ImageInfo look-up table for all image types */
FlashImageInfo_t FlashImgHdr[5] = {
	/*	Img_Sig	           SrcFlashAddr DstRAMAddr Img_len  sig_len ActImg   SVN Rev CRC Dummy  */
	{ BLOB_IDENTIFIER,                                      /* image type */
		(CAVM18_BLOB_HDR_FLSH_ADDR + CAVM18_BLOB_HDR_LEN),   /* source address in flash */
		0x08000000,                                           /* destination address in RAM */
		0,                                                    /* image length */
		0,                                                    /* signature length */
		1,                                                    /* 1 = field, 0 = factory */
		"",                                                   /* version information */
		0 },                                                  /* CRC32 */

	{ KERNEL_IDENTIFIER,
		FACTORY_KERNEL_FLASH_ADDR,
		0x06020000,
		0,
		0,
		0,
		"",
		0 }, /* For Fact Kernel: Active Img */

	{ KERNEL_IDENTIFIER,
		FIELD_KERNEL_FLASH_ADDR,
		0x06020000,
		0,
		0,
		1,
		"",
		0 }, /* For Field kernel : Active Img */

	{ UBOOT_IDENTIFIER,
		FACTORY_UBOOT_FLASH_ADDR,
		0x06020000,
		0,
		0,
		0,
		"",
		0 }, /* For Fact Uboot */

	{ UBOOT_IDENTIFIER,
		FIELD_UBOOT_FLASH_ADDR,
		0x06020000,
		0,
		0,
		1,
		"",
		0 } /* For Field uboot */

};

#endif


#define HW_REG_READ(addr)           (*((volatile unsigned int *)(addr)))
#define SECURITY_CFG_ADDR           0xfffff010
unsigned int SecHWGetRSAEnable(void)
{
	unsigned int ret;

	ret = HW_REG_READ(SECURITY_CFG_ADDR);
	ret = (ret >> 3) & 0x1;
	return ret ^ 0x1;
}

extern int do_nand(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[]);
extern int do_crypto(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);


enum {
	encrypt,
	decrypt
} CryptoType;

enum {
	keyA,
	keyB
} CryptoKeyType;


/*
 * Decrypt aSrcAddr Image to
 * 		crypto DECRYPT_AES128_CIPlus chl1 0x5000000 0x7000000 0x1600000
 *
 * 	Encrypt aSrcAddr Image to
 * 		crypto ENCRYPT_AES128_CIPlus chl1 0x5000000 0x7000000 0x1600000
 */
int Sec1800l_crypto(uint32_t aSrcAddr, uint32_t alen, uint32_t aDstAddr, uint8_t key_flag, uint8_t crypto_type)
{
	char* pCmd[] = {"crypto", NULL, NULL, NULL, NULL, NULL, NULL};
	char TempCmd1[32];
	char TempCmd2[16];
	char TempCmd3[16];
	char TempCmd4[16];
	char TempCmd5[16];
	char TempCmd6[16];

	int status=0;

	if (crypto_type == 0) { // aes-128-cbc encryption

		sprintf(TempCmd1 , "ENCRYPT_AES128_CIPlus");
		sprintf(TempCmd2 , "chl2");
	} else if (crypto_type == 1) { // decryption

		sprintf(TempCmd1 , "DECRYPT_AES128_CIPlus");
		sprintf(TempCmd2 , "chl1");
	} else {

		printf (" Invalid crypto_type .. exiting \n");
		return -1;
	}

	sprintf(TempCmd3 , "0x%x", aSrcAddr);
	sprintf(TempCmd4 , "0x%x", aDstAddr);
	sprintf(TempCmd5 , "0x%x", alen); // $(filesize)
	sprintf(TempCmd6 , "0x%x", key_flag);

	debug (" Crypto : Crypto_Type : %s Channel : %s Src_Addr : %s, Dst_Addr : %s, Img_Len : %s, key_flag : %s\n", \
			TempCmd1, TempCmd2, TempCmd3, TempCmd4, TempCmd5, TempCmd6);

	pCmd[1] =  TempCmd1; pCmd[2] =  TempCmd2; pCmd[3] =  TempCmd3; pCmd[4] = TempCmd4; pCmd[5] = TempCmd5;
	pCmd[6] = TempCmd6;

	status = do_crypto(NULL, 0, 7, pCmd);

	if ( 0 != status )
	{
		printf("ERROR: fail to Decrypt! (%d)\n", status);
	}

	return status;
}

void Sec1800l_BootM(uint32_t abootAddress)
{
	char* pCmd[] = {"bootm", NULL, NULL};
	char TempCmd1[16];

	sprintf(TempCmd1 , "0x%x", abootAddress);

	pCmd[1] =  TempCmd1;

	debug (" bootm addr : %s :: %s\n", pCmd[1], TempCmd1);

	do_bootm(NULL, 0,2, pCmd); //FIXME : do_bootm 3rd arg- takes val 1 .. not working, Works fine for 2

	return;
}


static int Sec1800l_NandRead(uint32_t aDstAddressInDram, uint32_t aSrcAddressInFlash, uint32_t alen)
{
	/* 		    nand     read   4000000 ${WORK_IMG} ${filesize}*/
	char* pCmd[5] = {  "nand" , "read" ,  NULL ,  NULL     , NULL };
	char TempCmd1[16];
	char TempCmd2[16];
	char TempCmd3[16];

	int status=0;

	sprintf(TempCmd1 , "0x%x", aDstAddressInDram); // Work_IMG : Flash address
	sprintf(TempCmd2 , "0x%x", aSrcAddressInFlash); // DDR Ram address
	sprintf(TempCmd3 , "0x%x", alen); // $(filesize)

	debug (" NandRd : SrcFlashAddr : %x DstAddressDram : %x len : %x\n",
			aSrcAddressInFlash, aDstAddressInDram, alen);

	pCmd[2] =  TempCmd1; pCmd[3] =  TempCmd2; pCmd[4] =  TempCmd3;

	if (aSrcAddressInFlash & 0x1FFFF) {
		/* NAND offset is not a multiple of the block size, do the kluge */
		kluge_flag_to_fix_bug3394 = 1;
	}

	status = do_nand(NULL, 0, 5, pCmd);
	kluge_flag_to_fix_bug3394 = 0;
	if ( 0 != status )
	{
		status = -1;
		printf("ERROR: fail to read nand secure sector\n");
	}
	return status;
}

static int nandWriteError = 0;

static int Sec1800l_NandWrite(uint32_t aSrcAddressInDram, uint32_t aDstAddressInFlash, uint32_t alen)
{
	/* nand write 4000000 ${WORK_IMG} ${filesize}*/
	char* pCmd[5] = { "nand" , "write" ,  NULL ,  NULL , NULL };
	char TempCmd1[16];
	char TempCmd2[16];
	char TempCmd3[16];

	int status = 0;
	nandWriteError = 0;

	sprintf(TempCmd1, "0x%x", aSrcAddressInDram ); // DDR Ram address
	sprintf(TempCmd2, "0x%x", aDstAddressInFlash ); // Work_IMG : Flash address
	sprintf(TempCmd3, "0x%x", alen); // $(filesize)

	pCmd[2] =  TempCmd1; pCmd[3] =  TempCmd2; pCmd[4] =  TempCmd3;

	status = do_nand(NULL, 0, 5, pCmd);
	if ( 0 != status )
	{
		status = -1;
		printf("ERROR: fail to write nand secure sector\n");
		nandWriteError = 1;
	}
	return status;
}

static int Sec1800l_NandErase(uint32_t aDstAddressInFlash, uint32_t alen, uint32_t spread)
{
	/* nand erase ${WORK_IMG} ${filesize} */
	char* pCmd[4] = { "nand" , NULL , NULL , NULL };
	char TempCmd0[16];
	char TempCmd1[16];
	char TempCmd2[16];

	int status = 0;

	sprintf(TempCmd0, "%s", spread ? "erase.spread" : "erase"); // Work_IMG : Flash address
	sprintf(TempCmd1, "0x%x", aDstAddressInFlash); // Work_IMG : Flash address
	sprintf(TempCmd2, "0x%x", alen); // $(filesize)

	pCmd[1] =  TempCmd0;
	pCmd[2] =  TempCmd1;
	pCmd[3] =  TempCmd2;

	status = do_nand(NULL, 0, 4, pCmd);
	if ( 0 != status )
	{
		status = -1;
		printf("ERROR: fail to erase nand secure sector\n");
	}
	return status;
}

#include <u-boot/md5.h>

static int sec1800l_md5sum(uint32_t aImgDramAddr,  uint32_t aImgLen, char *cmp_md5sum)
{
	int status = 0;
	//unsigned char output[16];
	unsigned char output[16];
	uint32_t md5_offset = aImgLen;
	register int j;

	unsigned char *temp_md5buf = (unsigned char *) aImgDramAddr;
	md5(temp_md5buf, md5_offset, output);

#if 0
	int i;
	for (i=0; i<16; i++) {

		if (i%8 == 0)
			printf ("\n");

		printf (" %02x", (unsigned char)output[i]);
	}
#endif

	if (memcmp (output, cmp_md5sum, 16) != 0) {

		status = -1;
		for (j = 0; j < 16; ++j) { printf(" %02X", cmp_md5sum[j]); }
		for (j = 0; j < 16; ++j) { printf(" %02X", output[j]); }
	}

	return status;
}

#if defined(CONFIG_CMD_BOOTD)

static int Crypto_Command(uint32_t source, uint32_t dest, uint32_t leng)
{
	char sa[16], da[16], le[16], *c[5];
	sprintf(sa, "0x%08x", source);
	sprintf(da, "0x%08x", dest);
	sprintf(le, "0x%08x", leng);
	c[0] = "read_decrypt"; c[1] = &sa[0]; c[2] = &da[0]; c[3] = &le[0]; c[4] = NULL;
	return do_crypto(NULL, 0, 4, &c[0]);
}

static int do_crypto_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *cmdptr[5];

	if ( argc != 4) {
		return cmd_usage(cmdtp);
	}

	cmdptr[0] = "read_decrypt";
	cmdptr[1] = argv[1];
	cmdptr[2] = argv[2];
	cmdptr[3] = argv[3];
	cmdptr[4] = NULL;
	return do_crypto(NULL, 0, 4, &cmdptr[0]);
}

U_BOOT_CMD (
		cryptotest ,        4,      1,      do_crypto_test,
		"execute decrypt/encrypt cycle on memory\n",
		"\tcryptotest source-address dest-address amount\n"
	   );

/*
 * Do Image Decryption on crypto channel: stream mode, algo : aes-128-cbc
 */
int Img_Crypto ( uint32_t crypto_inLoc, uint32_t len, uint32_t crypto_outLoc, uint8_t key_flag, uint8_t crypto_type)
{
	/* Img Decryption */
	if( Sec1800l_crypto(crypto_inLoc, len, crypto_outLoc, key_flag, crypto_type) != 0) {

		printf (" decryption failed\n");
		return -1;
	}

	return 0;
}

/*
 *  Perform RSA verification on an image (deprecated)
 */
int ImgRsa_Verify (uint32_t aImgAddress, uint32_t aLen, uint32_t asignAddress)
{
	debug (" Uploading Public key ... \n");

	/* Do hash of i/p Image */
	UpdateExternalRSAKey(pKey) ; // considered 2048 will generate 256 byte of public key only


	// debug only
	debug ("Img buf addr %x :: Img len : %x :: sign buf addr %x\n", aImgAddress, aLen, asignAddress);

	int rt = SignatureVerify((unsigned char *)aImgAddress, aLen,
			(unsigned char *)asignAddress);
	if (rt == 0) {

	} else {

		printf ( "Failed! Invalid Image.\n");
		return -1;
	}

	return 0;
}


/*
 * Perform RSA verification on an image using a hybrid hashing input, No SW hashing here.
 */
int ImgHybRsa_Verify (uint8_t hash[32], uint32_t asignAddress, unsigned char *publicKey)
{
	debug (" Uploading Public key ... \n");

	/* Do hash of i/p Image */
	UpdateExternalRSAKey(publicKey) ; // considered 2048 will generate 256 byte of public key only


	// debug only
	debug ("sign buf addr %x\n", asignAddress);

	int rt = HybridSignatureVerify( hash, (unsigned char *)asignAddress);

	if (rt == 0) {

		printf (" OK.\n");
	} else {

		printf ( "Failed! Invalid Image.\n");
		return -1;
	}

	return 0;
}

void hex_print (uint8_t *buff, uint32_t len)
{
	uint32_t cnt = 0;
	for (cnt = 0; cnt < len; cnt++)
		debug (" %02x", buff[cnt]);
	debug ("\n");
}


/*
 *	Decrypt and Authenticate an image for Kernel (Factory only) or blob.
 */
int Img_Authenticate (FlashImageInfo_t *hdr_p, int distImgType)
{
	char *ImgName = NULL;

	uint32_t chk_crc32;
	uint32_t crypto_outLoc;
	uint8_t hash[32];
	uint8_t inhash[32];
	uint32_t Img_Len;
	uint32_t Hash_Buf_Offset;
	int val;

	if (distImgType == 0) { // App Img

		ImgName = "Blob";
		crypto_outLoc = CRYPTO_OUT_BLOB_LINUX_ADDR;
	} else {

		ImgName = "Application";
		crypto_outLoc = CRYPTO_OUT_KERNEL_DRAM_ADDR;
	}
#if 0
	debug (" %s Hdr info : \n", ImgName);
	debug (" mHdrType : 0x%x mSourceAddressInFlash : 0x%x mDestinationAddressInDram : 0x%x \
			image mLength : %d siglen : %d ActiveImage flag : %d 		\
			mVersion : %s Crc32 : 0x%x \n", hdr_p->mHdrType, hdr_p->mSourceAddressInFlash,
			hdr_p->mDestinationAddressInDram, hdr_p->mLength,
			hdr_p->mSignLen, hdr_p->mActiveImage,
			hdr_p->mVersionNo, hdr_p->mCrc32);
#endif

	if (hdr_p->mHdrType == BLOB_IDENTIFIER ||
			hdr_p->mHdrType == KERNEL_IDENTIFIER) {

		debug (" Valid : %s Hdr Identifier \n", ImgName);

		debug (" %s Header crc ...\n", ImgName);
		chk_crc32 = crc32 (0, (unsigned char *)hdr_p, sizeof (FlashImageInfo_t) - 4);

		if (chk_crc32 == hdr_p->mCrc32) {


			unsigned int TotalLen = hdr_p->mLength + hdr_p->mSignLen;
			TotalLen = SECTOR_SIZE * ( (TotalLen + SECTOR_SIZE -1) / SECTOR_SIZE ) ;

			debug (" %s aligned len : %d\n", ImgName, TotalLen);

			/* step 4. netHDImg2 from Nand block to 0x0240_0000 */
			if ( Sec1800l_NandRead(hdr_p->mDestinationAddressInDram,
						hdr_p->mSourceAddressInFlash,
						TotalLen) != 0 )
			{
				printf (" Failed to read %s!\n", ImgName);
				return -1;
			}


			/* remove 32 byte Hash offset from mLength : use Img_Len for actual Img enc/dec now onwards */
			Img_Len =  hdr_p->mLength-32;
			debug (" Img_Len : %d\n", Img_Len);

			/* back_up the 32 byte of i/p hash */
			memcpy (inhash, (uint8_t *)(hdr_p->mDestinationAddressInDram + Img_Len), 32);
			debug ("\n input hash dump: "); 
			hex_print (inhash, 32);
			/* 2nd pass decryption for keyB :: o/p : plain_text Img */
			if (Img_Crypto (hdr_p->mDestinationAddressInDram, Img_Len , crypto_outLoc, keyB, decrypt) != 0) {
				/* Flush out decryted Kernel/Blob Image with zero */
				memset ((uint8_t *)crypto_outLoc, 0x0, hdr_p->mLength);
				return -1;
			}

			/* loop-back output as input;
			   do encryption to retrieve 1st pass hash of 16 byte; using keyA */
			if (Img_Crypto (crypto_outLoc, Img_Len , hdr_p->mDestinationAddressInDram, keyA, encrypt) != 0) {
				/* Flush out decryted Kernel/Blob Image with zero */
				memset ((uint8_t *)crypto_outLoc, 0x0, hdr_p->mLength);
				return -1;
			}

			/* flush hash buf */
			memset (hash, 0x0, sizeof (hash));

			/* jump to last 16 byte offset for 1st pass Hash record */
			Hash_Buf_Offset = (hdr_p->mDestinationAddressInDram + (Img_Len -16) );

			/* copy last 16byte hash store in local hash bufffer */
			memcpy (hash, (uint8_t *)( Hash_Buf_Offset), 16);
			debug ("\n 1st pass hash dump (16byte): "); 
			hex_print (hash, 16);

			/* copy inhash's last 26 byte (keyB) into hash last 16 byte index */
			memcpy (hash+16, (inhash + 16), 16);

			debug ("\n 2nd Encryption pass hash dump (16byte): "); 
			hex_print (hash+16, 16);

			debug ("\n 1st/2nd encryption pass hash dump (32 byte): "); 
			hex_print (inhash, 32);
			hex_print (hash, 32);
			/* compare 32 byte has with input Image */
			if (memcmp(hash, inhash, 32) != 0) {

				printf ("Error : Hashing failure %x--%x !\n",crypto_outLoc,hdr_p->mLength);

				/* Flush out decryted Kernel/Blob Image with zero */
				memset ((uint8_t *)crypto_outLoc, 0x0, hdr_p->mLength);
				return -1;
			}
			/* rsa verify i/p : hash (32 byte :: keyA enc hash(16 byte)| keyB enc hash(16 byte) )
			   i/p : 256 byte Image signature
			   */
			if (hdr_p->mHdrType == BLOB_IDENTIFIER) {
				val = ImgHybRsa_Verify (hash, (hdr_p->mDestinationAddressInDram + hdr_p->mLength), blob_pKey);
			} else  {
				val = ImgHybRsa_Verify (hash, (hdr_p->mDestinationAddressInDram + hdr_p->mLength), pKey);
			}

			if (val != 0) {
				/* Flush out decryted Kernel/Blob Image with zero */
				memset ((uint8_t *)crypto_outLoc, 0x0, hdr_p->mLength);
				return -1;
			}

		} else {

			printf (" Invalid %s header CRC! Header corrupt! \n", ImgName);
			return -1;
		}

	} else {

		printf (" Invalid %s header signature!\n", ImgName);
		return -1;
	}

#ifdef DEBUG_BLOB_FAIL
	if (distImgType == 0)
		return -1;
#endif

	return 0;
}

uint64_t read_free_running_clock(void)
{
	uint64_t ret;
	uint32_t lo1, hi1, lo2, hi2;

	/* Read twice so we can check for rollover between low and
	   high reads. */
	lo1 = *((volatile uint32_t *)0x80270020);
	hi1 = *((volatile uint32_t *)0x80270024);
	lo2 = *((volatile uint32_t *)0x80270020);
	hi2 = *((volatile uint32_t *)0x80270024);

	ret = (uint64_t) ((lo2 >= lo1)? hi1 : hi2);
	ret <<= 32;
	ret |= lo2;
	return ret;
}

unsigned measure_time_ms(uint64_t *marker)
{
	uint64_t prev, delta, q; unsigned ret = 0u;
	prev = *marker; *marker = read_free_running_clock();
	delta = *marker - prev;
	/* Free running clock ticks at 94.5 MHz = 10.6 ns */
	q = (delta*10ull + 477ull)/954ull;
	ret = (unsigned)q;
	return ret;
}

static int do_timexfer(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uint32_t SA, DA, LNG, count, elapsed = 0, rate;
	uint64_t marker = 0ull, scratch, scratch2;
	uint32_t *d;
	const uint32_t *s;

	/* Usage: timexfer source dest count */
	if ( argc != 4) {
		return cmd_usage(cmdtp);
	}

	SA = simple_strtoul(argv[1], NULL, 16);
	DA = simple_strtoul(argv[2], NULL, 16);
	LNG = simple_strtoul(argv[3], NULL, 16);

	if ( (SA <= DA && DA < SA + LNG) || (DA <= SA && SA < DA + LNG) ) {
		printf("Overlapping regions not allowed.\n");
		return 1;
	}

	if ( (SA & 3) != 0 || (DA & 3) != 0 || (LNG & 3) != 0 ) {
		printf("Addresses and lengths must be 32-bit aligned\n");
		return 1;
	}

	s = (const uint32_t *)SA;
	d = (uint32_t *)DA;
	count = LNG >> 2;

	elapsed = measure_time_ms(&marker);
	while (count != 0) {
		*d++ = *s++;
		--count;
	}
	elapsed = measure_time_ms(&marker);
	if (elapsed == 0u) {
		printf("cannot determine elapsed time!\n");
		return 1;
	}
	scratch = (uint64_t)elapsed;
	scratch2 = (uint64_t)LNG;
	scratch2 *= 1000000ull;
	scratch2 /= scratch;
	rate = (uint32_t)scratch2;

	printf("Time to xfer %u bytes: %u usec (%u bytes/sec)\n", LNG, elapsed, rate);

	return 0;
}

U_BOOT_CMD (
		timexfer ,        4,      1,      do_timexfer,
		"time memory transfer\n",
		"\ttimexfer source-address dest-address amount\n"
	   );

static void filter_bootargs(void)
{
	char *cmdline = NULL;
	char *temp = NULL;
	char newCmdLine[256];

	newCmdLine[0] = '\0';

	if ((cmdline = getenv("bootargs")) != NULL) {
		debug (" %s : %d\n", cmdline, strlen(cmdline));

		/* Check for valid arguments */
		temp = strstr(cmdline, "console=ttyS0,115200");
		if (temp) {
			strncat(newCmdLine, temp, strlen("console=ttyS0,115200"));
			strcat(newCmdLine, " ");
		}
		temp = strstr(cmdline, "nousb");
		if (temp) {
			strncat(newCmdLine, temp, strlen("nousb"));
			strcat(newCmdLine, " ");
		}
		temp = strstr(cmdline, "quiet");
		if (temp) {
			strncat(newCmdLine, temp, strlen("quiet"));
			strcat(newCmdLine, " ");
		}
		temp = strstr(cmdline, "outputmode=");
		if (temp) {
			strncat(newCmdLine, temp, strlen("outputmode=xx"));
			if ((temp[12] < '0') || (temp[12] > '9'))
				newCmdLine[strlen(newCmdLine)-1] = '\0'; /* single digit outputmode */
			strcat(newCmdLine, " ");
		}
		temp = strstr(cmdline, "colorformat=");
		if (temp) {
			strncat(newCmdLine, temp, strlen("colorformat=x"));
			strcat(newCmdLine, " ");
		}

		/* set bootargs with the filtered cmdline */
		setenv("bootargs", newCmdLine);

		debug (" filtered command line [%s]\n", newCmdLine);
	}
}

/*
 * Boots, does image authentication for kernel / secure blob Image,
 * accordingly boots kernel.
 * */
static int do_boots (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	FlashImageInfo_t *kernelImgHdr;
	FlashImageInfo_t *fact_kernelImgHdr;
	FlashImageInfo_t *blobImgHdr;
	char *ImgName = "Application";
	//uint32_t RegVal = 0;
	uint32_t fail_safe_flag = 0;

	uint8_t hash[32];
	uint8_t inhash[32];
	uint32_t Img_Len;
	uint32_t Hash_Buf_Offset;
	int val, retry_count=2;

	uint32_t mtd_part = 0;
#ifdef MEASURE_CRYPTO
	uint64_t total_time_marker = 0ull, crypto_marker = 0ull;
	i	unsigned total_elapsed = 0, crypto_elapsed = 0, dontcare = 0 /* , part1_elapsed = 0, part2_elapsed = 0 */ ;
#endif

	if( argc > 1 )
		return cmd_usage(cmdtp);

	/* Check it SCS_ActivationBit is set before proceeding */
	if (!SecHWGetRSAEnable()) {
		printf (" Invalid chip for Secure boot mode.\n");
#ifdef CHECK_SECURE_CHIP
		return -1;
#endif
	} else {
		debug (" Boot operational state : Secure Mode \n");
	}

	/*
	 * 	Field Img2 Hdr parsing...
	 *	1. Nand read frm 0x057c_0000 to Ram addr 0x0400_0000
	 */

	/* step 1.1 copy field hdr from flash to Dram */
	if ( Sec1800l_NandRead(CAVM18_FIELD_HDR_DRAM_ADDR,
				CAVM18_FIELD_KENREL_HDR_FLSH_ADDR,
				CAVM18_FIELD_HDR_LEN) != 0 )
	{
		printf (" Failed to read image 2 header! Jump to image1 Hdr\n");
		goto FAIL_SAFE_HDR;
	}

	/*
	 *	step 2.1 populate ImgHdr.mCavmUbootImage2 & ImgHdr.mCavmNetHDImage2
	 *	Copy Field Hdrs i.e Uboot2/netHD2_Img from DRAM location 0x0400_0000 into ImgHdr
	 */
	kernelImgHdr = (FlashImageInfo_t *)CAVM18_FIELD_HDR_DRAM_ADDR;

	/* step 2.1.1 Dump content of RXVD netHD2 Img */
	debug (" Field Hdr info ....\n\n");

	debug (" netHDImage \n");
	debug (" mHdrType : 0x%x mSourceAddressInFlash : 0x%x mDestinationAddressInDram : 0x%x \
			Field image mLength : %d siglen : %d ActiveImage flag : %d          \
			mVersion : %s Crc32 : 0x%x \n", kernelImgHdr->mHdrType,
			kernelImgHdr->mSourceAddressInFlash ,
			kernelImgHdr->mDestinationAddressInDram,
			kernelImgHdr->mLength,
			kernelImgHdr->mSignLen, kernelImgHdr->mActiveImage,
			kernelImgHdr->mVersionNo, kernelImgHdr->mCrc32);

FAIL_SAFE_HDR:
	/*
	   step 1.2 copy fact hdr from flash to Dram !!! Optional .. can be changed ...
	   Only used while Fact Img parsing ....
	   */
	if ( Sec1800l_NandRead(CAVM18_FACT_HDR_DRAM_ADDR,
				CAVM18_FACT_HDR_FLSH_ADDR,
				CAVM18_FACT_HDR_LEN) != 0 )
	{

		printf (" Failed to read image 1 Header! .. exiting!!\n");
		return -1;
	}

	/*
	 *	step 1.2 copy blob hdr from flash to Dram !!!
	 *
	 */
	if ( Sec1800l_NandRead(CAVM18_FACT_HDR_DRAM_ADDR + CAVM18_FACT_HDR_BLOB_OFFSET,
				CAVM18_BLOB_HDR_FLSH_ADDR,
				CAVM18_BLOB_HDR_LEN) != 0 )
	{

		printf (" Failed to read blob Header! .. exiting!!\n");
		return -1;
	}

	/*
	 *	step 2.2 populate ImgHdr.mCavmUbootImage2 & ImgHdr.mCavmNetHDImage2
	 *	Copy Field Hdrs i.e Uboot2/netHD2_Img from DRAM location 0x0400_0000 into ImgHdr
	 */

	/* skip uboot header at offset 0x0000 */

	/* point to Factory hdr offset */
	fact_kernelImgHdr = (FlashImageInfo_t *)(CAVM18_FACT_HDR_DRAM_ADDR + CAVM18_FACT_HDR_KERNEL_OFFSET);

	unsigned int chk_crc32;

	char *s = getenv ("pingpong_boot");
	int pingpong_boot = (s && (*s == 'y')) ? 1 : 0;

	if (pingpong_boot == 1) {

		debug (" Skloading Type : ping-pong ...\n");
		/*    Chk factory Img Hdr is next bootable kernel ; then FLIP the order such that
		      field img Hdr (kernelImgHdr) take hdr info for Factory hdr(next bootable one)
		      and factory hdr has field ImageInfo.

		      Else skloader flow is as is i.e...
		      */
		if (fact_kernelImgHdr->mActiveImage == 1) {

			debug (" Flip order for field to factory Img \n");

			/* point field header at factory header */
			kernelImgHdr = (FlashImageInfo_t *)(CAVM18_FACT_HDR_DRAM_ADDR + CAVM18_FACT_HDR_KERNEL_OFFSET);

			/* and vice versa */
			fact_kernelImgHdr = (FlashImageInfo_t *)CAVM18_FIELD_HDR_DRAM_ADDR;
		}

	} else {

		debug (" Skloading Type : default ...\n");
	}


	/* step 3. Do Parsing of field Img hdr */
	if (kernelImgHdr->mHdrType == KERNEL_IDENTIFIER) { // Field Img

		debug (" Valid : Field Hdr netHDImg identifier \n");

		debug (" Field Header netHDImg crc ...");

		chk_crc32 = crc32 (0, (unsigned char *)kernelImgHdr, sizeof (FlashImageInfo_t) - 4);

		if (chk_crc32 == kernelImgHdr->mCrc32) {


			unsigned int TotalLen = kernelImgHdr->mLength + kernelImgHdr->mSignLen;
			TotalLen = SECTOR_SIZE * ( (TotalLen + SECTOR_SIZE -1) / SECTOR_SIZE );

			/* debug (" netHDImg aligned len : %d\n", TotalLen); */

			debug (" processing of image about to start .. \n");

			/* step 4. netHDImg2 from Nand block to 0x0240_0000 */
#ifdef MEASURE_CRYPTO
			dontcare = measure_time_ms(&total_time_marker);
			accum_busy_wait_time = 0;
			accum_page_read_time = 0;
			accum_pgrd = 0;
			accum_ecccalc = 0;
			accum_correct = 0;
#endif
			if ( Sec1800l_NandRead(kernelImgHdr->mDestinationAddressInDram,
						kernelImgHdr->mSourceAddressInFlash,
						TotalLen) != 0 )
			{

				printf (" Failed to read %s image!\n", ImgName);
				fail_safe_flag = 1;
				goto FAIL_SAFE;
			}

			/* remove 32 byte Hash offset from mLength : use Img_Len for actual Img enc/dec now onwards */
			Img_Len =  kernelImgHdr->mLength-32;
			debug (" Img_Len : %d\n", Img_Len);

			if (Img_Len > MAX_KERNEL_IMAGE_SIZE) {
				fail_safe_flag = 1;
				goto FAIL_SAFE;
			}
			/* back_up the 32 byte of i/p hash */
			memset (inhash, 0x0, sizeof (inhash));
			memcpy (inhash, (uint8_t *)(kernelImgHdr->mDestinationAddressInDram + Img_Len), 32);
			debug ("\n input Img hash dump: "); hex_print (inhash, 32);

#ifdef MEASURE_CRYPTO
			i			dontcare = measure_time_ms(&crypto_marker);
			/* 2nd pass decryption for keyB :: o/p : plain_text Img */
			if (Img_Crypto (kernelImgHdr->mDestinationAddressInDram, Img_Len , CRYPTO_OUT_KERNEL_DRAM_ADDR, keyB, decrypt) != 0) {
				fail_safe_flag = 1;
				goto FAIL_SAFE;
			}
			part1_elapsed = measure_time_ms(&crypto_marker);
			crypto_elapsed += part1_elapsed;

			/* loop-back output as input;
			   do encryption to retrieve 1st pass hash of 16 byte; using keyA*/
			if (Img_Crypto (CRYPTO_OUT_KERNEL_DRAM_ADDR, Img_Len , kernelImgHdr->mDestinationAddressInDram, keyA, encrypt) != 0) {
				fail_safe_flag = 1;
				goto FAIL_SAFE;
			}
#else
			if (Crypto_Command(kernelImgHdr->mDestinationAddressInDram, CRYPTO_OUT_KERNEL_DRAM_ADDR, Img_Len) != 0) {
				fail_safe_flag = 1;
				goto FAIL_SAFE;
			}
#endif

#ifdef MEASURE_CRYPTO
			// part2_elapsed = measure_time_ms(&crypto_marker);
			// crypto_elapsed += part2_elapsed;
			crypto_elapsed = measure_time_ms(&crypto_marker);
			total_elapsed = measure_time_ms(&total_time_marker);

			printf("\n\n\n*** total elapsed time = %u usec\n", total_elapsed);
			printf("*** time spent in crypto = %u usec\n", crypto_elapsed);
			// printf("*** time spent in decrypt = %u usec\n", part1_elapsed);
			// printf("*** time spent in encrypt = %u usec\n", part2_elapsed);
			printf("*** time spent waiting for NAND ready = %u usec\n", accum_busy_wait_time);
			printf("*** time spent waiting to read page = %u usec\n", accum_page_read_time);
			printf("*** time spent       reading page   = %u usec\n", accum_pgrd);
			printf("*** time spent       doing   ecc    = %u usec\n", accum_ecccalc);
			printf("*** time spent     error correcting = %u usec\n", accum_correct);
#endif

			/* flush hash buf */
			memset (hash, 0x0, sizeof (hash));
			/* jump to last 16 byte offset for 1st pass Hash record */
			Hash_Buf_Offset = (kernelImgHdr->mDestinationAddressInDram + (Img_Len -16));

			/* copy last 16byte hash store in local hash bufffer */
			memcpy (hash, (uint8_t *)( Hash_Buf_Offset), 16);
			debug ("\n 1st encryption pass hash dump (16 byte): "); 
			hex_print (hash, 16);

			/* copy inhash's last 26 byte (keyB) into hash last 16 byte index */
			memcpy (hash+16, (inhash + 16), 16);

			debug ("\n 2nd Encryption pass hash dump (16byte): "); 
			hex_print (hash+16, 16);

			debug ("\n 1st/2nd encryption pass hash dump (32 byte): "); 
			hex_print (hash, 32);

			hex_print (inhash, 32);
			/* compare 32 byte hash with input Image */
			val = memcmp(hash, inhash, 32);
			if (val != 0) {

				fail_safe_flag = 1;
				printf ("Error : Hybrid hashing comparison failed ! Field Img\n");
				debug ("inhash (%p)(32 bytes): ", inhash); hex_print(inhash, 32);
				debug ("hash (%p)(32 bytes): ", hash); hex_print(hash, 32);

			} else {
				hex_print(kernelImgHdr->mDestinationAddressInDram + kernelImgHdr->mLength,256);
				/* Hybrid RSA Verify input : hash & signature 2048 bit size */
				val = ImgHybRsa_Verify (hash, (kernelImgHdr->mDestinationAddressInDram + kernelImgHdr->mLength), pKey);
				if (val != 0 ) {
					fail_safe_flag = 1;
					printf ("Error : Hybrid rsa_verify failed ! Field Img\n");
				}

				/* collect MTD (flash) address for bootargs cmdline option : Image upgrade condition
				   of ping-pong scheme */
				if (pingpong_boot == 1)
					mtd_part =  kernelImgHdr->mSourceAddressInFlash;
			}

		} else {
			fail_safe_flag = 1;
			printf (" Invalid image 2 CRC. Header corrupt!\n");
		}

	} else {
		fail_safe_flag = 1;
		printf (" Invalid image 2 header signature !!!\n");
	}

FAIL_SAFE:
	/* condition to check for switch to Factory Image */
	if (fail_safe_flag == 1) {

		/* Flush out decryted Kernel Image with zero */
		memset ((uint8_t *)CRYPTO_OUT_KERNEL_DRAM_ADDR, 0x0, MAX_KERNEL_IMAGE_SIZE);


		debug (" Factory Hdr info ....\n\n");
		debug (" netHDImage \n");
		debug (" mHdrType : 0x%x mSourceAddressInFlash : 0x%x mDestinationAddressInDram : 0x%x \
				Fail-safe image mLength : %d siglen : %d ActiveImage flag : %d          \
				mVersion : %s Crc32 : 0x%x \n", fact_kernelImgHdr->mHdrType,
				fact_kernelImgHdr->mSourceAddressInFlash ,
				fact_kernelImgHdr->mDestinationAddressInDram,
				fact_kernelImgHdr->mLength,
				fact_kernelImgHdr->mSignLen, fact_kernelImgHdr->mActiveImage,
				fact_kernelImgHdr->mVersionNo, fact_kernelImgHdr->mCrc32);

		val = Img_Authenticate(fact_kernelImgHdr, 1); // 1 : Factory kernel Img
		if (val != 0) {

			printf (" Fail-safe Img corrupt .. exiting Skloader !!!\n");
			return -1;
		}

		printf (" Factory Img Authentication passed \n");

		/* collect MTD (flash) address for bootargs cmdline option : fail-safe condition
		   of ping-pong scheme */
		if (pingpong_boot == 1)
			mtd_part =  fact_kernelImgHdr->mSourceAddressInFlash;

	} else {

		printf (" Field Img Authentication passed \n");
	}


	/* point to secure Blob header */
	blobImgHdr = (FlashImageInfo_t *)(CAVM18_FACT_HDR_DRAM_ADDR + CAVM18_FACT_HDR_BLOB_OFFSET);
	debug("Blob hdr (FlashImageInfo_t *) @ 0x%08x + 0x%08x = 0x%08x\n",
			CAVM18_FACT_HDR_DRAM_ADDR, CAVM18_FACT_HDR_BLOB_OFFSET,
			CAVM18_FACT_HDR_DRAM_ADDR + CAVM18_FACT_HDR_BLOB_OFFSET);


	/*      On old systems the header was in a separate sector than the blob data, so if the
	 *      header is bad try reading it from the old address.
	 */
	chk_crc32 = crc32 (0, (unsigned char *)blobImgHdr, sizeof (FlashImageInfo_t) - 4);
	debug("act crc = 0x%08x, expected = 0x%08x\n", chk_crc32, blobImgHdr->mCrc32);

	if (chk_crc32 != blobImgHdr->mCrc32) {

		debug("Memory is garbage. Reading blob from NAND @ offset 0x%08x\n", CAVM18_BLOB_HDR_FLSH_ADDR);

		printf("(old header)\n");

		if (Sec1800l_NandRead(CAVM18_FACT_HDR_DRAM_ADDR + CAVM18_FACT_HDR_BLOB_OFFSET,
					CAVM18_BLOB_HDR_FLSH_ADDR,
					CAVM18_BLOB_HDR_LEN) != 0 ) {
			printf (" Failed to read blob Header! .. exiting!!\n");
			return -1;
		}
		chk_crc32 = crc32 (0, (unsigned char *)blobImgHdr, sizeof (FlashImageInfo_t) - 4);
		debug("act crc = 0x%08x, expected = 0x%08x\n", chk_crc32, blobImgHdr->mCrc32);
		if (chk_crc32 != blobImgHdr->mCrc32) {
			printf("Blob is corrupt! Sorry...\n");
			return -1;
		}

	} else {
		printf("\n");
	}

	debug (" Secure Blob Image(HDCP/DTCP) Hdr info : \n");
	debug (" HdrType : 0x%x SrcAddrFlash : 0x%x DestAddrDram : 0x%x\n",
			blobImgHdr->mHdrType, blobImgHdr->mSourceAddressInFlash, blobImgHdr->mDestinationAddressInDram);
	debug (" App Img Leng: %d siglen: %d, active: %d\n", blobImgHdr->mLength, blobImgHdr->mSignLen, blobImgHdr->mActiveImage);
	debug (" Vers: %s Crc32: 0x%x\n", blobImgHdr->mVersionNo, blobImgHdr->mCrc32);
	{
		char *s = getenv("BOOTS_RETRY_COUNT");

		if (s)
			retry_count = (int)simple_strtoul (s, NULL, 10);
	}
	while ((val = Img_Authenticate(blobImgHdr, 0 /* Blob image */)) && (retry_count)) {
		printf (" WARNING : App Blob corrupt \n");
		retry_count--;
	}

	if (retry_count) {
		printf (" AppImg (Blob) authenticated and copied to RAM addr : 0x%x\n", \
				CRYPTO_OUT_BLOB_LINUX_ADDR);
	} else {
		/* FATAL ERROR. Could not authenticate blob */

		char *s = getenv("BOOTS_NO_RESET");

		if (!s || (*s == 'n'))  {
			do_reset (NULL, 0, 0, NULL);
		}
	}
	/* Only allow bootargs that we recognize */
	filter_bootargs();

	/*Set setenv u-boot args for Linux f/w upgrade application : In case of ping-pong Scheme only */
	if (pingpong_boot == 1) {

		char *cmdline = NULL;
		char *temp = NULL;
		char newCmdLine[256]; /* should be big enough after it is filtered */

		if (mtd_part == FlashImgHdr[1].mSourceAddressInFlash)
			temp = " mtd_skboot=14,16";
		else
			temp = " mtd_skboot=20,17";

		if ((cmdline = getenv("bootargs")) != NULL) {
			debug (" %s : %d\n", cmdline, strlen(cmdline));

			strcpy (newCmdLine, cmdline);
			strcat (newCmdLine, temp);
			setenv("bootargs", newCmdLine);
		} else {

			debug ("WARNING : bootargs is NULL.. Filling hard-coded content .. \n");
			cmdline = "console=ttyS0,115200";

			strcpy (newCmdLine, cmdline);
			strcat (newCmdLine, temp);
			setenv("bootargs", newCmdLine);
		}

		debug ("bootargs::  %s : %d\n", newCmdLine, strlen(newCmdLine));
	}

	printf ("\n\n ... OK to boot \n");

	Sec1800l_BootM(CRYPTO_OUT_KERNEL_DRAM_ADDR);

	return 0;
}

U_BOOT_CMD (
		boots,        1,      1,      do_boots,
		"Boot Secure Kernel\n",
		"\t Load, authenticate and run secure images (field, then factory) from NAND.\n"
	   );

#endif


#if defined(CONFIG_CMD_ESP1800L_FLASH_UTIL)

/************* esp1800l_flash_util ********************************/

#define UPGRADE_MODE_GREATER    0x00000001   // 0=Upgrade on version mismatch, 1=Upgrade on greater
#define UPGRADE_MODE_FORCE      0x00000002   // 0=Upgrade based on version, 1=Force
#define UPGRADE_MODE_FACTORY    0x00000004   // 0=Upgrade field images 1=Upgrade Factory images
#define UPGRADE_MODE_NO_KERNEL  0x00000008   // 0=Upgrade kernel 1=Don't upgrade kernel
#define UPGRADE_MODE_NO_UBOOT   0x00000010   // 0=Upgrade uboot 1=Don't upgrade uboot

// Header check signatures
typedef struct __ImageInfo_t
{
	uint32_t mHdrId;                   // Sanity check of image before processing!
	/*
#define UBOOT_IDENTIFIER  (0x11223344)    // fail-safe/factory uboot
#define KERNEL_IDENTIFIER (0x87543223)    // Fail-safe/factory kernel
#define BLOB_IDENTIFIER   (0x22113344)    // APP_SIG :: HDCP + DTCP + ...
*/
	uint32_t mLength;                  // image len + image_signature len
	uint32_t mSignLen;                 // size of signature len (stored at end of image)

	char mVersionNo[256];              // svn revision for Linux/u-boot

	char md5sum[16];                   // md5sum of image
	uint32_t mCrc32;                   // crc of ImageInfo_t header

} ImageInfo_t;

typedef struct __ActFlashImage_t
{
	uint32_t numImages;
	uint32_t reserved[3];
	ImageInfo_t mCavm_ubootImage;	   // u-boot image
	ImageInfo_t mCavm18_kernelImage;   // kernel image (ubootpImage)

} ActFlashImage_t;

ActFlashImage_t actImage;

typedef struct __DistFlashImage_t
{
	uint32_t numImages;
	uint32_t reserved[3];

	// Up to three ImageInfo_t+Image data follow here:
	ImageInfo_t image;	// mLength = total image_len + signature len
} DistFlashImage_t;

DistFlashImage_t distImage;


static void print_Flashhdr (void* Hdr)
{
	uint32_t *hdr_p = (uint32_t *)Hdr;
	hdr_p=hdr_p;
	/* Dump data of all three for ananlysis */
	debug (" Flash Hdr ... \n");
	debug ("numImages : 0x%02x, reserved[0:1:2] : 0x%02x 0x%02x 0x%02x \n", \
			*(hdr_p + 0), *(hdr_p + 1), *(hdr_p + 2),
			*(hdr_p + 3));
}

static void print_ImageInfo (ImageInfo_t *hdr_p, char *ImageBin)
{
	int i;

	printf (" %s ImageInfo_t ... \n", ImageBin);
	printf (" mHdrType : 0x%x mLength : %d siglen : %d mVersion : %s Crc32 : 0x%x \n", \
			hdr_p->mHdrId, hdr_p->mLength,
			hdr_p->mSignLen, hdr_p->mVersionNo,
			hdr_p->mCrc32);

	printf (" MD5SUM ... \n");
	for (i=0; i<16; i++) {

		printf (" %02x", (unsigned char)hdr_p->md5sum[i]);
	}

	printf ("\n\n");
}

enum {
	Factory,
	Field
}eActImgType;

// check image version of with what is in NAND with what is passed.
// If matching, return 0
// If new is less than existing return -1
// If new is greater than existing return 1
static int checkImageVersion (uint32_t dstFlashAddr, unsigned long newVersion)
{
	int status = -1;
	FlashImageInfo_t tmpImgHeader;
	FlashImageInfo_t *Img = &tmpImgHeader;
	char *endp;
	unsigned long oldVersion;

	// read whole header

	if ( Sec1800l_NandRead((uint32_t)Img, dstFlashAddr,
				sizeof(FlashImageInfo_t)) != 0 )
	{
		printf (" Failed to read NAND header location 0x%x\n", dstFlashAddr);
		status = -2;
		goto Error1 ;
	}

	// cmp version string of format svn_rev_x.x. FIXME this is broken for incrementing from 9 to 10.
	oldVersion = simple_strtoul(&Img->mVersionNo[0], &endp, 10);

	if (oldVersion == newVersion) {

		printf (" Version Match \n");
		status = 0;
	} else if (oldVersion > newVersion) {

		printf (" Version Mismatch : less \n");
		status = -1;
	} else {

		printf (" Version Mismatch : greater \n");
		status = 1;
	}

Error1:
	return status;
}

/*
 * Function to parse Image headers and flash onto respective loaction.
 * Data is verified with a md5sum verification on the image read back from NAND.
 */
static int parse_and_flashImg ( uint32_t SrcImgLoc,
		ImageInfo_t *SrcImgHdr,
		const FlashImageInfo_t *TemplateDstImgHdr,
		uint32_t flashHeaderAddr,
		uint32_t modeFlag,
		uint32_t actImgType)
{
	int status = -1;
	uint32_t chk_crc32 = 0;
	int versionDiff;
	uint32_t ImgLen;
	uint32_t hdrAddr = HEADER_DRAM_ADDR;

	FlashImageInfo_t *tmpDstImgHdr;

	if ( !( (SrcImgHdr->mHdrId == UBOOT_IDENTIFIER) ||
				(SrcImgHdr->mHdrId == BLOB_IDENTIFIER) ||
				(SrcImgHdr->mHdrId == KERNEL_IDENTIFIER) ) ) {
		printf (" Invalid input header Identifier (0x%08x)... exiting.\n", SrcImgHdr->mHdrId);
		goto Error1;
	}

	/* Validate Crc and Md5sum  */
	debug (" Valid : %s header id\n", SrcImgHdr->mHdrId == UBOOT_IDENTIFIER ? "uboot" :
			SrcImgHdr->mHdrId == BLOB_IDENTIFIER ? "blob" :"kernel");

	chk_crc32 = crc32 (0, (unsigned char *)SrcImgHdr, sizeof (ImageInfo_t) - 4);
	if (chk_crc32 != SrcImgHdr->mCrc32) {

		printf (" Invalid Crc.. ImageInfo_t hdr corrupt .. exiting ! \n");
		goto Error1;
	}

	printf(" Valid header CRC. Processing %s image. Checking md5sum... ",
			(SrcImgHdr->mHdrId == UBOOT_IDENTIFIER) ? "uboot" :
			(SrcImgHdr->mHdrId == KERNEL_IDENTIFIER) ? "kernel" :
			"blob");


	ImgLen = SrcImgHdr->mLength + SrcImgHdr->mSignLen;

	/* calc md5sum */
	if (sec1800l_md5sum(SrcImgLoc,  ImgLen, &SrcImgHdr->md5sum[0]) != 0) {

		printf (" md5sum check failed for header... exiting\n");
		goto Error1;
	}


	printf("\n done.  flashHeaderAddr=%x\n",flashHeaderAddr);

	if (flashHeaderAddr) {
		debug ("preparing Blob/kernel Hdr \n");
		/* read destination nand sector and copy sector size; check versioning;
		   based on modeFlag flash accordingly */
		unsigned long newVersion = simple_strtoul(&SrcImgHdr->mVersionNo[0], NULL, 10);

		versionDiff = checkImageVersion ( flashHeaderAddr, newVersion);
		if (versionDiff == -2) {
			goto Error1;
		}

		if (SrcImgHdr->mHdrId == BLOB_IDENTIFIER)
			modeFlag |= UPGRADE_MODE_FORCE; // Always force blob updates

		if (!(modeFlag & UPGRADE_MODE_FORCE) && (versionDiff == 0)) { // Default Img flash mode

			printf ("Version up to date.\n");
			goto Error1;
		} else if ((modeFlag & UPGRADE_MODE_GREATER) && (versionDiff == 1)) { // Upgrade mode

			printf ("Version in flash is newer. Use --force to force the upgrade.\n");
			goto Error1;
		} else { // force mode ; flash directly

			printf ("Forcing flash update.\n");
		}
		// Put the header into a DRAM area. The factory header is stored at an offset.
		memset ( (unsigned char *)hdrAddr, 0xff, HEADER_DRAM_SIZE);

		if ((SrcImgHdr->mHdrId == KERNEL_IDENTIFIER) &&
				(flashHeaderAddr == CAVM18_FACT_HDR_FLSH_ADDR)) 	{
			tmpDstImgHdr = (FlashImageInfo_t *)(hdrAddr + CAVM18_FACT_HDR_KERNEL_OFFSET);
		} else {
			tmpDstImgHdr = (FlashImageInfo_t *)hdrAddr;
		}

		// populate FlashImageInfo_t[x], which will be written into the flash.
		tmpDstImgHdr->mHdrType                  = TemplateDstImgHdr->mHdrType;
		tmpDstImgHdr->mSourceAddressInFlash     = TemplateDstImgHdr->mSourceAddressInFlash;
		tmpDstImgHdr->mDestinationAddressInDram = TemplateDstImgHdr->mDestinationAddressInDram;
		tmpDstImgHdr->mLength                   = SrcImgHdr->mLength;
		tmpDstImgHdr->mSignLen                  = SrcImgHdr->mSignLen;
		tmpDstImgHdr->mActiveImage              = actImgType; // 1 : field ;; 0 Factory
		memcpy(tmpDstImgHdr->mVersionNo, SrcImgHdr->mVersionNo, 256);
		tmpDstImgHdr->mCrc32 = crc32 (0, (unsigned char *)tmpDstImgHdr, sizeof (FlashImageInfo_t) - 4);

		if (SrcImgHdr->mHdrId == KERNEL_IDENTIFIER) {
			/* Flash ImgHdr to Nand sector. This is a separate sector for kernel images.
			*/
			if (Sec1800l_NandErase(flashHeaderAddr, SECTOR_SIZE, 1) != 0) {

				printf (" Error.. NAND erase failed for header!\n");
				goto Error1;
			}

			if (Sec1800l_NandWrite(hdrAddr, flashHeaderAddr, HEADER_DRAM_SIZE) != 0) {

				printf (" Error .. NAND write failed for header!\n");
				goto Error1;
			}
		}
	}

	uint32_t flashImageAddr = TemplateDstImgHdr->mSourceAddressInFlash;

	if (SrcImgHdr->mHdrId == BLOB_IDENTIFIER) {
		// Copy the blob to after the blob header and use that area to copy to NAND
		memcpy((unsigned char *)(hdrAddr + CAVM18_BLOB_HDR_LEN), (unsigned char *)SrcImgLoc, ImgLen);
		SrcImgLoc = hdrAddr;

		// Adjust the flashImageAddr to reflect blob header preceding the data.
		flashImageAddr -= CAVM18_BLOB_HDR_LEN;
		ImgLen += CAVM18_BLOB_HDR_LEN;
	}


	// work on Image alignment; should be multiple of 128KByte
	uint32_t ImgLen_aligned = SECTOR_SIZE * ( (ImgLen + SECTOR_SIZE -1) / SECTOR_SIZE ) ;

	debug ("aligned 128K boundry ImgLen : %x\n", ImgLen_aligned);

	// condition check for memory boundary so that ImgLen shouldn't be greater than 64M
	if (ImgLen_aligned >= MAX_KERNEL_IMAGE_SIZE) {
		printf (" Image Size greater than %d MByte.. Exiting \n",MAX_KERNEL_IMAGE_SIZE);
		goto Error1;
	}


	// jump to SrcImgLoc offset, make alignment to 128K (SECTOR_SIZE)
	// flash to Nand
	if (Sec1800l_NandErase(flashImageAddr, ImgLen_aligned, 1) != 0) {

		printf (" Error .. NAND Erase failed for header.\n");
		goto Error1;
	}

	if (Sec1800l_NandWrite((uint32_t)SrcImgLoc, flashImageAddr, ImgLen_aligned) != 0) {

		printf (" Error .. NAND write failed for header\n");
		goto Error1;
	}

	debug ("Reading back : Nand : %x\n", flashImageAddr);

	if (SrcImgHdr->mHdrId == BLOB_IDENTIFIER) {
		// Adjust the flashImageAddr to reflect blob header preceding the data.
		flashImageAddr += CAVM18_BLOB_HDR_LEN;
		ImgLen -= CAVM18_BLOB_HDR_LEN;
		ImgLen_aligned -= CAVM18_BLOB_HDR_LEN;
	}

	// Read back same image len, verify md5sum with ImageInfo *SrcImgHdr
	if ( Sec1800l_NandRead(READ_BACK_IMAGE_RAM_ADDR, flashImageAddr, ImgLen_aligned) != 0 )
	{
		printf (" Failed to read NAND header location 0x%x\n", flashImageAddr);
		goto Error1 ;
	}
	// calc md5sum
	if (sec1800l_md5sum(READ_BACK_IMAGE_RAM_ADDR,  ImgLen, &SrcImgHdr->md5sum[0]) != 0) {

		printf (" md5sum check failed for Image\n");
		goto Error1;
	}
	printf (" \n\n Valid md5sum .. NAND read back Successful\n");

	status = 0;

Error1 :
	return status;
}

static int find_needle_in_haystack (const unsigned char *needle, int len, const unsigned char *haystack, int limit)
{
	int i;
	unsigned char c;
	int state = 0;

	for (i = 0; i < limit; i++) {
		c = haystack[i];

		if (c == needle[state]) {
			state++;
			if (state == len) {
				return i+1; /* found needle */
			}
		} else {
			state = 0;
		}
	}

	return -1; /* did not find needle */
}

static void show_failed_search_results(const char *fmtstrg, const unsigned char *haystack, const unsigned char *needle, int needle_length)
{
	int i = 0, cols = 0;
	printf(fmtstrg, haystack, needle, needle_length);
	for (i = 0; i < 256; ++i) {
		if (cols == 0) {
			printf("\t");
		}
		printf(" %02x", needle[i]);
		if (++cols == 16) {
			printf("\n");
			cols = 0;
		}
	}
	if (cols != 0) {
		printf("\n");
	}
	return;
}

static int public_key_does_not_match_mine (unsigned long addr)
{
	unsigned char *needle, *haystack;
	int nBytesToCompare = 256, pos = 0, pos2 = 0;
	/* int i = 0, cols = 0; */

//	printf("pKey @ %p, blob_pKey @ %p,addr=%p\n", pKey, blob_pKey,addr);
	haystack = (unsigned char *)(addr+0x250);
	needle = pKey;
	pos = find_needle_in_haystack(needle, 256, haystack, 524288);
	if (pos != -1) {

		/* pKey was found. Look for matching blob pKey. */
//		printf("pKey found @ %p\n", haystack + (pos - 256));

		if (memcmp(pKey, blob_pKey, 256) == 0) {
			pos2 = find_needle_in_haystack(needle, 256, haystack + pos, 524288 - pos);
			if (pos2 != -1) {
//				printf("blob_pKey found @ %p\n", haystack + (pos + pos2 - 256));
				return 0;   /* Both key and blob pkey matched. */
			} else {
				show_failed_search_results("memory area begins @ %p (512K-256 bytes), blob_pKey @ %p, key length: %d, target blob_pKey:\n",
						haystack + pos, needle, 256);
				printf("mismatch because blob_pKey wasn't found in image\n");
				return 1;   /* Public key did not match. */
			}
		} else {
			printf("pKey and blob_pKey are not the same. Look for blob_pKey now\n");
			needle = blob_pKey;
			pos2 = find_needle_in_haystack(needle, 256, haystack, 524288);
			if (pos2 != -1) {
//				printf("blob_pKey found @ %p\n", haystack + (pos - 256));
				return 0;   /* Both key and blob pkey matched. */
			} else {
				show_failed_search_results("memory area begins @ %p (512K bytes), blob_pKey @ %p, key length: %d, target blob_pKey:\n",
						haystack, needle, 256);
				printf("mismatch because blob_pKey wasn't found in image\n");
				return 1;   /* Public key did not match. */
			}
		}
	} else {
		show_failed_search_results("memory area begins @ %p (512K bytes), pKey @ %p, key length: %d, target pKey:\n",
				haystack, needle, 256);
		printf("mismatch because pKey wasn't found in image\n");
		return 1;   /* Public key did not match. */
	}
#if 0
	if (memcmp(pKey, blob_pKey, 256) == 0) {
		/* pKey and blob_pKey are identical */

		if (pKey > blob_pKey)
			needle = blob_pKey;
		else
			needle = pKey;

		nBytesToCompare = 512;

	} else {
		/* pKey is not the same as blob_pKey.  Do not bother to scan for
		   blob_pKey because all boards share the same blob_pKey.  Not
		   all boards share the same pKey. */

		needle = pKey;
		nBytesToCompare = 256;
	}

	haystack = (unsigned char *)(addr+0x250);

	if (find_needle_in_haystack(needle, nBytesToCompare, haystack, 524288) == -1) {
		printf("memory area begins @ %p (search 512K bytes), pKey @ %p, key length: %d, target pKey:\n",
				haystack, needle, nBytesToCompare);
		for (i = 0; i < nBytesToCompare; ++i) {
			if (cols == 0) {
				printf("\t");
			}
			printf(" %02x", needle[i]);
			if (++cols == 16) {
				printf("\n");
				cols = 0;
			}
		}
		if (cols != 0) {
			printf("\n");
		}
		return 1; /* public keys do not match mine */
	} else {
		return 0; /* public keys match mine */
	}
#endif
}

/*
 * esp1800l_flash_util : Programm to flash distribution / Active Image in Nand Sectors
 */

int do_esp1800l_flash_util (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	int status = -1;
	int i;
	int ActImgType = Field;
	int modeFlag = 0;
	unsigned long addr;

	if (argc<2 || argc>6) {

		return cmd_usage(cmdtp);
	}

	for(i=1; i<argc-1; i++)
	{
		if (!strcmp("--upgrade", argv[i])) {
			modeFlag |= UPGRADE_MODE_GREATER;
			continue;
		}

		if (!strcmp("--force", argv[i])) {
			modeFlag |= UPGRADE_MODE_FORCE;
			continue;
		}

		if (!strcmp("--factory", argv[i])) {
			modeFlag |= UPGRADE_MODE_FACTORY;
			ActImgType = Factory;
			continue;
		}

		if (!strcmp("--kernel", argv[i])) {
			modeFlag |= UPGRADE_MODE_NO_UBOOT;
			continue;
		}

		if (!strcmp("--uboot", argv[i])) {
			modeFlag |= UPGRADE_MODE_NO_KERNEL;
			continue;
		}

		printf("Unexpected argument: %s\n", argv[i]);
		return cmd_usage(cmdtp);
	}

	// If both --uboot and --kernel were indicated clear the previously set flags
	if ((modeFlag & UPGRADE_MODE_NO_UBOOT) && (modeFlag & UPGRADE_MODE_NO_KERNEL))
		modeFlag &= ~(UPGRADE_MODE_NO_UBOOT | UPGRADE_MODE_NO_KERNEL);

	addr = simple_strtoul(argv[argc-1], NULL, 16);

	if (addr == 0) {
		printf("Unexpected image load address\n");
		return cmd_usage(cmdtp);
	}

	// Arguments good
	printf(" %s %s %s.\n",
			modeFlag & UPGRADE_MODE_FORCE ? "Force update to" :
			modeFlag & UPGRADE_MODE_GREATER ? "Checking version is greater to update" :
			"Checking version is different to update",
			modeFlag & UPGRADE_MODE_FACTORY ? "factory" : "field",
			modeFlag & UPGRADE_MODE_NO_UBOOT ? "kernel image" :
			modeFlag & UPGRADE_MODE_NO_KERNEL ? "uboot image" :
			"uboot/kernel/blob images");

	/* Rd mnumImage ; =1 then copy hdr of size distFlashImage OR > 1 then copy to hdr type
	   actFlashImage */

	/* Note that the following statement works only when the size of a pointer is the same
	   as the size of an unsigned. */
	unsigned char *Img = (unsigned char *) addr;

	debug ("Image hdr Ram addr : %p \n", Img);

	memcpy ( (unsigned char *)&distImage, Img, sizeof(distImage));

	if (distImage.image.mHdrId == BLOB_IDENTIFIER) {
		/* Sanity check. */
		if (distImage.numImages != 1) {
			printf("Number of images is %u, but only one is expected. No blob downloaded!\n", distImage.numImages);
			goto Error1;
		}

		/* These flags aren't allowed for a blob program. They don't make sense. */
		if ((modeFlag & UPGRADE_MODE_NO_UBOOT) ||
				(modeFlag & UPGRADE_MODE_NO_KERNEL)) {
			printf("Unexpected %s argument for blob image.\n",
					modeFlag & UPGRADE_MODE_NO_UBOOT ? "--kernel" : "--uboot");
			goto Error1;
		}

		debug ("Dist-specific Image Hdr dump... \n");

		print_Flashhdr ( &distImage );
		print_ImageInfo ( &distImage.image, "Secure Blob Image" );

		// jump to real Image
		/* Note that for a blob, distImage.numImages will be 1, so it's safe to assume that
		   the image starts following the first image header. */
		Img += sizeof (distImage);

		debug (" Img offset : %p\n", Img);

		if ( parse_and_flashImg ((uint32_t)Img,
					&distImage.image,
					&FlashImgHdr[0],
					CAVM18_BLOB_HDR_FLSH_ADDR,
					modeFlag,
					ActImgType) != 0)
		{
			goto Error1;
		}

	} else if (distImage.numImages == 2) { /* Contains both u-boot and kernel */

		if (public_key_does_not_match_mine(addr)) {
			printf("\nERROR: The RSA public key inside the U-Boot image inside your SBIN file does\n");
			printf("       not match mine.  I don't like your SBIN file!\n\n");

			if (modeFlag & UPGRADE_MODE_FORCE) {
				/* user typed the "--force" command line option */
				int tmp;
				printf("If you know what you're doing, you can ignore the error and still proceed.\n");
				printf("Do you want to ignore the error and still proceed?  (y/N)  ");
				tmp = getc();

				switch (tmp) {
					case 'y':
					case 'Y':
						printf("Yes\n\n");
						break;
					default:
						printf("No\n\n");
						goto Error1;
						break;
				}
			} else goto Error1;
		}

		memset ( (unsigned char *)&actImage, 0x00, sizeof (actImage));
		memcpy ( (unsigned char *)&actImage, Img, sizeof(actImage));

		debug ("Active Image Hdr dump... \n");
		print_Flashhdr ( &actImage );
		print_ImageInfo ( &actImage.mCavm_ubootImage, "U-boot image" );
		print_ImageInfo ( &actImage.mCavm18_kernelImage, "Kernel image" );

		void *pkernelHdr = NULL;
		void *pUbootHdr = NULL;
		uint32_t DstKernelHdrLoc; // kernel Img hdr flash Loc

		// Decode for Field/Fact; populate src Flash addr accordingly.
		if (ActImgType == Factory) {
			pkernelHdr = (FlashImageInfo_t *) (&FlashImgHdr[1]);
			pUbootHdr  = (FlashImageInfo_t *) (&FlashImgHdr[3]);
			DstKernelHdrLoc = CAVM18_FACT_HDR_FLSH_ADDR;
		} else {
			pkernelHdr = (FlashImageInfo_t *) (&FlashImgHdr[2]);
			pUbootHdr  = (FlashImageInfo_t *) (&FlashImgHdr[4]);
			DstKernelHdrLoc = CAVM18_FIELD_KENREL_HDR_FLSH_ADDR;
		}

		debug (" Img offset : %p, sizeof actFlashImage : %d\n", Img, sizeof (actImage));

		// jump to real image of uboot, following the header
		Img += sizeof (actImage);

		if (!(modeFlag & UPGRADE_MODE_NO_UBOOT) &&
				(parse_and_flashImg ((uint32_t)Img,
						     &actImage.mCavm_ubootImage,
						     (FlashImageInfo_t *)pUbootHdr,
						     0,
						     modeFlag,
						     ActImgType) != 0) )
		{

			goto Error1;
		}

		// jump to Image real Image of kernel
		Img += (actImage.mCavm_ubootImage.mLength + actImage.mCavm_ubootImage.mSignLen);

		if (!(modeFlag & UPGRADE_MODE_NO_KERNEL) &&
				(parse_and_flashImg ((uint32_t)Img,
						     &actImage.mCavm18_kernelImage,
						     (FlashImageInfo_t *)pkernelHdr,
						     DstKernelHdrLoc,
						     modeFlag,
						     ActImgType) != 0))
		{

			goto Error1;
		}

	} else {

		printf (" Invalid numImages value (%d) in flash image header, expected 1 or 2.\n",
				distImage.numImages);
		goto Error1;
	}

	status = 0;

Error1:

	if (nandWriteError) {
		status = -EIO;
		nandWriteError = 0;
	}
	return status;

}

U_BOOT_CMD (
		esp1800l_flash_util ,        6,      0,      do_esp1800l_flash_util,
		"Utility to flash signed_encrypted Image on Nand \n",
		"\t [--factory] [--upgrade|--force] [--kernel|--uboot] image_load_addr \n"
		"\t [--factory] Image type ; applicable for kernel/secureblob\n"
		"\t [--upgrade] Upgrade nand sector with new Image provided that new Image has greater svn version \
		compare with flashed Image\n"
		"\t [--force]   Forceably flash Image: no version checking\n"
		"\t [--kernel]  Only upgrade kernel\n"
		"\t [--uboot]  Only upgrade uboot\n"
		"\t [Image_load_addr] load addr of Image; should be 0x2000000 \n"
	   );

#endif

#if defined(CONFIG_CMD_XRSA_VERIFY)
/*
   do_xrsa_verify - Program to rsa verify signed Image.
   This program capable of doing rsa verify for PKCS15. key format 2048.
   */
static int do_xrsa_verify (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	if (argc < 2 || argc > 4)
		return cmd_usage(cmdtp);

	/* signed Image address */
	unsigned long aImgAddress = simple_strtoul(argv[1], NULL, 16);

	/* image len excludes */
	unsigned long aLen = simple_strtoul(argv[2], NULL, 16);

	/* image len excludes */
	unsigned long asignAddress = simple_strtoul(argv[3], NULL, 16);

	printf ("Signed image: %x, Len: %x, signature addr: %x \n",(unsigned int) aImgAddress,(unsigned int) aLen,(unsigned int) asignAddress);

	int val = ImgRsa_Verify (aImgAddress, aLen, asignAddress);

	if (val != 0)
		return -1;

	return 0;
}

U_BOOT_CMD (
		xrsa_verify ,        4,      1,      do_xrsa_verify,
		"Verify rsa signed Image using s/w hashing \n",
		"\t Test Image address \n"
		"\t Test image length \n"
		"\t Test Image signature address \n"
		"\t RSA verify of signed image, algo -pkcs15, 2048-bit Key format, signature len : 256 ; function uses s/w hashing \n"
		"\t argc 1 : signed Img len \n"
		"\t argc 2 : Image len (with out signature) \n"
		"\t argc 3 : Imaged Signature address ( 256 byte len as per algo) \n"
	   );
#endif

#ifdef CONFIG_CMD_SEC1800L_MEM_PROTECT
/*
   Protect write access to ddr memory region for relocated uboot code (from gd->relocaddr to top
   of memory.

DISABLE_SEC_PROTECT : 0 : MEM PROTECT ON
1 : MEM PROTECT OFF
*/

int check_write_to_code_area ( unsigned long addr, unsigned long size)
{
	int disable_protect_flag;	/* 0 : No mem protect for secure region
					 *  1 : mem protect
					 */
	/* Handle out of range case */
	addr &= (PHYS_SDRAM_1_SIZE-1);

#if ( DISABLE_SEC_PROTECT == 1)

	disable_protect_flag = 1;
#else
	disable_protect_flag = 0;
#endif

	if (disable_protect_flag == 1) {

		printf ("WARNING:  No memory protection for code area..\n");
		return 0;
	} else {

		debug (" addr : %x\n", (unsigned int)addr);
		debug (" gd->relocaddr-0x%08x\n", (unsigned int)gd->relocaddr);

		/* mem check for code area */

		if ((addr >= gd->relocaddr && addr <= PHYS_SDRAM_1_SIZE) ||
				(addr + size > gd->relocaddr))	{

			printf (" Write access on code area denied 0x%08x - 0x%08x\n",
					(unsigned)gd->relocaddr, (unsigned)(PHYS_SDRAM_1_SIZE - 1));
			return -1;
		}
	}

	return 0;
}

#endif



