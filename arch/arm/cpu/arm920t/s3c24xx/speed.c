// SPDX-License-Identifier: GPL-2.0+
/*
 * Cirrus Logic EP93xx PLL support.
 *
 * Copyright (C) 2009 Matthias Kaehlcke <matthias@kaehlcke.net>
 */

#include <common.h>
#include <asm/mach-types.h>
#include <asm/arch/s3c2440.h>
#include <asm/arch/s3c24xx.h>
#include <asm/io.h>
#include <div64.h>


DECLARE_GLOBAL_DATA_PTR;


#define MPLL 0
#define UPLL 1


/* for s3c2440 */
#define S3C2440_CLKDIVN_PDIVN        (1<<0)
#define S3C2440_CLKDIVN_HDIVN_MASK   (3<<1)
#define S3C2440_CLKDIVN_HDIVN_1      (0<<1)
#define S3C2440_CLKDIVN_HDIVN_2      (1<<1)
#define S3C2440_CLKDIVN_HDIVN_4_8    (2<<1)
#define S3C2440_CLKDIVN_HDIVN_3_6    (3<<1)
#define S3C2440_CLKDIVN_UCLK         (1<<3)

#define S3C2440_CAMDIVN_CAMCLK_MASK  (0xf<<0)
#define S3C2440_CAMDIVN_CAMCLK_SEL   (1<<4)
#define S3C2440_CAMDIVN_HCLK3_HALF   (1<<8)
#define S3C2440_CAMDIVN_HCLK4_HALF   (1<<9)
#define S3C2440_CAMDIVN_DVSEN        (1<<12)


/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static ulong get_pllclk(int pllreg)
{
    S3C24X0_CLOCK_POWER *clk_power = (S3C24X0_CLOCK_POWER *)S3C24X0_CLOCK_POWER_BASE;
    ulong r, m, p, s;

    if (pllreg == MPLL)
    r = clk_power->MPLLCON;
    else if (pllreg == UPLL)
    r = clk_power->UPLLCON;
    else
    hang();

    m = ((r & 0xFF000) >> 12) + 8;
    p = ((r & 0x003F0) >> 4) + 2;
    s = r & 0x3;

    /* support both of S3C2410 and S3C2440, by www.embedsky.net */
 //   if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
 //       return((CONFIG_SYS_CLK_FREQ * m) / (p << s));
 //   else
        return((CONFIG_SYS_CLK_FREQ * m * 2) / (p << s));   /* S3C2440 */
}

/* return FCLK frequency */
ulong get_fclk(void)
{
    return(get_pllclk(MPLL));
}


/* return HCLK frequency */
ulong get_hclk(void)
{
    S3C24X0_CLOCK_POWER *clk_power = (S3C24X0_CLOCK_POWER *)S3C24X0_CLOCK_POWER_BASE;
    unsigned long clkdiv;
    unsigned long camdiv;
    int hdiv = 1;

    /* support both of S3C2410 and S3C2440, by www.embedsky.net */
//    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
//        return((clk_power->CLKDIVN & 0x2) ? get_fclk()/2 : get_fclk());
//    else
    {
        clkdiv = clk_power->CLKDIVN;
        camdiv = clk_power->CAMDIVN;

        /* work out clock scalings */

        switch (clkdiv & S3C2440_CLKDIVN_HDIVN_MASK) {
        case S3C2440_CLKDIVN_HDIVN_1:
            hdiv = 1;
            break;

        case S3C2440_CLKDIVN_HDIVN_2:
            hdiv = 2;
            break;

        case S3C2440_CLKDIVN_HDIVN_4_8:
            hdiv = (camdiv & S3C2440_CAMDIVN_HCLK4_HALF) ? 8 : 4;
            break;

        case S3C2440_CLKDIVN_HDIVN_3_6:
            hdiv = (camdiv & S3C2440_CAMDIVN_HCLK3_HALF) ? 6 : 3;
            break;
        }

        return get_fclk() / hdiv;
    }
}

/* return PCLK frequency */
unsigned long get_pclk(void)
{
    S3C24X0_CLOCK_POWER * clk_power = (S3C24X0_CLOCK_POWER *)S3C24X0_CLOCK_POWER_BASE;
    unsigned long clkdiv;
    unsigned long camdiv;
    int hdiv = 1;

    /* support both of S3C2410 and S3C2440, by www.embedsky.net */
//    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
//        return((clk_power->CLKDIVN & 0x1) ? get_hclk()/2 : get_hclk());
//    else
    {   
        clkdiv = clk_power->CLKDIVN;
        camdiv = clk_power->CAMDIVN;

        /* work out clock scalings */

        switch (clkdiv & S3C2440_CLKDIVN_HDIVN_MASK) {
        case S3C2440_CLKDIVN_HDIVN_1:
            hdiv = 1;
            break;

        case S3C2440_CLKDIVN_HDIVN_2:
            hdiv = 2;
            break;

        case S3C2440_CLKDIVN_HDIVN_4_8:
            hdiv = (camdiv & S3C2440_CAMDIVN_HCLK4_HALF) ? 8 : 4;
            break;

        case S3C2440_CLKDIVN_HDIVN_3_6:
            hdiv = (camdiv & S3C2440_CAMDIVN_HCLK3_HALF) ? 6 : 3;
            break;
        }

        return get_fclk() / hdiv / ((clkdiv & S3C2440_CLKDIVN_PDIVN)? 2:1);
    }        
}

