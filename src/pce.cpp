/*
 * pce.c
 *
 * contains PC-Engine specific code and data.
 */

/* $Id: pce.cpp,v 1.3 2005/01/18 00:39:03 guest Exp $ */

#include <stdlib.h>
#include <stdio.h>

#include "tool.h"
#include "system.h"
#include "cal.h"
#include "ui.h"
#include "event.h"
#include "pce_vdp.h"
#include "cpu6280int.h" /* FIXME: technically a no-no */

#ifdef PCE_CD_SUPPORT
u8 pce_cd_read(unsigned short address);
void pce_cd_write(unsigned short address, u8 data);
#endif

cal_cpu* pce_cpu;
pce_vdp* pce_video;
int pce_joyswitch;
int pce_country_select; /* 0x40 or 0x00 */
char *pce_mpr; /* FIXME: should only be compiled in when compiling both 6280 cores */
rom_file* pce_rom_file;
u8 *pce_ram;

typedef struct tagTPceTimer{
	u8 preload;
	u8 value;
	u8 status;
}TPceTimer; 

TPceTimer pce_timer;

u32 pce_joypad = 0;

#define PCE_RAMSIZE 0x8000
#define PCE_SAVERAM 0x2000

u8 *pce_mmap[256];

u8 pce_read_mem(cal_cpu* cpu, unsigned long addr)
{
u8 bank= pce_mpr[(addr >> 13) & 7];

	if (bank == 0xff)
	{
		switch (addr & 0x1c00)
		{
		case 0x0000: /* HuC6270 VDC */
			return pce_vdp_read(pce_video, (unsigned short)addr);
			break;
		case 0x0400: /* HuC6260 VCE */
			return pce_vce_read(pce_video, (unsigned short)addr);
			break;
		case 0x0800: /* PSG */
			break;
		case 0x0c00: /* Timers */
			if(addr & 1)
				return pce_timer.status;
			else
				return pce_timer.value;
			break;
		case 0x1000: /* I/O (joypad) */
			joypad_Update(0, &pce_joypad);
			if (pce_joyswitch)
				return (u8)(((pce_joypad >> 4) | pce_country_select) ^ 0xff);
			else
				return (u8)(((pce_joypad & 0x0f) | pce_country_select) ^ 0xff);
			break;
		case 0x1400: /* IRQ control */
			break;
#ifdef PCE_CD_SUPPORT
		case 0x1800: /* PCE CD system */
			return pce_cd_read(addr & 0x1fff);
			break;
#endif
		default:
			printf("pce: I/O read 0x%04hx, returning 0x00.\n", (unsigned int)addr & 0x1fff);
		}
		return 0x00;
	}
	else
	{
		register u8 *memptr= pce_mmap[bank];

		if (!memptr)
		{
			printf("pce: read from NULL memory region 0x%02x (0x%04hx).\n", (unsigned int)bank, (unsigned int)addr);
			system_flags |= F_QUIT;
			return 0;
		}
		else
			return memptr[addr & 0x1fff];
    }
}

void pce_write_mem(cal_cpu* cpu, unsigned long addr, u8 data)
{
#ifdef PCE_CD_SUPPORT
extern int pce_cd_init;
#endif
    
u8 bank= pce_mpr[(addr >> 13) & 7];

	if(bank == 0xff)
	{
		switch (addr & 0x1c00)
		{
		case 0x0000: /* HuC6270 VDC */
			pce_vdp_write(pce_video, (unsigned short)addr, data);
			break;
		case 0x0400: /* HuC6260 VCE */
			pce_vce_write(pce_video, (unsigned short)addr, data);
			break;
		case 0x0800: /* PSG */
			break;
		case 0x0c00: /* Timers */
			if (addr & 1)
			{
				if ((!(pce_timer.status & 1)) && (data & 1))
					pce_timer.value = pce_timer.preload & 0x7f;
			
				pce_timer.status = data;
			}
			else
				pce_timer.preload = data;
			
			break;
		case 0x1000: /* I/O (joypad) */
			pce_joyswitch = data & 1;
			break;
		case 0x1400: /* IRQ control */
			break;
#ifdef PCE_CD_SUPPORT
		case 0x1800:
			pce_cd_write(addr & 0x1fff, data);
			break;
#endif
		default:
			printf("pce: I/O write 0x%04hx = 0x%02x.\n", (unsigned int)addr & 0x1fff, (unsigned int)data);
		}
    }
	else if (bank == 0xf7)	/* Savegame RAM */
		pce_mmap[bank][addr & 0x1fff] = data;
	else if (bank > 0xf7)	/* Normal RAM */
		pce_mmap[bank][addr & 0x1fff] = data;
#ifdef PCE_CD_SUPPORT
    else if (pce_cd_init && (((bank >= 0x68) && (bank <= 0x87))) && (pce_mmap[bank]))	/* More RAM */
		pce_mmap[bank][addr & 0x1fff] = data;
#endif
	else if((pce_rom_file->size == 0x280200) || (pce_rom_file->size == 0x280000))	/* It's in the ROM area */
	{																				/* support for SF2CE silliness */
		if ((addr & 0x1ffc) == 0x1ff0)
		{
			int i;
		
			pce_mmap[0x40] = pce_mmap[0] + 0x80000;
			pce_mmap[0x40] += (addr & 3) * 0x80000;
			
			for (i = 0x41; i <= 0x7f; i++)
				pce_mmap[i] = pce_mmap[i-1] + 0x2000;
	    }
	}
	else
		printf("pce: rom write %02x:%04hx = 0x%02x.\n", (unsigned int)bank, (unsigned int)addr & 0x1fff, (unsigned int)data);
}

