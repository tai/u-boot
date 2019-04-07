#include <common.h>

void debug_enable(void)
{
    unsigned long i;
    asm ("mrc p15, 0, %0, c1, c1, 0":"=r" (i));
    i |= 0x1;
    asm ("mcr p15, 0, %0, c1, c1, 0": :"r" (i));
}

void debug_disable(void)
{
    unsigned long i;
    asm ("mrc p15, 0, %0, c1, c1, 0":"=r" (i));
    i &= (~0x1);
    asm ("mcr p15, 0, %0, c1, c1, 0": :"r" (i));
}