/* return UCLK frequency */
ulong get_uclk(void)
{
    return(get_pllclk(UPLL));
}


/* S3C2440: Mpll = (2*m * Fin) / (p * 2^s), UPLL = (m * Fin) / (p * 2^s)
 * m = M (the value for divider M)+ 8, p = P (the value for divider P) + 2
 */
/* Fin = 12.0000MHz */
#define S3C2440_MPLL_400MHZ	((0x5c<<12)|(0x01<<4)|(0x01))						//HJ 400MHz
#define S3C2440_MPLL_405MHZ	((0x7f<<12)|(0x02<<4)|(0x01))						//HJ 405MHz
#define S3C2440_MPLL_440MHZ	((0x66<<12)|(0x01<<4)|(0x01))						//HJ 440MHz
#define S3C2440_MPLL_480MHZ	((0x98<<12)|(0x02<<4)|(0x01))						//HJ 480MHz
#define S3C2440_MPLL_200MHZ	((0x5c<<12)|(0x01<<4)|(0x02))
#define S3C2440_MPLL_100MHZ	((0x5c<<12)|(0x01<<4)|(0x03))

#define S3C2440_UPLL_48MHZ	((0x38<<12)|(0x02<<4)|(0x02))						//HJ 100MHz

#define S3C2440_CLKDIV		0x05    /* FCLK:HCLK:PCLK = 1:4:8, UCLK = UPLL */		//HJ 100MHz
#define S3C2440_CLKDIV136	0x07    /* FCLK:HCLK:PCLK = 1:3:6, UCLK = UPLL */		//HJ 133MHz
#define S3C2440_CLKDIV188	0x04    /* FCLK:HCLK:PCLK = 1:8:8 */
#define S3C2440_CAMDIVN188	((0<<8)|(1<<9)) /* FCLK:HCLK:PCLK = 1:8:8 */

/* Fin = 16.9344MHz */
#define S3C2440_MPLL_399MHz     		((0x6e<<12)|(0x03<<4)|(0x01))
#define S3C2440_UPLL_48MHZ_Fin16MHz		((60<<12)|(4<<4)|(2))

void clock_init(void)
{
	S3C24X0_CLOCK_POWER *clk_power = (S3C24X0_CLOCK_POWER *)S3C24X0_CLOCK_POWER_BASE;

	/* FCLK:HCLK:PCLK = ?:?:? */
#if CONFIG_133MHZ_SDRAM
	clk_power->CLKDIVN = S3C2440_CLKDIV136;			//HJ 1:3:6
#else
	clk_power->CLKDIVN = S3C2440_CLKDIV;				//HJ 1:4:8
#endif
	/* change to asynchronous bus mod */
	__asm__(    "mrc    p15, 0, r1, c1, c0, 0\n"    /* read ctrl register   */  
                    "orr    r1, r1, #0xc0000000\n"      /* Asynchronous         */  
                    "mcr    p15, 0, r1, c1, c0, 0\n"    /* write ctrl register  */  
                    :::"r1"
                    );

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	clk_power->LOCKTIME = 0xFFFFFFFF;

	/* configure UPLL */
	clk_power->UPLLCON = S3C2440_UPLL_48MHZ;		//fin=12.000MHz
//	clk_power->UPLLCON = S3C2440_UPLL_48MHZ_Fin16MHz;	//fin=16.934MHz

	/* some delay between MPLL and UPLL */
	delay (4000);

	/* configure MPLL */
	clk_power->MPLLCON = S3C2440_MPLL_400MHZ;		//fin=12.000MHz
//	clk_power->MPLLCON = S3C2440_MPLL_405MHZ;				//HJ 405MHz
//	clk_power->MPLLCON = S3C2440_MPLL_440MHZ;				//HJ 440MHz
//	clk_power->MPLLCON = S3C2440_MPLL_480MHZ;				//HJ 480MHz
//	clk_power->MPLLCON = S3C2440_MPLL_399MHz;		//fin=16.934MHz
	/* some delay between MPLL and UPLL */
	delay (8000);
}

