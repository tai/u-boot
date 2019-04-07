#include <command.h>
#include <command.h>

extern int printf(const char* fmt, ...);

int do_crypto(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
    int i;
    char *dvu_argv[9]={0};

    if (argc > 7)
    {
        printf("Too many arguments for DVU.\n");
        return -1;
    }

    dvu_argv[0]="call_external_proc";
    dvu_argv[1]="DVU_";

    for (i=0; i<argc; i++)
    {
        dvu_argv[i+2] = argv[i];
    }

    return do_call_external_proc(NULL, 0, argc+2, dvu_argv);
}

