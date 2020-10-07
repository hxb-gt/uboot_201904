/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Low-level initialization for EP93xx
 *
 * Copyright (C) 2009 Matthias Kaehlcke <matthias@kaehlcke.net>
 * Copyright (C) 2013
 * Sergey Kostanabev <sergey.kostanbaev <at> fairwaves.ru>
 *
 * Copyright (C) 2006 Dominic Rath <Dominic.Rath@gmx.de>
 * Copyright (C) 2006 Cirrus Logic Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 */

#include <config.h>
/* #include <asm/arch-ep93xx/ep93xx.h> */


#define BWSCON	0x48000000

/* BWSCON */
#define DW8		 		(0x0)
#define DW16		 	(0x1)
#define DW32		 	(0x2)
#define WAIT		 	(0x1<<2)
#define UBLB		 	(0x1<<3)

#define B1_BWSCON	  	(DW16)
#define B2_BWSCON	  	(DW16)
#define B3_BWSCON	  	(DW16 + WAIT + UBLB)
#define B4_BWSCON	  	(DW16 + WAIT)
#define B5_BWSCON	  	(DW8)
#define B6_BWSCON	  	(DW32)
#define B7_BWSCON	  	(DW32)

/* BANK0CON */
#define B0_Tacs		 	0x0	/*  0clk */
#define B0_Tcos		 	0x0	/*  0clk */
#define B0_Tacc		 	0x7	/* 14clk */
#define B0_Tcoh		 	0x0	/*  0clk */
#define B0_Tah		 	0x0	/*  0clk */
#define B0_Tacp		 	0x0
#define B0_PMC		 	0x0	/* normal */

/* BANK1CON */
#define B1_Tacs		 	0x0	/*  0clk */
#define B1_Tcos		 	0x0	/*  0clk */
#define B1_Tacc		 	0x7	/* 14clk */
#define B1_Tcoh		 	0x0	/*  0clk */
#define B1_Tah		 	0x0	/*  0clk */
#define B1_Tacp		 	0x0
#define B1_PMC		 	0x0

#define B2_Tacs		 	0x0
#define B2_Tcos		 	0x0
#define B2_Tacc		 	0x7
#define B2_Tcoh		 	0x0
#define B2_Tah		 	0x0
#define B2_Tacp		 	0x0
#define B2_PMC		 	0x0

#define B3_Tacs		 	0x0	/*  0clk */
#define B3_Tcos		 	0x3	/*  4clk */
#define B3_Tacc		 	0x7	/* 14clk */
#define B3_Tcoh		 	0x1	/*  1clk */
#define B3_Tah		 	0x0	/*  0clk */
#define B3_Tacp		 	0x3     /*  6clk */
#define B3_PMC		 	0x0	/* normal */

#if 0
#define B4_Tacs		 	0x0	/*  0clk */
#define B4_Tcos		 	0x1	/*  0clk */
#define B4_Tacc		 	0x7	/* 14clk */
#define B4_Tcoh		 	0x1	/*  0clk */
#define B4_Tah		 	0x1	/*  0clk */
#define B4_Tacp		 	0x1
#define B4_PMC		 	0x0	/* normal */
#endif

#define B4_Tacs		 	0x0	/*  0clk */
#define B4_Tcos		 	0x1	/*  0clk */
#define B4_Tacc		 	0x3	/*  4clk */
#define B4_Tcoh		 	0x0	/*  0clk */
#define B4_Tah		 	0x0	/*  0clk */
#define B4_Tacp		 	0x0
#define B4_PMC		 	0x0	/* normal */

#define B5_Tacs		 	0x0	/*  0clk */
#define B5_Tcos		 	0x0	/*  0clk */
#define B5_Tacc		 	0x7	/* 14clk */
#define B5_Tcoh		 	0x0	/*  0clk */
#define B5_Tah		 	0x0	/*  0clk */
#define B5_Tacp		 	0x0
#define B5_PMC		 	0x0	/* normal */

#define B6_MT		 	0x3	/* SDRAM */
/*#define B6_Trcd	 	 	0x1	*//* 3clk */	/*HJ_del	100MHz */
/*#define B6_SCAN		0x1	*//* 9bit */	/*HJ_del 64MB SDRAM */
/*#define B6_Trcd	 	 	0x0	*//* 2clk */	/*HJ_del 133MHz */
/*#define B6_SCAN		0x2	*//* 10bit */	/*HJ_del 128MB SDRAM */
#if CONFIG_133MHZ_SDRAM
#define B6_Trcd	 	 	0x0
#else
#define B6_Trcd			0x1
#endif
#if CONFIG_128MB_SDRAM
#define B6_SCAN		 	0x2
#else
#define B6_SCAN			0x1
#endif

#define B7_MT		 	0x3	/* SDRAM */
/*#define B7_Trcd		 	0x1	*//* 3clk */	/*HJ_del	100MHz */
/*#define B7_SCAN		0x1	*//* 9bit */	/*HJ_del 64MB SDRAM */
/*#define B7_Trcd	 	 	0x0	*//* 2clk */	/*HJ_add 133MHz */
/*#define B7_SCAN		0x2	*//* 10bit */	/*HJ_del 128MB SDRAM */
#if CONFIG_133MHZ_SDRAM
#define B7_Trcd	 	 	0x0
#else
#define B7_Trcd			0x1
#endif
#if CONFIG_128MB_SDRAM
#define B7_SCAN		 	0x2
#else
#define B7_SCAN			0x1
#endif

