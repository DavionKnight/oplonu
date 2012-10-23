/******************************************************************************/
/**  COPYRIGHT (C) 2003- Opulan, INC. ALL RIGHTS RESERVED.                   **/
/**--------------------------------------------------------------------------**/
/** This software embodies materials and concepts which are proprietary and  **/
/** confidential to Opulan, Inc.                                             **/
/**--------------------------------------------------------------------------**/
/**                                                                          **/
/******************************************************************************/
#include <common.h>
#include <configs/opipmux.h>
#ifdef CFG_SPI_FLASH

#include "ipmux_spi.h"

flash_info_t flash_info[CFG_MAX_FLASH_BANKS];

UINT32 CS0_REDEFINED = PHYS_FLASH_1; /*zw 921*/
void spi_init()
{
	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		/* set control register 0010 0100
		   open automatically Slave Select and set Tx Falling Edge Control
		*/
		p_spi->ctl = 0x24; 

		p_spi->div = 0x01; /* set divide frequency */

		spi_write_status(0x00);//clear write protect
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);

		/* set control register 0010 0100
		   open automatically Slave Select and set Tx Falling Edge Control
		*/
		p_spi->ctl = 0x24; 

		p_spi->div = 0x01; /* set divide frequency */

		spi_write_enable();

		spi_write_status(0x00);//clear write protect
	}
}

/*open spi flash write enable*/
void spi_write_enable()
{
	int busy;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		p_spi->len = 0x08; /* set transmit data length */

		p_spi->tr0 = 0x06; /* set transmit register */

		p_spi->ss = 0x01; /* set slave select */

		p_spi->str= 0x01; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		p_spi->len = 0x0808; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr4 = 0x06; /* set transmit register */
		}
		else
		{
			p_spi->tr0 = 0x06; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}
	}
}

/* read spi flash status register */
unsigned long spi_read_status()
{
	int busy;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		p_spi->len = 0x10;/* set transmit data length */

		p_spi->tr0 = 0x0500;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}
		
		return	p_spi->rr0;
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		p_spi->len = 0x1010; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr4 = 0x0500; /* set transmit register */
		}
		else
		{
			p_spi->tr0 = 0x0500; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		if (buf_id)
		{
			return p_spi->rr4;
		}
		else
		{
			return p_spi->rr0;
		}
	}
}

void spi_write_status(unsigned int val)
{ 
	int busy;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		spi_write_enable();

		p_spi->len = 0x10;/* set transmit data length */

		p_spi->tr0 = 0x0100 + val;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		spi_write_enable();

		p_spi->len = 0x1010; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr4 = 0x0100 + val; /* set transmit register */
		}
		else
		{
			p_spi->tr0 = 0x0100 + val; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */
	}
}

unsigned long spi_read_one_word(unsigned long ofs)
{
	int busy;

	//ofs += SPI_AGENT_READ_OFS;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		p_spi->len = 0x40;/* set transmit data length */

		p_spi->tr1 = 0x03000000 + ofs;/* set transmit register */

		p_spi->tr0 = 0x0;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		return 	p_spi->rr0;
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		p_spi->len = 0x4040; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr5 = 0x03000000 + ofs; /* set transmit register */
			p_spi->tr4 = 0; /* set transmit register */
		}
		else
		{
			p_spi->tr1 = 0x03000000 + ofs; /* set transmit register */
			p_spi->tr0 = 0; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		if (buf_id)
		{
			return p_spi->rr4;
		}
		else
		{
			return p_spi->rr0;
		}
	}
}

void spi_write_one_word(unsigned long ofs,  long value)
{
	int busy;

	ofs += SPI_AGENT_READ_OFS;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		spi_write_enable();

		p_spi->len = 0x40;/* set transmit data length */

		p_spi->tr1 = 0x02000000 + ofs;/* set transmit register */

		p_spi->tr0 = value;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		busy = 1;
		while(busy){
			busy = spi_read_status();
			busy = busy & 0x3;
		}
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		spi_write_enable();

		p_spi->len = 0x4040; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr5 = 0x02000000 + ofs; /* set transmit register */
			p_spi->tr4 = value; /* set transmit register */
		}
		else
		{
			p_spi->tr1 = 0x02000000 + ofs; /* set transmit register */
			p_spi->tr0 = value; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
			//udelay(50);
		}

		busy = 1;
		while(busy){
			busy = spi_read_status();
			busy = busy & 0x3;
			//udelay(50);
		}
	}
}


