#include <config.h>
/*
 * Processor reset for Nomadik
 */

	.align 5
.globl reset_cpu
reset_cpu:
	ldr  r1, WDT_BASE
	mov  r2, #0
	strh r2, [r1, #0x4]        /* WDT_TORR, minimum value */
	strh r2, [r1, #0x6]
	strh r2, [r1, #0x8]        /* WDT_CCVR, minimum value */
	strh r2, [r1, #0xa]
	mov  r2, #1
	strh r2, [r1, #0x0]        /* WDT_CR, enable WDT and generated System Reset */

_loop_forever:
	b	_loop_forever

WDT_BASE:
	.word   0x101e1000
