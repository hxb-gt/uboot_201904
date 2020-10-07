// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2007-2008
 * Stelian Pop <stelian@popies.net>
 * Lead Tech Design <www.leadtechdesign.com>
 *
 * (C) Copyright 2006 ATMEL Rousset, Lacressonniere Nicolas
 *
 * Add Programmable Multibit ECC support for various AT91 SoC
 *     (C) Copyright 2012 ATMEL, Hong Xu
 */

#include <common.h>
#include <asm/io.h>

#include <malloc.h>
#include <nand.h>
#include <watchdog.h>
#include <linux/mtd/nand_ecc.h>
#include <asm/arch/s3c2440.h>
#include <asm/arch/s3c24xx.h>

//#define S3C24XX_NAND_DEBUG
#ifdef S3C24XX_NAND_DEBUG
	#define s3c24xx_nand_dbg(fmt, args...)	printf(fmt, ##args)
#else
	#define s3c24xx_nand_dbg(fmt, args...)	
#endif

#define S3C2410_NFREG(x) (x)

#define S3C2410_NFCONF  S3C2410_NFREG(0x00)
#define S3C2410_NFCMD   S3C2410_NFREG(0x04)
#define S3C2410_NFADDR  S3C2410_NFREG(0x08)
#define S3C2410_NFDATA  S3C2410_NFREG(0x0C)
#define S3C2410_NFSTAT  S3C2410_NFREG(0x10)
#define S3C2410_NFECC   S3C2410_NFREG(0x14)

#define S3C2440_NFCONT   S3C2410_NFREG(0x04)
#define S3C2440_NFCMD    S3C2410_NFREG(0x08)
#define S3C2440_NFADDR   S3C2410_NFREG(0x0C)
#define S3C2440_NFDATA   S3C2410_NFREG(0x10)
#define S3C2440_NFECCD0  S3C2410_NFREG(0x14)
#define S3C2440_NFECCD1  S3C2410_NFREG(0x18)
#define S3C2440_NFECCD   S3C2410_NFREG(0x1C)
#define S3C2440_NFSTAT   S3C2410_NFREG(0x20)
#define S3C2440_NFESTAT0 S3C2410_NFREG(0x24)
#define S3C2440_NFESTAT1 S3C2410_NFREG(0x28)
#define S3C2440_NFMECC0  S3C2410_NFREG(0x2C)
#define S3C2440_NFMECC1  S3C2410_NFREG(0x30)
#define S3C2440_NFSECC   S3C2410_NFREG(0x34)
#define S3C2440_NFSBLK   S3C2410_NFREG(0x38)
#define S3C2440_NFEBLK   S3C2410_NFREG(0x3C)

#define S3C2412_NFSBLK		S3C2410_NFREG(0x20)
#define S3C2412_NFEBLK		S3C2410_NFREG(0x24)
#define S3C2412_NFSTAT		S3C2410_NFREG(0x28)
#define S3C2412_NFMECC_ERR0	S3C2410_NFREG(0x2C)
#define S3C2412_NFMECC_ERR1	S3C2410_NFREG(0x30)
#define S3C2412_NFMECC0		S3C2410_NFREG(0x34)
#define S3C2412_NFMECC1		S3C2410_NFREG(0x38)
#define S3C2412_NFSECC		S3C2410_NFREG(0x3C)

#define S3C2410_NFCONF_EN          (1<<15)
#define S3C2410_NFCONF_512BYTE     (1<<14)
#define S3C2410_NFCONF_4STEP       (1<<13)
#define S3C2410_NFCONF_INITECC     (1<<12)
#define S3C2410_NFCONF_nFCE        (1<<11)
#define S3C2410_NFCONF_TACLS(x)    ((x)<<8)
#define S3C2410_NFCONF_TWRPH0(x)   ((x)<<4)
#define S3C2410_NFCONF_TWRPH1(x)   ((x)<<0)

#define S3C2410_NFSTAT_BUSY        (1<<0)

#define S3C2440_NFCONF_BUSWIDTH_8	(0<<0)
#define S3C2440_NFCONF_BUSWIDTH_16	(1<<0)
#define S3C2440_NFCONF_ADVFLASH		(1<<3)
#define S3C2440_NFCONF_TACLS(x)		((x)<<12)
#define S3C2440_NFCONF_TWRPH0(x)	((x)<<8)
#define S3C2440_NFCONF_TWRPH1(x)	((x)<<4)

