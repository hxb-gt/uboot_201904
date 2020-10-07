/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Aeronix Zipit Z2 configuration file
 *
 * Copyright (C) 2009-2010 Marek Vasut <marek.vasut@gmail.com>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Board Configuration Options
 */
//#define	CONFIG_CPU_PXA27X		1	/* Marvell PXA270 CPU */

/* High-level configuration options */
#define CONFIG_S3C24XX		1		/* This is a Cirrus Logic 93xx SoC */

//#define DEBUG	1

#undef	CONFIG_SKIP_LOWLEVEL_INIT
#define	CONFIG_PREBOOT

#define CONFIG_MACH_TYPE			MACH_TYPE_EMBEDSKY2440


/* RAM */
#define CONFIG_SYS_SDRAM_BASE		0x30000000
#define CONFIG_SYS_SDRAM_SIZE		0x4000000



/* U-Boot */
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE + 0x100000

#define LINUX_BOOT_PARAM_ADDR		CONFIG_SYS_SDRAM_BASE + 0x100

/*  stack point */
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_SDRAM_BASE + CONFIG_SYS_SDRAM_SIZE - 0x800000

/*
 * SPL
 */
#define CONFIG_SPL_TARGET	"u-boot-spl.bin"
#define CONFIG_SPL_MAX_SIZE	4096
#define CONFIG_SPL_TEXT_BASE    0x00000000


#define CONFIG_SYS_UBOOT_SIZE	(512*1024)

/* NAND boot config */
#define CONFIG_SYS_NAND_SELF_INIT

#define CONFIG_SYS_MAX_NAND_DEVICE		1
#define CONFIG_SYS_NAND_BASE			(0x4E000000)

#define CONFIG_SYS_NAND_U_BOOT_START    CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_NAND_U_BOOT_OFFS		0x20000
#define CONFIG_SYS_NAND_U_BOOT_DST		CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_NAND_U_BOOT_SIZE		(CONFIG_SYS_UBOOT_SIZE - 0x800)

/* Ethernet */

#define CONFIG_ETHADDR		0a:1b:2c:3d:4e:5f
#define CONFIG_IPADDR		192.168.0.103
#define CONFIG_SERVERIP		192.168.0.106
#define CONFIG_GATEWAYIP	192.168.0.1
#define CONFIG_NETMASK		255.255.255.0

#define CONFIG_DRIVER_DM9000
#define CONFIG_DM9000_BASE		0x20000000
#define DM9000_IO			CONFIG_DM9000_BASE
#define DM9000_DATA			(CONFIG_DM9000_BASE + 4)
#define CONFIG_DM9000_USE_16BIT
#define CONFIG_DM9000_NO_SROM
#define CONFIG_NET_RETRY_COUNT		20
//#define CONFIG_RESET_PHY_R


/* NOR boot config */
#define CONFIG_SYS_NOR_U_BOOT_START     0


#define CONFIG_SYS_NOR_U_BOOT_DST		CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_NOR_U_BOOT_SIZE		CONFIG_SYS_UBOOT_SIZE - 0x800


/* input clock of PLL */
#define CONFIG_SYS_CLK_FREQ		12000000/* the TQ2440 has 12MHz input clock */
//#define CONFIG_SYS_CLK_FREQ		16934400 /* the TQ2440 has 16.9344MHz input clock */


/*
 * Environment settings
 */

/*  nand flash layout : uboot [0 ~ 895K], env [896k  ~ 1023k] , kernel [1024k - 5119k], other [5120k -] */

#define	CONFIG_ENV_OVERWRITE
//#define CONFIG_ENV_SECT_SIZE	0x0010000
//#define CONFIG_ENV_ADDR			0x40000
#define CONFIG_ENV_SIZE			0x20000
#define CONFIG_ENV_OFFSET		(0x100000 - CONFIG_ENV_SIZE)

#define	CONFIG_SYS_MALLOC_LEN		(128*1024)
#define	CONFIG_ARCH_CPU_INIT

#define	CONFIG_BOOTCOMMAND	"nand read 0x30008000 0x100000 0x400000; bootm 0x300080000"
#define	CONFIG_TIMESTAMP
#define	CONFIG_CMDLINE_TAG
#define	CONFIG_SETUP_MEMORY_TAGS
#define	CONFIG_USE_BOOTARGS
#define	CONFIG_BOOTARGS	"root=/dev/nfs nfsroot=192.168.0.106:/nfs_root/rootfs" \
						"ip=192.168.0.122:192.168.0.106:192.168.0.1:255.255.255.0" \
						":embedsky:eth0:any:192.168.0.1"


/*
 * Serial Console Configuration
 * STUART - the lower serial port on Colibri board
 */
#define	CONFIG_STUART			1

/*
 * Bootloader Components Configuration
 */

#endif	/* __CONFIG_H */
