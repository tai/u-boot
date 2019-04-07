#include <common.h>
#include <command.h>

#if defined(CONFIG_AUTO_UPDATE) && defined(CONFIG_USB_STORAGE)
extern int do_auto_update(int size);

int do_update (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *pCmd[5] = { "nand", "read", NULL, NULL, NULL};
	char TempCmd1[16], TempCmd2[16], TempCmd3[16];

	pCmd[2] = TempCmd1;
	pCmd[3] = TempCmd2;
	pCmd[4] = TempCmd3;

	unsigned int *ptr, size;
	int i;
	if(argc == 1){
		do_auto_update(0);
	}else if(argc == 2){
		if(*argv[1] == '*'){
			ptr = simple_strtoul(argv[1] + 1, NULL, 16);
			return do_auto_update(*ptr);
		}else{
			return do_auto_update(simple_strtoul(argv[1], NULL, 16));
		}
	}else if(argc == 3){
		sprintf(TempCmd1, "0x%x", CONFIG_USB_UPGRADE_DATA_ADDR);
		sprintf(TempCmd2, "%s", argv[1]);
		if(*argv[2] == '*'){
			ptr = simple_strtoul(argv[2] + 1, NULL, 16);
			size = *ptr;			
		}else{
			size = simple_strtoul(argv[2], NULL, 16);
		}
		sprintf(TempCmd3, "0x%x", size);
		printf("nand read %s %s %s\n", pCmd[2], pCmd[3], pCmd[4]);
		i = do_nand(NULL, 0, 5, pCmd);
		if(i)return i;
		return do_auto_update(size);
	}
}

U_BOOT_CMD(
	update, 3, 1, do_update,
	"update - update your u-boot.bin / uImage / ramdiskg.img by USB storage device. \n",
	"\n"
);
#endif

