#define S3C2440_NFCONT_LOCKTIGHT	(1<<13)
#define S3C2440_NFCONT_SOFTLOCK		(1<<12)
#define S3C2440_NFCONT_ILLEGALACC_EN	(1<<10)
#define S3C2440_NFCONT_RNBINT_EN	(1<<9)
#define S3C2440_NFCONT_RN_FALLING	(1<<8)
#define S3C2440_NFCONT_SPARE_ECCLOCK	(1<<6)
#define S3C2440_NFCONT_MAIN_ECCLOCK	(1<<5)
#define S3C2440_NFCONT_INITECC		(1<<4)
#define S3C2440_NFCONT_nFCE			(1<<1)
#define S3C2440_NFCONT_ENABLE		(1<<0)

#define S3C2440_NFSTAT_READY		(1<<0)
#define S3C2440_NFSTAT_nCE			(1<<1)
#define S3C2440_NFSTAT_RnB_CHANGE	(1<<2)
#define S3C2440_NFSTAT_ILLEGAL_ACCESS	(1<<3)

#define S3C2412_NFCONF_NANDBOOT		(1<<31)
#define S3C2412_NFCONF_ECCCLKCON	(1<<30)
#define S3C2412_NFCONF_ECC_MLC		(1<<24)
#define S3C2412_NFCONF_TACLS_MASK	(7<<12)	/* 1 extra bit of Tacls */

#define S3C2412_NFCONT_ECC4_DIRWR	(1<<18)
#define S3C2412_NFCONT_LOCKTIGHT	(1<<17)
#define S3C2412_NFCONT_SOFTLOCK		(1<<16)
#define S3C2412_NFCONT_ECC4_ENCINT	(1<<13)
#define S3C2412_NFCONT_ECC4_DECINT	(1<<12)
#define S3C2412_NFCONT_MAIN_ECC_LOCK	(1<<7)
#define S3C2412_NFCONT_INIT_MAIN_ECC	(1<<5)
#define S3C2412_NFCONT_nFCE1		(1<<2)
#define S3C2412_NFCONT_nFCE0		(1<<1)

#define S3C2412_NFSTAT_ECC_ENCDONE	(1<<7)
#define S3C2412_NFSTAT_ECC_DECDONE	(1<<6)
#define S3C2412_NFSTAT_ILLEGAL_ACCESS	(1<<5)
#define S3C2412_NFSTAT_RnB_CHANGE	(1<<4)
#define S3C2412_NFSTAT_nFCE1		(1<<3)
#define S3C2412_NFSTAT_nFCE0		(1<<2)
#define S3C2412_NFSTAT_Res1			(1<<1)
#define S3C2412_NFSTAT_READY		(1<<0)

#define S3C2412_NFECCERR_SERRDATA(x)	(((x) >> 21) & 0xf)
#define S3C2412_NFECCERR_SERRBIT(x)		(((x) >> 18) & 0x7)
#define S3C2412_NFECCERR_MERRDATA(x)	(((x) >> 7) & 0x3ff)
#define S3C2412_NFECCERR_MERRBIT(x)		(((x) >> 4) & 0x7)
#define S3C2412_NFECCERR_SPARE_ERR(x)	(((x) >> 2) & 0x3)
#define S3C2412_NFECCERR_MAIN_ERR(x)	(((x) >> 2) & 0x3)
#define S3C2412_NFECCERR_NONE		(0)
#define S3C2412_NFECCERR_1BIT		(1)
#define S3C2412_NFECCERR_MULTIBIT	(2)
#define S3C2412_NFECCERR_ECCAREA	(3)

#ifdef CONFIG_SYS_NAND_SELF_INIT

static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];

#endif

#ifdef CONFIG_MTD_NAND_S3C2410_DEBUG
#define DEBUG
#endif

#ifdef CONFIG_MTD_NAND_S3C2410_HWECC
static int hardware_ecc = 1;
#else
static int hardware_ecc = 0;
#endif



/* new oob placement block for use with hardware ecc generation
 */