void spi_write_one_word_directly(unsigned long ofs,  long value)
{
	int busy;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		spi_write_enable();

		p_spi->len = 0x40;/* set transmit data length */

		p_spi->tr1 = 0x02000000 + ofs;/* set transmit register */

		p_spi->tr0 = value;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		busy = 1;
		while(busy){
			busy = spi_read_status();
			busy = busy & 0x3;
		}
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		spi_write_enable();

		p_spi->len = 0x4040; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr5 = 0x02000000 + ofs; /* set transmit register */
			p_spi->tr4 = value; /* set transmit register */
		}
		else
		{
			p_spi->tr1 = 0x02000000 + ofs; /* set transmit register */
			p_spi->tr0 = value; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
			//udelay(50);
		}

		busy = 1;
		while(busy){
			busy = spi_read_status();
			busy = busy & 0x3;
			//udelay(50);
		}
	}
}


void spi_protect_sector(unsigned long ofs)
{
	int busy,i;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		spi_write_enable();

		p_spi->len = 0x20;/* set transmit data length */

		p_spi->tr0 = 0x36000000 + ofs;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		busy = 1;
		while(busy){
			busy = spi_read_status();
			busy = busy & 0x3;
		}
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		spi_write_enable();

		p_spi->len = 0x2020; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr4 = 0x36000000 + ofs; /* set transmit register */
		}
		else
		{
			p_spi->tr0 = 0x36000000 + ofs; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		busy = 1;
		while(busy){
			busy = spi_read_status();
			busy = busy & 0x3;
		}
	}
}

void spi_unprotect_sector(unsigned long ofs)
{
	int busy,i;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		spi_write_enable();

		p_spi->len = 0x20;/* set transmit data length */

		p_spi->tr0 = 0x39000000 + ofs;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		busy = 1;
		while(busy){
			busy = spi_read_status();
			busy = busy & 0x3;
		}
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		spi_write_enable();

		p_spi->len = 0x2020; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr4 = 0x39000000 + ofs; /* set transmit register */
		}
		else
		{
			p_spi->tr0 = 0x39000000 + ofs; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		busy = 1;
		while(busy){
			busy = spi_read_status();
			busy = busy & 0x3;
		}
	}
}


void spi_erase_one_sector(unsigned long ofs)
{
	int busy,i;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		spi_write_enable();

		p_spi->len = 0x20;/* set transmit data length */

		p_spi->tr0 = 0xd8000000 + ofs*SECTOR_SIZE;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		spi_write_enable();

		p_spi->len = 0x2020; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr4 = 0xd8000000 + ofs*SECTOR_SIZE; /* set transmit register */
		}
		else
		{
			p_spi->tr0 = 0xd8000000 + ofs*SECTOR_SIZE; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}
	}

	busy = 1;
	while(busy){
		busy = spi_read_status();
		busy = busy & 0x3;
	}
}

void spi_erase_chip()
{
	int busy,i;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		spi_write_enable();

		p_spi->len = 0x08;/* set transmit data length */

		p_spi->tr0 = 0xc7;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		spi_write_enable();

		p_spi->len = 0x0808; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr4 = 0xc7; /* set transmit register */
		}
		else
		{
			p_spi->tr0 = 0xc7; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}
	}

	busy = 1;
	while(busy){
		busy = spi_read_status();

		busy = busy & 0x3;
		for(i=0;i<100000;i++){

		}
		printf(".");
	}

	printf("\nErase chip finished.\n");

}

