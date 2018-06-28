/*
 * nes.c
 *
 * contains NES specific code and data.
 */

/* $Id: nes.cpp,v 1.4 2005/01/18 00:39:03 guest Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lib.h"
#include "libTypes.h"
#include "libMacros.h"
#include "cal.h"
#include "tool.h"
#include "ui.h"
#include "nes_ppu.h"
#include "mappers.h"
#include "system.h"
#include "nesStruct.h"
#include "nes.h"
#include "nes_psg.h"
#include "memtypes.h"
#include "event.h"
#include "cpu6502.h"

u32	nes_joypad_1= 0;
u32	nes_joypad_2= 0;

struct cpu6502_context *nes_cpu;

nes_mapper* mapper;
nes_rom* nes_romfile;

#define NES_RAMSIZE 0x2800    /* Size of System and Cart Ram */

rom_file* battery_file;

u8 *vromimage = NULL;  /* Cart VRom area */

u8 *bank_0 = NULL;     /* System Ram */

u8 *bank_6 = NULL;     /* Cart Ram */
u8 *bank_8 = NULL;     /* Rom Page */
u8 *bank_A = NULL;     /* Rom Page */
u8 *bank_C = NULL;     /* Rom Page */
u8 *bank_E = NULL;     /* Rom Page */

u8 bank8_reg;
u8 bankA_reg;
u8 bankC_reg;
u8 bankE_reg;

int joy1_latch;
int joy2_latch;

readhook_t nes_bank4_read_hook;
readhook_t nes_bank6_read_hook;
writehook_t nes_bank4_write_hook;
writehook_t nes_bank6_write_hook;

u8 mask_bank_addr(u8 bank)
{
u8 i= 0xff;
	
	while ((bank & i) >= (nes_romfile->prg_size << 1))
		i >>= 1;
	
	return (bank & i);
}

u8 *bank_regs[4] = {&bank8_reg, &bankA_reg, &bankC_reg, &bankE_reg};
u8 **banks[4] = {&bank_8, &bank_A, &bank_C, &bank_E};

void nesprg_map_4k(int bank, int page)
{
}

void nesprg_map_8k(int bank, int page)
{
	*bank_regs[bank] = mask_bank_addr(page);
	*banks[bank] = nes_romfile->prg_data + (*bank_regs[bank] * 0x2000);
}

void nesprg_map_16k(int bank, int page)
{
	nesprg_map_8k((bank << 1) + 0, (page << 1) + 0);
	nesprg_map_8k((bank << 1) + 1, (page << 1) + 1);
}

void nesprg_map_32k(int bank, int page)
{
	nesprg_map_16k((bank << 1) + 0, (page << 1) + 0);
	nesprg_map_16k((bank << 1) + 1, (page << 1) + 1);
}

void do_sprite_dma(int page)
{
	if (page < 0x20) /* in ram area */
		memcpy(PPU_sprite_ram, bank_0 + ((page & 7) << 8), 0x100);
	else if (page < 0x60)
		printf("PPU_do_dma(): sprite DMA from I/O space, ignoring.\n");
	else if (page < 0x80)
		memcpy(PPU_sprite_ram, bank_6 + ((page & 0x1f) << 8), 0x100);
	else if (page < 0xa0)
		memcpy(PPU_sprite_ram, bank_8 + ((page & 0x1f) << 8), 0x100);
	else if (page < 0xc0)
		memcpy(PPU_sprite_ram, bank_A + ((page & 0x1f) << 8), 0x100);
	else if (page < 0xe0)
		memcpy(PPU_sprite_ram, bank_C + ((page & 0x1f) << 8), 0x100);
	else
		memcpy(PPU_sprite_ram, bank_E + ((page & 0x1f) << 8), 0x100);
}

u8 nes_read_bank_0(cal_cpu *cpu, unsigned long addr)
{
    return bank_0[addr & 0x7ff];
}

u8 nes_read_bank_2(cal_cpu *cpu, unsigned long addr)
{
    return ppu_io_read(nes_ppu_true, (unsigned short)addr);
}

