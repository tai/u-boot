#include <common.h>

void do_irq (struct pt_regs *pt_regs)
{
    orion_do_interrupt();
}