unsigned int spi_read_man()
{
	int busy;

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		volatile struct ipmux_spi *p_spi = (volatile struct ipmux_spi*)(SPI_ADDR);

		p_spi->len = 0x28;/* set transmit data length */

		p_spi->tr1 = 0x9f;/* set transmit register */

		p_spi->tr0 = 0x0;/* set transmit register */

		p_spi->ss = 0x01;/* set slave select */

		p_spi->str= 0x01;/* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		return 	p_spi->rr0;
	}
	else
	{
		volatile struct onu_respin_spi *p_spi = (volatile struct onu_respin_spi*)(SPI_ADDR);
		unsigned int buf_id;

		p_spi->len = 0x2828; /* set transmit data length */

		buf_id = (((p_spi->intr) & 0x10) >> 4) & 0x1; /* get buffer id */

		if (buf_id)
		{
			p_spi->tr5 = 0x9f; /* set transmit register */
			p_spi->tr4 = 0; /* set transmit register */
		}
		else
		{
			p_spi->tr1 = 0x9f; /* set transmit register */
			p_spi->tr0 = 0; /* set transmit register */
		}

		p_spi->ss = 0x01; /* set slave select */

		p_spi->ready = 1 << buf_id; /* set buffer ready */

		p_spi->str= SPI_RESPIN_START; /* start transmit */

		busy = 1;
		while(busy){
			busy = p_spi->str;
		}

		if (buf_id)
		{
			return p_spi->rr4;
		}
		else
		{
			return p_spi->rr0;
		}
	}
}


void burn_spi_bootloader(unsigned long *src, unsigned int count)
{
	unsigned long val;
	unsigned long *p;
	int i,busy;
	unsigned long ofs;
	flash_info_t *info;
	for(i=BOOTLOADER_BEGIN;i<BOOTLOADER_SECTOR;i++)
	{
		//ofs = SECTOR_SIZE*i;  /*comment by kxu 20110302*/
		ofs = i;
		spi_erase_one_sector(ofs);
		printf(".");
	}
	
	printf("\nerase chip finished.\n");
	printf("begin burn...\n");
	p = src;
	
	for(i=0;i<count;i=i+4){
		
		val = *p;
		
    	//printf("i:%d,p:%x,val:%x\n",i,p,val);
		spi_write_one_word(i,val);

		p = p+1;
		if(i%20000 == 0){
			
			printf(".");			
			
		}

	}

	printf("Burn SPI flash finished\n");
	
}

void burn_spi_os(unsigned long *src, unsigned int count)
{
	unsigned long val;
	unsigned long *p;
	int i,sectors,busy;
	unsigned long ofs;

	sectors = 6+(count/0x10000)+1; 
	for(i=6;i<sectors;i++){ //from 0xbfc40000
		//ofs = 0x10000*i;/*comment by kxu 20110302*/
		ofs = i;
		//printf("erase sector %d \n",ofs);
		spi_erase_one_sector(ofs);
		printf(".");			
	}
	printf("\nerase %d sectors finished.\n",sectors);
	printf("begin burn...\n");
	p = src;
	int w = 0;
	for(i=0x60000;i<0x60000+count;i=i+4){
		
		val = *p;
		
    	 //printf("i:%d,p:%x,val:%x\n",i,p,val);
		spi_write_one_word(i,val);

		p = p+1;
		if(i%20000 == 0){
			w++;
			printf(".");			
			
		}

	}

	printf("\nBurn SPI flash finished\n");
}

OPL_STATUS spiProgramUb(void)
{
	OPL_STATUS retVal = 0;
	__u8 *tmp_buf = 0xa1300000;
	__u32 volatile count;
	__u32 header[4] ={0x03030000 ,0x10000000,0x00000000,0x00000000};// {0x030b0108 ,0x10000000,0x00000000,0x00000000};

	run_command("Xdown 0xa1300000", 0);

	for(count=0;count<4;count++)
	{
	    spi_erase_one_sector(count);
	}
	count=0;
	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==2))
	{
		/*program 4 word head data to flash*/
		for(;count<4;count++)
		{
		    spi_write_one_word(&header[count],4*count);
		}
	}
	/*program file to flash*/
	for(;count<(0x40000/4);count++)
	{
		spi_write_one_word(4*count,tmp_buf+4*count);
	    if(!(count%656))
		{
			printf("\b\b\b");
			printf("%d%%",(count*100)/0x10000);
		}
	}

	printf("\nProgram file finished.\n");

	return retVal;
}

/*-----------------------------------------------------------------------
 */

/*
 * The following code cannot be run from FLASH!
 */
static void flash_get_offsets (flash_info_t *info)
{
	int i;
       unsigned int sect_size = 0x10000 ;    /* 64k sectors*/

       for( i = 0; i < info->sector_count; i++ )
            info->start[i] = PHYS_FLASH_1 + (i * sect_size);   /*each 64k*/
    
}