u8 nes_read_bank_4(cal_cpu *cpu, unsigned long addr)
{
u8 retval;

	if (addr < 0x4016)
		return 0x00; /* FIXME: sound register reads?? */
	else if (addr == 0x4016)
	{
		retval = 0x40 | (joy1_latch & 1);
		joy1_latch >>= 1;
		return retval;
	}
	else if (addr == 0x4017)
	{
		retval = 0x40 | (joy2_latch & 1);
		joy2_latch >>= 1;
		return retval;
	}
	else if (nes_bank4_read_hook)
		return (u8)nes_bank4_read_hook(mapper, (unsigned short)addr);
	else
	{
		printf("unknown I/O read (0x%04x), returning 0.\n", (unsigned int)addr);
		return 0;
    }
}

u8 nes_read_bank_6(cal_cpu *cpu, unsigned long addr)
{
	if(nes_bank6_read_hook)
		return nes_bank6_read_hook(mapper, (unsigned short)addr);
	else
		return bank_6[addr & 0x1fff];
}

u8 nes_read_bank_8(cal_cpu *cpu, unsigned long addr)
{
	return bank_8[addr & 0x1fff];
}

u8 nes_read_bank_a(cal_cpu *cpu, unsigned long addr)
{
	return bank_A[addr & 0x1fff];
}

u8 nes_read_bank_c(cal_cpu *cpu, unsigned long addr)
{
	return bank_C[addr & 0x1fff];
}

u8 nes_read_bank_e(cal_cpu *cpu, unsigned long addr)
{
	return bank_E[addr & 0x1fff];
}

void nes_write_bank_0(cal_cpu *cpu, unsigned long addr, u8 data)
{
	bank_0[addr & 0x7ff] = data;
}

void nes_write_bank_2(cal_cpu *cpu, unsigned long addr, u8 data)
{
	ppu_io_write(nes_ppu_true, (unsigned short)addr, data);
}

void nes_write_bank_4(cal_cpu *cpu, unsigned long addr, u8 data)
{
	if (addr < 0x4010)
		sound_regs[addr & 15](data);
	else if (addr < 0x4014)
		;/* FIXME: more sound regs go here */
	else if (addr == 0x4014)
		do_sprite_dma(data);
	else if (addr == 0x4015)
		nes_psg_write_control(data);
	else if (addr == 0x4016)
	{
		joypad_Update(0, &nes_joypad_1);
		joypad_Update(1, &nes_joypad_2);
		joy1_latch = nes_joypad_1;
		joy2_latch = nes_joypad_2;
	}
	else if (addr == 0x4017)
		;/* FIXME: IRQ control? */
	else if (nes_bank4_write_hook)
		nes_bank4_write_hook(mapper, (unsigned short)addr, data);
	else
		printf("unknown I/O write (0x%04x, 0x%02x).\n", (unsigned int)addr, (unsigned int)data);
}

void nes_write_bank_6(cal_cpu *cpu, unsigned long addr, u8 data)
{
    if(nes_bank6_write_hook)
		nes_bank6_write_hook(mapper, (unsigned short)addr, data);
	else
		bank_6[addr & 0x1fff] = data;
}

void nes_write_mapper(cal_cpu *cpu, unsigned long addr, u8 data)
{
	mapper->write(mapper, (unsigned short)addr, data);
}

memread8_t nes_real_readtable[8] = {
	(memread8_t)nes_read_bank_0,
	(memread8_t)nes_read_bank_2,
	(memread8_t)nes_read_bank_4,
	(memread8_t)nes_read_bank_6,
	(memread8_t)nes_read_bank_8,
	(memread8_t)nes_read_bank_a,
	(memread8_t)nes_read_bank_c,
	(memread8_t)nes_read_bank_e,
};

memwrite8_t nes_real_writetable[8] = {
	(memwrite8_t)nes_write_bank_0,
	(memwrite8_t)nes_write_bank_2,
	(memwrite8_t)nes_write_bank_4,
	(memwrite8_t)nes_write_bank_6,
	(memwrite8_t)nes_write_mapper,
	(memwrite8_t)nes_write_mapper,
	(memwrite8_t)nes_write_mapper,
	(memwrite8_t)nes_write_mapper,
};

u8 nes_read_handler(cal_cpu *cpu, u32 addr)
{
	return nes_real_readtable[(addr >> 13) & 7](cpu, addr);
}

void nes_write_handler(cal_cpu *cpu, u32 addr, u8 data)
{
	nes_real_writetable[(addr >> 13) & 7](cpu, addr, data);
}