static struct nand_ecclayout nand_hw_eccoob = {
	.eccbytes = 3,
	.eccpos = {0, 1, 2},
	.oobfree = {{8, 8}}
};

enum s3c_cpu_type {
	TYPE_S3C2410,
	TYPE_S3C2412,
	TYPE_S3C2440,
};

static enum s3c_cpu_type nand_chip_control = TYPE_S3C2440;

enum s3c_nand_clk_state {
	CLOCK_DISABLE	= 0,
	CLOCK_ENABLE,
	CLOCK_SUSPEND,
};

struct s3c2410_platform_nand {
	/* timing information for controller, all times in nanoseconds */

	int	tacls;	/* time for active CLE/ALE to nWE/nOE */
	int	twrph0;	/* active time for nWE/nOE */
	int	twrph1;	/* time for release CLE/ALE from nWE/nOE inactive */
};


/* overview of the s3c2410 nand state */

/**
 * struct s3c2410_nand_info - NAND controller state.
 * @mtds: An array of MTD instances on this controoler.
 * @platform: The platform data for this board.
 * @device: The platform device we bound to.
 * @area: The IO area resource that came from request_mem_region().
 * @clk: The clock resource for this controller.
 * @regs: The area mapped for the hardware registers described by @area.
 * @sel_reg: Pointer to the register controlling the NAND selection.
 * @sel_bit: The bit in @sel_reg to select the NAND chip.
 * @mtd_count: The number of MTDs created from this controller.
 * @save_sel: The contents of @sel_reg to be saved over suspend.
 * @clk_rate: The clock rate from @clk.
 * @clk_state: The current clock state.
 * @cpu_type: The exact type of this controller.
 */
struct s3c2410_nand_info {
	/* mtd info */
	struct nand_hw_control		controller;
	struct s3c2410_platform_nand	*platform;
	struct nand_chip		*chip;

	/* device info */
	struct device			*device;
	struct resource			*area;
	struct clk			*clk;
	void __iomem			*regs;
	void __iomem			*sel_reg;
	int				sel_bit;
	int				mtd_count;
	unsigned long			save_sel;
	unsigned long			clk_rate;
	enum s3c_nand_clk_state		clk_state;

	enum s3c_cpu_type		cpu_type;

};

struct s3c2410_platform_nand nand_k9f2g08u0a = 
{
	.tacls = 10,
	.twrph0 = 50,
	.twrph1 = 20
};

/* conversion functions */
struct s3c2410_nand_info s3c24xx_nand_info = {
	.platform = &nand_k9f2g08u0a,
};

static struct s3c2410_nand_info *s3c2410_nand_mtd_toinfo(struct mtd_info *mtd)
{
	return mtd_to_nand(mtd)->priv;
}
static inline int allow_clk_suspend(struct s3c2410_nand_info *info)
{
	return 0;
}

/**
 * s3c2410_nand_clk_set_state - Enable, disable or suspend NAND clock.
 * @info: The controller instance.
 * @new_state: State to which clock should be set.
 */
static void s3c2410_nand_clk_set_state(struct s3c2410_nand_info *info,
		enum s3c_nand_clk_state new_state)
{
	if (!allow_clk_suspend(info) && new_state == CLOCK_SUSPEND)
		return;

	if (info->clk_state == CLOCK_ENABLE) {
		if (new_state != CLOCK_ENABLE)
			;
//			clk_disable(info->clk);
	} else {
		if (new_state == CLOCK_ENABLE)
			;
//			clk_enable(info->clk);
	}

	info->clk_state = new_state;
}

/* timing calculations */

#define NS_IN_KHZ 1000000


/**
 * s3c_nand_calc_rate - calculate timing data.
 * @wanted: The cycle time in nanoseconds.
 * @clk: The clock rate in kHz.
 * @max: The maximum divider value.
 *
 * Calculate the timing value from the given parameters.
 */
static int s3c_nand_calc_rate(int wanted, unsigned long clk, int max)
{
	int result;

	result = DIV_ROUND_UP((wanted * clk), NS_IN_KHZ);

	s3c24xx_nand_dbg("result %d from %ld, %d\n", result, clk, wanted);

	if (result > max) {
		printf("%d ns is too big for current clock rate %ld\n", wanted, clk);
		return -1;
	}

	if (result < 1)
		result = 1;

	return result;
}

