// SPDX-License-Identifier: GPL-2.0+
/*
 * Board initialization for EP93xx
 *
 * Copyright (C) 2013
 * Sergey Kostanbaev <sergey.kostanbaev <at> fairwaves.ru>
 *
 * Copyright (C) 2009
 * Matthias Kaehlcke <matthias <at> kaehlcke.net>
 *
 * (C) Copyright 2002 2003
 * Network Audio Technologies, Inc. <www.netaudiotech.com>
 * Adam Bezanson <bezanson <at> netaudiotech.com>
 */

#include <config.h>
#include <common.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/arch/s3c2440.h>
#include <asm/arch/s3c24xx.h>
//#include <dm/platform_data/serial_s3c24xx_dm.h>
#include <serial_s3c24xx.h>


DECLARE_GLOBAL_DATA_PTR;

/*
 * usb_div: 4, nbyp2: 1, pll2_en: 1
 * pll2_x1: 368640000.000000, pll2_x2ip: 15360000.000000,
 * pll2_x2: 384000000.000000, pll2_out: 192000000.000000
 */
#define CLKSET2_VAL	(23 << SYSCON_CLKSET_PLL_X2IPD_SHIFT |	\
			24 << SYSCON_CLKSET_PLL_X2FBD2_SHIFT |	\
			24 << SYSCON_CLKSET_PLL_X1FBD1_SHIFT |	\
			1 << SYSCON_CLKSET_PLL_PS_SHIFT |	\
			SYSCON_CLKSET2_PLL2_EN |		\
			SYSCON_CLKSET2_NBYP2 |			\
			3 << SYSCON_CLKSET2_USB_DIV_SHIFT)

#define SMC_BCR6_VALUE	(2 << SMC_BCR_IDCY_SHIFT | 5 << SMC_BCR_WST1_SHIFT | \
			SMC_BCR_BLE | 2 << SMC_BCR_WST2_SHIFT | \
			1 << SMC_BCR_MW_SHIFT)

/* delay execution before timers are initialized */
static inline void early_udelay(uint32_t usecs)
{
	/* loop takes 4 cycles at 5.0ns (fastest case, running at 200MHz) */
	register uint32_t loops = (usecs * 1000) / 20;

	__asm__ volatile ("1:\n"
			"subs %0, %1, #1\n"
			"bne 1b" : "=r" (loops) : "0" (loops));
}
void s3c24xx_io_init(void)
{
	S3C24X0_GPIO * const gpio = (S3C24X0_GPIO *)S3C24X0_GPIOA_BASE;

	/* set up the I/O ports */
	gpio->GPACON = 0x007FFFFF;
	gpio->GPBCON = 0x00055555;
	gpio->GPBUP = 0x000007FF;
	gpio->GPCCON = 0xAAAAAAAA;
	gpio->GPCUP = 0x0000FFFF;
	gpio->GPDCON = 0xAAAAAAAA;
	gpio->GPDUP = 0x0000FFFF;
	gpio->GPECON = 0xAAAAAAAA;
	gpio->GPEUP = 0x0000FFFF;
	gpio->GPFCON = 0x000055AA;
	gpio->GPFUP = 0x000000FF;
	gpio->GPGCON = 0xFF94FFBA;
	gpio->GPGUP = 0x0000FFEF;
	gpio->GPGDAT = gpio->GPGDAT & ((~(1<<4)) | (1<<4)) ;
	gpio->GPHCON = 0x002AFAAA;
	gpio->GPHUP = 0x000007FF;
	gpio->GPJCON = 0x02aaaaaa;
	gpio->GPJUP = 0x00001fff;

}

void watchdog_init(void)
{
	
	S3C24X0_WATCHDOG * const watchdog = (S3C24X0_WATCHDOG *)S3C24X0_WATCHDOG_BASE;

	watchdog->WTCON = 0x00;
}
int board_init(void)
{
	/*
	 * Setup PLL2, PPL1 has been set during lowlevel init
	 */
//	struct syscon_regs *syscon = (struct syscon_regs *)SYSCON_BASE;
//	writel(CLKSET2_VAL, &syscon->clkset2);

	/*
	 * the user's guide recommends to wait at least 1 ms for PLL2 to
	 * stabilize
	 */
	printf("s3c2400 board init\n");
	
	early_udelay(1000);

	/* Go to Async mode */
	__asm__ volatile ("mrc p15, 0, r0, c1, c0, 0");
	__asm__ volatile ("orr r0, r0, #0xc0000000");
	__asm__ volatile ("mcr p15, 0, r0, c1, c0, 0");

	icache_enable();

#ifdef USE_920T_MMU
	dcache_enable();
#endif

	/* Machine number, as defined in linux/arch/arm/tools/mach-types */
	gd->bd->bi_arch_number = CONFIG_MACH_TYPE;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR;

	/* We have a console */
	gd->have_console = 1;

	enable_interrupts();
	watchdog_init();

//	flash_cfg();

//	green_led_on();
//	red_led_off();
//
	timer_init();

	return 0;
}

int board_early_init_f(void)
{
	return 0;
}


#if 0
static void dram_fill_bank_addr(unsigned dram_addr_mask, unsigned dram_bank_cnt,
				unsigned dram_bank_base[CONFIG_NR_DRAM_BANKS])
{

}

/* called in board_init_f (before relocation) */
static unsigned dram_init_banksize_int(int print)
{
	return 0;
}
#endif

/* called in board_init_f (before relocation) */
int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE, CONFIG_SYS_SDRAM_SIZE);

	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;

	rc = dm9000_initialize(bis);
	printf("embedsky 2440 board_eth_init: %d\n", rc);

	return rc;
}
#endif


#ifdef CONFIG_SPL_BUILD

//#define DEBUG
void nand_boot(void);
int printf(const char *fmt, ...) {return 0;}
void hang(void) {};

#ifdef DEBUG
void putc(char c)
{
//	if (c == '\n')
//		s3c24xx_serial_putc('\r');
	s3c24xx_serial_putc(c);
}

void puts(const char *str)
{
	while (*str)
		putc(*str++);
}
#else
void puts(const char *str) {}

#endif

void board_init_f(ulong dummy)
{
	board_init();
	s3c24xx_serial_init_spl();
	
//	while(1);
	nand_boot();
}

#endif
