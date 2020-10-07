// SPDX-License-Identifier: GPL-2.0+
/*
 * (c) 2007 Sascha Hauer <s.hauer@pengutronix.de>
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <watchdog.h>
#include <asm/arch/s3c2440.h>
#include <asm/arch/s3c24xx.h>
#include <serial.h>
#include <linux/compiler.h>
#include <dm/platform_data/serial_s3c24xx_dm.h>
#include <serial_s3c24xx.h>


/* UART Control Register Bit Fields.*/

//UFCON
#define UFCON_TXFIFO_TRIG_LEVEL0	(0 << 6)
#define UFCON_TXFIFO_TRIG_LEVEL16	(1 << 6)
#define UFCON_TXFIFO_TRIG_LEVEL32	(2 << 6)
#define UFCON_TXFIFO_TRIG_LEVEL48	(3 << 6)
#define UFCON_RXFIFO_TRIG_LEVEL1	(0 << 4)
#define UFCON_RXFIFO_TRIG_LEVEL8	(1 << 4)
#define UFCON_RXFIFO_TRIG_LEVEL16	(2 << 4)
#define UFCON_RXFIFO_TRIG_LEVEL32	(3 << 4)
#define UFCON_TX_FIFO_RESET			(1 << 2)
#define UFCON_RX_FIFO_RESET			(1 << 1)
#define UFCON_FIFO_ENABLE			(1 << 0)

//UMCON
#define UMCON_AFC					(1 << 4)
#define UMCON_REQUEST_TRANSFER		(1 << 0)
			
//ULCON
#define ULCON_INFRARED_MODE			(1 << 6)

#define ULCON_PARITY_NONE			(1 << 3)
#define ULCON_PARITY_ODD			(4 << 3)
#define ULCON_PARITY_EVEN			(5 << 3)
#define ULCON_PARITY_FIX1			(6 << 3)
#define ULCON_PARITY_FIX0			(7 << 3)

#define ULCON_STOP_BIT1				(0 << 2)
#define ULCON_STOP_BIT2				(1 << 2)

#define ULCON_BYTE_BIT5				(0 << 0)
#define ULCON_BYTE_BIT6				(1 << 0)
#define ULCON_BYTE_BIT7				(2 << 0)
#define ULCON_BYTE_BIT8				(3 << 0)

//UCON
#define UCON_FCLK_DIV_SHIFT			(12)

#define UCON_CLOCK_FROM_PCLK		(0 << 10)
#define UCON_CLOCK_FROM_PCLK1		(1 << 10)
#define UCON_CLOCK_FROM_UEXTCLK		(2 << 10)
#define UCON_CLOCK_FROM_FCLK		(3 << 10)

#define UCON_TX_INT_PLUSE			(0 << 9)
#define UCON_TX_INT_LEVEL			(1 << 9)

#define UCON_RX_INT_PLUSE			(0 << 8)
#define UCON_RX_INT_LEVEL			(1 << 8)

#define UCON_RX_INT_TIMEOUT_ENABLE	(1 << 7)
#define UCON_RX_INT_FAULT_ENABLE	(1 << 6)

#define UCON_LOOP_MODE				(1 << 5)
#define UCON_BREAK_SIGNAL			(1 << 4)

#define UCON_TX_MODE_NONE			(0 << 2)
#define UCON_TX_MODE_INT			(1 << 2)
#define UCON_TX_MODE_DMA0			(2 << 2)
#define UCON_TX_MODE_DMA1			(3 << 2)

#define UCON_RX_MODE_NONE			(0 << 0)
#define UCON_RX_MODE_INT			(1 << 0)
#define UCON_RX_MODE_DMA0			(2 << 0)
#define UCON_RX_MODE_DMA1			(3 << 0)

//UTRSTAT
#define UTRSTAT_TX_COMPLITE			(1 << 2)
#define UTRSTAT_TX_BUF_EMPTY		(1 << 1)
#define UTRSTAT_RX_BUF_READY		(1 << 0)


//UFSTAT
#define UFSTAT_TX_FIFO_FULL			(1 << 14)
#define UFSTAT_TX_FIFO_CNT_MASK		(0x3f << 8)
#define UFSTAT_RX_FIFO_FULL			(1 << 6)
#define UFSTAT_RX_FIFO_CNT_MASK		(0x3f << 0)


#define URXD_RX_DATA	(0xFF)

/* UART Control Register Bit Fields.*/

DECLARE_GLOBAL_DATA_PTR;

