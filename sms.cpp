#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "system.h"
#include "cal.h"
#include "tool.h"
#include "sms9918.h"
#include "sms_psg.h"
#include "event.h"

#define SMS_RAMSIZE 0xa000 /* 0x2000 ram + 0x8000 cart ram */

unsigned char *sms_memory;
cal_cpu* sms_cpu;
sms9918* sms_vdp;

unsigned char *sms_rombase;
unsigned char *sms_bank0;
unsigned char *sms_bank1;
unsigned char *sms_bank2;
unsigned char *sms_cart_memory;
int sms_romsize;
int sms_uses_battery;
rom_file* sms_battery_file;

u32 sms_joypad_1= 0;
u32 sms_joypad_2= 0;

unsigned char sms_pageregs[4];

int is_gg;

ranged_mmu sms_mmu[] = {
    {0x0000, 0x03ff, NULL, 0, 0},
    {0x0400, 0x3fff, NULL, 0, 0},
    {0x4000, 0x7fff, NULL, 0, 0},
    {0x8000, 0xbfff, NULL, 0, 0},
    {0xc000, 0xdfff, NULL, 0, 0},
    {0xe000, 0xffff, NULL, 0, 0},
};

void sms_pagesync(void)
{
	sms_bank0 = &sms_rombase[(sms_pageregs[1] << 14) & (sms_romsize - 1)];
	sms_bank1 = &sms_rombase[(sms_pageregs[2] << 14) & (sms_romsize - 1)];
    
	if (sms_pageregs[0] & 0x08)
	{
		sms_bank2 = &sms_cart_memory[((sms_pageregs[0] & 0x04) << 12)];
		sms_uses_battery = 1;
	}
	else
		sms_bank2 = &sms_rombase[(sms_pageregs[3] << 14) & (sms_romsize - 1)];

	sms_mmu[1].data = sms_bank0;
	sms_mmu[2].data = sms_bank1 - 0x4000;
	sms_mmu[3].data = sms_bank2 - 0x8000;
}

void sms_init_mmu(rom_file* romfile)
{
	if (romfile->size & 0x200)
	{
		sms_rombase = romfile->data + 0x200;
		sms_romsize = romfile->size - 0x200;
    }
	else
	{
		sms_rombase = romfile->data;
		sms_romsize = romfile->size;
	}

	if(sms_romsize & 0x40)
	{
		/* Okay, who the heck thought up the 64-byte footer? */
		printf("64-byte footer detected, ignoring.\n");
		sms_romsize -= 0x40;
	}
    
	sms_mmu[0].data = sms_rombase;
	sms_mmu[4].data = sms_memory - 0xc000;
	sms_mmu[5].data = sms_memory - 0xe000;
	
	sms_pageregs[0] = 0;
	sms_pageregs[1] = 0;
	sms_pageregs[2] = 1;
	sms_pageregs[3] = 2;
	sms_pagesync();
}

unsigned char sms_read(cal_cpu* cpu, unsigned long addr)
{
	if (addr < 0x400)
		return sms_rombase[addr];
	else if (addr < 0x4000)
		return sms_bank0[addr];
	else if (addr < 0x8000)
		return sms_bank1[addr & 0x3fff];
	else if (addr < 0xc000)
		return sms_bank2[addr & 0x3fff];
	else
		return sms_memory[addr & 0x1fff];

}

void sms_write(cal_cpu* cpu, unsigned long addr, unsigned char data)
{
	if(addr >= 0xc000)
	{
		if (addr >= 0xfffc)
		{
			sms_pageregs[addr & 3] = data;
			sms_pagesync();
		}
		
		sms_memory[addr & 0x1fff] = data;
    }
	else if (addr > 0x8000)
	{
		if (sms_pageregs[0] & 0x08)
			sms_bank2[addr & 0x3fff] = data;
    }
}

memread8_t sms_readfunc = (memread8_t)sms_read;
memwrite8_t sms_writefunc = (memwrite8_t)sms_write;