#define to_ns(ticks,clk) (((ticks) * NS_IN_KHZ) / (unsigned int)(clk))

/* controller setup */

/**
 * s3c2410_nand_setrate - setup controller timing information.
 * @info: The controller instance.
 *
 * Given the information supplied by the platform, calculate and set
 * the necessary timing registers in the hardware to generate the
 * necessary timing cycles to the hardware.
 */
static int s3c2410_nand_setrate(struct s3c2410_nand_info *info)
{
	struct s3c2410_platform_nand *plat = info->platform;
	int tacls_max = (info->cpu_type == TYPE_S3C2412) ? 8 : 4;
	int tacls, twrph0, twrph1;
	unsigned long clkrate = get_hclk();
	unsigned long set = 0, cfg, mask = 0;
	unsigned long flags;

	/* calculate the timing information for the controller */

	info->clk_rate = clkrate;
	clkrate /= 1000;	/* turn clock into kHz for ease of use */

	if (plat != NULL) {
		tacls = s3c_nand_calc_rate(plat->tacls, clkrate, tacls_max);
		twrph0 = s3c_nand_calc_rate(plat->twrph0, clkrate, 8);
		twrph1 = s3c_nand_calc_rate(plat->twrph1, clkrate, 8);
	} else {
		/* default timings */
		tacls = tacls_max;
		twrph0 = 8;
		twrph1 = 8;
	}

	if (tacls < 0 || twrph0 < 0 || twrph1 < 0) {
		printf("cannot get suitable timings\n");
		return -EINVAL;
	}

	s3c24xx_nand_dbg("clk rate=%d, Tacls=%d, %dns Twrph0=%d %dns, Twrph1=%d %dns\n",
	       (int)clkrate, tacls, to_ns(tacls, clkrate), twrph0, to_ns(twrph0, clkrate), twrph1, to_ns(twrph1, clkrate));

	switch (info->cpu_type) {
	case TYPE_S3C2410:
		mask = (S3C2410_NFCONF_TACLS(3) |
			S3C2410_NFCONF_TWRPH0(7) |
			S3C2410_NFCONF_TWRPH1(7));
		set = S3C2410_NFCONF_EN;
		set |= S3C2410_NFCONF_TACLS(tacls - 1);
		set |= S3C2410_NFCONF_TWRPH0(twrph0 - 1);
		set |= S3C2410_NFCONF_TWRPH1(twrph1 - 1);
		break;

	case TYPE_S3C2440:
	case TYPE_S3C2412:
		mask = (S3C2440_NFCONF_TACLS(tacls_max - 1) |
			S3C2440_NFCONF_TWRPH0(7) |
			S3C2440_NFCONF_TWRPH1(7));

		set = S3C2440_NFCONF_TACLS(tacls - 1);
		set |= S3C2440_NFCONF_TWRPH0(twrph0 - 1);
		set |= S3C2440_NFCONF_TWRPH1(twrph1 - 1);
		break;

	default:
		BUG();
	}

	local_irq_save(flags);

	cfg = readl(info->regs + S3C2410_NFCONF);
	cfg &= ~mask;
	cfg |= set;
	writel(cfg, info->regs + S3C2410_NFCONF);

	local_irq_restore(flags);

	s3c24xx_nand_dbg("NF_CONF is 0x%lx, set :%x\n", cfg, set);

	return 0;
}

/**
 * s3c2410_nand_inithw - basic hardware initialisation
 * @info: The hardware state.
 *
 * Do the basic initialisation of the hardware, using s3c2410_nand_setrate()
 * to setup the hardware access speeds and set the controller to be enabled.
*/
static int s3c2410_nand_inithw(struct s3c2410_nand_info *info)
{
	int ret;
	
	ret = s3c2410_nand_setrate(info);
	if (ret < 0)
		return ret;

 	switch (info->cpu_type) {
 	case TYPE_S3C2410:
	default:
		break;

 	case TYPE_S3C2440:
 	case TYPE_S3C2412:
		/* enable the controller and de-assert nFCE */

		writel(S3C2440_NFCONT_ENABLE, info->regs + S3C2440_NFCONT);
	}

	return 0;
}

