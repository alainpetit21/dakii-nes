/*
 * coleco.c
 *
 * ColecoVision emulation.
 */

/* $Id: coleco.cpp,v 1.3 2005/01/18 00:39:03 guest Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "system.h"
#include "cal.h"
#include "tool.h"
#include "tms9918.h"
#include "sms_psg.h"
#include "event.h"

#define CV_RAMSIZE 0x400

unsigned char *cv_memory;
unsigned char *cv_rom;
unsigned char *cv_game_rom;
cal_cpu* cv_cpu;
tms9918* cv_vdp;

int cv_last_nmi_state;

unsigned char *cv_rombase;

u32 cv_joypad_1 = 0;

u32 cv_keypad_1;

int cv_joypad_state;

ranged_mmu cv_mmu[] = {
    {0x0000, 0x1fff, NULL, 0, 0},
    {0x2000, 0x5fff, NULL, 0, 0},
    {0x6000, 0x63ff, NULL, 0, 0},
    {0x6400, 0x67ff, NULL, 0, 0},
    {0x6800, 0x6bff, NULL, 0, 0},
    {0x6c00, 0x6fff, NULL, 0, 0},
    {0x7000, 0x73ff, NULL, 0, 0},
    {0x7400, 0x77ff, NULL, 0, 0},
    {0x7800, 0x7bff, NULL, 0, 0},
    {0x7c00, 0x7fff, NULL, 0, 0},
    {0x8000, 0xffff, NULL, 0, 0},
};

void cv_init_mmu(void)
{
    cv_mmu[0].data = cv_rom;
    cv_mmu[2].data = cv_memory - 0x6000;
    cv_mmu[3].data = cv_memory - 0x6400;
    cv_mmu[4].data = cv_memory - 0x6800;
    cv_mmu[5].data = cv_memory - 0x6c00;
    cv_mmu[6].data = cv_memory - 0x7000;
    cv_mmu[7].data = cv_memory - 0x7400;
    cv_mmu[8].data = cv_memory - 0x7800;
    cv_mmu[9].data = cv_memory - 0x7c00;
    cv_mmu[10].data = cv_game_rom - 0x8000;
}

unsigned char cv_read(cal_cpu* cpu, unsigned long addr)
{
    if (addr < 0x2000) {
	return cv_rom[addr];
    } else if (addr < 0x6000) {
	printf("cv_read: read unassigned address 0x%04x.\n", (unsigned int)addr);
	return 0;
    } else if (addr < 0x8000) {
	return cv_memory[addr & 0x3ff];
    } else {
	return cv_game_rom[addr & 0x7fff];
    }
}

void cv_write(cal_cpu* cpu, unsigned long addr, unsigned char data)
{
    if ((addr & 0xe000) == 0x6000) {
	cv_memory[addr & 0x3ff] = data;
    }
}

memread8_t cv_readfunc = (memread8_t)cv_read;
memwrite8_t cv_writefunc = (memwrite8_t)cv_write;

u8 cv_keypad_codes[12] = {
    0x5, 0x2, 0x8, 0x3, 0xd, 0xc, 0x1, 0xa, 0xe, 0x4, 0x6, 0x9,
};

u8 cv_keypad_read(void)
{
    int i;

    for (i = 0; i < 12; i++) {
	if (cv_keypad_1 & (1 << i)) {
	    return cv_keypad_codes[i];
	}
    }
    
    return 0x00;
}

unsigned char cv_io_read(cal_cpu* cpu, unsigned long address)
{
    if ((address & 0xe0) == 0xa0) {
	if (address & 1) {
	    return tms9918_readport1(cv_vdp);
	} else {
	    return tms9918_readport0(cv_vdp);
	}
    } else if ((address & 0xe0) == 0xe0) {
	if (address & 0x02) {
	    /* FIXME: controller 2? */
	    return 0xff;
	} else {
		keypad_Update(0, &cv_keypad_1);
	    joypad_Update(0, &cv_joypad_1);
	    if (cv_joypad_state) {
		/* joystick mode */
		return (unsigned char)(((cv_joypad_1 & 0x0f) | ((cv_joypad_1 & 0x10)? 0x40: 0)) ^ 0xff);
	    } else {
		/* keypad mode */
		return (cv_keypad_read() | ((cv_joypad_1 & 0x20)? 0x40: 0)) ^ 0xff;
	    }
	}
    } else {
	printf("cv_io_read: 0x%02x.\n", (unsigned int)address & 0xff);
    }

    return 0;
}

