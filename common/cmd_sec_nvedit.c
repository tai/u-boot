#include <common.h>
#include <command.h>
#include <environment.h>
#include <watchdog.h>
#include <serial.h>
#include <linux/stddef.h>
#include <asm/byteorder.h>
// #include <asm/global_data.h>
#if defined(CONFIG_CMD_NET)
#include <net.h>
#endif

#include <search.h>
#include <errno.h>
#include <malloc.h>
#include <nand.h>

typedef int (*UBootCommandFunction)(cmd_tbl_t *, int, int, char * const[]);

static int readenv2(size_t offset, u_char *buf, size_t env_length, size_t env_range);
static int env_import2(const char *buf, int check, struct hsearch_data *env_hash_table, size_t env_size);
static void set_default_env2(const char *s, struct hsearch_data *environ_hash_table, char *def_env, size_t def_env_size);
static int env_print2(char *name, struct hsearch_data *env_hash_table);

extern int env_import(const char *buf, int check);
static int writeenv(size_t offset, u_char *buf);

void dispsecenv(void);

// DECLARE_GLOBAL_DATA_PTR;

#define SECURED_PARAMETER_SIZE      (ENV_SIZE)
#define SECURED_PARAMETER_IN_FLASH  ( CONFIG_ENV_SECT_IN_NAND_OFFSET )
#define SECURED_PARAMETER_SIGNATURE ( 0xCA98CADE )
#define EXTRA_BLOCKS_IN_FLASH       ( 1 )


/* Hash table for secure environment variables. */
struct hsearch_data sec_env_htbl;
env_t secure_environment;


#define FACTORY_PASSWORD   ( "cavm_secret" )
#define FLASH_SECTOR_SIZE (128*1024)

static int AllowEdit = 0;

//-----------------------------------------------------------------------------------------

/*
 * The legacy NAND code saved the environment in the first NAND device i.e.,
 * nand_dev_desc + 0. This is also the behaviour using the new NAND code.
 */
static int writeenv(size_t offset, u_char *buf)
{
	size_t end = offset + CONFIG_ENV_SIZE;
	size_t amount_saved = 0;
	size_t blocksize, len;

	u_char *char_ptr;

	blocksize = nand_info[0].erasesize;
	len = min(blocksize, CONFIG_ENV_SIZE);
	end += (blocksize * EXTRA_BLOCKS_IN_FLASH);

	while (amount_saved < CONFIG_ENV_SIZE && offset < end) {
		if (nand_block_isbad(&nand_info[0], offset)) {
            printf("%s: ***** block at offset 0x%07x is bad, skipping.\n", __func__, offset);
			offset += blocksize;
		} else {
			char_ptr = &buf[amount_saved];
			if (nand_write(&nand_info[0], offset, &len,
					char_ptr))
				return 1;
            printf("%s: wrote 0x%07x bytes of data to offset 0x%07x\n", __func__, len, offset);
			offset += blocksize;
			amount_saved += len;
		}
	}
    printf("%s: 0x%07x/0x%07x saved\n", __func__, amount_saved, CONFIG_ENV_SIZE);
	if (amount_saved != CONFIG_ENV_SIZE)
		return 1;

	return 0;
}

/*
 * Command interface: print one or all environment variables
 *
 * Returns 0 in case of error, or length of printed string
 */
static int env_print2(char *name, struct hsearch_data *env_hash_table)
{
	char *res = NULL;
	size_t len;

	if (name) {		/* print a single name */
		ENTRY e, *ep;

		e.key = name;
		e.data = NULL;
		hsearch_r(e, FIND, &ep, env_hash_table);
		if (ep == NULL)
			return 0;
		len = printf("%s=%s\n", ep->key, ep->data);
		return len;
	}

	/* print whole list */
	len = hexport_r(env_hash_table, '\n', &res, 0);

	if (len > 0) {
		puts(res);
		free(res);
		return len;
	}

	/* should never happen */
	return 0;
}

