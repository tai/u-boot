#include <common.h>

void do_irq (struct pt_regs *pt_regs)
{
    cnc1800l_do_interrupt();
}

