/*
 * MTD SPI driver for ST opconn_spixx (and similar) serial flash chips
 *
 * Author: Mike Lavender, mike@steroidmicros.com
 *
 * Copyright (c) 2005, Intec Automation Inc.
 *
 * Some parts are based on lart.c by Abraham Van Der Merwe
 *
 * Cleaned up and generalized based on mtd_dataflash.c
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/interrupt.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <linux/platform_device.h>
#include <asm/semaphore.h>




#define SPI_CTL 0x24
#define SPI_SS 0x01
#define SPI_START 0x01

#define SPI_ERASE_MAX_WAIT 0x100000


typedef struct ipmux_spi {
	volatile unsigned int tr0; /* transmit register */
	volatile unsigned int tr1; /* transmit register */
	volatile unsigned int tr2; /* transmit register */
	volatile unsigned int tr3; /* transmit register */

	volatile unsigned int rr0; /* receive register */
	volatile unsigned int rr1; /* receive register */
	volatile unsigned int rr2; /* receive register */
	volatile unsigned int rr3; /* receive register */

	volatile unsigned int div; /* divide frequency */

	volatile unsigned int len; /* length of data to transmit*/

	volatile unsigned int ctl; /* control register */

	volatile unsigned int ss;  /* slave select */

	volatile unsigned int str; /* start a transmit */


}IPMUX_SPI;

#define SPI_ADDR       0xBF000c00
#define IPMUX_REG_BASE 0xbf000000

#define SPI_FS_FLASH_BASE_ADDR 0xb0000000
#define OPCONN_SPI_FS_BA (SPI_FS_FLASH_BASE_ADDR + 0x220000)

/* NOTE: AT 25F and SST 25LF series are very similar,
 * but commands for sector erase and chip id differ...
 */

#define FLASH_PAGESIZE      256

/* Flash opcodes. */
#define OPCODE_WREN     0x06    /* Write enable */
#define OPCODE_RDSR     0x05    /* Read status register */
#define OPCODE_READ     0x03    /* Read data bytes (low frequency) */
#define OPCODE_FAST_READ    0x0b    /* Read data bytes (high frequency) */
#define OPCODE_PP       0x02    /* Page program (up to 256 bytes) */
#define OPCODE_BE_4K        0x20    /* Erase 4K block */
#define OPCODE_BE_32K       0x52    /* Erase 32K block */
#define OPCODE_SE       0xd8    /* Sector erase (usually 64K) */

#define OPCODE_RDID     0x9f    /* Read JEDEC ID */

/* Status Register bits. */
#define SR_WIP          1   /* Write in progress */
#define SR_WEL          2   /* Write enable latch */
/* meaning of other SR_* bits may differ between vendors */
#define SR_BP0          4   /* Block protect 0 */
#define SR_BP1          8   /* Block protect 1 */
#define SR_BP2          0x10    /* Block protect 2 */
#define SR_SRWD         0x80    /* SR write protect */

/* Define max times to check status register before we give up. */
#define MAX_READY_WAIT_COUNT    100000


#ifdef CONFIG_MTD_PARTITIONS
#define mtd_has_partitions()    (1)
#else
#define mtd_has_partitions()    (0)
#endif

/****************************************************************************/

struct opconn_spi {
	struct spi_device   *spi;
	struct semaphore    lock;
	struct mtd_info     mtd;
	unsigned        partitioned:1;
	u8          erase_opcode;
	u8          command[4];
};

void spi_write_enable(void);
void spi_write_status(void);

static inline struct opconn_spi *mtd_to_opconn_spi(struct mtd_info *mtd)
{
	return container_of(mtd, struct opconn_spi, mtd);
}

/****************************************************************************/

/*
 * Set write enable latch with Write Enable command.
 * Returns negative if error occurred.
 */
static inline int write_enable(struct opconn_spi *flash)
{
	return 0;
}

/* read spi flash status register */
unsigned long spi_read_status(void)
{
	int busy,i;
	volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

	p_spi->ctl = 0x24;

	p_spi->div = 0x01; /* set divide frequency */

	p_spi->len = 0x10;/* set transmit data length */

	p_spi->tr0 = 0x0500;/* set transmit register */

	p_spi->ss = 0x01;/* set slave select */

	p_spi->str= 0x01;/* start transmit */

	busy = 1;
	i=1;
	while(busy){
		busy = p_spi->str;
		i ++;
		if (i > SPI_ERASE_MAX_WAIT) {
			printk("timeout when %s():%d.\n", __FUNCTION__, __LINE__);
			return 0;
		}
	}

	return  p_spi->rr0;

}