u8 pce_mem_read_byte(unsigned short addr)
{
	return pce_read_mem(pce_cpu, addr);
}

memread8_t pce_memread[1] = {(memread8_t)pce_read_mem,};
memwrite8_t pce_memwrite[1] = {(memwrite8_t)pce_write_mem,};

void pce_st0_hack(u8 data)
{
	pce_vdp_write(pce_video, 0, data);
}

void pce_st1_hack(u8 data)
{
	pce_vdp_write(pce_video, 2, data);
}

void pce_st2_hack(u8 data)
{
	pce_vdp_write(pce_video, 3, data);
}

void pce_scanline(void)
{
	if (pce_vdp_periodic(pce_video))
		pce_cpu->irq(pce_cpu, 2);
}

void pce_timerhit(void)
{
	if(!(pce_timer.status & 1))
		return;

	if(!pce_timer.value--)
	{
		pce_timer.value = pce_timer.preload & 0x7f;
		/* FIXME: TIQD? */
		pce_cpu->irq(pce_cpu, 4);
	}
}

event pce_events[] = {
    {NULL, 0, 496, pce_scanline},
    {NULL, 0, 1115, pce_timerhit},
    {NULL, 0, 496 * 262, NULL},
};

void pce_init_events(event_scheduler* scheduler)
{
u32 i;

	for (i = 0; i < (sizeof(pce_events) / sizeof(pce_events[0])); i++)
		event_register(scheduler, &pce_events[i]);
}

u8 bitswizzle[0x100];

void pce_swizzle_bits(void)
{
int i;

	for(i = 0; i < 0x100; i++)
	{
		bitswizzle[i] = (i >> 7) & 0x01;
		bitswizzle[i] |= (i >> 5) & 0x02;
		bitswizzle[i] |= (i >> 3) & 0x04;
		bitswizzle[i] |= (i >> 1) & 0x08;
		bitswizzle[i] |= (i << 1) & 0x10;
		bitswizzle[i] |= (i << 3) & 0x20;
		bitswizzle[i] |= (i << 5) & 0x40;
		bitswizzle[i] |= (i << 7) & 0x80;
	}
    
	for(i = 0; i < pce_rom_file->size; i++)
		pce_rom_file->data[i] = bitswizzle[pce_rom_file->data[i]];

}

void pce_init_mmap(void)
{
int offset= 0;
int romsize= pce_rom_file->size;
u8	*romimage= pce_rom_file->data;
int i;

    
	if(romsize & 0x200)
	{
		printf("pce_init_mmap(): compensating for 512-byte header.\n");
		offset = 0x200;
	}
    
	for(i = 0; i < 0xf7; i++)
		pce_mmap[i] = ((((i * 0x2000) + offset) < romsize)? (romimage + (i * 0x2000) + offset): 0);

	if(romsize == (offset + 0x60000))
	{
		printf("pce: compensating for possible split rom.\n");
		
		for(i = 0; i < 0x10; i++)
			pce_mmap[0x40 + i] = romimage + offset + (i * 0x2000) + 0x40000;
    }
    
    if(romsize == (offset + 0x80000))
	{
		printf("pce: compensating for possible split rom.\n");
		
		for (i = 0; i < 0x20; i++)
			pce_mmap[0x40 + i] = romimage + offset + (i * 0x2000) + 0x40000;
    }
    
	pce_mmap[0xf7] = pce_ram + PCE_RAMSIZE;

	for (i = 0xf8; i < 0xfc; i++)
		pce_mmap[i] = pce_ram + ((i - 0xf8) * 0x2000);

	for (i = 0xfc; i < 0x100; i++)
		pce_mmap[i] = NULL;
}

void pce_run(rom_file* romfile)
{
event_scheduler* scheduler;
    
	pce_ram = new u8[PCE_RAMSIZE + PCE_SAVERAM];
	
	pce_rom_file = romfile;
	pce_init_mmap();

/*      pce_cpu = cal_create(CPUT_MARAT6280); */
/*      pce_mpr = pce_cpu->data.d_marat6280->mpr; */
	pce_cpu = cal_create(CPUT_CPU6280);
	pce_mpr = (char*)pce_cpu->data.d_cpu6280->mpr;

	scheduler= new_event_scheduler(cal_event_delay_callback, pce_cpu);
    
	pce_video = pce_vdp_create();

/*     pce_swizzle_bits(); */
	pce_country_select = 0x40;
    
	if((pce_ram) && (pce_cpu) && (pce_video) && (scheduler))
	{
		pce_init_events(scheduler);
		
		pce_cpu->setzpage(pce_cpu, pce_ram);
		pce_cpu->setmmu8(pce_cpu, 0, 0, pce_memread, pce_memwrite);
		pce_cpu->reset(pce_cpu);
			
		emulation_SetTimeslice((void (*)(void *)) &event_timeslice, scheduler);
    }
	else
		printf("out of memory\n");
}
