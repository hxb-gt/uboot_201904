// SPDX-License-Identifier: GPL-2.0+
/*
 * Cirrus Logic EP93xx timer support.
 *
 * Copyright (C) 2009, 2010 Matthias Kaehlcke <matthias@kaehlcke.net>
 *
 * Copyright (C) 2004, 2005
 * Cory T. Tusar, Videon Central, Inc., <ctusar@videon-central.com>
 *
 * Based on the original intr.c Cirrus Logic EP93xx Rev D. interrupt support,
 * author unknown.
 */

#include <common.h>
#include <linux/types.h>
#include <asm/arch/s3c2440.h>
#include <asm/arch/s3c24xx.h>
#include <asm/io.h>
#include <div64.h>


#define TIMER4_PRESCALER	(255)
#define TIMER4_MUX4			(1)
#define TIMER4_ENABLE		(1 << 20)
#define TIMER4_MANNUL_RELOAD	(1 << 21)
#define TIMER4_AUTO_RELOAD	(1 << 22)
#define TIMER4_RELOAD_VALUE	(0xFFFF)
#define TIMER_MAX_VAL		0xFFFF

static struct s3c2440_timer
{
	unsigned long long ticks;
	unsigned long last_read;
} timer;

unsigned long timer_freq;
static inline unsigned long long usecs_to_ticks(unsigned long usecs)
{
	unsigned long long ticks = (unsigned long long)usecs * timer_freq;
	do_div(ticks, 1000 * 1000);

	return ticks;
}

static inline void read_timer(void)
{
	S3C24X0_TIMERS *timer_regs = (S3C24X0_TIMERS *)S3C24X0_TIMER_BASE;
	const unsigned long now = TIMER_MAX_VAL - readl(&timer_regs->TCNTO4);

	if (now >= timer.last_read)
		timer.ticks += now - timer.last_read;
	else
		/* an overflow occurred */
		timer.ticks += TIMER_MAX_VAL - timer.last_read + now;

	timer.last_read = now;
}

/*
 * Get the number of ticks (in CONFIG_SYS_HZ resolution)
 */
unsigned long long get_ticks(void)
{
	unsigned long long sys_ticks;

	read_timer();

	sys_ticks = timer.ticks * CONFIG_SYS_HZ;
	do_div(sys_ticks, timer_freq);

	return sys_ticks;
}

unsigned long get_timer(unsigned long base)
{
	return get_ticks() - base;
}

void __udelay(unsigned long usec)
{
	unsigned long long target;

	read_timer();

	target = timer.ticks + usecs_to_ticks(usec);

	while (timer.ticks < target)
		read_timer();
}

int timer_init(void)
{
	S3C24X0_TIMERS *timer_regs = (S3C24X0_TIMERS *)S3C24X0_TIMER_BASE;

	/* use timer 3 with 508KHz and free running, not enabled now */
	writel(TIMER4_PRESCALER << 8, &timer_regs->TCFG0);

	/* set div */
	writel(TIMER4_MUX4 << 16, &timer_regs->TCFG1);

	/* set initial timer value */
	writel(TIMER4_RELOAD_VALUE, &timer_regs->TCNTB4);
	
	/* mannul update timer */
	writel(TIMER4_MANNUL_RELOAD, &timer_regs->TCON);
	
	/* Enable the timer */
	writel(TIMER4_AUTO_RELOAD | TIMER4_ENABLE, &timer_regs->TCON);
	
	timer_freq = (get_pclk() / (TIMER4_PRESCALER + 1) / (1 << (TIMER4_MUX4 + 1)));

//	printf("pclk:%d ,timer freq :%d \n", get_pclk(), timer_freq);
	
	/* Reset the timer */
	read_timer();
	timer.ticks = 0;

	return 0;
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
unsigned long get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}
