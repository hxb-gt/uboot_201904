/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2014 Google, Inc
 */


#ifndef __SERIAL_S3C24XX__
#define __SERIAL_S3C24XX__

#include <linux/types.h>


#define s3c24xx_base	((S3C24X0_UART *)S3C24X0_UART0_BASE)

#if !CONFIG_IS_ENABLED(DM_SERIAL)

void s3c24xx_serial_setbrg(void);
int s3c24xx_serial_getc(void);
void s3c24xx_serial_putc(const char c);

/* Test whether a character is in the RX buffer */
int s3c24xx_serial_tstc(void);

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
int s3c24xx_serial_init(void);

#ifdef CONFIG_SPL_BUILD
int s3c24xx_serial_init_spl(void);
#endif

#endif

#if CONFIG_IS_ENABLED(DM_SERIAL)

int s3c24xx_serial_setbrg(struct udevice *dev, int baudrate);

int s3c24xx_serial_probe(struct udevice *dev);

int s3c24xx_serial_getc(struct udevice *dev);
int s3c24xx_serial_putc(struct udevice *dev, const char ch);
int s3c24xx_serial_pending(struct udevice *dev, bool input);


#if CONFIG_IS_ENABLED(OF_CONTROL)
int s3c24xx_serial_ofdata_to_platdata(struct udevice *dev);

#endif

#endif

#ifdef CONFIG_DEBUG_UART_S3C24XX
#include <debug_uart.h>

inline void _debug_uart_init(void);

inline void _debug_uart_putc(int ch);

DEBUG_UART_FUNCS

#endif

#endif /*__SERIAL_S3C24XX__*/