/****************************************************************************/

/*
 * MTD implementation
 */

int spi_erase_one_sector(unsigned long ofs, unsigned char erasecode)
{
	int busy,i;
	volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

	spi_write_enable();

	p_spi->ctl = 0x24;

	p_spi->div = 0x01;/* set divide frequency */

	p_spi->len = 0x20;/* set transmit data length */

	/*4k or 64K*/
	p_spi->tr0 = (erasecode<<24 & 0xff000000) | ofs;/* set transmit register */

	p_spi->ss = 0x01;/* set slave select */

	p_spi->str= 0x01;/* start transmit */

	busy = 1;
	i = 1;
	while(busy){

		busy = p_spi->str;
		i ++;
		if (i > SPI_ERASE_MAX_WAIT) {
			printk("timeout when %s():%d.\n", __FUNCTION__, __LINE__);
			return -1;
		}
	}

	busy = 1;
	i = 1;
	while(busy){
		busy = spi_read_status();
		busy = busy & 0x3;
		i ++;
		if (i > SPI_ERASE_MAX_WAIT) {
			printk("timeout when %s():%d.\n", __FUNCTION__, __LINE__);
			return -1;
		}

		if (0 == (i % 0x10000)) {
			printk(".");
		}
	}

	return 0;
}

/*
 * Erase an address range on the flash chip.  The address range may extend
 * one or more erase sectors.  Return an error is there is a problem erasing.
 */
static int opconn_spi_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct opconn_spi *flash = mtd_to_opconn_spi(mtd);

	u32 addr,len;

	/* hide by zttan
	printk("%s: %s %s 0x%08x, len %d\n",
			flash->spi->dev.bus_id, __FUNCTION__, "at",
			(u32)instr->addr, instr->len);
	*/

	/* sanity checks */
	if (instr->addr + instr->len > flash->mtd.size)
		return -EINVAL;
	if ((instr->addr % mtd->erasesize) != 0
			|| (instr->len % mtd->erasesize) != 0) {
		return -EINVAL;
	}

	addr = instr->addr;
	len = instr->len;

	down(&flash->lock);

	/* REVISIT in some cases we could speed up erasing large regions
	 * by using OPCODE_SE instead of OPCODE_BE_4K
	 */

	/* now erase those sectors */
	while (len) {

		//printk("erase addr 0x%x, code : flash->erase_opcode 0x%x.\n", addr | 0x100000 , flash->erase_opcode);
		//bdump(0xbfc00000, 10);
		if (spi_erase_one_sector(addr, flash->erase_opcode)) {
			instr->state = MTD_ERASE_FAILED;
			up(&flash->lock);
			return -EIO;
		}
		//bdump(0xbfc00000, 10);

		addr += mtd->erasesize;
		len -= mtd->erasesize;
	}

	up(&flash->lock);

	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);

	return 0;
}

/*
 * Read an address range from the flash chip.  The address range
 * may be any size provided it is within the physical boundaries.
 */
static int opconn_spi_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	// printk("sum : 0x%08x[from : 0x%llx, OPCONN_SPI_FS_BA : 0x%x], len %d\n",
	//                (u32)((unsigned int)from + SPI_FS_FLASH_BASE_ADDR), from, OPCONN_SPI_FS_BA, len);
	/*
	   printk(" OPCONN_SPI_FS_BA = 0x%x.\n", OPCONN_SPI_FS_BA);
	   printk(" from = 0x%llx, from = 0x%x, (unsigned int)from = 0x%x.\n", from, from, (unsigned int)from);
	   printk(" (unsigned int)from + OPCONN_SPI_FS_BA = 0x%x.\n", (unsigned int)from + OPCONN_SPI_FS_BA);
	   */
	*retlen = len;
	memcpy(buf, (char *)((unsigned int)from + SPI_FS_FLASH_BASE_ADDR), len);
	return 0;
}

void spi_flash_init (void)
{
	volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

	spi_write_status();
	spi_write_enable();

	p_spi->ctl = 0x24;

	p_spi->div = 0x01;/* set divide frequency */

	p_spi->ss = 0x01;/* set slave select */
}

