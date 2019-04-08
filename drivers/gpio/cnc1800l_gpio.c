
/*
 * Basic GPIO functions
 *
 * Copyright (c) 2008 Celestial Semiconductor
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Tu BaoZhao bz.tu@celestialsemi.com, Celestial Semiconductor
 *
 * Copyright (c) 2007 Celestial Semiconductor
 *
 */

#include <common.h>

#define GPIO_READ 	0
#define GPIO_WRITE	1

static volatile unsigned char *gpio_base  = (volatile unsigned int *)(CNC1800L_GPIO_BASE);

int
gpio_hw_set_direct(int gpio_id, int dir)
{	
	unsigned int flags;
	if((gpio_id < 32)&&(gpio_id >= 0))
	{
		flags = (1 << gpio_id);
		switch (dir) {
			case GPIO_READ:
				*(unsigned int *)(gpio_base+REG_GPIO_SWPORTA_DDR) =(*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_SWPORTA_DDR))|(flags);
				break;
			case GPIO_WRITE:
				*(unsigned int *)(gpio_base+REG_GPIO_SWPORTA_DDR) =(*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_SWPORTA_DDR))&(~flags);
				break;
			default:
				return -1;
		}
	}
	else if((gpio_id >= 32)&&(gpio_id < 41))
	{
		gpio_id = gpio_id - 32;
		flags = (1 << gpio_id);
		switch (dir) {
			case GPIO_READ:
				*(unsigned int *)(gpio_base+REG_GPIO_SWPORTB_DDR) =(*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_SWPORTB_DDR))|(flags);
				break;
			case GPIO_WRITE:
				*(unsigned int *)(gpio_base+REG_GPIO_SWPORTB_DDR) =(*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_SWPORTB_DDR))&(~flags);
				break;
			default:
				return -1;
		}
	}

	return 0;
}

unsigned short
gpio_hw_read(int gpio_id)
{
	gpio_hw_set_direct(gpio_id, GPIO_READ);
	if((gpio_id < 32)&&(gpio_id >= 0))
		return((*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_EXT_PORTA))>>gpio_id)& 0x1;
	else if((gpio_id >= 32)&&(gpio_id < 41))
	{
		gpio_id = gpio_id - 32;
		return((*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_EXT_PORTB))>>gpio_id)& 0x1;
	}
	
	return 0;
}

int
gpio_hw_write(int gpio_id, unsigned short data)
{
	unsigned int flags ;
	gpio_hw_set_direct(gpio_id, GPIO_WRITE);

	if((gpio_id < 32)&&(gpio_id >= 0))
	{
		flags = (1<<gpio_id);
		if (!data) {
			*(unsigned int *)(gpio_base+REG_GPIO_SWPORTA_DR) =(*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_SWPORTA_DR))&(~flags);
		} else {
			*(unsigned int *)(gpio_base+REG_GPIO_SWPORTA_DR) =(*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_SWPORTA_DR))|(flags);
		}
	}
	else if((gpio_id >= 32)&&(gpio_id < 41))
	{
		gpio_id = gpio_id - 32;
		flags = (1<<gpio_id);
		if (!data) {
		*(unsigned int *)(gpio_base+REG_GPIO_SWPORTB_DR) =(*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_SWPORTB_DR))&(~flags);
			//gpio_base[REG_GPIO_SWPORTB_DR ] &= ~flags;
		} else {
			//printf("\n befs  *(unsigned int *)(gpio_base+REG_GPIO_SWPORTB_DR) = %0x\n",*(unsigned int *)(gpio_base+REG_GPIO_SWPORTB_DR));
			*(unsigned int *)(gpio_base+REG_GPIO_SWPORTB_DR) =(*(unsigned int *)(CNC1800L_GPIO_BASE+REG_GPIO_SWPORTB_DR))|(flags);
		}
	}
	return 0;
}

