#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <asm/mach-types.h>

#include <asm/arch/s3c2440.h>
#include <asm/arch/s3c24xx.h>

#define GSTATUS1        (*(volatile unsigned int *)0x560000B0)
#define BUSY            1

#define NAND_SECTOR_SIZE	512
#define NAND_BLOCK_MASK	(NAND_SECTOR_SIZE - 1)

#define NAND_SECTOR_SIZE_LP	2048
#define NAND_BLOCK_MASK_LP	(NAND_SECTOR_SIZE_LP - 1)


char bLARGEBLOCK;			//HJ_add 20090807
char b128MB = 1;				//HJ_add 20090807


/* 供外部调用的函数 */
void nand_init_ll(void);

int nand_read_ll(unsigned char *buf, unsigned int start_addr, int size);
int nand_read_ll_lp(unsigned char *buf, unsigned int start_addr, int size);

/* NAND Flash操作的总入口, 它们将调用S3C2410或S3C2440的相应函数 */
static void nand_reset(void);
static void wait_idle(void);
static void nand_select_chip(void);
static void nand_deselect_chip(void);
static void write_cmd(int cmd);
static void write_addr(unsigned int addr);
static void write_addr_lp(unsigned int addr);
static unsigned char read_data(void);
int NF_ReadID(void);				//HJ_add 20090807


/* S3C2440的NAND Flash处理函数 */
static void s3c2440_nand_reset(void);
static void s3c2440_wait_idle(void);
static void s3c2440_nand_select_chip(void);
static void s3c2440_nand_deselect_chip(void);
static void s3c2440_write_cmd(int cmd);
static void s3c2440_write_addr(unsigned int addr);
static void s3c2440_write_addr_lp(unsigned int addr);
static unsigned char s3c2440_read_data(void);

/* S3C2440的NAND Flash操作函数 */

/* 复位 */
static void s3c2440_nand_reset(void)
{
	s3c2440_nand_select_chip();
	s3c2440_write_cmd(0xff);   // 复位命令
	s3c2440_wait_idle();
	s3c2440_nand_deselect_chip();
}
void delay_loop(unsigned int loop)
{	
//	unsigned int i;
//	for(i = 0; i < loop; i++);
}

/* 等待NAND Flash就绪 */
static void s3c2440_wait_idle(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFSTAT;

	while(!(*p & BUSY))
        delay_loop(10);
}

/* 发出片选信号 */
static void s3c2440_nand_select_chip(void)
{
	S3C2440_NAND *s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;

	s3c2440nand->NFCONT &= ~(1<<1);
	delay_loop(10);   
}

/* 取消片选信号 */
static void s3c2440_nand_deselect_chip(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;

	s3c2440nand->NFCONT |= (1<<1);
}

/* 发出命令 */
static void s3c2440_write_cmd(int cmd)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;

	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFCMD;
	*p = cmd;
}

/* 发出地址 */
static void s3c2440_write_addr(unsigned int addr)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;
    
	*p = addr & 0xff;
	delay_loop(10);   
	*p = (addr >> 9) & 0xff;
	delay_loop(10);   
	*p = (addr >> 17) & 0xff;
	delay_loop(10);   
	*p = (addr >> 25) & 0xff;
	delay_loop(10);   
}


/* 发出地址 */
static void s3c2440_write_addr_lp(unsigned int addr)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;
	int col, page;

	col = addr & NAND_BLOCK_MASK_LP;
	page = addr / NAND_SECTOR_SIZE_LP;
	
	*p = col & 0xff;			/* Column Address A0~A7 */
	delay_loop(10);   	
	*p = (col >> 8) & 0x0f;		/* Column Address A8~A11 */
	delay_loop(10);   
	*p = page & 0xff;			/* Row Address A12~A19 */
	delay_loop(10);   
	*p = (page >> 8) & 0xff;	/* Row Address A20~A27 */
	delay_loop(10);   
	
	if (b128MB == 0) {
		*p = (page >> 16) & 0x03;	/* Row Address A28~A29 */
		delay_loop(10);   
	}
}

/* 读取数据 */
static unsigned char s3c2440_read_data(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFDATA;
	return *p;
}


/* 在第一次使用NAND Flash前，复位一下NAND Flash */
static void nand_reset(void)
{
	s3c2440_nand_reset();
}