/* REFRESH parameter */
#define REFEN		 	0x1	/* Refresh enable */
#define TREFMD		 	0x0	/* CBR(CAS before RAS)/Auto refresh */
/*#define Trp		 	0x0	*//* 2clk */						/* HJ_del 100MHz */
/*#define Trc		    	0x3	*//* 7clk */						/* HJ_del 100MHz */
/*#define Tchr		 	0x2	*//* 3clk */						/* HJ_del 100MHz */
/*#define REFCNT		 	0x4f4	*//* period=7.8125us, HCLK=100Mhz, (2048+1-7.8125*100) */	/* HJ_del 100MHz */
/*#define Trp		 	0x1	*//* 3clk */						/* HJ_del 133MHz */
/*#define Tsrc		    	0x3	*//* 7clk */						/* HJ_del 133MHz */
/*#define REFCNT		 	0x3f4	*//* period=7.8125us, HCLK=133Mhz, (2048+1-7.8125*133) */	/* HJ_del 133MHz */
#if CONFIG_133MHZ_SDRAM
#define Trp		 	0x1
#define Tsrc		    	0x3
#define REFCNT		 	0x3f4
#else
#define Trp		 	0x0
#define Trc		    	0x3
#define Tchr		 	0x2
#define REFCNT		 	0x4f4
#endif
/**************************************/


	.ltorg
/* the literal pools origin */

SMRDATA:
    .word (0+(B1_BWSCON<<4)+(B2_BWSCON<<8)+(B3_BWSCON<<12)+(B4_BWSCON<<16)+(B5_BWSCON<<20)+(B6_BWSCON<<24)+(B7_BWSCON<<28))
    .word ((B0_Tacs<<13)+(B0_Tcos<<11)+(B0_Tacc<<8)+(B0_Tcoh<<6)+(B0_Tah<<4)+(B0_Tacp<<2)+(B0_PMC))
    .word ((B1_Tacs<<13)+(B1_Tcos<<11)+(B1_Tacc<<8)+(B1_Tcoh<<6)+(B1_Tah<<4)+(B1_Tacp<<2)+(B1_PMC))
    .word ((B2_Tacs<<13)+(B2_Tcos<<11)+(B2_Tacc<<8)+(B2_Tcoh<<6)+(B2_Tah<<4)+(B2_Tacp<<2)+(B2_PMC))
    .word ((B3_Tacs<<13)+(B3_Tcos<<11)+(B3_Tacc<<8)+(B3_Tcoh<<6)+(B3_Tah<<4)+(B3_Tacp<<2)+(B3_PMC))
    .word ((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC))
    .word ((B5_Tacs<<13)+(B5_Tcos<<11)+(B5_Tacc<<8)+(B5_Tcoh<<6)+(B5_Tah<<4)+(B5_Tacp<<2)+(B5_PMC))
    .word ((B6_MT<<15)+(B6_Trcd<<2)+(B6_SCAN))
    .word ((B7_MT<<15)+(B7_Trcd<<2)+(B7_SCAN))
/*    .word ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Trc<<18)+(Tchr<<16)+REFCNT)	*/	/* HJ_del 100MHz */
/*    .word 0x32		*/							/* HJ_del 100MHz */
/*    .word 0x30		*/							/* HJ_del 100MHz */	
/*    .word 0x30		*/							/* HJ_del 100MHz */
/*    .word ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Tsrc<<18)+REFCNT)	*/		/* HJ_del 133MHz */
/*    .word 0xB2		*/							/* HJ_del 133MHz */
/*    .word 0x20		*/							/* HJ_del 133MHz */	
/*    .word 0x20		*/							/* HJ_del 133MHz */
#if CONFIG_133MHZ_SDRAM
    .word ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Tsrc<<18)+REFCNT)
    .word 0xB2
    .word 0xB0
    .word 0xB0
#else
    .word ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Trc<<18)+(Tchr<<16)+REFCNT)
    .word 0x32
    .word 0x30
    .word 0x30
#endif

/*
/* Configure the SDRAM based on the supplied settings.
 *
 * Input:	r0 - SDRAM DEVCFG register
 *		r2 - configuration for SDRAM chips
 * Output:	none
 * Modifies:	r3, r4
 */
s3c24xx_sdram_config:
	/* memory control configuration */
	/* make r0 relative the current location so that it */
	/* reads SMRDATA out of FLASH rather than memory ! */
	adr     r0, SMRDATA
	ldr		r1, =BWSCON	/* Bus Width Status Controller */
	add     r2, r0, #13*4
0:
	ldr     r3, [r0], #4
	str     r3, [r1], #4
	cmp     r2, r0
	bne     0b

	/* everything is fine now */
	mov	pc, lr