void nes_load_battery_file(rom_file* romfile)
{
	battery_file = init_battery_file(romfile, 0x2000);
    
	if (battery_file)
		bank_6 = battery_file->data;
}

void nes_save_battery_file(void)
{
	if (battery_file)
		save_battery_file(battery_file);
}

void nes_external_irq(void)
{
	cpu6502_irq(nes_cpu);
}

void nes_scanline(void)
{
	if (nesppu_periodic(nes_ppu_true))
		cpu6502_nmi(nes_cpu);
}

event nes_events[] = {
	{NULL, 0, 113, nes_scanline},
	{NULL, 0, 7457, nes_psg_frame},
	{NULL, 0, 113 * 262, NULL},
};

event_scheduler* nes_init_events(void)
{
event_scheduler* scheduler= new_event_scheduler(cpu6502_event_delay_callback, nes_cpu);

	event_register(scheduler, &nes_events[0]);
	event_register(scheduler, &nes_events[1]);
	event_register(scheduler, &nes_events[2]);
	
	return scheduler;
}

void nes_shutdown(void)
{
	if(nes_romfile->uses_battery)
		nes_save_battery_file();

	nes_psg_done();
}

nes_rom* nes_create_romfile(rom_file* romfile)
{
nes_rom* retval= new nes_rom;

	if (!retval)
		return NULL;

	retval->file = romfile;
	retval->prg_size = romfile->data[4];
	retval->chr_size = romfile->data[5];
	retval->prg_data = romfile->data + 0x10;
	retval->uses_battery = romfile->data[6] & 2;
	retval->mirror_vertical = romfile->data[6] & 1;

	/* this works as is for FDS */
    if (retval->chr_size > 0) 
		retval->chr_data = retval->prg_data + (retval->prg_size * 0x4000);
    else
		retval->chr_data = NULL;

	/* NOTE: Cheap hack */
    if (romfile->data[0] == 'F')
		retval->mapper = 20;
	else
	{
		retval->mapper= romfile->data[6] >> 4;
		if(*((u32 *)&romfile->data[12]))
		{
			printf("nes: dirty iNES header.\n");
			printf("nes: using 4-bit mapper id.\n");
		}
		else
		{
			printf("nes: using 8-bit mapper id.\n");
			retval->mapper|= romfile->data[7] & 0xf0;
		}
	}

	return retval;
}

void nes_init_cpu(void)
{
    nes_cpu = cpu6502_create();
    cpu6502_setzpage(nes_cpu, bank_0);
    cpu6502_setmemhandlers(nes_cpu, (u8 (*)(void *,unsigned long))nes_read_handler, (void (*)(void *,unsigned long,u8))nes_write_handler);
    cpu6502_reset(nes_cpu);
}

void 
nes_run(rom_file* romfile)
{
//event_scheduler* scheduler= (event_scheduler*)calloc(1, NES_RAMSIZE);
event_scheduler* scheduler= (event_scheduler*)new u8[NES_RAMSIZE];

//	bank_0 = (u8*)calloc(1, NES_RAMSIZE);
	bank_0 = new u8[NES_RAMSIZE];
	nes_romfile= nes_create_romfile(romfile);

	if(!(nes_romfile && bank_0))
	{
		SAFE_DELETE(nes_romfile);
		SAFE_DELETE(bank_0);
		
		printf("out of memory\n");
		return;
	}

	if(!mapper_supported(nes_romfile->mapper)) 
	{
		SAFE_DELETE(nes_romfile);
		SAFE_DELETE(bank_0);

		printf("darcnes: mapper #%d unsupported.\n", nes_romfile->mapper);
		return;
	}

	emulation_SetShutdown(nes_shutdown);
    
	bank_6 = bank_0 + 0x800;
    
	if(nes_romfile->uses_battery)
		nes_load_battery_file(nes_romfile->file);
   
	PPU_init(nes_romfile);
	
	mapper = create_mapper(nes_ppu_true, nes_romfile);
	nesppu_set_mapper(nes_ppu_true, mapper);
	
	nes_psg_init();
	
	nes_init_cpu();
	
	scheduler = nes_init_events();
	
	emulation_SetTimeslice((void (*)(void *)) &event_timeslice, scheduler);
}