void _s3c24xx_serial_init(S3C24X0_UART *base, int use_dte)
{
	int i;

	/* Normal,No parity,1 stop,8 bit */
	writel(ULCON_BYTE_BIT8, &base->ULCON);


	/* FIFO enable, Tx/Rx FIFO clear */
	writel(UFCON_TXFIFO_TRIG_LEVEL0 | UFCON_RXFIFO_TRIG_LEVEL1 | 
		UFCON_TX_FIFO_RESET | UFCON_RX_FIFO_RESET | UFCON_FIFO_ENABLE, &base->UFCON);

	writel(0, &base->UMCON);
	
	/*
	 * tx=level,rx=edge,disable timeout int.,enable rx error int.,
	 * normal,interrupt or polling
	 */
	 writel(UCON_TX_INT_LEVEL | UCON_RX_INT_FAULT_ENABLE | 
	 	UCON_CLOCK_FROM_PCLK | UCON_TX_MODE_INT | UCON_RX_MODE_INT, &base->UCON);


#ifdef CONFIG_HWFLOW
//	uart->UMCON = 0x1; /* RTS up */
#endif
	for (i = 0; i < 100; i++);
}

void _s3c24xx_serial_setbrg(S3C24X0_UART *base, unsigned long clk,
			       unsigned long baudrate, bool use_dte)
{
	u32 reg;

	/* value is calculated so : (int)(PCLK/16./baudrate) -1 */
	reg = clk / (16 * baudrate) - 1;

	writel(reg, &base->UBRDIV);
}

unsigned long s3c24xx_get_uartclk(void)
{
	unsigned long clk;
	
	clk = get_pclk();

	return clk;
}



#if !CONFIG_IS_ENABLED(DM_SERIAL)


void s3c24xx_serial_setbrg(void)
{
	unsigned long clk = s3c24xx_get_uartclk();

	if (!gd->baudrate)
		gd->baudrate = CONFIG_BAUDRATE;

//	_s3c24xx_serial_setbrg(s3c24xx_base, clk, gd->baudrate, false);
	
	_s3c24xx_serial_setbrg(s3c24xx_base, clk, 115200, false);
}

int s3c24xx_serial_getc(void)
{
	while (!(readl(&s3c24xx_base->UTRSTAT) & UTRSTAT_RX_BUF_READY))
		WATCHDOG_RESET();
	return (readl(&s3c24xx_base->URXH) & URXD_RX_DATA); /* mask out status from upper word */
}

void s3c24xx_serial_putc(const char ch)
{
	/* If \n, also do \r */
	if (ch == '\n') {
		writeb('\r', &s3c24xx_base->UTXH);
		/* wait for transmitter to be ready */
		while (!(readl(&s3c24xx_base->UTRSTAT) & UTRSTAT_TX_COMPLITE))
			WATCHDOG_RESET();
	}

	writeb(ch, &s3c24xx_base->UTXH);

	/* wait for transmitter to be ready */
	while (!(readl(&s3c24xx_base->UTRSTAT) & UTRSTAT_TX_COMPLITE))
		WATCHDOG_RESET();
}

/* Test whether a character is in the RX buffer */
int s3c24xx_serial_tstc(void)
{
	/* If receive fifo is empty, return false */
	if (readl(&s3c24xx_base->UTRSTAT) & UTRSTAT_RX_BUF_READY)
		return 1;
	return 0;
}

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
int s3c24xx_serial_init(void)
{
	_s3c24xx_serial_init(s3c24xx_base, false);

	serial_setbrg();

	return 0;
}


static struct serial_device s3c24xx_serial_drv = {
	.name	= "s3c24xx_serial",
	.start	= s3c24xx_serial_init,
	.stop	= NULL,
	.setbrg	= s3c24xx_serial_setbrg,
	.putc	= s3c24xx_serial_putc,
	.puts	= default_serial_puts,
	.getc	= s3c24xx_serial_getc,
	.tstc	= s3c24xx_serial_tstc,
};

void s3c24xx_serial_initialize(void)
{
	serial_register(&s3c24xx_serial_drv);
}

__weak struct serial_device *default_serial_console(void)
{
	return &s3c24xx_serial_drv;
}

#ifdef CONFIG_SPL_BUILD
int s3c24xx_serial_init_spl(void)
{
	_s3c24xx_serial_init(s3c24xx_base, false);
	s3c24xx_serial_setbrg();
	return 0;
}	
#endif


#endif

#if CONFIG_IS_ENABLED(DM_SERIAL)

int s3c24xx_serial_setbrg(struct udevice *dev, int baudrate)
{
	struct s3c24xx_serial_platdata *plat = dev->platdata;
	struct (S3C24X0_UART *) uart = plat->base;
	u32 clk = s3c24xx_get_uartclk();

	_s3c24xx_serial_setbrg(uart, clk, baudrate, plat->use_dte);

	return 0;
}

