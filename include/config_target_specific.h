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
	
	#define CONFIG_UPDATE_MD5_CHECK
    #define CONFIG_CMD_EXT4
	#define CONFIG_UPDATE_WITH_EXT
#elif (CONFIG_TARGET_NAME == CNC1800L_TMS_128)
	/*hardware parameters*/
	#define CONFIG_MINILOADER	256M
	#define CONFIG_DDRPLL	1500

	/*environment variables*/
	#define CONFIG_BOOTARGS 	"root=/dev/ram0 console=ttyS0,115200"
	#define CONFIG_BOOTCOMMAND	"nand read 0x2100000 0x2400000 0x2000000;bootm 0x2100000"

	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_CMD_UNZIP
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage */
	#define CONFIG_CMD_SEC_NVEDIT

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
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage */

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
	#define CONFIG_BOOTARGS         "root=/dev/mtdblock3 rootfstype=jffs2 console=ttyS0,115200 outputmode=6 colorformat=6 mtdparts=cnc_nand.0:132m@9m(rootfs),3m@141m(blob),132m@144m(user),132m@276m(update)"
	#define CONFIG_DISPLAY_LOGO "dcache on;sfre 0x1000000 7bc00 32000;unzip 0x1000000 0xe000000;dcache off;logo 6 6 0xe000000 1920 1080 0 0 0 0xe7e9000;"
	#define CONFIG_DISPLAY_LOGO2 "dcache on;sfre 0x1000000 adc00 32000;unzip 0x1000000 0xe000000;dcache off;logo 6 6 0xe000000 1920 1080 0 0 0 0xe7e9000;"
	#define CONFIG_BOOTCOMMAND      "run check_ip_kernel_update;run display_logo;update;nand read 0x3000000 0 0x300000;bootm 0x3000000"
	#define CONFIG_CHECK_IP_KERNEL_UPDATE    "eeprom read 0x6100000 0x0 8;if itest.b *0x6100001 == 0x64;then run boot_ip_kernel_update;fi;"
	#define CONFIG_BOOT_IP_KERNEL_UPDATE   "run display_logo2;update 11400000 *6100004;mw.b 0x6100000 0 1;eeprom write 0x6100000 0x1 1;nand read 0x3000000 0 0x300000;bootm 0x3000000"
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
	#define CONFIG_LED_BLINK
	#define CONFIG_LED_BLINK_GPIO 0
	#define CONFIG_LED_BLINK_HZ	2

	#define CONFIG_SERVERIP    192.168.18.247
	#define CONFIG_IPADDR      192.168.18.249
#elif (CONFIG_TARGET_NAME == CNC1800L_DONGLE)
	/*hardware parameters*/
	#define CONFIG_MINILOADER	256M
	#define CONFIG_DDRPLL	1500

	/*environment variables*/
	#define CONFIG_BOOTARGS 	"root=/dev/mtdblock5 rw rootfstype=jffs2 console=ttyS0,115200 mtdparts=cnc_nand.0:1m(u-boot),3m(kernel),-(fs)"
	#define CONFIG_BOOTCOMMAND	"nand read 0x1000000 0x100000 0x300000;bootm 1000000"

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
	#define CONFIG_CMD_SEC_NVEDIT
#elif (CONFIG_TARGET_NAME == CNC1800L_TMS_256_128_OTT)
    /*hardware parameters*/
	#define CONFIG_MINILOADER	256M
	#define CONFIG_DDRPLL	1500

	/*environment variables*/
	#define CONFIG_BOOTARGS         "root=/dev/mtdblock5 rootfstype=jffs2 console=ttyS0,115200 outputmode=6 colorformat=6 mtdparts=cnc_nand.0:2m@1m(logo),1m@9m(uboot2),100m@14m(rootfs),100m@114m(update),42m@214m(user)"
//	#define CONFIG_DISPLAY_LOGO "dcache on;sfre 0x1000000 7bc00 32000;unzip 0x1000000 0x2000000;dcache off;logo 6 6 0x2000000 1920 1080 0 0 0 0x27e9000;"
//	#define CONFIG_DISPLAY_LOGO2 "dcache on;sfre 0x1000000 adc00 32000;unzip 0x1000000 0x2000000;dcache off;logo 6 6 0x2000000 1920 1080 0 0 0 0x27e9000;"
	#define CONFIG_BOOTCOMMAND      "run display_logo;update;nand read 0x3000000 0x300000 0x300000;bootm 0x3000000"