static flash_info_t *flash_get_info(ulong base)
{
    int i;
    flash_info_t * info;

    for (i = 0; i < CFG_MAX_FLASH_BANKS; i ++) {
        info = & flash_info[i];
        if (info->start[0] <= base && base < info->start[0] + info->size)
            break;
    }

    return i == CFG_MAX_FLASH_BANKS ? 0 : info;
}

ulong flash_get_size (flash_info_t *info)
{
    unsigned int id ;

    id = spi_read_man();

	printf("========================x\n");
	printf("flash id %x\n",id);
    switch (id >> 24 & 0xff) {

	case FLASH_MAN_ATMEL:
		printf("ATMEL SPI flash detected.\n");
        info->flash_id = FLASH_MAN_ATMEL;	 
        break;
	case FLASH_MAN_MX:
		printf("MX SPI flash detected.\n");
        info->flash_id = FLASH_MAN_MX;	 
        break;
	case FLASH_MAN_WB:
		printf("WB SPI flash detected.\n");
        info->flash_id = FLASH_MAN_WB;	 
        break;
    case FLASH_MAN_SST:
	    printf("SST SPI flash detected.\n");
	    info->flash_id = FLASH_MAN_SST;	 
	    break;		 
    case FLASH_MAN_ST:
	    printf("ST SPI flash detected.\n");
	    info->flash_id = FLASH_MAN_SST;	 
	    break;		 
    default:
        printf("UNKNOWN spi flash Factory 0x%X.\n",id >> 24 & 0xff);
        info->flash_id = FLASH_UNKNOWN;
        info->sector_count = 0;
        info->size = 0;
        break;
    }

    if (info->flash_id != FLASH_UNKNOWN) {
		switch (id >> 16 & 0xff) {

    		case FLASH_AT26_ATMEL:    
				info->sector_count = 64;
        		info->size = 0x00400000;
        		break;             
			case FLASH_MX_L320:    
				info->sector_count = 64;
        		info->size = 0x00400000;
        		break; 
		    case FLASH_WB_25X16:    
				info->sector_count = 64;
        		info->size = 0x00400000;
        		break; 
		    case FLASH_SST_25VF:	
			    info->sector_count = 64;
			    info->size = 0x00400000;
			    break; 					
     		default:
        		printf("--------- FLASH_UNKNOWN .\n");
        		info->flash_id = FLASH_UNKNOWN;
				info->sector_count = 0;
				info->size = 0;
        		return (0);         /* => no or unknown flash */
    		}
    	}
    flash_get_offsets(info);
    return (info->size);
}

