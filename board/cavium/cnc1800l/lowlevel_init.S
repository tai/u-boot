/*
 * Board specific setup info
 *
 * (C) Copyright 2003, ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
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

#include <config.h>
#include <version.h>

/* Set up the platform, once the cpu has been initialized */
.globl lowlevel_init
lowlevel_init:

#define PINMUX_UART_ENABLE     0xB2110008
#define PLL_CTRL               0xB2100140
#define VID_DAC_CTRL           0xB2100210
#define AUD_DAC_CTRL           0xB210021C
#define REMAP_CTRL             0x801E0008
/* set pinmux to enable uart 0 */
	ldr  r0,=(PINMUX_UART_ENABLE)
	mov  r1,#1
	str  r1,[r0]

/* enable all needed clk */
	ldr  r0,=(PLL_CTRL)
	ldr  r1,=0xEFFE01D3      /*=0xEFFF00C3 */     /*=0xffffffff*/
	str  r1,[r0]
	ldr  r0,=(VID_DAC_CTRL)
	ldr  r1,=0x3F
	str  r1,[r0]
	ldr  r0,=(AUD_DAC_CTRL)
	ldr  r1,=0x0
	str  r1,[r0]

linebuffer_relocate:
	ldr  r0,=(REMAP_CTRL)
	mov  r1,#0x1
	str  r1,[r0]

	mov  pc, lr

