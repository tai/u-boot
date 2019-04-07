#include <command.h>
#include <command.h>

extern int printf(const char* fmt, ...);

int do_crypto(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
    extern unsigned _end_ofs;
    extern unsigned _TEXT_BASE;
    extern unsigned long do_go_exec (unsigned long (*entry)(int, char * const []), int argc, char * const argv[]);
    static unsigned magic[] = {0xe92d01f0, 0xe92d0003};
    unsigned *app;

    app = (unsigned *)(_TEXT_BASE+_end_ofs);

    if (app[0] == magic[0] && app[1] == magic[1]) {
        /* call the standalone U-Boot app (dvu.bin) which does the actual work */
        return do_go_exec((void*)app, argc, argv);
    }

    /* I didn't find the magic words, so that means the app is not there */
    printf("dvu.bin app is missing\n");
    return -1;
}