static void wait_idle(void)
{
	s3c2440_wait_idle();
}

static void nand_select_chip(void)
{
	
	s3c2440_nand_select_chip();
	
	delay_loop(10);   
}

static void nand_deselect_chip(void)
{
	s3c2440_nand_deselect_chip();
}

static void write_cmd(int cmd)
{
	s3c2440_write_cmd(cmd);
}
static void write_addr(unsigned int addr)
{
	s3c2440_write_addr(addr);
}

static void write_addr_lp(unsigned int addr)
{
	s3c2440_write_addr_lp(addr);
}

static unsigned char read_data(void)
{
	return s3c2440_read_data();
}

/* 初始化NAND Flash */
void nand_init_ll(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;

	#define TACLS   1
	#define TWRPH0  5
	#define TWRPH1  2

	/* 设置时序 */
	s3c2440nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	/* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
//	s3c2440nand->NFCONT = (1<<4)|(1<<1)|(1<<0);
	s3c2440nand->NFCONT = (1<<0);

	/* 复位NAND Flash */
	nand_reset();
}
#if 1
int NF_ReadID(void)
{
	char pMID;
	char pDID;
	int  nBuff;
	char   n4thcycle;
	char id[32];
	int cnt = 0;
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;

	b128MB = 1;
//	n4thcycle = nBuff = 0;

	nand_init_ll();
	nand_select_chip();
	write_cmd(0x90);	// read id command
	*p = 0x00 & 0xff;
	delay_loop(1000);   

	pMID = read_data();
	pDID =  read_data();
	nBuff =  read_data();
	n4thcycle = read_data();

	nand_deselect_chip();
	
	if (pDID >= 0xA0)
	{
		b128MB = 0;
	}

	return (pDID);
}
#endif

/* 读函数 */
int nand_read_ll(unsigned char *buf, unsigned int start_addr, int size)
{
	int i, j;
	char dat;
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;

    
	if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK))
	{
		return -1;    /* 地址或长度不对齐 */
	}

	/* 选中芯片 */
	nand_select_chip();

	for(i=start_addr; i < (start_addr + size);)
	{
		/* Check Bad Block */
		if(1){
		/* 发出READ0命令 */
			write_cmd(0x50);

			*p = 5;
			delay_loop(10);
			*p = (i >> 9) & 0xff;
			delay_loop(10);
			*p = (i >> 17) & 0xff;
			delay_loop(10);
			*p = (i >> 25) & 0xff;
			delay_loop(10);
			wait_idle();

			dat = read_data();
			write_cmd(0);
			
			/* 取消片选信号 */
			nand_deselect_chip();
			if(dat != 0xff)
				i += 16384;		// 1 Block = 512*32= 16384
	/* Read Page */
			/* 选中芯片 */
			nand_select_chip();
		}
		/* 发出READ0命令 */
		write_cmd(0);

		/* Write Address */
		write_addr(i);
		wait_idle();

		for(j=0; j < NAND_SECTOR_SIZE; j++, i++)
		{
			*buf = read_data();
			buf++;
		}
	}

	/* 取消片选信号 */
	nand_deselect_chip();

	return 0;
}

/* 读函数 
  * Large Page
  */

int nand_read_ll_lp(unsigned char *buf, unsigned int start_addr, int size)
{
	int i, j;
	char dat;
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;

	if ((start_addr & NAND_BLOCK_MASK_LP) || (size & NAND_BLOCK_MASK_LP))
	{
		return -1;     /* 地址或长度不对齐 */
	}

	/* 选中芯片 */
	nand_select_chip();

	for(i=start_addr; i < (start_addr + size);)
	{
		/* Check Bad Block */
		if(1) {
			int page;

	//		col = i & NAND_BLOCK_MASK_LP;
			page = i / NAND_SECTOR_SIZE_LP;
			/* 发出READ0命令 */
			write_cmd(0x00);

			*p = 5;
			delay_loop(10);
			*p = 8;
			delay_loop(10);
			*p = page & 0xff;		/* Row Address A12~A19 */
			delay_loop(10);
			*p = (page >> 8) & 0xff;		/* Row Address A20~A27 */
			delay_loop(10);
			if (b128MB == 0) {
				*p = (page >> 16) & 0x03;		/* Row Address A28~A29 */
				delay_loop(10);
			}

			write_cmd(0x30);
			wait_idle();

			dat = read_data();
			
			/* 取消片选信号 */
			nand_deselect_chip();
			if(dat != 0xff) {
				i += 131072;		// 1 Block = 2048*64= 131072
			//	printf("invalid block :0x%8x\n", i);
			}
			/* Read Page */
			/* 选中芯片 */
			nand_select_chip();
		}
		/* 发出READ0命令 */
		write_cmd(0);

		/* Write Address */
		write_addr_lp(i);
		write_cmd(0x30);
		wait_idle();

		for(j=0; j < NAND_SECTOR_SIZE_LP; j++, i++)
		{
			*buf = read_data();
			buf++;
		}
	}

	/* 取消片选信号 */
	nand_deselect_chip();

	return 0;
}

