/*
 * (C) Copyright 2003
 * David Müller ELSOFT AG Switzerland. d.mueller@elsoft.ch
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

/************************************************
 * NAME     : s3c2440.h
 * Version  : 31.3.2003
 *
 * Based on S3C2410X User's manual Rev 1.1
 ************************************************/

#ifndef __S3C2440_H__
#define __S3C2440_H__


#define S3C24X0_UART_CHANNELS   3
#define S3C24X0_SPI_CHANNELS    2

/* S3C2440 only supports 512 Byte HW ECC */
#define S3C2410_ECCSIZE     512
#define S3C2410_ECCBYTES    3

typedef enum {
    S3C24X0_UART0,
    S3C24X0_UART1,
    S3C24X0_UART2
} S3C24X0_UARTS_NR;

/* S3C2410 device base addresses */
#define S3C24X0_MEMCTL_BASE     0x48000000
#define S3C24X0_USB_HOST_BASE       0x49000000
#define S3C24X0_INTERRUPT_BASE      0x4A000000
#define S3C24X0_DMA_BASE        0x4B000000
#define S3C24X0_CLOCK_POWER_BASE    0x4C000000
#define S3C24X0_LCD_BASE        0x4D000000
#define S3C2410_NAND_BASE       0x4E000000

#define S3C24X0_UART0_BASE       0x50000000
#define S3C24X0_UART1_BASE       0x50004000
#define S3C24X0_UART2_BASE       0x50008000

#define S3C24X0_TIMER_BASE      0x51000000
#define S3C24X0_USB_DEVICE_BASE     0x52000140
#define S3C24X0_WATCHDOG_BASE       0x53000000
#define S3C24X0_I2C_BASE        0x54000000
#define S3C24X0_I2S_BASE        0x55000000

#define S3C24X0_GPIOA_BASE       0x56000000
#define S3C24X0_GPIOB_BASE       0x56000010
#define S3C24X0_GPIOC_BASE       0x56000020
#define S3C24X0_GPIOD_BASE       0x56000030
#define S3C24X0_GPIOE_BASE       0x56000040
#define S3C24X0_GPIOF_BASE       0x56000050
#define S3C24X0_GPIOG_BASE       0x56000060
#define S3C24X0_GPIOH_BASE       0x56000070
#define S3C24X0_GPIOJ_BASE       0x560000D0


#define S3C24X0_RTC_BASE        0x57000000
#define S3C2410_ADC_BASE        0x58000000
#define S3C24X0_SPI_BASE        0x59000000
#define S3C2410_SDI_BASE        0x5A000000


#define ISR_EINT0_OFT     0
#define ISR_EINT1_OFT     1
#define ISR_EINT2_OFT     2
#define ISR_EINT3_OFT     3
#define ISR_EINT4_7_OFT   4
#define ISR_EINT8_23_OFT  5
#define ISR_NOTUSED6_OFT  6
#define ISR_BAT_FLT_OFT   7
#define ISR_TICK_OFT      8
#define ISR_WDT_OFT       9
#define ISR_TIMER0_OFT    10
#define ISR_TIMER1_OFT    11
#define ISR_TIMER2_OFT    12
#define ISR_TIMER3_OFT    13
#define ISR_TIMER4_OFT    14
#define ISR_UART2_OFT     15
#define ISR_LCD_OFT       16
#define ISR_DMA0_OFT      17
#define ISR_DMA1_OFT      18
#define ISR_DMA2_OFT      19
#define ISR_DMA3_OFT      20
#define ISR_SDI_OFT       21
#define ISR_SPI0_OFT      22
#define ISR_UART1_OFT     23
#define ISR_NOTUSED24_OFT 24
#define ISR_USBD_OFT      25
#define ISR_USBH_OFT      26
#define ISR_IIC_OFT       27
#define ISR_UART0_OFT     28
#define ISR_SPI1_OFT      29
#define ISR_RTC_OFT       30
#define ISR_ADC_OFT       31