static void set_default_env2(const char *s, struct hsearch_data *environ_hash_table, char *def_env, size_t def_env_size)
{
	if (def_env_size > ENV_SIZE) {
		puts("*** Error - default environment is too large\n\n");
		return;
	}

	if (s) {
		if (*s == '!') {
			printf("*** Warning - %s, "
				"using default environment\n\n",
				s+1);
		} else {
			puts(s);
		}
	} else {
		puts("Using default environment\n\n");
	}

	if (himport_r(environ_hash_table, def_env, def_env_size, '\0', 0) == 0) {
		error("Environment import failed: errno = %d\n", errno);
	}
	return;
}

static int readenv2(size_t offset, u_char *buf, size_t env_length, size_t env_range)
{
	size_t end = offset + env_range;
	size_t amount_loaded = 0;
	size_t blocksize, len;

	u_char *char_ptr;

	blocksize = nand_info[0].erasesize;
	if (!blocksize)
		return 1;
	len = min(blocksize, env_length);
	end += (blocksize * EXTRA_BLOCKS_IN_FLASH);

	while (amount_loaded < env_length && offset < end) {
		if (nand_block_isbad(&nand_info[0], offset)) {
			offset += blocksize;
		} else {
			char_ptr = &buf[amount_loaded];
			if (nand_read_skip_bad(&nand_info[0], offset, &len, char_ptr))
				return 1;
			offset += blocksize;
			amount_loaded += len;
		}
	}
	if (amount_loaded != env_length)
		return 1;

	return 0;
}

static uint32_t Sec_GetCrc32(void)
{
    return crc32(0, &(secure_environment.data[0]), SECURED_PARAMETER_SIZE);
}

void sec_set_default_env(const char *s) 
{
    char defenv[4];

    memset(&defenv[0], 0, sizeof(defenv));
    set_default_env2(s, &sec_env_htbl, defenv, sizeof(defenv));
}

/*
 * Check if CRC is valid and (if yes) import the environment.
 * Note that "buf" may or may not be aligned.
 */
static int env_import2(const char *buf, int check, struct hsearch_data *env_hash_table, size_t env_size)
{
	env_t *ep = (env_t *)buf;

	if (check) {
		uint32_t exp_crc, act_crc;

		memcpy(&exp_crc, &ep->crc, sizeof(exp_crc));

        act_crc = crc32(0, ep->data, env_size);
        if (act_crc != exp_crc)
			return 0;
	}

	if (himport_r(env_hash_table, (char *)ep->data, env_size, '\0', 0)) {
		return 1;
	}

	error("Cannot import environment: errno = %d\n", errno);

	sec_set_default_env("!secure import failed");

	return 0;
}

void sec_env_relocate(void)
{
    int rv; uint32_t actual_crc;
    printf("initializing secure environment\n");

    /* Create a 67-element empty hash table. */
    sec_env_htbl.table = NULL;
    if (hcreate_r(67, &sec_env_htbl)) {
        memset(&secure_environment, 0xff, sizeof(secure_environment));
        rv = readenv2(SECURED_PARAMETER_IN_FLASH,
                      (u_char *)&secure_environment,
                      CONFIG_ENV_SIZE, CONFIG_ENV_SIZE);
        /* dispsecenv(); */
        if (rv == 0) {
            actual_crc = Sec_GetCrc32();
            if (actual_crc != secure_environment.crc) {
                printf("bad factory default area CRC: 0x%08x (actual) vs 0x%08x (expected)\n",
                      actual_crc, secure_environment.crc);
                sec_set_default_env("!bad factory default environment CRC");
            } else {
                env_import2((const char *)(&secure_environment), 0, &sec_env_htbl, SECURED_PARAMETER_SIZE);
            }
        } else {
            printf("there was a problem reading the factory default environment!\n");
        }
    } else {
        printf("ERROR! could not create a new hash table for factory default environment!\n");
    }
    return;
}