int bBootFrmNORFlash(void)
{
	volatile unsigned int *pdw = (volatile unsigned int *)0;
	unsigned int dwVal;

	/*
	 * 无论是从NOR Flash还是从NAND Flash启动，
	 * 地址0处为指令"b	Reset", 机器码为0xEA00000B，
	 * 对于从NAND Flash启动的情况，其开始4KB的代码会复制到CPU内部4K内存中，
	 * 对于从NOR Flash启动的情况，NOR Flash的开始地址即为0。
	 * 对于NOR Flash，必须通过一定的命令序列才能写数据，
	 * 所以可以根据这点差别来分辨是从NAND Flash还是NOR Flash启动:
	 * 向地址0写入一个数据，然后读出来，如果没有改变的话就是NOR Flash
	 */
	dwVal = *pdw;       
	*pdw = 0x12345678;
	if (*pdw != 0x12345678) {
		return 1;
	} else {
		*pdw = dwVal;
		return 0;
	}
}

int nand_spl_load_image(uint32_t offs, unsigned int size, void *dst)
{
	/*  初始化nand 控制器  */
	nand_init_ll();

	/* 从 NAND Flash启动 */
	if (NF_ReadID() == 0x76 ) {
		puts("nand_read_ll\n");
		nand_read_ll(dst, offs, (size + NAND_BLOCK_MASK)&~(NAND_BLOCK_MASK));
	} else {
		puts("nand_read_ll_lp\n");
		nand_read_ll_lp(dst, offs, (size + NAND_BLOCK_MASK_LP)&~(NAND_BLOCK_MASK_LP));
		puts("nand_read_ll_lp end\n");
	}
	return 0;
}

#ifndef CONFIG_SPL_FRAMEWORK

void nand_boot(void)
{
	__attribute__((noreturn)) void (*uboot)(void);
	unsigned int *pdwDest;
	unsigned int *pdwSrc;
	int i;
	
	puts("nand boot0\n");
	if (bBootFrmNORFlash())
	{
		puts("nor boot\n");
		pdwDest = (unsigned int *)CONFIG_SYS_NOR_U_BOOT_DST;
		pdwSrc  = (unsigned int *)CONFIG_SYS_NOR_U_BOOT_START;
		/* 从 NOR Flash启动 */
		for (i = 0; i < CONFIG_SYS_NOR_U_BOOT_SIZE / 4; i++) {
			pdwDest[i] = pdwSrc[i];
		}
		uboot = (void *)CONFIG_SYS_NOR_U_BOOT_DST;
		uboot();
		/* Unrecoverable error when copying from NAND */
		for(;;);
	}
	else
	{
		/*
		 * CONFIG_SYS_NAND_U_BOOT_OFFS and CONFIG_SYS_NAND_U_BOOT_SIZE must
		 * be aligned to full pages
		 */
		 
		puts("nand boot1\n");
		if (!nand_spl_load_image(CONFIG_SYS_NAND_U_BOOT_OFFS,
				CONFIG_SYS_NAND_U_BOOT_SIZE,
				(uchar *)CONFIG_SYS_NAND_U_BOOT_DST)) {
//				while(1);
			/* Copy from NAND successful, start U-Boot */
			uboot = (void *)CONFIG_SYS_NAND_U_BOOT_START;
			uboot();
		} else {
			/* Unrecoverable error when copying from NAND */
			for(;;);
		}
	}
}

#endif