// PENDING BIT
#define BIT_EINT0		(0x1)
#define BIT_EINT1		(0x1<<1)
#define BIT_EINT2		(0x1<<2)
#define BIT_EINT3		(0x1<<3)
#define BIT_EINT4_7		(0x1<<4)
#define BIT_EINT8_23	(0x1<<5)
#define BIT_CAM			(0x1<<6)		// Added for 2440.
#define BIT_BAT_FLT		(0x1<<7)
#define BIT_TICK			(0x1<<8)
#define BIT_WDT_AC97	(0x1<<9)
#define BIT_TIMER0		(0x1<<10)
#define BIT_TIMER1		(0x1<<11)
#define BIT_TIMER2		(0x1<<12)
#define BIT_TIMER3		(0x1<<13)
#define BIT_TIMER4		(0x1<<14)
#define BIT_UART2		(0x1<<15)
#define BIT_LCD			(0x1<<16)
#define BIT_DMA0		(0x1<<17)
#define BIT_DMA1		(0x1<<18)
#define BIT_DMA2		(0x1<<19)
#define BIT_DMA3		(0x1<<20)
#define BIT_SDI			(0x1<<21)
#define BIT_SPI0			(0x1<<22)
#define BIT_UART1		(0x1<<23)
#define BIT_NFCON		(0x1<<24)		// Added for 2440.
#define BIT_USBD		(0x1<<25)
#define BIT_USBH		(0x1<<26)
#define BIT_IIC			(0x1<<27)
#define BIT_UART0		(0x1<<28)
#define BIT_SPI1			(0x1<<29)
#define BIT_RTC			(0x1<<30)
#define BIT_ADC			(0x1<<31)
#define BIT_ALLMSK		(0xffffffff)

#define BIT_SUB_ALLMSK	(0x7fff)
#define BIT_SUB_AC97 	(0x1<<14)
#define BIT_SUB_WDT 	(0x1<<13)
#define BIT_SUB_CAM_S	(0x1<<12)		// Added for 2440.
#define BIT_SUB_CAM_C	(0x1<<11)		// Added for 2440.
#define BIT_SUB_ADC		(0x1<<10)
#define BIT_SUB_TC		(0x1<<9)
#define BIT_SUB_ERR2	(0x1<<8)
#define BIT_SUB_TXD2	(0x1<<7)
#define BIT_SUB_RXD2	(0x1<<6)
#define BIT_SUB_ERR1	(0x1<<5)
#define BIT_SUB_TXD1	(0x1<<4)
#define BIT_SUB_RXD1	(0x1<<3)
#define BIT_SUB_ERR0	(0x1<<2)
#define BIT_SUB_TXD0	(0x1<<1)
#define BIT_SUB_RXD0	(0x1<<0)


/* the watchdog can either generate a reset pulse, or an
 * interrupt.
 */

#define S3C2410_WTCON_RSTEN   (0x01)
#define S3C2410_WTCON_INTEN   (1<<2)
#define S3C2410_WTCON_ENABLE  (1<<5)

#define S3C2410_WTCON_DIV16   (0<<3)
#define S3C2410_WTCON_DIV32   (1<<3)
#define S3C2410_WTCON_DIV64   (2<<3)
#define S3C2410_WTCON_DIV128  (3<<3)

#define S3C2410_WTCON_PRESCALE(x) ((x) << 8)
#define S3C2410_WTCON_PRESCALE_MASK (0xff00)


static inline void delay(int loop)
{
	int i;
	
	while(loop--) {
		i = 1000;
		while(i--);
	}
}

/* return FCLK frequency */
ulong get_fclk(void);

/* return HCLK frequency */
ulong get_hclk(void);

/* return PCLK frequency */
unsigned long get_pclk(void);

/* return UCLK frequency */
ulong get_uclk(void);

#endif /*__S3C2440_H__*/