unsigned char sms_io_read(cal_cpu* cpu, unsigned long address)
{
	if ((address == 0x00) && is_gg)
		return (unsigned char)((sms_joypad_1 & 0x80) ^ 0x80); /* simulated pause button */
	else if ((address & 0xfe) == 0xbe)
	{
		if(address & 1)
			return sms9918_readport1(sms_vdp);
		else
			return sms9918_readport0(sms_vdp);
    }
	else if (((address & 0xfe) == 0xdc) || ((address & 0xfe) == 0xc0))
	{
	/* Dummy controller emulation */
		if (address & 1)
			/* FIXME: Country select goes here and at writes to 0x3f */
			return (unsigned char)((sms_joypad_2 >> 2) ^ 0xff);
		else
		{
			joypad_Update(0, &sms_joypad_1);
			joypad_Update(1, &sms_joypad_2);
			return (unsigned char)(((sms_joypad_1 & 0x3f) | (sms_joypad_2 << 6)) ^ 0xff);
		}
	}
	else if ((address & 0xfe) == 0x7e)
		return sms9918_readscanline(sms_vdp);
	else
		printf("sms: I/O read 0x%02x.\n", (unsigned int)address & 0xff);

	return 0;
}

void sms_io_write(cal_cpu* cpu, unsigned long address, unsigned char data)
{
	if ((address & 0xfe) == 0xbe)
	{
		if (address & 1)
			sms9918_writeport1(sms_vdp, data);
		else
			sms9918_writeport0(sms_vdp, data);
	}
	else if ((address & 0xfe) == 0x7e)
		sms_psg_write(data);
	else if ((address & 0xfe) == 0xf0)
		;
    else
		printf("sms: I/O write 0x%02x = 0x%02x.\n", (unsigned int)address & 0xff, (unsigned int)data);
}

void sms_init_cpu(void)
{
	sms_cpu = cal_create(CPUT_MARATZ80); /* FIXME: return value? */
	sms_cpu->setmmu0(sms_cpu, sms_mmu);
	sms_cpu->setmmu8(sms_cpu, 0, 0, &sms_readfunc, &sms_writefunc);
	sms_cpu->setiou(sms_cpu, (memread8_t)sms_io_read, (memwrite8_t)sms_io_write);
	sms_cpu->reset(sms_cpu);
}

void sms_scanline(void)
{
	if (sms9918_periodic(sms_vdp))
		sms_cpu->irq(sms_cpu, 0xff);
}

void sms_check_pause(void)
{
	/* NOTE: the use of the otherwise unused 0x40 bit is a hack */
	joypad_Update(0, &sms_joypad_1);
	
	if (sms_joypad_1 & 0x80)
	{
		if (!(sms_joypad_1 & 0x40))
			sms_cpu->nmi(sms_cpu);
			
		sms_joypad_1 |= 0x40;
	}
	else
		sms_joypad_1 &= ~0x40;
}

event sms_events[] = {
    {NULL, 0, 228, sms_scanline},
    {NULL, 0, 228 * 200, sms_check_pause},
    {NULL, 0, 228 * 262, sms_psg_vsync},
    {NULL, 0, 228 * 262, NULL},
};

event_scheduler* sms_init_events(void)
{
event_scheduler* scheduler= new_event_scheduler(cal_event_delay_callback, sms_cpu);

	event_register(scheduler, &sms_events[0]);
	
	if (!is_gg)
		event_register(scheduler, &sms_events[1]);
	
	event_register(scheduler, &sms_events[2]);
	event_register(scheduler, &sms_events[3]);
	
	return scheduler;
}

void sms_load_battery_file(rom_file* romfile)
{
	sms_battery_file = init_battery_file(romfile, 0x4000);
	
	if(sms_battery_file)
		sms_cart_memory = sms_battery_file->data;
	else
		sms_cart_memory = sms_memory + 0x2000;
}

void sms_save_battery_file(void)
{
	if (sms_battery_file)
		save_battery_file(sms_battery_file);
}

void sms_shutdown(void)
{
	if (sms_uses_battery)
		sms_save_battery_file();
	
	sms_psg_done();
}

void sms_run_common(rom_file* romfile)
{
event_scheduler* scheduler;
    
//	sms_memory = (unsigned char*)calloc(1, SMS_RAMSIZE);
	sms_memory = new u8[SMS_RAMSIZE];
	
	if(!sms_memory)
	{
		printf("out of memory.\n");
		return;
	}
	
	sms_load_battery_file(romfile);
	sms_uses_battery = 0;
	
	sms_init_mmu(romfile);
	
	sms_init_cpu();
	
	sms_vdp = sms9918_create(is_gg); /* FIXME: return value? */
	
	sms_psg_init();
	
	emulation_SetShutdown(sms_shutdown);
	
	scheduler = sms_init_events();
	
	emulation_SetTimeslice((void (*)(void *)) &event_timeslice, scheduler);
}

void sms_run(rom_file* romfile)
{
	is_gg = 0;
	sms_run_common(romfile);
}

void gg_run(rom_file* romfile)
{
	is_gg = 1;
	sms_run_common(romfile);
}
