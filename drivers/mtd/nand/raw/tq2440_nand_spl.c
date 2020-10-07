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


/* ���ⲿ���õĺ��� */
void nand_init_ll(void);

int nand_read_ll(unsigned char *buf, unsigned int start_addr, int size);
int nand_read_ll_lp(unsigned char *buf, unsigned int start_addr, int size);

/* NAND Flash�����������, ���ǽ�����S3C2410��S3C2440����Ӧ���� */
static void nand_reset(void);
static void wait_idle(void);
static void nand_select_chip(void);
static void nand_deselect_chip(void);
static void write_cmd(int cmd);
static void write_addr(unsigned int addr);
static void write_addr_lp(unsigned int addr);
static unsigned char read_data(void);
int NF_ReadID(void);				//HJ_add 20090807


/* S3C2440��NAND Flash������ */
static void s3c2440_nand_reset(void);
static void s3c2440_wait_idle(void);
static void s3c2440_nand_select_chip(void);
static void s3c2440_nand_deselect_chip(void);
static void s3c2440_write_cmd(int cmd);
static void s3c2440_write_addr(unsigned int addr);
static void s3c2440_write_addr_lp(unsigned int addr);
static unsigned char s3c2440_read_data(void);

/* S3C2440��NAND Flash�������� */

/* ��λ */
static void s3c2440_nand_reset(void)
{
	s3c2440_nand_select_chip();
	s3c2440_write_cmd(0xff);   // ��λ����
	s3c2440_wait_idle();
	s3c2440_nand_deselect_chip();
}
void delay_loop(unsigned int loop)
{	
//	unsigned int i;
//	for(i = 0; i < loop; i++);
}

/* �ȴ�NAND Flash���� */
static void s3c2440_wait_idle(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFSTAT;

	while(!(*p & BUSY))
        delay_loop(10);
}

/* ����Ƭѡ�ź� */
static void s3c2440_nand_select_chip(void)
{
	S3C2440_NAND *s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;

	s3c2440nand->NFCONT &= ~(1<<1);
	delay_loop(10);   
}

/* ȡ��Ƭѡ�ź� */
static void s3c2440_nand_deselect_chip(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;

	s3c2440nand->NFCONT |= (1<<1);
}

/* �������� */
static void s3c2440_write_cmd(int cmd)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;

	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFCMD;
	*p = cmd;
}

/* ������ַ */
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


/* ������ַ */
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

/* ��ȡ���� */
static unsigned char s3c2440_read_data(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFDATA;
	return *p;
}


/* �ڵ�һ��ʹ��NAND Flashǰ����λһ��NAND Flash */
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

/* ��ʼ��NAND Flash */
void nand_init_ll(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;

	#define TACLS   1
	#define TWRPH0  5
	#define TWRPH1  2

	/* ����ʱ�� */
	s3c2440nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	/* ʹ��NAND Flash������, ��ʼ��ECC, ��ֹƬѡ */
//	s3c2440nand->NFCONT = (1<<4)|(1<<1)|(1<<0);
	s3c2440nand->NFCONT = (1<<0);

	/* ��λNAND Flash */
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

/* ������ */
int nand_read_ll(unsigned char *buf, unsigned int start_addr, int size)
{
	int i, j;
	char dat;
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)S3C2410_NAND_BASE;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;

    
	if ((start_addr & NAND_BLOCK_MASK) || (size & NAND_BLOCK_MASK))
	{
		return -1;    /* ��ַ�򳤶Ȳ����� */
	}

	/* ѡ��оƬ */
	nand_select_chip();

	for(i=start_addr; i < (start_addr + size);)
	{
		/* Check Bad Block */
		if(1){
		/* ����READ0���� */
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
			
			/* ȡ��Ƭѡ�ź� */
			nand_deselect_chip();
			if(dat != 0xff)
				i += 16384;		// 1 Block = 512*32= 16384
	/* Read Page */
			/* ѡ��оƬ */
			nand_select_chip();
		}
		/* ����READ0���� */
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

	/* ȡ��Ƭѡ�ź� */
	nand_deselect_chip();

	return 0;
}

/* ������ 
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
		return -1;     /* ��ַ�򳤶Ȳ����� */
	}

	/* ѡ��оƬ */
	nand_select_chip();

	for(i=start_addr; i < (start_addr + size);)
	{
		/* Check Bad Block */
		if(1) {
			int page;

	//		col = i & NAND_BLOCK_MASK_LP;
			page = i / NAND_SECTOR_SIZE_LP;
			/* ����READ0���� */
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
			
			/* ȡ��Ƭѡ�ź� */
			nand_deselect_chip();
			if(dat != 0xff) {
				i += 131072;		// 1 Block = 2048*64= 131072
			//	printf("invalid block :0x%8x\n", i);
			}
			/* Read Page */
			/* ѡ��оƬ */
			nand_select_chip();
		}
		/* ����READ0���� */
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

	/* ȡ��Ƭѡ�ź� */
	nand_deselect_chip();

	return 0;
}

int bBootFrmNORFlash(void)
{
	volatile unsigned int *pdw = (volatile unsigned int *)0;
	unsigned int dwVal;

	/*
	 * �����Ǵ�NOR Flash���Ǵ�NAND Flash������
	 * ��ַ0��Ϊָ��"b	Reset", ������Ϊ0xEA00000B��
	 * ���ڴ�NAND Flash������������俪ʼ4KB�Ĵ���Ḵ�Ƶ�CPU�ڲ�4K�ڴ��У�
	 * ���ڴ�NOR Flash�����������NOR Flash�Ŀ�ʼ��ַ��Ϊ0��
	 * ����NOR Flash������ͨ��һ�����������в���д���ݣ�
	 * ���Կ��Ը�����������ֱ��Ǵ�NAND Flash����NOR Flash����:
	 * ���ַ0д��һ�����ݣ�Ȼ������������û�иı�Ļ�����NOR Flash
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
	/*  ��ʼ��nand ������  */
	nand_init_ll();

	/* �� NAND Flash���� */
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
		/* �� NOR Flash���� */
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