#ifdef CONFIG_SYS_FLASH_PROTECTION
zzzw;[]
static int SecChangeFlashProtection(int protect)
{
    int status = 0;
    char TempCmd2_1[16], TempCmd2_2[16];
    char *pCmd3[4] = { "protect", "off", NULL, NULL };
    uint32_t aDestinationAddressInFlash = SECURED_PARAMETER_IN_FLASH;
    uint32_t aLength = ((sizeof(gSecNvRam) + FLASH_SECTOR_SIZE - 1)/FLASH_SECTOR_SIZE) * FLASH_SECTOR_SIZE;

    if (protect) pCmd3[1] = "on";
    sprintf(TempCmd2_1, "0x%x", aDestinationAddressInFlash);
    sprintf(TempCmd2_2, "0x%x", aDestinationAddressInFlash + aLength - 1);
    pCmd3[2] = TempCmd2_1;
    pCmd3[3] = TempCmd2_2;
    if (do_protect(NULL, 0, 4, pCmd3) != 0)
    {
        status = -1;
        if (protect)
            printf("ERROR: fail to protect secure parameter sector\n");
        else
            printf("ERROR: fail to un-protect secure parameter sector\n");
    }
    return status;
}
#endif

void dispsecenv(void)
{
    register int k, j; char c;
    printf("secure environment CRC = 0x%08x\n", secure_environment.crc);
    printf("first 512 bytes:\n");
    for (k = 0; k < 32; ++k) {
        printf("0x%03x:", k*16);
        for (j = 0; j < 8; ++j) printf(" %02x", secure_environment.data[k*16+j]);
        printf("  ");
        for (j = 8; j < 16; ++j) printf(" %02x", secure_environment.data[k*16+j]);
        printf("    ");
        for (j = 0; j < 8; ++j) {
            c = secure_environment.data[k*16+j];
            if (c >= ' ' && c <= '~')
                putc(c);
            else
                putc('.');
        }
        printf("  ");
        for (j = 8; j < 16; ++j) {
            c = secure_environment.data[k*16+j];
            if (c >= ' ' && c <= '~')
                putc(c);
            else
                putc('.');
        }
        printf("\n");
    }
    return;
}

static int Sec_SaveNvramToFlash(void)
{
    int status = -1;
    nand_erase_options_t nand_erase_options;
    unsigned char *buffer;

    memset(&secure_environment, 0xff, sizeof(secure_environment));
    buffer = &(secure_environment.data[0]);
    if (hexport_r(&sec_env_htbl, '\0', (char **)&buffer, SECURED_PARAMETER_SIZE) != -1) {

        secure_environment.crc = Sec_GetCrc32();
        memset(&nand_erase_options, 0, sizeof(nand_erase_options));

        nand_erase_options.length = SECURED_PARAMETER_SIZE + (nand_info[0].erasesize*EXTRA_BLOCKS_IN_FLASH);
        nand_erase_options.offset = SECURED_PARAMETER_IN_FLASH;

        puts("Erasing NAND...\n");
        if (nand_erase_opts(&nand_info[0], &nand_erase_options))
            goto done;

        /* dispsecenv(); */
        puts("Writing to NAND... ");
        if (writeenv(SECURED_PARAMETER_IN_FLASH, (u_char *)&secure_environment)) {
            puts("FAILED!\n");
        } else {
            puts("done\n");
            status = 0;
        }

    } else {

        error("Cannot export environment: errno = %d\n", errno);

    }

done:
    return status;

}

//-----------------------------------------------------------------------------------------

void Sec_NvramPrint(char* apTag)
{
    env_print2(apTag, &sec_env_htbl);
    return;
}

int Sec_SetNvRam (char *name, char *value)
{
	ENTRY e, *ep;

	if (strchr(name, '=')) {
		printf("## Error: illegal character '=' in variable name \"%s\"\n", name);
		return -1;
	}

	/* Delete only ? */
	if (value == NULL) {
        printf("Deleting secure environment key \"%s\"\n", name);
        return hdelete_r(name, &sec_env_htbl)? 0 : -1;
	}

	/*
	 * Insert / replace new value
	 */
	e.key  = name;
	e.data = value;
	hsearch_r(e, ENTER, &ep, &sec_env_htbl);
	if (!ep) {
		printf("## Error inserting \"%s\" variable, errno=%d\n",
			name, errno);
		return -1;
	}

	return 0;
}

