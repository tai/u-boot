#if !defined(__CUSTOMER_SPECIFIC_DEFINITIONS_H__)
#define __CUSTOMER_SPECIFIC_DEFINITIONS_H__ 1

/* These are customer-specific definitions for U-Boot.  Currently, this includes
   only the addresses in flash memory where certain interesting things may be
   found. This version of the header file contains definitions specific to the
   following customer: dlink */

/* Where the blob header can be found in flash. */
#define CAVM18_BLOB_HDR_FLSH_ADDR          (0x07c80000)    /* default value is 0x05800000 */

/* Where the factory kernel header is in flash. */
// #define CAVM18_FACT_HDR_FLSH_ADDR           (0x07f80000)    (* default value is 0x003c0000 *)
#define CAVM18_FACT_HDR_FLSH_ADDR           (0x003c0000)    /* default value is 0x003c0000 */

/* Where the field kernel header is in flash. */
// #define CAVM18_FIELD_KENREL_HDR_FLSH_ADDR	(0x07fc0000)    (* default value is 0x057c0000 *)
#define CAVM18_FIELD_KENREL_HDR_FLSH_ADDR	(0x057c0000)    /* default value is 0x057c0000 */

/* Location of the factory kernel */
#define FACTORY_KERNEL_FLASH_ADDR           (0x00400000)

/* Location of the field kernel */
#define FIELD_KERNEL_FLASH_ADDR             (0x02400000)

/* Location of the factory u-boot */
#define FACTORY_UBOOT_FLASH_ADDR            (0x00020000)

/* Location of the field u-boot */
#define FIELD_UBOOT_FLASH_ADDR              (0x05b00000)

/* Location (PHYSICAL, not virtual) where the decrypted secure blob is stored.
   The default value is 0xbfff000 */
#define CRYPTO_OUT_BLOB_LINUX_ADDR          (0xefff000)

#endif