unsigned long flash_init (void)
{
	unsigned long size = 0;
	int i;

	spi_init();

	if ((((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==1))
	{
		ipMuxRegWrite(REG_CS0_BASE_ADDR, (PHYS_FLASH_1&0x1FFFFFFF)>>12);
	}

	for (i=0; i < CFG_MAX_FLASH_BANKS; ++i) {
				
		memset(&flash_info[i], 0, sizeof(flash_info_t));
		size = flash_get_size(&flash_info[i]);

		flash_protect(FLAG_PROTECT_SET,
              BOOTLOADER_ADDR,
              BOOTLOADER_ADDR+SECTOR_SIZE*4-1,
              &flash_info[i]);
	}

	cs0Cfgdefault();  
	return size; 
}

void flash_print_info (flash_info_t *info)
{
	uchar *fmt;
	int i;
	reCfgCs0(flash_info[0].size);	
	if (info->flash_id == FLASH_UNKNOWN) {
		printf ("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id & 0xff) {
		case FLASH_MAN_ATMEL:
			fmt = "AT26S321 SPI flash (32 Mbit)";
			break;
		case FLASH_MAN_MX:
			fmt = "MX SPI flash (32 Mbit)";
			break;
		case FLASH_MAN_SST:
			fmt = "SST SPI flash (32 Mbit)";
			break;
		case FLASH_MAN_WB:
			fmt = "WB SPI flash (32 Mbit)";
			break;
		default:
			fmt = "Unknown Chip Type\n";
			break;
	}

	printf (fmt);

	printf ("  ID: %x, Size: %ld MB in %d Sectors\n",
		info->flash_id & 0xff,
		info->size >> 20,
		info->sector_count);

	printf ("  Sector Start Addresses:");

	for (i=0; i<info->sector_count; ++i) {
		if ((i % 5) == 0) {
			printf ("\n   ");
		}
		printf (" %08lX%s", info->start[i],
			info->protect[i] ? " (RO)" : "     ");
	}
	printf ("\n\n  flash space admesure:\n");
	printf("\t\t\t\tuboot space  : 0x%x --- 0x%x\n",info->start[0],info->start[0]+envoffset-1);
	printf("\t\t\t\tenv space    : 0x%x --- 0x%x\n",info->start[0]+envoffset,info->start[0]+cfg_offset-1);
	printf("\t\t\t\tcfg space    : 0x%x --- 0x%x\n",info->start[0]+cfg_offset,info->start[0]+knl_1offset-1);
	printf("\t\t\t\tos1 space    : 0x%x --- 0x%x\n",info->start[0]+knl_1offset,info->start[0]+knl_2offset-1);
	printf("\t\t\t\tos2 space    : 0x%x --- 0x%x\n",info->start[0]+knl_2offset,info->start[0]+ flash_info[0].size-1);
	cs0Cfgdefault();
	printf ("\n");	
}



int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
	ulong data = 0; /* 16 or 32 bit word, matches flash bus width on MPC8XX */
	ulong ofs;
	int bytes;	  /* number of bytes to program in current word		*/
	int left;	  /* number of bytes left to program			*/
	int i, res;

	reCfgCs0(flash_info[0].size);
	for (left = cnt, res = 0;
		left > 0 && res == 0;
		addr += sizeof(data), left -= sizeof(data) - bytes) {

		bytes = addr & (sizeof(data) - 1);
		addr &= ~(sizeof(data) - 1);

		/* combine source and destination data so can program
		 * an entire word of 16 or 32 bits
		 */
/*wangxin:big endian*/
#ifdef BIGENDIAN	
		for (i = 0; i < sizeof(data); i++) {
			data <<= 8;
			if (i < bytes || i - bytes >= left )
			{
				data += *((uchar *)addr + i);
				udelay(5000);/*kxu add at 20100331 for spi host read done*/          
			}
			else
			data += *src++;
		}
#else
/*wangxin:little endian*/
		add=0;
		for (i = 0; i < sizeof(data); i++) {
			data <<= 8;
			if (i < bytes || i - bytes >= left )
			data += *((uchar *)addr +(sizeof(data)-i));
			else
			{
				data += *(src+sizeof(data)-1-i);
				add++;
			}
		}
		src += add;
#endif	

		/* write one word to the flash */
       	ofs = addr - PHYS_FLASH_1;
		spi_write_one_word(ofs,data);
	}
	cs0Cfgdefault();
    return (res);
}

int	flash_erase (flash_info_t *info, int s_first, int s_last)
{
	int num,i,prot;
	int sect;
	unsigned int ofs;
	__u32 header[4] ={0x01030200 ,0x10000000,0x00000000,0x00000000};// {0x030b0108 ,0x10000000,0x00000000,0x00000000};
	int cnt;

	reCfgCs0(flash_info[0].size);
	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf ("- missing\n");
		} else {
			printf ("- no sectors to erase\n");
		}
		cs0Cfgdefault(); /*zw 922*/
		return 1;
	}

	prot = 0;
	for (sect=s_first; sect<=s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf ("- Warning: %d protected sectors will not be erased!\n",
			prot);
		cs0Cfgdefault(); /*zw 922*/
		return 1;
	} else {
		printf ("\n");
	}

	num = s_last - s_first +1;
	for(i=0;i<num;i++){
		//ofs = 0x10000 * (s_first+i);		/*comment by kxu 20110302*/
		ofs = (s_first+i);		
		spi_erase_one_sector(ofs);
		if ((ofs == 0) && 
			(((*(volatile u32 *)(IPMUX_REG_BASE))&0xF)==2))
		{
			/*program 4 word head data to flash*/
			for(cnt = 0;cnt<4;cnt++)
			{
			    spi_write_one_word_directly(4*cnt,header[cnt]);
			}
		}
		printf(".");
	}
	cs0Cfgdefault();
	printf("\n");
}

#endif /*CFG_SPI_FLASH*/