void cv_io_write(cal_cpu* cpu, unsigned long address, unsigned char data)
{
    if ((address & 0xe0) == 0x80) {
	/* set controls to keypad mode */
	cv_joypad_state = 0;
    } else if ((address & 0xe0) == 0xa0) {
	if (address & 1) {
	    tms9918_writeport1(cv_vdp, data);
	} else {
	    tms9918_writeport0(cv_vdp, data);
	}
    } else if ((address & 0xe0) == 0xc0) {
	/* set controls to joystick mode */
	cv_joypad_state = 1;
    } else if ((address & 0xe0) == 0xe0) {
	sms_psg_write(data);
    } else {
		printf("cv_io_write: 0x%02x = 0x%02x.\n", (unsigned int)address & 0xff, (unsigned int)data);
    }
}

void cv_scanline(void)
{
    if (tms9918_periodic(cv_vdp)) {
	if (!cv_last_nmi_state) {
	    cv_cpu->nmi(cv_cpu);
	    cv_last_nmi_state = 1;
	}
    } else {
	cv_last_nmi_state = 0;
    }
}

event cv_events[] = {
    {NULL, 0, 228, cv_scanline},
    {NULL, 0, 228 * 262, sms_psg_vsync},
    {NULL, 0, 228 * 262, NULL},
};

event_scheduler* cv_init_events(void)
{
    event_scheduler* scheduler;
    scheduler = new_event_scheduler(cal_event_delay_callback, cv_cpu);

    event_register(scheduler, &cv_events[0]);
    event_register(scheduler, &cv_events[1]);
    event_register(scheduler, &cv_events[2]);

    return scheduler;
}

void cv_load_bios(char *p_baseDir)
{
rom_file* romfile;
char temp[256];
    
	memset(temp, 0, 256);
	printf("attempting to load bios file.\n");
    
	sprintf(temp, "%scoleco.bios", p_baseDir);
    romfile = read_romimage(temp);

    if (!romfile) {
	printf("could not open bios file.\n");
	printf("things are about to get ugly.\n");
	return;
    }
    
    cv_rom = romfile->data;

    /* FIXME: memory leak */
}

void cv_init_controllers(void)
{
}

void cv_init_cpu(void)
{
    cv_cpu = cal_create(CPUT_MARATZ80);
    cv_cpu->setmmu0(cv_cpu, cv_mmu);
    cv_cpu->setmmu8(cv_cpu, 0, 0, &cv_readfunc, &cv_writefunc);
    cv_cpu->setiou(cv_cpu, (memread8_t)cv_io_read, (memwrite8_t)cv_io_write);
    cv_cpu->reset(cv_cpu);

    cv_last_nmi_state = 0;
}

void cv_shutdown(void)
{
    sms_psg_done();
}

void cv_run(rom_file* romfile)
{
event_scheduler* scheduler;
char temp[256];
int i;

	memset(temp, 0, 256);
//	cv_memory = (unsigned char*)calloc(1, CV_RAMSIZE);
	cv_memory = new u8[CV_RAMSIZE];
	
	if (!cv_memory)
		printf("out of memory.\n");
	
	cv_game_rom = romfile->data;
	
	/*Base dit calculation*/
	for(i= 0;;++i)
	{
		if(!strncmp(&romfile->filename[i], "coleco/", 7))
			break;
	}
	memcpy(temp, romfile->filename, i + 7);
	
		
	cv_load_bios(temp);
	
	cv_init_mmu();
	
	cv_init_cpu();
	
	cv_vdp = tms9918_create();
	
	emulation_SetShutdown(cv_shutdown);
	
	sms_psg_init();
	
	cv_init_controllers();
	
	scheduler = cv_init_events();
	
	emulation_SetTimeslice((void (*)(void *)) &event_timeslice, scheduler);
}