#if 0
int Sec_Init(void)
{
	int status = -1;
    memset( & gSecNvRam , 0, sizeof (SecNvRam_t) );
    status = Sec_LoadNvramFromFlash();
    if (0 == status )
    {
        printf("Secured parameter OK...\n");
    }
    else
    {
        printf("ERROR: Invalid Secured parameter found, OR it looks like secured parameter was never programmed on this board.\n");
        memset( & gSecNvRam , 0, sizeof (SecNvRam_t) );
        //Sec_SaveNvramToFlash();
    }
	Sec_ProtectSector();
	return status;
}
#endif


int do_printsecenv(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int i;
    int status = 0;
    if ( argc == 1 )
    {
        Sec_NvramPrint(NULL);
    }
    else
    {
        for (i = 1; i < argc; i++)
        {
            Sec_NvramPrint(argv[i]);
        }
    }
    return status;
}

#define TEMPBUF_SIZE (256)
int do_setsecenv(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int i, status = -1, overflowed = 0, remaining;
    char *dst, *src, TempBuf[TEMPBUF_SIZE] = {0};
    if (argc == 2 )
    {
        Sec_SetNvRam( argv[1], NULL );
        status = 0;
    }
    else
    {
        dst = &TempBuf[0]; remaining = TEMPBUF_SIZE; src = &argv[2][0];
        while (--remaining > 0 && *src != '\0')
            *dst++ = *src++;
        if (*src == '\0' && remaining > 0)
        {
            for (i = 3; i < argc; ++i)
            {
                *dst++ = ' ';
                src = &argv[i][0];
                while (--remaining > 0 && *src != '\0')
                    *dst++ = *src++;
                if (*src != '\0') { overflowed = 1; break; }
            }
            if (!overflowed)
            {
                *dst = '\0';
                Sec_SetNvRam( argv[1], TempBuf );
                status = 0;
            }
        }
    }
    return status;
}

int Sec_SaveToFlash(void)
{
    int status = 0;


    if ( 0 == AllowEdit  )
    {
        printf("ERROR: Sorry, you need password to save secured parameter\n");
        return -1;
    }

    status = Sec_SaveNvramToFlash();
    if ( 0 != status )
    {
        printf("ERROR: failed to store Secured paramter, please check hardware or contact vendor\n");
    }
    return status;
}

int do_savesecenv(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    return Sec_SaveToFlash();
}

int do_pswdsec(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int status = -1;
    if ( 0 == strcmp(argv[1] , FACTORY_PASSWORD ) )
    {
        AllowEdit = 1;
        printf("Password OK, now you can save factory parameters.\n");
        status = 0;
    }
    else
    {
        printf("ERROR: wrong password entered, please contact Cavium.\n");
    }
    return status;
}

