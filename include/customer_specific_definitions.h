#if !defined(__CUSTOMER_SPECIFIC_DEFINITIONS_H__)
#define __CUSTOMER_SPECIFIC_DEFINITIONS_H__ 1

/* These are customer-specific definitions for U-Boot.  Currently, this includes
   only the addresses in flash memory where certain interesting things may be
   found. This version of the header file contains definitions specific to the
   following customer: cavium */
#if (CONFIG_TARGET_NAME == CNC1800L_TMS_512_256)
/* Where the blob header can be found in flash. */
#define CAVM18_BLOB_HDR_FLSH_ADDR           (0x09800000)

/* Where the factory kernel header is in flash. */
#define CAVM18_FACT_HDR_FLSH_ADDR           (0x003c0000)

/* Where the field kernel header is in flash. */
#define CAVM18_FIELD_KENREL_HDR_FLSH_ADDR	(0x097c0000)

/* Location of the factory kernel */
#define FACTORY_KERNEL_FLASH_ADDR           (0x00400000)

/* Location of the field kernel */
#define FIELD_KERNEL_FLASH_ADDR             (0x04400000)

/* Location of the factory u-boot */
#define FACTORY_UBOOT_FLASH_ADDR            (0x00020000)

/* Location of the field u-boot */
#define FIELD_UBOOT_FLASH_ADDR              (0x09b00000)

/* Location (PHYSICAL, not virtual) where the decrypted secure blob is stored. */
#define CRYPTO_OUT_BLOB_LINUX_ADDR          (0xafff000)

#else
/* Where the blob header can be found in flash. */
#define CAVM18_BLOB_HDR_FLSH_ADDR           (0x05800000)

/* Where the factory kernel header is in flash. */
#define CAVM18_FACT_HDR_FLSH_ADDR           (0x003c0000)

/* Where the field kernel header is in flash. */
#define CAVM18_FIELD_KENREL_HDR_FLSH_ADDR	(0x057c0000)

/* Location of the factory kernel */
#define FACTORY_KERNEL_FLASH_ADDR           (0x00400000)

/* Location of the field kernel */
#define FIELD_KERNEL_FLASH_ADDR             (0x02400000)

/* Location of the factory u-boot */
#define FACTORY_UBOOT_FLASH_ADDR            (0x00020000)

/* Location of the field u-boot */
#define FIELD_UBOOT_FLASH_ADDR              (0x05b00000)

/* Location (PHYSICAL, not virtual) where the decrypted secure blob is stored. */
#define CRYPTO_OUT_BLOB_LINUX_ADDR          (PHYS_SDRAM_1_SIZE - 0X4001000)
#endif
#endif