void spi_write_status(void){
	volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

	spi_write_enable();
	p_spi->ctl = 0x24;

	p_spi->div = 0x01;/* set divide frequency */

	p_spi->len = 0x10;/* set transmit data length */

	p_spi->tr0 = 0x0100;/* set transmit register */

	p_spi->ss = 0x01;/* set slave select */

	p_spi->str= 0x01;/* start transmit */
}


/*open spi flash write enable*/
void spi_write_enable()
{
	int busy,i;
	volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

	/* set control register 0010 0100
	   open automatically Slave Select and set Tx Falling Edge Control
	   */
	p_spi->ctl = 0x24;

	p_spi->div = 0x01; /* set divide frequency */

	p_spi->len = 0x08; /* set transmit data length */

	p_spi->tr0 = 0x06; /* set transmit register */

	p_spi->ss = 0x01; /* set slave select */

	p_spi->str= 0x01; /* start transmit */

	busy = 1;
	i = 1;
	while(busy){
		busy = p_spi->str;

		i ++;
		if (i > SPI_ERASE_MAX_WAIT) {
			printk("timeout when %s():%d.\n", __FUNCTION__, __LINE__);
			return;
		}
	}

}

void spi_write_one_byte(unsigned long ofs, unsigned char value)
{
	int busy,i ;
	volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

	//debugKernelSpi("ofs : 0x%x, value 0x%x.\n",ofs,value);

	spi_write_enable();

	p_spi->ctl = 0x24;

	p_spi->div = 0x01;/* set divide frequency */


	p_spi->len = 32 + 8;/* set transmit data length */

	p_spi->tr1 = 0x02;/* set transmit register */

	p_spi->tr0 = (ofs << 8) | value;/* set transmit register */

	p_spi->ss = 0x01;/* set slave select */

	p_spi->str= 0x01;/* start transmit */

	busy = 1;
	i = 1;
	while(busy){
		busy = p_spi->str;
		i ++;
		if (i > SPI_ERASE_MAX_WAIT) {
			printk("timeout when wait str @ %s():%d.\n", __FUNCTION__, __LINE__);
			return;
		}
	}

	busy = 1;
	i = 1;
	while(busy){
		busy = spi_read_status();
		busy = busy & 0x3;
		i ++;
		if (i > SPI_ERASE_MAX_WAIT) {
			printk("timeout when wait busy @ %s():%d.\n", __FUNCTION__, __LINE__);
			return;
		}
	}
}


/*
 * Write an address range to the flash chip.  Data must be written in
 * FLASH_PAGESIZE chunks.  The address range may be any size provided
 * it is within the physical boundaries.
 */
static int opconn_spi_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	unsigned int i;
	//debugKernelSpi("%s 0x%08x, len %zd, data : 0x%x\n", "to",0x100000 + (u32)to, len, *(int *)buf);

	*retlen = len;
	for (i=0; i<len; i++) {
		spi_write_one_byte(to + i, buf[i]);
	}
	return 0;
}


/****************************************************************************/

/*
 * SPI device driver setup and teardown
 */

struct flash_info {
	char        *name;

	/* JEDEC id zero means "no ID" (most older chips); otherwise it has
	 * a high byte of zero plus three data bytes: the manufacturer id,
	 * then a two byte device id.
	 */
	u32     jedec_id;

	/* The size listed here is what works with OPCODE_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */

	unsigned    sector_size;

	u16     n_sectors;

	u16     flags;
#define SECT_4K     0x01        /* OPCODE_BE_4K works uniformly */
};


/* NOTE: double check command sets and memory organization when you add
 * more flash chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 */
static struct flash_info __devinitdata opconn_spi_data [] = {
	/* REVISIT: fill in JEDEC ids, for parts that have them */
	/* Atmel -- some are (confusingly) marketed as "DataFlash" */
	{ "at25fs010",  0x1f6601, 32 * 1024, 4, SECT_4K, },
	{ "at25fs040",  0x1f6604, 64 * 1024, 8, SECT_4K, },

	{ "at25df041a", 0x1f4401, 64 * 1024, 8, SECT_4K, },

	{ "at26f004",   0x1f0400, 64 * 1024, 8, SECT_4K, },
	{ "at26df081a", 0x1f4501, 64 * 1024, 16, SECT_4K, },
	{ "at26df161a", 0x1f4601, 64 * 1024, 32, SECT_4K, },