//	#define CONFIG_CHECK_IP_KERNEL_UPDATE    "eeprom read 0x6100000 0x0 8;if itest.b *0x6100001 == 0x64;then run boot_ip_kernel_update;fi;"
//	#define CONFIG_BOOT_IP_KERNEL_UPDATE   "run display_logo2;update 11400000 *6100004;mw.b 0x6100000 0 1;eeprom write 0x6100000 0x1 1;nand read 0x3000000 0 0x300000;bootm 0x3000000"
	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage. There is no EEPROM. Add this for compile error*/
	#define CONFIG_DRIVER_CN100
	#define CN100_BASEADDRESS	0xb1400000
	#define CONFIG_BOOTFILE		"uImage"

	#define CONFIG_SERVERIP    192.168.18.247
	#define CONFIG_IPADDR      192.168.18.249

	#define CONFIG_CMD_SEC_NVEDIT
#elif (CONFIG_TARGET_NAME == CNC1800L_TMS_512_128_OTT)
    /*hardware parameters*/
	#define CONFIG_MINILOADER	512M
	#define CONFIG_DDRPLL	1500

	/*environment variables*/
	#define CONFIG_BOOTARGS         "root=/dev/mtdblock5 rootfstype=jffs2 console=ttyS0,115200 outputmode=6 colorformat=6 mtdparts=cnc_nand.0:2m@1m(logo),1m@9m(uboot2),100m@14m(rootfs),100m@114m(update),42m@214m(user)"
//	#define CONFIG_DISPLAY_LOGO "dcache on;sfre 0x1000000 7bc00 32000;unzip 0x1000000 0x2000000;dcache off;logo 6 6 0x2000000 1920 1080 0 0 0 0x27e9000;"
//	#define CONFIG_DISPLAY_LOGO2 "dcache on;sfre 0x1000000 adc00 32000;unzip 0x1000000 0x2000000;dcache off;logo 6 6 0x2000000 1920 1080 0 0 0 0x27e9000;"
	#define CONFIG_BOOTCOMMAND      "run display_logo;update;nand read 0x3000000 0x300000 0x300000;bootm 0x3000000"
//	#define CONFIG_CHECK_IP_KERNEL_UPDATE    "eeprom read 0x6100000 0x0 8;if itest.b *0x6100001 == 0x64;then run boot_ip_kernel_update;fi;"
//	#define CONFIG_BOOT_IP_KERNEL_UPDATE   "run display_logo2;update 11400000 *6100004;mw.b 0x6100000 0 1;eeprom write 0x6100000 0x1 1;nand read 0x3000000 0 0x300000;bootm 0x3000000"
	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage. There is no EEPROM. Add this for compile error*/
	#define CONFIG_DRIVER_CN100
	#define CN100_BASEADDRESS	0xb1400000
	#define CONFIG_BOOTFILE		"uImage"

	#define CONFIG_SERVERIP    192.168.18.247
	#define CONFIG_IPADDR      192.168.18.249

	#define CONFIG_CMD_SEC_NVEDIT
#elif (CONFIG_TARGET_NAME == CNC1800L_TMS_256_256)
    /*hardware parameters*/
	#define CONFIG_MINILOADER	256M
	#define CONFIG_DDRPLL	1500

	/*environment variables*/
	#define CONFIG_BOOTARGS         "root=/dev/mtdblock5 rootfstype=jffs2 console=ttyS0,115200 outputmode=6 colorformat=6 mtdparts=cnc_nand.0:2m@1m(logo),1m@9m(uboot2),100m@14m(rootfs),100m@114m(update),42m@214m(user)"
//	#define CONFIG_DISPLAY_LOGO "dcache on;sfre 0x1000000 7bc00 32000;unzip 0x1000000 0x2000000;dcache off;logo 6 6 0x2000000 1920 1080 0 0 0 0x27e9000;"
//	#define CONFIG_DISPLAY_LOGO2 "dcache on;sfre 0x1000000 adc00 32000;unzip 0x1000000 0x2000000;dcache off;logo 6 6 0x2000000 1920 1080 0 0 0 0x27e9000;"
	#define CONFIG_BOOTCOMMAND      "run display_logo;update;nand read 0x3000000 0x300000 0x300000;bootm 0x3000000"
//	#define CONFIG_CHECK_IP_KERNEL_UPDATE    "eeprom read 0x6100000 0x0 8;if itest.b *0x6100001 == 0x64;then run boot_ip_kernel_update;fi;"
//	#define CONFIG_BOOT_IP_KERNEL_UPDATE   "run display_logo2;update 11400000 *6100004;mw.b 0x6100000 0 1;eeprom write 0x6100000 0x1 1;nand read 0x3000000 0 0x300000;bootm 0x3000000"
	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage. There is no EEPROM. Add this for compile error*/
	#define CONFIG_DRIVER_CN100
	#define CN100_BASEADDRESS	0xb1400000
	#define CONFIG_BOOTFILE		"uImage"

	#define CONFIG_SERVERIP    192.168.18.247
	#define CONFIG_IPADDR      192.168.18.249

	#define CONFIG_CMD_SEC_NVEDIT