int s3c24xx_serial_probe(struct udevice *dev)
{
	struct s3c24xx_serial_platdata *plat = dev->platdata;

	_s3c24xx_serial_init(plat->base, plat->use_dte);

	return 0;
}

int s3c24xx_serial_getc(struct udevice *dev)
{
	struct s3c24xx_serial_platdata *plat = dev->platdata;
	S3C24X0_UART * uart = plat->base;

	if (!(readl(&uart->UTRSTAT) & UTRSTAT_RX_BUF_READY))
		return -EAGAIN;
	return (readl(&uart->URXH) & URXD_RX_DATA); /* mask out status from upper word */
}

int s3c24xx_serial_putc(struct udevice *dev, const char ch)
{
	struct s3c24xx_serial_platdata *plat = dev->platdata;
	S3C24X0_UART *uart = plat->reg;

	/* If \n, also do \r */
	if (ch == '\n') {
		writeb('\r', &uart->UTXH);
		/* wait for transmitter to be ready */
		while (!(readl(&uart->UTRSTAT) & UTRSTAT_TX_COMPLITE))
			WATCHDOG_RESET();
	}

	writeb(ch, &uart->UTXH);
	/* wait for transmitter to be ready */
	while (!(readl(&uart->UTRSTAT) & UTRSTAT_TX_COMPLITE))
		WATCHDOG_RESET();

	return 0;
}

int s3c24xx_serial_pending(struct udevice *dev, bool input)
{
	struct s3c24xx_serial_platdata *plat = dev->platdata;
	S3C24X0_UART *uart = plat->reg;
	uint32_t utrstat = readl(&uart->UTRSTAT);
	uint32_t ufstat = readl(&uart->UFSTAT);
	
	if (input)
		return utrstat & UTRSTAT_RX_BUF_READY ? 1 : 0;
	else
		return ufstat & UFSTAT_TX_FIFO_FULL ? 1 : 0;
}

static const struct dm_serial_ops s3c24xx_serial_ops = {
	.putc = s3c24xx_serial_putc,
	.pending = s3c24xx_serial_pending,
	.getc = s3c24xx_serial_getc,
	.setbrg = s3c24xx_serial_setbrg,
};

#if CONFIG_IS_ENABLED(OF_CONTROL)
static int s3c24xx_serial_ofdata_to_platdata(struct udevice *dev)
{
	struct s3c24xx_serial_platdata *plat = dev->platdata;
	fdt_addr_t addr;

	addr = devfdt_get_addr(dev);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	plat->reg = (struct S3C24X0_UART *)addr;

	plat->use_dte = fdtdec_get_bool(gd->fdt_blob, dev_of_offset(dev),
					"samsung,dte-mode");
	return 0;
}

static const struct udevice_id s3c24xx_serial_ids[] = {
	{ .compatible = "samsung,s3c2440-uart" },
	{ .compatible = "samsung,s3c2430-uart" },
	{ .compatible = "samsung,s3c2410-uart" },
	{ }
};
#endif

U_BOOT_DRIVER(serial_s3c24xx) = {
	.name	= "serial_s3c24xx",
	.id	= UCLASS_SERIAL,
#if CONFIG_IS_ENABLED(OF_CONTROL)
	.of_match = s3c24xx_serial_ids,
	.ofdata_to_platdata = s3c24xx_serial_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct s3c24xx_serial_platdata),
#endif
	.probe = s3c24xx_serial_probe,
	.ops	= &s3c24xx_serial_ops,
#if !CONFIG_IS_ENABLED(OF_CONTROL)
	.flags = DM_FLAG_PRE_RELOC,
#endif
};
#endif

#ifdef CONFIG_DEBUG_UART_S3C24XX
#include <debug_uart.h>

static inline void _debug_uart_init(void)
{
	S3C24X0_UART *base = (struct S3C24X0_UART *)CONFIG_DEBUG_UART_BASE;

	_s3c24xx_serial_init(base, false);
	_s3c24xx_serial_setbrg(base, CONFIG_DEBUG_UART_CLOCK,
			   CONFIG_BAUDRATE, false);
}

static inline void _debug_uart_putc(int ch)
{
	S3C24X0_UART *base = (struct S3C24X0_UART *)CONFIG_DEBUG_UART_BASE;

	writel(ch, &uart->UTXH);
	
	/* wait for transmitter to be ready */
	while (!(readl(&uart->UTRSTAT) & UTRSTAT_TX_COMPLITE))
		WATCHDOG_RESET();
}

DEBUG_UART_FUNCS

#endif