/**
 * s3c2410_nand_select_chip - select the given nand chip
 * @mtd: The MTD instance for this chip.
 * @chip: The chip number.
 *
 * This is called by the MTD layer to either select a given chip for the
 * @mtd instance, or to indicate that the access has finished and the
 * chip can be de-selected.
 *
 * The routine ensures that the nFCE line is correctly setup, and any
 * platform specific selection code is called to route nFCE to the specific
 * chip.
 */
static void s3c2410_nand_select_chip(struct mtd_info *mtd, int chip)
{

	struct nand_chip *this = mtd_to_nand(mtd);;
	struct s3c2410_nand_info *info = nand_get_controller_data(this);
	unsigned long cur;


	if (chip != -1)
		s3c2410_nand_clk_set_state(info, CLOCK_ENABLE);

	cur = readl(info->sel_reg);

	if (chip == -1) {
		cur |= info->sel_bit;
	} else {
		cur &= ~info->sel_bit;
	}

	writel(cur, info->sel_reg);

	s3c24xx_nand_dbg("select_chip cur:%8x, chip:%d, sel_reg:%8x, sel_bit:%d\n", (int)cur, chip, (int)info->sel_reg, info->sel_bit);
	
	if (chip == -1)
		s3c2410_nand_clk_set_state(info, CLOCK_SUSPEND);
}

/* s3c2410_nand_hwcontrol
 *
 * Issue command and address cycles to the chip
*/

static void s3c2410_nand_hwcontrol(struct mtd_info *mtd, int cmd,
				   unsigned int ctrl)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);

	if (cmd == NAND_CMD_NONE)
		return;

	if (ctrl & NAND_CLE)
		writeb(cmd, info->regs + S3C2410_NFCMD);
	else
		writeb(cmd, info->regs + S3C2410_NFADDR);
}

/* command and control functions */

static void s3c2440_nand_hwcontrol(struct mtd_info *mtd, int cmd,
				   unsigned int ctrl)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);

	if (cmd == NAND_CMD_NONE)
		return;
	
	s3c24xx_nand_dbg("hw_ctrl:%x,%x,0x%8x,0x%8x\n",cmd, ctrl, (unsigned int)info->regs, (unsigned int)(info->regs + S3C2440_NFCMD));

	if (ctrl & NAND_CLE)
		writeb(cmd, info->regs + S3C2440_NFCMD);
	else
		writeb(cmd, info->regs + S3C2440_NFADDR);
}

/* s3c2410_nand_devready()
 *
 * returns 0 if the nand is busy, 1 if it is ready
*/

static int s3c2410_nand_devready(struct mtd_info *mtd)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	return readb(info->regs + S3C2410_NFSTAT) & S3C2410_NFSTAT_BUSY;
}

static int s3c2440_nand_devready(struct mtd_info *mtd)
{
	int ret;
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	
	ret = readb(info->regs + S3C2440_NFSTAT) & S3C2440_NFSTAT_READY;
	
	s3c24xx_nand_dbg("nand_ready:%d\n", ret);
	
	return ret;
}

static int s3c2412_nand_devready(struct mtd_info *mtd)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	return readb(info->regs + S3C2412_NFSTAT) & S3C2412_NFSTAT_READY;
}

/* ECC handling functions */