#elif (CONFIG_TARGET_NAME == CNC1800L_TMS_512_256)
    /*hardware parameters*/
	#define CONFIG_MINILOADER	512M
	#define CONFIG_DDRPLL	1500
	#define DDR_SIZE_512M

	/*environment variables*/
	#define CONFIG_BOOTARGS         "console=ttyS0,115200 64m@192m(update)"
//	#define CONFIG_DISPLAY_LOGO "dcache on;sfre 0x1000000 7bc00 32000;unzip 0x1000000 0x2000000;dcache off;logo 6 6 0x2000000 1920 1080 0 0 0 0x27e9000;"
//	#define CONFIG_DISPLAY_LOGO2 "dcache on;sfre 0x1000000 adc00 32000;unzip 0x1000000 0x2000000;dcache off;logo 6 6 0x2000000 1920 1080 0 0 0 0x27e9000;"
	#define CONFIG_BOOTCOMMAND      "gpio write 10 1;nand read 3000000 4400000 4000000;bootm 3000000;boots"
//	#define CONFIG_CHECK_IP_KERNEL_UPDATE    "eeprom read 0x6100000 0x0 8;if itest.b *0x6100001 == 0x64;then run boot_ip_kernel_update;fi;"
//	#define CONFIG_BOOT_IP_KERNEL_UPDATE   "run display_logo2;update 11400000 *6100004;mw.b 0x6100000 0 1;eeprom write 0x6100000 0x1 1;nand read 0x3000000 0 0x300000;bootm 0x3000000"
	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage. There is no EEPROM. Add this for compile error*/
	#define CONFIG_DRIVER_CN100
	#define CN100_BASEADDRESS	0xb1400000
	#define CONFIG_BOOTFILE		"zImage"

	#define CONFIG_SERVERIP    192.168.18.247
	#define CONFIG_IPADDR      192.168.18.249
	
	#define CONFIG_CMD_SEC_NVEDIT

#elif (CONFIG_TARGET_NAME == CNC1800L_OTT)
	/*hardware parameters*/
	#define CONFIG_MINILOADER	512M
	#define CONFIG_DDRPLL	1500
	#define DDR_SIZE_512M

	/*environment variables*/
	#define CONFIG_BOOTARGS         "root=/dev/mtdblock5 rootfstype=jffs2 console=ttyS0,115200 outputmode=6 colorformat=6 mtdparts=cnc_nand.0:2m@1m(logo),1m@9m(uboot2),100m@14m(rootfs),100m@114m(update),42m@214m(user)"
	#define CONFIG_BOOTCOMMAND      "run display_logo;update;nand read 0x3000000 0x300000 0x300000;bootm 0x3000000"

	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_CNC1800L_MSHCI
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage. There is no EEPROM. Add this for compile error*/
	#define CONFIG_USB_HOST_ETHER
	#define CONFIG_USB_ETHER_SMSC95XX
	#define CN100_BASEADDRESS	0xb1400000
	#define CONFIG_BOOTFILE		"uImage"

	#define CONFIG_SERVERIP    192.168.18.247
	#define CONFIG_IPADDR      192.168.18.249

#elif (CONFIG_TARGET_NAME == CNC1800L_TMS_MINI)
	/*hardware parameters*/
	#define CONFIG_MINILOADER	256M
	#define CONFIG_DDRPLL	1500

	/*environment variables*/
	#define CONFIG_BOOTARGS         "root=/dev/mtdblock5 rootfstype=jffs2 console=ttyS0,115200 outputmode=6 colorformat=6 mtdparts=cnc_nand.0:2m@1m(logo),1m@9m(uboot2),100m@14m(rootfs),100m@114m(update),42m@214m(user)"
	#define CONFIG_BOOTCOMMAND      "run display_logo;update;nand read 0x3000000 0x300000 0x300000;bootm 0x3000000"

	/*logo format*/
	#define CONFIG_LOGO_FORMAT 8888
	#define CONFIG_LOGO_WIDTH 1920
	#define CONFIG_LOGO_HEIGHT 1080

	/*miscellaneous target specific macros*/
	#define CONFIG_CMD_EEPROM /* There is EEPROM storage. There is no EEPROM. Add this for compile error*/
//	#define CONFIG_USB_HOST_ETHER
//	#define CONFIG_USB_ETHER_ASIX
//	#define CONFIG_BOOTFILE		"uImage"

	#define CONFIG_SERVERIP    192.168.18.247
	#define CONFIG_IPADDR      192.168.18.249

	#define CONFIG_CMD_SEC_NVEDIT
	#define CONFIG_CMD_UNZIP
#else
	#error "Invalid target!"
#endif


#endif