	/* xwang 0x1f4701->0x1f4700
	   { "at26df321",  0x1f4701, 64 * 1024, 64, SECT_4K, },
	   */
	{ "at26df321",  0x1f4700, 64 * 1024, 64, },
	{ "mx25",  0xc22016, 64 * 1024, 64, },

	/* Spansion -- single (large) sector size only, at least
	 * for the chips listed here (without boot sectors).
	 */
	{ "s25sl004a", 0x010212, 64 * 1024, 8, },
	{ "s25sl008a", 0x010213, 64 * 1024, 16, },
	{ "s25sl016a", 0x010214, 64 * 1024, 32, },
	{ "s25sl032a", 0x010215, 64 * 1024, 64, },
	{ "s25sl064a", 0x010216, 64 * 1024, 128, },

	/* SST -- large erase sizes are "overlays", "sectors" are 4K */
	{ "sst25vf040b", 0xbf258d, 64 * 1024, 8, SECT_4K, },
	{ "sst25vf080b", 0xbf258e, 64 * 1024, 16, SECT_4K, },
	{ "sst25vf016b", 0xbf2541, 64 * 1024, 32, SECT_4K, },
	{ "sst25vf032b", 0xbf254a, 64 * 1024, 64, SECT_4K, },

	/* ST Microelectronics -- newer production may have feature updates */
	{ "m25p05",  0x202010,  32 * 1024, 2, },
	{ "m25p10",  0x202011,  32 * 1024, 4, },
	{ "m25p20",  0x202012,  64 * 1024, 4, },
	{ "m25p40",  0x202013,  64 * 1024, 8, },
	{ "m25p80",         0,  64 * 1024, 16, },
	{ "m25p16",  0x202015,  64 * 1024, 32, },
	{ "m25p32",  0x202016,  64 * 1024, 64, },
	{ "m25p64",  0x202017,  64 * 1024, 128, },
	{ "m25p128", 0x202018, 256 * 1024, 64, },

	{ "m45pe80", 0x204014,  64 * 1024, 16, },
	{ "m45pe16", 0x204015,  64 * 1024, 32, },

	{ "m25pe80", 0x208014,  64 * 1024, 16, },
	{ "m25pe16", 0x208015,  64 * 1024, 32, SECT_4K, },

	/* Winbond -- w25x "blocks" are 64K, "sectors" are 4K */
	{ "w25x10", 0xef3011, 64 * 1024, 2, SECT_4K, },
	{ "w25x20", 0xef3012, 64 * 1024, 4, SECT_4K, },
	{ "w25x40", 0xef3013, 64 * 1024, 8, SECT_4K, },
	{ "w25x80", 0xef3014, 64 * 1024, 16, SECT_4K, },
	{ "w25x16", 0xef3015, 64 * 1024, 32, SECT_4K, },
	{ "w25x32", 0xef3016, 64 * 1024, 64, SECT_4K, },
	{ "w25q32bv", 0xef4016, 64 * 1024, 64, SECT_4K, },
	{ "w25x64", 0xef3017, 64 * 1024, 128, SECT_4K, },

	{"kh25l", 0xc22016, 64*1024, 64, SECT_4K,},
};


/**
 * spi_write_then_read - SPI synchronous write followed by read
 * @spi: device with which data will be exchanged
 * @txbuf: data to be written (need not be dma-safe)
 * @n_tx: size of txbuf, in bytes
 * @rxbuf: buffer into which data will be read
 * @n_rx: size of rxbuf, in bytes (need not be dma-safe)
 *
 * This performs a half duplex MicroWire style transaction with the
 * device, sending txbuf and then reading rxbuf.  The return value
 * is zero for success, else a negative errno status code.
 * This call may only be used from a context that may sleep.
 *
 * Parameters to this routine are always copied using a small buffer;
 * performance-sensitive or bulk transfer code should instead use
 * spi_{async,sync}() calls with dma-safe buffers.
 */
/*tbd : Now can only read spi flash ID. */
int opconn_spi_write_then_read(struct spi_device *spi,
		const u8 *txbuf, unsigned n_tx,
		u8 *rxbuf, unsigned n_rx)
{
	int busy,i ;
	volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

	p_spi->ctl = 0x24;

	p_spi->div = 0x01; /* set divide frequency */

	p_spi->len = 0x28;/* set transmit data length */

	p_spi->tr1 = 0x9f;/* set transmit register */

	p_spi->tr0 = 0x0;/* set transmit register */

	p_spi->ss = 0x01;/* set slave select */

	p_spi->str= 0x01;/* start transmit */

	busy = 1;
	i = 1;
	while(busy){
		busy = p_spi->str;
		i ++;
		if (i > SPI_ERASE_MAX_WAIT) {
			printk("timeout when %s():%d.\n", __FUNCTION__, __LINE__);
			return -1;
		}
	}

	*(int *)rxbuf = p_spi->rr0;

	return 0;
}

