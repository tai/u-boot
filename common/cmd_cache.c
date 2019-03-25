/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
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

/*
 * Cache support: switch on or off, get status
 */
#include <common.h>
#include <command.h>

static int on_off (const char *);

int do_icache ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	switch (argc) {
	case 2:			/* on / off	*/
		switch (on_off(argv[1])) {
		case 0:	icache_disable();
			break;
		case 1:	icache_enable ();
			break;
		}
		/* FALL TROUGH */
	case 1:			/* get status */
		printf ("Instruction Cache is %s\n",
			icache_status() ? "ON" : "OFF");
		return 0;
	default:
		return cmd_usage(cmdtp);
	}
	return 0;
}


int do_dcache ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	switch (argc) {
	case 2:			/* on / off	*/
		switch (on_off(argv[1])) {
		case 0:	dcache_disable();
			break;
		case 1:	dcache_enable ();
			break;
		}
		/* FALL TROUGH */
	case 1:			/* get status */
		printf ("Data (writethrough) Cache is %s\n",
			dcache_status() ? "ON" : "OFF");
		return 0;
	default:
		return cmd_usage(cmdtp);
	}
	return 0;

}

static int on_off (const char *s)
{
	if (strcmp(s, "on") == 0) {
		return (1);
	} else if (strcmp(s, "off") == 0) {
		return (0);
	}
	return (-1);
}



int do_debug ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	switch (argc) {
	case 2:			/* on / off	*/
		switch (on_off(argv[1])) {
		case 0:	debug_disable();
			break;
		case 1:	debug_enable ();
			break;
		}
		break;
	default:
		return cmd_usage(cmdtp);
	}
	return 0;
}

#ifdef CONFIG_MMU_REMAP
int do_mmu_remap ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 phys_addr, virt_addr, size;
	if(argc < 4){
		return cmd_usage(cmdtp);
	}
	phys_addr = simple_strtoul(argv[1], NULL, 16);
	virt_addr = simple_strtoul(argv[2], NULL, 16);
	size = simple_strtoul(argv[3], NULL, 16);
	if((phys_addr & 0xfffff) || (virt_addr & 0xfffff) || (size & 0xfffff)){
		printf("address and size must be 1MB aligned\n");
		return -1;
	}
	mmu_remap(phys_addr, virt_addr, size);
	return 0;
}

U_BOOT_CMD(
	mmu_remap,   4,   0,     do_mmu_remap,
	"Remap physical to virtual address",
	"<phys_addr> <virt_addr> <size>\n"
	"    - address must be 1MB aligned"
);
#endif




U_BOOT_CMD(
	icache,   2,   1,     do_icache,
	"enable or disable instruction cache",
	"[on, off]\n"
	"    - enable or disable instruction cache"
);

U_BOOT_CMD(
	dcache,   2,   1,     do_dcache,
	"enable or disable data cache",
	"[on, off]\n"
	"    - enable or disable data (writethrough) cache"
);

U_BOOT_CMD(
	debug,   2,   1,     do_debug,
	"enable debug",
	"[on, off]\n"
	"    - enable or disable debug"
);


