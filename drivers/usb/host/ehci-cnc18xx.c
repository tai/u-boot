/****************************************************************************
  * Copyright (C) 2012 Cavium Inc.
  * All rights reserved
  *
  * [RELEASE HISTORY]                           
  * VERSION  DATE       AUTHOR                  DESCRIPTION
  * 0.1      12-6-4     Guo Yi         		Original
  *****************************************************************************/

#include <common.h>
#include <pci.h>
#include <usb.h>
#include <asm/io.h>

#include "ehci.h"
#include "ehci-core.h"

/*
 * Initialize SOC EHCI Controller
 *
 * 
 *
 */
int ehci_hcd_init(void)
{
	uint32_t val;
	*((volatile unsigned long *)0xb2100218) =0x3;

	/* Hook the memory mapped registers for EHCI-Controller */
	hccr = (struct ehci_hccr *)CONFIG_SYS_USB_EHCI_REGS_BASE;
	hcor = (struct ehci_hcor *)((uint32_t) hccr +
				HC_LENGTH(ehci_readl(&hccr->cr_capbase)));
	return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int ehci_hcd_stop(void)
{
	if (hcor) {
		/* Unhook struct */
		hccr = NULL;
		hcor = NULL;

		/* Stop the USB controller */
		*((volatile unsigned long *)0xb2100218) =0x0;
	}

	return 0;
}