static struct flash_info *__devinit jedec_probe(struct spi_device *spi)
{
	int         tmp;
	u8          code = OPCODE_RDID;
	u8          id[3];
	u32         jedec;
	struct flash_info   *info;

	/* JEDEC also defines an optional "extended device information"
	 * string for after vendor-specific data, after the three bytes
	 * we use here.  Supporting some chips might require using it.
	 */
	tmp = opconn_spi_write_then_read(spi, &code, 1, id, 3);
	if (tmp < 0) {
		DEBUG(MTD_DEBUG_LEVEL0, "%s: error %d reading JEDEC ID\n",
				spi->dev.bus_id, tmp);
		return NULL;
	}
	jedec = id[0];
	jedec = jedec << 8;
	jedec |= id[1];
	jedec = jedec << 8;
	jedec |= id[2];

	for (tmp = 0, info = opconn_spi_data;
			tmp < ARRAY_SIZE(opconn_spi_data);
			tmp++, info++) {
		if (info->jedec_id == jedec)
			return info;
	}
	dev_err(&spi->dev, "unrecognized JEDEC id %06x\n", jedec);
	return NULL;
}


/*
 * board specific setup should have ensured the SPI clock used here
 * matches what the READ command supports, at least until this driver
 * understands FAST_READ (for clocks over 25 MHz).
 */
