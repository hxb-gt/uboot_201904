/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2014 Google, Inc
 */


#ifndef __SERIAL_S3C24XX_DM__
#define __SERIAL_S3C24XX_DM__

/* Information about a serial port */
struct s3c24xx_serial_platdata {
	struct S3C24X0_UART *base;  /* address of registers in physical memory */
	bool use_dte;
};


#endif /*__SERIAL_S3C24XX__*/
