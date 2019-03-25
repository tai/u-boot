#include <common.h>
#include <command.h>

extern int do_auto_update(int size);

#if defined(CONFIG_AUTO_UPDATE) && defined(CONFIG_USB_STORAGE)

int do_update (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	if(argc == 1){
		do_auto_update(0);
	}else{
		do_auto_update(simple_strtoul(argv[1], NULL, 16));
	}
}

U_BOOT_CMD(
	update, 2, 1, do_update,
	"update - update your misc.img by USB storage device. \n",
	"\n"
);
#endif

















