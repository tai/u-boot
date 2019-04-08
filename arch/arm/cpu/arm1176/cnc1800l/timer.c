/*
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#define TIMER_CLOCK             (378000000 / 8)
#define COUNT_TO_USEC(x)        ((x) * 4 / 189)
#define USEC_TO_COUNT(x)        ((x) * 189 / 4)
#define TICKS_PER_HZ            (TIMER_CLOCK / CONFIG_SYS_HZ)
#define TICKS_TO_HZ(x)          ((x) / TICKS_PER_HZ)

#define TIMER_LOAD_VAL 0xffffffff

/* macro to read the decrementing 32 bit timer as an increasing count */
#define READ_TIMER()  ((ulong)((ulong)0xffffffff - cnc1800l_apb_read32(CONFIG_SYS_TIMERBASE+4))) //

static ulong timestamp;
static ulong lastdec;

/* ORION only support 8/16bit write to APB bus */
static void cnc1800l_apb_write32(ulong addr, ulong data)
{
    *(volatile ushort *)(addr + 0) = (ushort)(data);
    *(volatile ushort *)(addr + 2) = (ushort)(data >> 16);
}

static ulong cnc1800l_apb_read32(ulong addr)
{

    ulong timeval_high,timeval_high2, timeval;

    timeval_high = *(volatile ushort *)(addr + 2);

    timeval =  *(volatile ushort *)(addr);

    timeval_high2= *((volatile ushort *)(addr + 2));

    if (timeval_high2  != timeval_high) { 
        timeval = (timeval_high2 << 16) | (*(volatile ushort *)(addr));
    } 
    else
        timeval = timeval_high << 16 | timeval;


    return timeval;
}

/* nothing really to do with interrupts, just starts up a counter. */
int timer_init (void)
{
	cnc1800l_apb_write32((CONFIG_SYS_TIMERBASE + 0), TIMER_LOAD_VAL);	/* TimerLoad */
	cnc1800l_apb_write32((CONFIG_SYS_TIMERBASE + 4), TIMER_LOAD_VAL);	/* TimerValue */
	cnc1800l_apb_write32((CONFIG_SYS_TIMERBASE + 8), 0x1);

	/* init the timestamp and lastdec value */
	reset_timer_masked();

	return 0;
}

/*
 * timer without interrupts
 */
void reset_timer (void)
{
	reset_timer_masked ();
}

#if 0
ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}
#else
ulong get_timer (ulong base)
{
    return  TICKS_TO_HZ(READ_TIMER()) - base;
}
#endif

ulong get_timer_u(ulong base)
{
	return COUNT_TO_USEC(READ_TIMER()- base);
}

void set_timer (ulong t)
{
	timestamp = t;
}

#if 0
/* delay x useconds AND perserve advance timstamp value */
void __udelay (unsigned long usec)
{
	ulong tmo, tmp;

    tmo = usec * (CONFIG_SYS_PCLK_FREQ / (1000 * 1000));

	tmp = get_timer (0);		/* get current timestamp */
	if( (tmo + tmp + 1) < tmp )	/* if setting this fordward will roll time stamp */
		reset_timer_masked ();	/* reset "advancing" timestamp to 0, set lastdec value */
	else
		tmo += tmp;		/* else, set advancing stamp wake up time */

	while (get_timer_masked () < tmo)/* loop till event */
		/*NOP*/;
}
#else
void __udelay (unsigned long usec)
{
	ulong ini, end;

	ini = READ_TIMER();
	end = ini + USEC_TO_COUNT(usec);
	while ((signed)(end - READ_TIMER()) > 0)
		;
}
#endif

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER();  /* capure current decrementer value time */
	timestamp = 0;	       /* start "advancing" time stamp from 0 */
}

#if 0
ulong get_timer_masked (void)
{
	ulong now = READ_TIMER();		/* current tick value */

	if (lastdec >= now) {		/* normal mode (non roll) */
		/* normal mode */
		timestamp += lastdec - now; /* move stamp fordward with absoulte diff ticks */
	} else {			/* we have overflow of the count down timer */
		/* nts = ts + ld + (TLV - now)
		 * ts=old stamp, ld=time that passed before passing through -1
		 * (TLV-now) amount of time after passing though -1
		 * nts = new "advancing time stamp"...it could also roll and cause problems.
		 */
		timestamp += lastdec + TIMER_LOAD_VAL - now;
	}
	lastdec = now;

	return timestamp;
}

/* waits specified delay value and resets timestamp */
void udelay_masked (unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

    tmo = usec * (CONFIG_SYS_HZ / (1000 * 1000));

	endtime = get_timer_masked () + tmo;

	do {
		ulong now = get_timer_masked ();
		diff = endtime - now;
	} while (diff >= 0);
}
#endif

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}
 
unsigned long long get_ticks_u(void)
{
	return get_timer_u(0);
}


/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}
