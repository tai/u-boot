#include <common.h>
#include <command.h>
#include <environment.h>
#include <watchdog.h>
#include <serial.h>
#include <linux/stddef.h>
#include <asm/byteorder.h>

#include <search.h>
#include <errno.h>
#include <malloc.h>

#define SECURE_BASE             0xfffff800
#define SECURE_MAILBOX0         (SECURE_BASE + 0x00)
#define SECURE_MAILBOX1         (SECURE_BASE + 0x04)
#define SECURE_MAILBOX2         (SECURE_BASE + 0x08)
#define SECURE_MAILBOX3         (SECURE_BASE + 0x0c)
#define SECURE_MAILBOX4         (SECURE_BASE + 0x10)
#define SECURE_MAILBOX5         (SECURE_BASE + 0x14)
#define SECURE_MAILBOX6         (SECURE_BASE + 0x18)
#define SECURE_MAILBOX7         (SECURE_BASE + 0x1c)

#define OTP_REG_WRITE(addr, val) (*((volatile unsigned int *)(addr)) = (val))
#define OTP_REG_READ(addr)       (*((volatile unsigned int *)(addr)))

typedef struct {
    const char *text;
    unsigned int code;
} status_code_entry;

static status_code_entry core_status_codes[] = {
    { "invalid persistent value index.", 1 },
    { "secret persistent value illegal access reguest.", 2 },
    { "factory programmed persistent value illegal program request.", 3 },
    { "lockable persistent value illegal program request.", 4 },
    { "one-Way persistent value illegal program request.", 5 },
    { "persistent value access failure.", 6 },
    { "invalid command.", 1024 },
    { NULL, 0 }
};

static void PrintCoreStatus(unsigned int s)
{
    register int k;
    int located = 0;
    for (k = 0; core_status_codes[k].text != NULL; ++k) {
        if (core_status_codes[k].code == s) {
            printf("Core status indicates: %s\n", core_status_codes[k].text);
            located = 1;
        }
    }
    if (!located) {
        printf("Unrecognized core status %u.\n", s);
    }
    return;
}

int SecOTPRead(const unsigned int addr, unsigned int *value)
{
    int status = -1;
    unsigned int RegVal = 0;
    unsigned int timeout;

    /* 1.0 check last command reponded */
    RegVal = OTP_REG_READ(SECURE_MAILBOX0);
    timeout = 0;
    while(RegVal & 0x80000000)
    {
        /* X_EXIT(timeout++ > 100000); */
        if (++timeout > 100000) goto done;
        RegVal = OTP_REG_READ(SECURE_MAILBOX0);    
    }

    /* 2.0 Write index of PV value */
    OTP_REG_WRITE(SECURE_MAILBOX1, addr);

    /* 3.0 clear status register */
    OTP_REG_WRITE(SECURE_MAILBOX7, 0x0);
    
    /* 4.0 send OTP Read command */
    OTP_REG_WRITE(SECURE_MAILBOX0, 0x80000012);
    
    /* 5.0 Check status register */
    RegVal = OTP_REG_READ(SECURE_MAILBOX7);
    timeout = 0;
    while (!(RegVal & 0x80000000))
    {
        /* X_EXIT(timeout++ > 100000); */
        if (++timeout > 100000) goto done;
        RegVal = OTP_REG_READ(SECURE_MAILBOX7);
    }
    
    /* 6.0 Check core status */
    /* X_EXIT(RegVal & 0xffff); */
    RegVal &= 0xffff;
    if (RegVal != 0) {
        PrintCoreStatus(RegVal);
        goto done;
    }

    /* 7.0 Read PV out */
    *value = OTP_REG_READ(SECURE_MAILBOX2);

    /* 8.0 Clear status register to 0 */
    OTP_REG_WRITE(SECURE_MAILBOX7, 0x0);

    status = 0;

done:
    return status;
}

int SecHWGetNUID(unsigned int *value)
{
    return SecOTPRead(11u, value);
}

int do_read_otp_uid(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int status = 1;
    unsigned int otp_uid;

    if (argc == 1) {
        if (SecHWGetNUID(&otp_uid) == 0) {
            printf("0x%08x\n", otp_uid);
            status = 0;
        }
        else {
            printf("## Error: timeout.\n");
        }
    }
    else {
        printf("## Error: \"%s\" takes no arguments.\n", argv[0]);
    }

    return status;
}

int do_cuid(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    extern unsigned long do_go_exec (unsigned long (*entry)(int, char * const []), int argc, char * const argv[]);
    static unsigned magic[] = {0xe92d01f0, 0xe92d0003};
    unsigned *app;
    unsigned sizeOfdvu_bin, sizeOfhdmi_bin;

    app = (unsigned *)(_TEXT_BASE+_end_ofs);
    /* app is now pointing to dvu.bin */

    if (app[0] == magic[0] && app[1] == magic[1]) {
        sizeOfdvu_bin = app[36];
        app = (unsigned *)(_TEXT_BASE+_end_ofs+sizeOfdvu_bin);
        /* app is now pointing to hdmi.bin */

        if (app[0] == magic[0] && app[1] == magic[1]) {
            sizeOfhdmi_bin = app[36];
            app = (unsigned *)(_TEXT_BASE+_end_ofs+sizeOfdvu_bin+sizeOfhdmi_bin);
            /* app is now pointing to cuid.uboot.app */

            if (app[0] == magic[0] && app[1] == magic[1]) {
                /* call the stand-alone U-Boot app that does the actual work */
                return do_go_exec((void*)app, argc, argv);
            }
        }
    }

    printf("cuid.uboot.app is missing\n");
    return -1;
}

/*
Commands are added to U-Boot by creating a new command structure.
This is done by first including command.h

Then using the U_BOOT_CMD() macro to fill in a cmd_tbl_t struct.

U_BOOT_CMD(name,maxargs,repeatable,command,"usage" ,"help")

name:* is the name of the commad. THIS IS NOT a string.
maxargs: the maximumn numbers of arguments this function takes
command: Function pointer (*cmd)(struct cmd_tbl_s *, int, int, char *[]);
usage:* Short description. This is a string
help:* long description. This is a string
[/b] 
*/
U_BOOT_CMD(
    readotpuid,
    CONFIG_SYS_MAXARGS,
    1,
    do_read_otp_uid,
    "read the unique chip ID from the e-fuses",
    "read the unique chip ID from the e-fuses"
);

U_BOOT_CMD(
    cuid,
    3,
    1,
    do_cuid,
    "read/write the customer unique chip ID from/to the e-fuses",
    "read/write the customer unique chip ID from/to the e-fuses"
);
