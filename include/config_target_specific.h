#ifndef _CONFIG_TARGET_SPECIFIC_
#define _CONFIG_TARGET_SPECIFIC_

#include <config_target_names.h>

#if (CONFIG_TARGET_NAME == CNC1800L_EVB)
	/*hardware parameters*/
	#define CONFIG_MINILOADER	256M
	#define CONFIG_DDRPLL	1500
	
	/*environment variables*/
	#define CONFIG_BOOTARGS 	"root=/dev/ram0 console=ttyS0,115200"
	//#define CONFIG_BOOTCOMMAND	"tftp 100000 uImage; bootm"
	
	/*logo format*/
	#define CONFIG_LOGO_FORMAT 565
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080
	
	/*miscellaneous target specific macros*/
	#define CONFIG_CNC1800L_MSHCI
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage */
	#define CONFIG_DRIVER_CN100
	#define CN100_BASEADDRESS	0xb1400000
	#define CONFIG_BOOTFILE 	"uImage"
	#define CONFIG_CMD_UNZIP
	
#elif (CONFIG_TARGET_NAME == CNC1800L_HDK)
	/*hardware parameters*/
	#define CONFIG_MINILOADER	512M
	#define CONFIG_DDRPLL	1500
	#define DDR_SIZE_512M

	/*environment variables*/
	#define CONFIG_BOOTARGS 	"root=/dev/ram0 console=ttyS0,115200"
	#define CONFIG_BOOTCOMMAND	"tftp 100000 uImage; bootm"	
	
	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_CNC1800L_MSHCI
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage */	
	#define CONFIG_DRIVER_CN100
	#define CN100_BASEADDRESS	0xb1400000
	#define CONFIG_BOOTFILE 	"uImage"
	#define CONFIG_CMD_UNZIP

#elif (CONFIG_TARGET_NAME == CNC1800L_6611L)
	/*hardware parameters*/
	#define CONFIG_MINILOADER	256M
	#define CONFIG_DDRPLL	1500

	/*environment variables*/
	#define CONFIG_BOOTARGS 	"root=/dev/mtdblock5 rw rootfstype=jffs2 console=ttyS0,115200 mtdparts=cnc_nand.0:1m(u-boot),3m(kernel),-(fs)"
	#define CONFIG_BOOTCOMMAND	"nand read 0x1000000 0x100000 0x300000;bootm 1000000"
	
	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_USB_HOST_ETHER
	#define CONFIG_USB_ETHER_SMSC95XX
	#define CONFIG_USB_ETHER_ASIX
	#define CONFIG_ENV_IS_IN_NAND
	#define CONFIG_CMD_UNZIP

#elif (CONFIG_TARGET_NAME == CNC1800L_1GX8)
	/*hardware parameters*/
	#define CONFIG_MINILOADER	1Gx8
	#define CONFIG_DDRPLL	1500
	#define DDR_SIZE_1G

	/*environment variables*/
	
	
	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_CMD_UNZIP
	
	
#elif (CONFIG_TARGET_NAME == CNC1800L_NETGEAR_1_0)
	/*hardware parameters*/
	#define CONFIG_MINILOADER	512Mx8
	#define CONFIG_DDRPLL	1500
	#define DDR_SIZE_512M
	
	/*environment variables*/
	#define CONFIG_BOOTARGS 	"root=/dev/mtdblock5 ro rootfstype=jffs2 console=ttyS0,115200 mtdparts=cnc_nand.0:3584k@2m(kernel1)ro,3584k@5632k(kernel2),132m@9m(rootfs)ro,3m@141m(blob)ro,264m@144m(user)"
	#define CONFIG_DISPLAY_LOGO "mw.b f100000 0 f00000;sfre 0x2300000 90000 50000;unzip 0x2300000 3000000 50000;logo 6 6 3000000 1920 1080 0 0;"
	#define CONFIG_BOOTCOMMAND	"run display_logo;update;run check_ip_kernel_update;nand read.jffs2 0x3000000 0x200000 0x300000;bootm 0x3000000"
	#define CONFIG_CHECK_IP_KERNEL_UPDATE	 "eeprom read 0x6100000 0x1 1;if itest.b *0x6100000 == 0x64;then run boot_ip_kernel_update;fi;"
	#define CONFIG_BOOT_IP_KERNEL_UPDATE   "fsload 10000000 misc.img;update ${filesize};mw.b 0x6100000 0 1;eeprom write 0x6100000 0x1 1;"
	
	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080
	
	/*miscellaneous target specific macros*/
	#define CONFIG_SN_DEVID
	#define CONFIG_CMD_UNZIP
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage */
	#define CONFIG_DRIVER_CN100
	#define CN100_BASEADDRESS	0xb1400000
	#define CONFIG_BOOTFILE		"uImage"	
	#define CONFIG_CMD_DATE
	#define CONFIG_RTC_RS5C372A
	#define CONFIG_JFFS2_NAND
	#define CONFIG_JFFS2_LZO
	#define CONFIG_JFFS2_DEV "nand0"
	#define CONFIG_JFFS2_PART_OFFSET 0x9000000
	#define CONFIG_JFFS2_PART_SIZE 0x10800000
	#define CONFIG_SHA256
	#define CONFIG_RSA

	#define CONFIG_SERVERIP    192.168.18.247
	#define CONFIG_IPADDR      192.168.18.249	
#else
	#error "Invalid target!"
#endif
	

#endif