static int s3c2410_nand_correct_data(struct mtd_info *mtd, u_char *dat,
				     u_char *read_ecc, u_char *calc_ecc)
{
//	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	unsigned int diff0, diff1, diff2;
	unsigned int bit, byte;

	pr_debug("%s(%p,%p,%p,%p)\n", __func__, mtd, dat, read_ecc, calc_ecc);

	diff0 = read_ecc[0] ^ calc_ecc[0];
	diff1 = read_ecc[1] ^ calc_ecc[1];
	diff2 = read_ecc[2] ^ calc_ecc[2];

	pr_debug("%s: rd %02x%02x%02x calc %02x%02x%02x diff %02x%02x%02x\n",
		 __func__,
		 read_ecc[0], read_ecc[1], read_ecc[2],
		 calc_ecc[0], calc_ecc[1], calc_ecc[2],
		 diff0, diff1, diff2);

	if (diff0 == 0 && diff1 == 0 && diff2 == 0)
		return 0;		/* ECC is ok */

	/* sometimes people do not think about using the ECC, so check
	 * to see if we have an 0xff,0xff,0xff read ECC and then ignore
	 * the error, on the assumption that this is an un-eccd page.
	 */
	if (read_ecc[0] == 0xff && read_ecc[1] == 0xff && read_ecc[2] == 0xff
	    )
		return 0;

	/* Can we correct this ECC (ie, one row and column change).
	 * Note, this is similar to the 256 error code on smartmedia */

	if (((diff0 ^ (diff0 >> 1)) & 0x55) == 0x55 &&
	    ((diff1 ^ (diff1 >> 1)) & 0x55) == 0x55 &&
	    ((diff2 ^ (diff2 >> 1)) & 0x55) == 0x55) {
		/* calculate the bit position of the error */

		bit  = ((diff2 >> 3) & 1) |
		       ((diff2 >> 4) & 2) |
		       ((diff2 >> 5) & 4);

		/* calculate the byte position of the error */

		byte = ((diff2 << 7) & 0x100) |
		       ((diff1 << 0) & 0x80)  |
		       ((diff1 << 1) & 0x40)  |
		       ((diff1 << 2) & 0x20)  |
		       ((diff1 << 3) & 0x10)  |
		       ((diff0 >> 4) & 0x08)  |
		       ((diff0 >> 3) & 0x04)  |
		       ((diff0 >> 2) & 0x02)  |
		       ((diff0 >> 1) & 0x01);

		dev_dbg(info->device, "correcting error bit %d, byte %d\n",
			bit, byte);

		dat[byte] ^= (1 << bit);
		return 1;
	}

	/* if there is only one bit difference in the ECC, then
	 * one of only a row or column parity has changed, which
	 * means the error is most probably in the ECC itself */

	diff0 |= (diff1 << 8);
	diff0 |= (diff2 << 16);

	if ((diff0 & ~(1<<fls(diff0))) == 0)
		return 1;

	return -1;
}

/* ECC functions
 *
 * These allow the s3c2410 and s3c2440 to use the controller's ECC
 * generator block to ECC the data as it passes through]
*/

static void s3c2410_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	unsigned long ctrl;

	ctrl = readl(info->regs + S3C2410_NFCONF);
	ctrl |= S3C2410_NFCONF_INITECC;
	writel(ctrl, info->regs + S3C2410_NFCONF);
}

static void s3c2412_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	unsigned long ctrl;

	ctrl = readl(info->regs + S3C2440_NFCONT);
	writel(ctrl | S3C2412_NFCONT_INIT_MAIN_ECC, info->regs + S3C2440_NFCONT);
}

static void s3c2440_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	unsigned long ctrl;

	ctrl = readl(info->regs + S3C2440_NFCONT);
	writel(ctrl | S3C2440_NFCONT_INITECC, info->regs + S3C2440_NFCONT);
}

static int s3c2410_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);

	ecc_code[0] = readb(info->regs + S3C2410_NFECC + 0);
	ecc_code[1] = readb(info->regs + S3C2410_NFECC + 1);
	ecc_code[2] = readb(info->regs + S3C2410_NFECC + 2);

	pr_debug("%s: returning ecc %02x%02x%02x\n", __func__,
		 ecc_code[0], ecc_code[1], ecc_code[2]);

	return 0;
}

static int s3c2412_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	unsigned long ecc = readl(info->regs + S3C2412_NFMECC0);

	ecc_code[0] = ecc;
	ecc_code[1] = ecc >> 8;
	ecc_code[2] = ecc >> 16;

	pr_debug("calculate_ecc: returning ecc %02x,%02x,%02x\n", ecc_code[0], ecc_code[1], ecc_code[2]);

	return 0;
}

static int s3c2440_nand_calculate_ecc(struct mtd_info *mtd, const u_char *dat, u_char *ecc_code)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	unsigned long ecc = readl(info->regs + S3C2440_NFMECC0);

	printf("cal_ecc\n");

	ecc_code[0] = ecc;
	ecc_code[1] = ecc >> 8;
	ecc_code[2] = ecc >> 16;

	pr_debug("%s: returning ecc %06lx\n", __func__, ecc & 0xffffff);

	return 0;
}

/* over-ride the standard functions for a little more speed. We can
 * use read/write block to move the data buffers to/from the controller
*/