//static int __init opconn_probe(struct platform_device *plat_dev)
static int __init opconn_probe(struct platform_device *plat_dev)
{
	// struct flash_platform_data  *data;
	struct flash_platform_data *data = plat_dev->dev.platform_data;
	struct opconn_spi         *flash;
	struct flash_info       *info;
	unsigned            i;
	struct spi_device spi_dev;
	struct spi_device *spi = &spi_dev;
	spi->dev = plat_dev->dev;

	/*tbd : Got FS_OFFSET from device */
	//plat_dev->dev.platform_data.flash_platform_data.parts[0].offset
	//printk("@@@@ Dev name : %s, dev : 0x%p 0x%p.\n", plat_dev->name, &(plat_dev->dev), spi->dev.platform_data);
	/* Platform data helps sort out which chip type we have, as
	 * well as how this board partitions it.  If we don't have
	 * a chip ID, try the JEDEC id commands; they'll work for most
	 * newer chips, even if we don't recognize the particular chip.
	 */
	if (data && data->type) {

		for (i = 0, info = opconn_spi_data;
				i < ARRAY_SIZE(opconn_spi_data);
				i++, info++) {
			// debugKernelSpi("data->type 0x%s, info->name 0x%s.\n", data->type, info->name);
			if (strcmp(data->type, info->name) == 0)
			{
				break;
			}

		}

		/* unrecognized chip? */
		if (i == ARRAY_SIZE(opconn_spi_data)) {
			DEBUG(MTD_DEBUG_LEVEL0, "%s: unrecognized id %s\n",
					spi->dev.bus_id, data->type);
			info = NULL;

			/* recognized; is that chip really what's there? */
		} else if (info->jedec_id) {
			struct flash_info   *chip = jedec_probe(spi);

			if (!chip) {
				dev_warn(&spi->dev, "found %s, expected %s\n",
						chip ? chip->name : "UNKNOWN",
						info->name);
				info = NULL;
			}

			if (chip != info) {
				dev_warn(&spi->dev, "found %s, expected %s\n",
						chip ? chip->name : "UNKNOWN",
						info->name);
			}
		}
	} else
		info = jedec_probe(spi);

	if (!info)
		return -ENODEV;

	flash = kzalloc(sizeof *flash, GFP_KERNEL);
	if (!flash)
		return -ENOMEM;

	//flash->spi = spi;
	init_MUTEX(&flash->lock);
	dev_set_drvdata(&spi->dev, flash);

	if (data && data->name)
		flash->mtd.name = data->name;
	else
		flash->mtd.name = spi->dev.bus_id;

	flash->mtd.type = MTD_NORFLASH;
	flash->mtd.writesize = 1;
	/* writeable */
	flash->mtd.flags = MTD_CAP_NORFLASH | MTD_WRITEABLE;
	flash->mtd.size = info->sector_size * info->n_sectors;

	flash->mtd.erase = opconn_spi_erase;
	flash->mtd.read = opconn_spi_read;
	flash->mtd.write = opconn_spi_write;

	/* prefer "small sector" erase if possible */
	if (info->flags & SECT_4K) {
		flash->erase_opcode = OPCODE_BE_4K;
		flash->mtd.erasesize = 4096;
	} else {
		flash->erase_opcode = OPCODE_SE;
		flash->mtd.erasesize = info->sector_size;
	}

	dev_info(&spi->dev, "%s (%d Kbytes)\n", info->name,
			flash->mtd.size / 1024);

	DEBUG(MTD_DEBUG_LEVEL2,
			"mtd .name = %s, .size = 0x%.8x (%uM) "
			".erasesize = 0x%.8x (%uK) .numeraseregions = %d\n",
			flash->mtd.name,
			flash->mtd.size, flash->mtd.size / (1024*1024),
			flash->mtd.erasesize, flash->mtd.erasesize / 1024,
			flash->mtd.numeraseregions);

	if (flash->mtd.numeraseregions)
		for (i = 0; i < flash->mtd.numeraseregions; i++)
			DEBUG(MTD_DEBUG_LEVEL2,
					"mtd.eraseregions[%d] = { .offset = 0x%.8x, "
					".erasesize = 0x%.8x (%uK), "
					".numblocks = %d }\n",
					i, flash->mtd.eraseregions[i].offset,
					flash->mtd.eraseregions[i].erasesize,
					flash->mtd.eraseregions[i].erasesize / 1024,
					flash->mtd.eraseregions[i].numblocks);


	/* partitions should match sector boundaries; and it may be good to
	 * use readonly partitions for writeprotected sectors (BP2..BP0).
	 */
	if (mtd_has_partitions()) {
		struct mtd_partition    *parts = NULL;
		int         nr_parts = 0;

#ifdef CONFIG_MTD_CMDLINE_PARTS

		static const char *part_probes[] = { "cmdlinepart", NULL, };

		printk("CONFIG_MTD_CMDLINE_PARTS\n");
		nr_parts = parse_mtd_partitions(&flash->mtd,
				part_probes, &parts, 0);
#endif

		if (nr_parts <= 0 && data && data->parts) {
			parts = data->parts;
			nr_parts = data->nr_parts;
		}

		if (nr_parts > 0) {
			for (i = 0; i < nr_parts; i++) {
				printk( "partitions[%d] = "
						"{.name = %s, .offset = 0x%.8x, "
						".size = 0x%.8x (%uK) }\n",
						i, parts[i].name,
						parts[i].offset,
						parts[i].size,
						parts[i].size / 1024);
			}
			flash->partitioned = 1;
			return add_mtd_partitions(&flash->mtd, parts, nr_parts);
		}
	} else if (data->nr_parts)
		printk(&spi->dev, "ignoring %d default partitions on %s\n",
				data->nr_parts, data->name);


	return add_mtd_device(&flash->mtd) == 1 ? -ENODEV : 0;
}


static int __exit opconn_remove(struct spi_device *spi)
{
	struct opconn_spi *flash = dev_get_drvdata(&spi->dev);
	int     status;

	/* Clean up MTD stuff. */
	if (mtd_has_partitions() && flash->partitioned)
		status = del_mtd_partitions(&flash->mtd);
	else
		status = del_mtd_device(&flash->mtd);
	if (status == 0)
		kfree(flash);
	return 0;
}

static struct platform_driver opconn_spidrv = {
	.probe      = opconn_probe,
	.remove     = __devexit_p(opconn_remove),
	.driver     = {
		.name   = "Opulan-flash",
		.owner  = THIS_MODULE,
	},
};

int opconn_spi_drv_init(void)
{
	/* register driver */
	return platform_driver_register(&opconn_spidrv);
}

static void opconn_spi_drv_exit(void)
{
	platform_driver_unregister(&opconn_spidrv);
}

module_init(opconn_spi_drv_init);
module_exit(opconn_spi_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("xwang");
MODULE_DESCRIPTION("MTD SPI driver for spi flash chips on opconn");

