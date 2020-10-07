// SPDX-License-Identifier: GPL-2.0+
/*
 * Cirrus Logic EP93xx CPU-specific support.
 *
 * Copyright (C) 2009 Matthias Kaehlcke <matthias@kaehlcke.net>
 *
 * Copyright (C) 2004, 2005
 * Cory T. Tusar, Videon Central, Inc., <ctusar@videon-central.com>
 */

#include <common.h>
#include <asm/arch/s3c2440.h>
#include <asm/arch/s3c24xx.h>
#include <asm/io.h>

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	printf("CPU: s3c2440, SPEED: %dMhz\n", (int)(get_fclk() / 1000000));
	return 0;
}
#endif /* CONFIG_DISPLAY_CPUINFO */


/* We reset the CPU by generating a 1-->0 transition on DeviceCfg bit 31. */
void reset_cpu(ulong addr)
{
	S3C24X0_WATCHDOG *watdog = (S3C24X0_WATCHDOG *)S3C24X0_WATCHDOG_BASE;

	watdog->WTCON = 0;
	watdog->WTCNT = 0x80;
	watdog->WTDAT = 0x80;
	watdog->WTCON = S3C2410_WTCON_ENABLE|S3C2410_WTCON_DIV16|S3C2410_WTCON_RSTEN |
		     S3C2410_WTCON_PRESCALE(0x20);

	/* Dying... */
	while (1)
		; /* noop */
}