static void s3c2410_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	struct nand_chip *this = mtd->priv;
	readsb(this->IO_ADDR_R, buf, len);
}

static void s3c2440_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);
	s3c24xx_nand_dbg("read_buf:%d\n", len);

	readsl(info->regs + S3C2440_NFDATA, buf, len >> 2);

	/* cleanup if we've got less than a word to do */
	if (len & 3) {
		buf += len & ~3;

		for (; len & 3; len--)
			*buf++ = readb(info->regs + S3C2440_NFDATA);
	}
}

static void s3c2410_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	struct nand_chip *this = mtd->priv;
	writesb(this->IO_ADDR_W, buf, len);
}

static void s3c2440_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	struct s3c2410_nand_info *info = s3c2410_nand_mtd_toinfo(mtd);

	s3c24xx_nand_dbg("write_buf:%d\n", len);

	writesl(info->regs + S3C2440_NFDATA, buf, len >> 2);

	/* cleanup any fractional write */
	if (len & 3) {
		buf += len & ~3;

		for (; len & 3; len--, buf++)
			writeb(*buf, info->regs + S3C2440_NFDATA);
	}
}


/**
 * s3c2410_nand_init_chip - initialise a single instance of an chip
 * @info: The base NAND controller the chip is on.
 * @nmtd: The new controller MTD instance to fill in.
 * @set: The information passed from the board specific platform data.
 *
 * Initialise the given @nmtd from the information in @info and @set. This
 * readies the structure for use with the MTD layer functions by ensuring
 * all pointers are setup and the necessary control routines selected.
 */
static void s3c2410_nand_init_chip(struct s3c2410_nand_info *info,
				   struct nand_chip *chip)
{
	void __iomem *regs = info->regs = (void __iomem *)S3C2410_NAND_BASE;

	s3c24xx_nand_dbg("s3c2410_nand_init_chip begin\n");

	chip->write_buf    = s3c2410_nand_write_buf;
	chip->read_buf     = s3c2410_nand_read_buf;
	chip->select_chip  = s3c2410_nand_select_chip;
	chip->chip_delay   = 50;
	nand_set_controller_data(chip, info);
//	chip->options	   = set->options;
	chip->controller   = &info->controller;
	info->cpu_type = nand_chip_control;

	s3c24xx_nand_dbg("s3c2410_nand_init_chip switch\n");
	switch (info->cpu_type) {
	case TYPE_S3C2410:
		chip->IO_ADDR_W = regs + S3C2410_NFDATA;
		info->sel_reg   = regs + S3C2410_NFCONF;
		info->sel_bit	= S3C2410_NFCONF_nFCE;
		chip->cmd_ctrl  = s3c2410_nand_hwcontrol;
		chip->dev_ready = s3c2410_nand_devready;
		break;

	case TYPE_S3C2440:
		chip->IO_ADDR_W = regs + S3C2440_NFDATA;
		info->sel_reg   = regs + S3C2440_NFCONT;
		info->sel_bit	= S3C2440_NFCONT_nFCE;
		chip->cmd_ctrl  = s3c2440_nand_hwcontrol;
		chip->dev_ready = s3c2440_nand_devready;
		chip->read_buf  = s3c2440_nand_read_buf;
		chip->write_buf	= s3c2440_nand_write_buf;
		break;

	case TYPE_S3C2412:
		chip->IO_ADDR_W = regs + S3C2440_NFDATA;
		info->sel_reg   = regs + S3C2440_NFCONT;
		info->sel_bit	= S3C2412_NFCONT_nFCE0;
		chip->cmd_ctrl  = s3c2440_nand_hwcontrol;
		chip->dev_ready = s3c2412_nand_devready;

		if (readl(regs + S3C2410_NFCONF) & S3C2412_NFCONF_NANDBOOT)
			dev_info(info->device, "System booted from NAND\n");

		break;
	default:
		break;
  	}

	chip->IO_ADDR_R = chip->IO_ADDR_W;

	info->chip = chip;
	