int do_set_sec_defaults(void)
{
    int status = -1, rv;
    u_char *buffer = NULL;
    uint32_t exp_crc, act_crc;
    int rc=0;
    char argv0[20];
    char argv1[20];
    char argv2[20];
    char argv3[20];
    char argv4[20];

    /* Verify CRC on factory area */
    printf("Read factory default environment and verify CRC.\n");
    buffer = (u_char *)malloc(CONFIG_ENV_SIZE);
    if (buffer == NULL) {
        printf("ERROR: not enough memory to execute command!\n");
        goto done;
    }
    rv = readenv2(SECURED_PARAMETER_IN_FLASH, buffer, CONFIG_ENV_SIZE, CONFIG_ENV_SIZE);
    if (rv != 0) {
        printf("ERROR: could not read factory default environment!\n");
        goto free_then_done;
    }
    act_crc = crc32(0, buffer + (CONFIG_ENV_SIZE-SECURED_PARAMETER_SIZE), SECURED_PARAMETER_SIZE);
    exp_crc = *(uint32_t *)buffer;
    if (act_crc != exp_crc) {
        printf("ERROR: actual CRC of 0x%08x does not match expected CRC of 0x%08x in secure NVRAM area!\n",
               act_crc, exp_crc);
        goto free_then_done;
    }

#if defined(CONFIG_ENV_IS_IN_SPI_FLASH)
    /* Now that we have the secure environment in memory, erase the regular environment. */
    printf("Erase current environment in SPI flash.\n");
    seeprom_erase(CMD_SE, CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE);

    /* Write environment to SPI flash area (this function always returns 0). */
    printf("Write factory default environment to SPI flash.\n");
    seeprom_write(CONFIG_ENV_OFFSET, buffer, CONFIG_ENV_SIZE);
#elif defined(CONFIG_ENV_IS_IN_NAND)
    /* Now that we have the secure environment in memory, erase the regular environment. */
    printf("Erase current environment in NAND flash.\n");
    sprintf( argv0, "%s", "nand" );
    sprintf( argv1, "%s", "erase" );
    sprintf( argv2, "%X", CONFIG_ENV_OFFSET );
    sprintf( argv3, "%X", CONFIG_ENV_SIZE );

    char *argv_e[] = {argv0,argv1,argv2,argv3};
    
    printf("running:%s %s %s %s\n", argv0, argv1, argv2, argv3);
    rc = do_nand( NULL, 0, sizeof(argv_e)/sizeof(char *), argv_e );
    if (rc != 0) {
		printf("Flashing failed due to error %d\n", rc);
		return -1;
    }
    printf ("nand erase %X %X finished!\n",CONFIG_ENV_OFFSET,CONFIG_ENV_SIZE);

    /* Write environment to NAND flash area (this function always returns 0). */
    printf("Write factory default environment to NAND flash.\n");
    sprintf( argv0, "%s", "nand" );
    sprintf( argv1, "%s", "write" );
    sprintf( argv2, "%X", (unsigned int)buffer );
    sprintf( argv3, "%X", CONFIG_ENV_OFFSET );
    sprintf( argv4, "%X", CONFIG_ENV_SIZE );
    char *argv_w[] = {argv0,argv1,argv2,argv3,argv4};
    printf("running:%s %s %s %s %s\n", argv0, argv1, argv2, argv3, argv4);
    rc = do_nand( NULL, 0, sizeof(argv_w)/sizeof(char *), argv_w );
    if (rc != 0) {
        printf("Flashing failed due to error %d\n", rc);
        return -1;
    }
    printf ("nand write %X %X finished!\n",CONFIG_ENV_OFFSET,CONFIG_ENV_SIZE);
#endif

    /* Re-load environment hash table */
    printf("Sync memory with new environment.\n");
    env_import((char *)buffer, 0);

    /* We're done! */
    status = 0;

free_then_done:
    free(buffer);
done:
    return status;
}

#if defined(CONFIG_CMD_SEC_NVEDIT)
/**************************************************/
U_BOOT_CMD(
	printsecenv, CONFIG_SYS_MAXARGS, 1, (UBootCommandFunction)do_printsecenv,
	"printsecenv- print secured environment variables\n",
	"\n    - print values of all secured environment variables\n"
	"printenv name ...\n"
	"    - print value of environment variable 'name'\n"
);

U_BOOT_CMD(
	setsecenv, CONFIG_SYS_MAXARGS, 0, (UBootCommandFunction)do_setsecenv,
	"setsecenv  - set environment variables\n",
	"name value ...\n"
	"    - set secured environment variable 'name' to 'value ...'\n"
	"setsecenv name\n"
	"    - delete environment variable 'name'\n"
);

U_BOOT_CMD(
	savesecenv, 1+0, 0,	(UBootCommandFunction)do_savesecenv,
	"savesecenv - save secured environment variables to storage\n",
	NULL
);

U_BOOT_CMD(
	pswdsec, 1+1, 0,	(UBootCommandFunction)do_pswdsec,
	"pswdsec - to save secured parameter enter password\n",
	NULL
);

U_BOOT_CMD(
	secinit, CONFIG_SYS_MAXARGS, 0,	(UBootCommandFunction)sec_env_relocate,
	"secinit- initialize secure environment stuff\n"
	" (for testing purposes only)\n",
    NULL
);

U_BOOT_CMD(
    nvram_set_defaults, 1, 0, (UBootCommandFunction)do_set_sec_defaults,
    "nvram_set_defaults - reset environment to factory defaults\n",
    NULL
);
#endif
