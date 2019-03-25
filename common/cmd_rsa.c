/*
 * Copyright 2000-2009
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

#include <common.h>
#include <command.h>
#include <rsa_verify.h>
#include "key.h"

extern unsigned char default_rsa_key_e[SECU_RSA_KEY_E_LEN];

int do_rsa_verify(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned char *buffer;
	int size, i;
	unsigned char sha256[32];
	if(argc < 3)return -1;
	buffer = (unsigned char *)simple_strtoul(argv[1], NULL, 16);
	size = simple_strtoul(argv[2], NULL, 16);
	printf("Checking signature on 0x%08x, size = 0x%x\n", (unsigned int)buffer, size);
	calculate_sha256(buffer, size - SECU_RSA_SIG_LEN, sha256);
	printf("SHA256 = ");
	for(i = 0 ; i < 32; i ++)printf("%02x", sha256[i]);
	printf("\n");
	i = rsa_verify_sig(test_rsa_key_n, default_rsa_key_e, sha256, buffer + size - SECU_RSA_SIG_LEN);
	if(i == 0){
		printf("RSA verify OK\n");
	}else{
		printf("RSA verify FAILED: -%x\n", -i);
	}	 
	return i;
}

U_BOOT_CMD(
	rsa_verify,	3,		1,	do_rsa_verify,
	"rsa_verify_testcase",
	""
);