	s3c24xx_nand_dbg("s3c2410_nand_init_chip hd ecc\n");
	if (hardware_ecc) {
		chip->ecc.calculate = s3c2410_nand_calculate_ecc;
		chip->ecc.correct   = s3c2410_nand_correct_data;
		chip->ecc.mode	    = NAND_ECC_HW;
		chip->ecc.strength  = 1;

		switch (info->cpu_type) {
		case TYPE_S3C2410:
			chip->ecc.hwctl	    = s3c2410_nand_enable_hwecc;
			chip->ecc.calculate = s3c2410_nand_calculate_ecc;
			break;

		case TYPE_S3C2412:
  			chip->ecc.hwctl     = s3c2412_nand_enable_hwecc;
  			chip->ecc.calculate = s3c2412_nand_calculate_ecc;
			break;

		case TYPE_S3C2440:
  			chip->ecc.hwctl     = s3c2440_nand_enable_hwecc;
  			chip->ecc.calculate = s3c2440_nand_calculate_ecc;
			break;

		}
	} else {
		chip->ecc.mode	    = NAND_ECC_SOFT;
	}

//	if (set->ecc_layout != NULL)
//		chip->ecc.layout = set->ecc_layout;

//	if (set->disable_ecc)
//		chip->ecc.mode	= NAND_ECC_NONE;

	s3c24xx_nand_dbg("s3c2410_nand_init_chip ecc mode\n");
	switch (chip->ecc.mode) {
	case NAND_ECC_NONE:
		dev_info(info->device, "ECC disabled\n");
		break;
	case NAND_ECC_SOFT:
		dev_info(info->device, "soft ECC\n");
		break;
	case NAND_ECC_HW:
		dev_info(info->device, "hardware ECC\n");
		break;
	default:
		dev_info(info->device, "ECC UNKNOWN\n");
		break;
	}

	/* If you use u-boot BBT creation code, specifying this flag will
	 * let the kernel fish out the BBT from the NAND, and also skip the
	 * full NAND scan that can take 1/2s or so. Little things... */
//	if (set->flash_bbt) {
//		chip->bbt_options |= NAND_BBT_USE_FLASH;
//		chip->options |= NAND_SKIP_BBTSCAN;
//	}
}

/**
 * s3c2410_nand_update_chip - post probe update
 * @info: The controller instance.
 * @nmtd: The driver version of the MTD instance.
 *
 * This routine is called after the chip probe has successfully completed
 * and the relevant per-chip information updated. This call ensure that
 * we update the internal state accordingly.
 *
 * The internal state is currently limited to the ECC state information.
*/
static void s3c2410_nand_update_chip(struct nand_chip *chip)
{

	dev_dbg(info->device, "chip %p => page shift %d\n",
		chip, chip->page_shift);

	if (chip->ecc.mode != NAND_ECC_HW)
		return;

		/* change the behaviour depending on wether we are using
		 * the large or small page nand device */

	if (chip->page_shift > 10) {
		chip->ecc.size	    = 256;
		chip->ecc.bytes	    = 3;
	} else {
		chip->ecc.size	    = 512;
		chip->ecc.bytes	    = 3;
		chip->ecc.layout    = &nand_hw_eccoob;
	}
}

void board_nand_init(void)
{
//	int i;
	static int chip_nr = 0;
	struct nand_chip *nand = &nand_chip[0];
	struct mtd_info * mtd = nand_to_mtd(nand);
	int maxchips = CONFIG_SYS_NAND_MAX_CHIPS;

	s3c24xx_nand_dbg("s3c24xx nand init start\n");
	
	mtd = nand_to_mtd(nand);

	s3c24xx_nand_dbg("s3c2410_nand_init_chip\n");
	s3c2410_nand_init_chip(&s3c24xx_nand_info, nand);
	
	s3c24xx_nand_dbg("s3c2410_nand_inithw\n");
	s3c2410_nand_inithw(&s3c24xx_nand_info);
	
	s3c24xx_nand_dbg("s3c2410_nand_update_chip\n");
	s3c2410_nand_update_chip(nand);
	
	s3c24xx_nand_dbg("nand_scan\n");
	if (nand_scan(mtd, maxchips)) {
		printf("nand flash scan failure\n");
		return;
	}
	
	s3c24xx_nand_dbg("nand_register\n");
	nand_register(chip_nr, mtd);

	/* scan nand flash bad block  */
//	mtd->_block_markbad(mtd, 0);
	
}