/*
 * Test to see if the SDRAM has been configured in a usable mode.
 *
 * Input:	r0 - Test address of SDRAM
 * Output:	r0 - 0 -- Test OK, -1 -- Failed
 * Modifies:	r0-r5
 */
s3cxx_sdram_test:
	/* Load the test patterns to be written to SDRAM. */
	ldr	r1, =0xf00dface
	ldr	r2, =0xdeadbeef
	ldr	r3, =0x08675309
	ldr	r4, =0xdeafc0ed

	/* Store the test patterns to SDRAM. */
	stmia	r0, {r1-r4}

	/* Load the test patterns from SDRAM one at a time and compare them
	 * to the actual pattern.
	 */
	ldr	r5, [r0]
	cmp	r5, r1
	ldreq	r5, [r0, #0x0004]
	cmpeq	r5, r2
	ldreq	r5, [r0, #0x0008]
	cmpeq	r5, r3
	ldreq	r5, [r0, #0x000c]
	cmpeq	r5, r4

	/* Return -1 if a mismatch was encountered, 0 otherwise. */
	mvnne	r0, #0xffffffff
	moveq	r0, #0x00000000

	/* Return to the caller. */
	mov	pc, lr

/*
 * Determine the size of the SDRAM. Use data=address for the scan.
 *
 * Input:	r0 - Start SDRAM address
 * Return:	r0 - Single block size
 *		r1 - Valid block mask
 *		r2 - Total block count
 * Modifies:	r0-r5
 */
ep93xx_sdram_size:
	/* Store zero at offset zero. */
	str	r0, [r0]

	/* Start checking for an alias at 1MB into SDRAM. */
	ldr	r1, =0x00100000

	/* Store the offset at the current offset. */
check_block_size:
	str	r1, [r0, r1]

	/* Read back from zero. */
	ldr	r2, [r0]

	/* Stop searching of an alias was found. */
	cmp	r1, r2
	beq	found_block_size

	/* Advance to the next power of two boundary. */
	mov	r1, r1, lsl #1

	/* Loop back if the size has not reached 256MB. */
	cmp	r1, #0x10000000
	bne	check_block_size

	/* A full 256MB of memory was found, so return it now. */
	ldr	r0, =0x10000000
	ldr	r1, =0x00000000
	ldr	r2, =0x00000001
	mov	pc, lr

	/* An alias was found. See if the first block is 128MB in size. */
found_block_size:
	cmp	r1, #0x08000000

	/* The first block is 128MB, so there is no further memory. Return it
	 * now.
	 */
	ldreq	r0, =0x08000000
	ldreq	r1, =0x00000000
	ldreq	r2, =0x00000001
	moveq	pc, lr

	/* Save the block size, set the block address bits to zero, and
	 * initialize the block count to one.
	 */
	mov	r3, r1
	ldr	r4, =0x00000000
	ldr	r5, =0x00000001

	/* Look for additional blocks of memory by searching for non-aliases. */
find_blocks:
	/* Store zero back to address zero. It may be overwritten. */
	str	r0, [r0]

	/* Advance to the next power of two boundary. */
	mov	r1, r1, lsl #1

	/* Store the offset at the current offset. */
	str	r1, [r0, r1]

	/* Read back from zero. */
	ldr	r2, [r0]

	/* See if a non-alias was found. */
	cmp	r1, r2

	/* If a non-alias was found, then or in the block address bit and
	 * multiply the block count by two (since there are two unique
	 * blocks, one with this bit zero and one with it one).
	 */
	orrne	r4, r4, r1
	movne	r5, r5, lsl #1

	/* Continue searching if there are more address bits to check. */
	cmp	r1, #0x08000000
	bne	find_blocks

	/* Return the block size, address mask, and count. */
	mov	r0, r3
	mov	r1, r4
	mov	r2, r5

	/* Return to the caller. */
	mov	pc, lr
	
light_led0_on:
	mov	pc, lr
	
light_led0_off:
	mov	pc, lr

light_led1_on:
	mov	pc, lr

light_led1_off:
	mov	pc, lr

light_led2_on:
	mov	pc, lr

light_led2_off:
	mov	pc, lr
	
.globl lowlevel_init
lowlevel_init:

	mov	r6, lr

	/* init gpio */
	bl s3c24xx_io_init	
	
	bl light_led0_on

	/* Make sure caches are off and invalidated. */
	ldr	r0, =0x00000000
	mcr	p15, 0, r0, c1, c0, 0
	nop
	nop
	nop
	nop
	nop

	/*  system frequence init  */
	bl clock_init
	
	bl light_led1_on

#ifdef CONFIG_SPL_BUILD
	bl s3c24xx_sdram_config
#endif

	bl light_led2_on

#if 0
	/* Turn off the green LED and turn on the red LED. If the red LED
	 * is left on for too long, the external reset circuit described
	 * by application note AN258 will cause the system to reset.
	 */
	ldr	r1, =EP93XX_LED_DATA
	ldr	r0, [r1]
	bic	r0, r0, #EP93XX_LED_GREEN_ON
	orr	r0, r0, #EP93XX_LED_RED_ON
	str	r0, [r1]
#endif

	mov	lr, r6
	mov	pc, lr
