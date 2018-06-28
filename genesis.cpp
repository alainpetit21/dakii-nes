/*
 * genesis.c
 *
 * Sega Genesis emulation.
 */

/* $Id: genesis.cpp,v 1.2 2005/01/18 00:39:03 guest Exp $ */

#include <stdlib.h>
#include <stdio.h>

#include "ui.h"
#include "system.h"
#include "cal.h"
#include "tool.h"
#include "gen_vdp.h"
#include "emu68k.h"
#include "types.h"

#define GENESIS_RAMSIZE 0x10000 /* FIXME: Get real ram size data */
#define GENESIS_Z80_RAMSIZE 0x2000

unsigned char *genesis_ram;
unsigned char *genesis_z80_ram;
cal_cpu* genesis_68k;
cal_cpu* genesis_z80;
gen_vdp* genesis_vdp;
unsigned char *genesis_rom;
u32 genesis_rom_size;

/*
 * memory map:
 *
 * 000000-3fffff ROM
 * 400000-9fffff Reserved
 * a00000-afffff Z80/Sound
 * b00000-bfffff Reserved
 * c00000-dfffff VDP
 * e00000-feffff RAM Mirrors
 * ff0000-ffffff RAM
 */

unsigned char gen_read8_rom(cal_cpu* cpu, unsigned long address)
{
    address &= 0x00ffffff;
    
    if (address >= genesis_rom_size) {
	printf("gen_read8_rom: address 0x%06lx beyond end of ROM (0x%06lx).\n", address, genesis_rom_size);
	return 0;
    }
    
    return genesis_rom[address & 0x3fffff];
}

unsigned short gen_read16_rom(cal_cpu* cpu, unsigned long address)
{
    address &= 0x00ffffff;
    
    if (address & 1) {
	printf("gen_read16_rom: CPU BUG: non-aligned 16-bit access.\n");
    }

    if (address >= genesis_rom_size) {
	printf("gen_read16_rom: address 0x%06lx beyond end of ROM (0x%06lx).\n", address, genesis_rom_size);
	return 0;
    }
    
    return mtoh16(*((unsigned short *)(genesis_rom + address)));
}

unsigned char gen_read8_reserved(cal_cpu* cpu, unsigned long address)
{
    address &= 0x00ffffff;
    
    printf("gen_read8_reserved: address 0x%06x.\n", (unsigned int)address);
    return 0;
}

unsigned short gen_read16_reserved(cal_cpu* cpu, unsigned long address)
{
    address &= 0x00ffffff;
    
    if (address & 1) {
	printf("gen_read16_reserved: CPU BUG: non-aligned 16-bit access.\n");
    }
    printf("gen_read16_reserved: address 0x%06x.\n", (unsigned int)address);
    return 0;
}

unsigned char gen_read8_misc(cal_cpu* cpu, unsigned long address)
{
    address &= 0x00ffffff;
    
    if (address < 0xa02000) {
	return genesis_z80_ram[address - 0xa00000];
    } else if ((address >= 0xa04000) && (address < 0xa05000)) {
	/* Yamaha chip (FM synth?) */
	return 0;
    }

    printf("gen_read8_misc: 0x%06x.\n", (unsigned int)address);
    return 0;
}

unsigned short gen_read16_misc(cal_cpu* cpu, unsigned long address)
{
    address &= 0x00ffffff;
    
    if (address & 1) {
	printf("gen_read16_misc: CPU BUG: non-aligned 16-bit access.\n");
    }
    printf("gen_read16_misc: 0x%06x.\n", (unsigned int)address);
    return 0;
}

unsigned char gen_read8_vdp(cal_cpu* cpu, unsigned long address)
{
    address &= 0x00ffffff;
    
    printf("gen_read8_vdp: 0x%06x.\n", (unsigned int)address);
    return 0;
}

unsigned short gen_read16_vdp(cal_cpu* cpu, unsigned long address)
{
    address &= 0x00ffffff;
    
    if (address & 1) {
	printf("gen_read16_vdp: CPU BUG: non-aligned 16-bit access.\n");
    }
    printf("gen_read16_vdp: 0x%06x.\n", (unsigned int)address);
    return 0;
}

unsigned char gen_read8_ram(cal_cpu* cpu, unsigned long address)
{
    return genesis_ram[address & 0xffff];
}

unsigned short gen_read16_ram(cal_cpu* cpu, unsigned long address)
{
    if (address & 1) {
	printf("gen_read16_ram: CPU BUG: non-aligned 16-bit access.\n");
    }
    return mtoh16(*((unsigned short *)(genesis_ram + (address & 0xffff))));
}

void gen_write8_reserved(cal_cpu* cpu, unsigned long address, unsigned char data)
{
    address &= 0x00ffffff;
    
    printf("gen_write8_reserved: address 0x%06x, data 0x%02x.\n", (unsigned int)address, (unsigned int)data);
}

void gen_write16_reserved(cal_cpu* cpu, unsigned long address, unsigned short data)
{
    address &= 0x00ffffff;
    
    if (address & 1) {
	printf("gen_write16_reserved: CPU BUG: non-aligned 16-bit access.\n");
    }
    printf("gen_write16_reserved: address 0x%06x, data 0x%04hx.\n", (unsigned int)address, (unsigned int)data);
}

void gen_write8_misc(cal_cpu* cpu, unsigned long address, unsigned char data)
{
    address &= 0x00ffffff;
    
    if (address < 0xa02000) {
        genesis_z80_ram[address - 0xa00000] = data;
	return;
    } else if ((address >= 0xa04000) && (address < 0xa05000)) {
	/* Yamaha chip (FM synth?) */
	return;
    }
    
    printf("gen_write8_misc: address 0x%06x, data 0x%02x.\n", (unsigned int)address, (unsigned int)data);
}

void gen_write16_misc(cal_cpu* cpu, unsigned long address, unsigned short data)
{
    address &= 0x00ffffff;
    
    if (address & 1) {
	printf("gen_write16_misc: CPU BUG: non-aligned 16-bit access.\n");
    }
    printf("gen_write16_misc: address 0x%06x, data 0x%04hx.\n", (unsigned int)address, (unsigned int)data);
}

void gen_write8_vdp(cal_cpu* cpu, unsigned long address, unsigned char data)
{
    address &= 0x00ffffff;
    
    printf("gen_write8_vdp: address 0x%06x, data 0x%02x.\n", (unsigned int)address, (unsigned int)data);
}

void gen_write16_vdp(cal_cpu* cpu, unsigned long address, unsigned short data)
{
    if (address & 1) {
	printf("gen_write16_vdp: CPU BUG: non-aligned 16-bit access.\n");
    }
/*     printf("gen_write16_vdp: address 0x%06x, data 0x%04hx.\n", address, data); */
    if ((address & 0xfffffc) == 0xc00000) {
	gen_vdp_writeport0(genesis_vdp, data);
    } else if ((address & 0xfffffc) == 0xc00004) {
	gen_vdp_writeport1(genesis_vdp, data);
/* 	printf("gen_write16_vdp: CNTRL_LO, data 0x%04hx (%c%c).\n", data, (data & 0x8000)? '1': '0', (data & 0x4000)? '1': '0'); */
/*     } else if (address == 0xc00006) { */
/* 	printf("gen_write16_vdp: CNTRL_HI, data 0x%04hx.\n", data); */
    } else {
	printf("gen_write16_vdp: address 0x%06x, data 0x%04hx.\n", (unsigned int)address & 0x00ffffff, (unsigned int)data);
    }
}

void gen_write8_ram(cal_cpu* cpu, unsigned long address, unsigned char data)
{
    genesis_ram[address & 0xffff] = data;
}

void gen_write16_ram(cal_cpu* cpu, unsigned long address, unsigned short data)
{
    if (address & 1) {
	printf("gen_write16_ram: CPU BUG: non-aligned 16-bit access.\n");
    }
    (*((unsigned short *)(genesis_ram + (address & 0xffff)))) = htom16(data);
}

memread8_t gen_read8[16] = 
{
	(memread8_t)gen_read8_rom,
	(memread8_t)gen_read8_rom,
	(memread8_t)gen_read8_rom,
	(memread8_t)gen_read8_rom,
	(memread8_t)gen_read8_reserved,
	(memread8_t)gen_read8_reserved,
	(memread8_t)gen_read8_reserved,
	(memread8_t)gen_read8_reserved,
	(memread8_t)gen_read8_reserved,
	(memread8_t)gen_read8_reserved,
	(memread8_t)gen_read8_misc,
	(memread8_t)gen_read8_reserved,
	(memread8_t)gen_read8_vdp,
	(memread8_t)gen_read8_vdp,
	(memread8_t)gen_read8_ram,
	(memread8_t)gen_read8_ram,
};
memread16_t gen_read16[16] = 
{
    (memread16_t)gen_read16_rom,
    (memread16_t)gen_read16_rom,
    (memread16_t)gen_read16_rom,
    (memread16_t)gen_read16_rom,
    (memread16_t)gen_read16_reserved,
    (memread16_t)gen_read16_reserved,
    (memread16_t)gen_read16_reserved,
    (memread16_t)gen_read16_reserved,
    (memread16_t)gen_read16_reserved,
    (memread16_t)gen_read16_reserved,
    (memread16_t)gen_read16_misc,
    (memread16_t)gen_read16_reserved,
    (memread16_t)gen_read16_vdp,
    (memread16_t)gen_read16_vdp,
    (memread16_t)gen_read16_ram,
    (memread16_t)gen_read16_ram,
};

memwrite8_t gen_write8[16] = 
{
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_misc,
	(memwrite8_t)gen_write8_reserved,
	(memwrite8_t)gen_write8_vdp,
	(memwrite8_t)gen_write8_vdp,
	(memwrite8_t)gen_write8_ram,
	(memwrite8_t)gen_write8_ram,
};
memwrite16_t gen_write16[16] = 
{
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_misc,
	(memwrite16_t)gen_write16_reserved,
	(memwrite16_t)gen_write16_vdp,
	(memwrite16_t)gen_write16_vdp,
	(memwrite16_t)gen_write16_ram,
	(memwrite16_t)gen_write16_ram,
};

void genesis_convert_rom(rom_file* romfile)
{
/* FIXME: kill the person who devised the SMD format */
u8	*newrom	= new u8[romfile->size - 0x200];
u8	*cur_ptr= newrom;
int j		= 0x200;
int i;

    while(j < romfile->size){
		for(i= 0; i < 0x2000; ++i, ++j){
			*cur_ptr++ = romfile->data[j + 0x2000];
			*cur_ptr++ = romfile->data[j];
		}

		j += 0x2000;
    }

    delete romfile->data;
    romfile->data= newrom;
    romfile->size-= 0x200;
}

void genesis_timeslice(cal_cpu* cpu)
{
    int i;
    int j;
    
    for (i = 262; i && (system_flags == F_NONE); i--) {
	cpu->runfor(cpu, 488);
	for (j = 1; (j < 20) && (!(system_flags & F_UNIMPL)); j++) {
	    cpu->run(cpu);
	}

	if (gen_vdp_periodic(genesis_vdp)) {
	    cpu->irq(cpu, 6);
	}

	if (system_flags & F_UNIMPL) {
	    printf("Unimplimented Instruction.\n");
	    system_flags |= F_QUIT;
	}
    }
}

void genesis_run(rom_file* romfile)
{
//    genesis_ram = (unsigned char*)calloc(1, GENESIS_RAMSIZE + GENESIS_Z80_RAMSIZE);
    genesis_ram = new u8[GENESIS_RAMSIZE + GENESIS_Z80_RAMSIZE];
    
    if (genesis_ram) {
	genesis_z80_ram = genesis_ram + GENESIS_RAMSIZE;
	
	if (romfile->size & 0x200) {
	    printf("genesis_run: hi/lo paged rom image (SMD format).\n");
	    printf("genesis_run: converting.\n");
	    genesis_convert_rom(romfile);
	} else {
	    printf("genesis_run: linear rom image (BIN format).\n");
	}

	genesis_rom = romfile->data;
	genesis_rom_size = romfile->size;
	
	genesis_68k = cal_create(CPUT_JUNK68K);
/* 	genesis_68k = cal_create(CPUT_TORR68K); */
	genesis_68k->setmmu8(genesis_68k, 20, 15, gen_read8, gen_write8);
	genesis_68k->setmmu16(genesis_68k, 20, 15, gen_read16, gen_write16);

	genesis_68k->reset(genesis_68k);
	
	genesis_vdp = gen_vdp_create();

#if 1
	emulation_SetTimeslice((void (*)(void *)) &genesis_timeslice, genesis_68k);
#else
	{
	    extern int dt68000_disassemble(unsigned char *romimage, unsigned long start_addr, unsigned long end_addr);
	    dt68000_disassemble(genesis_rom, 0x001560, 0x001580);
	    system_flags |= F_QUIT;
	}
#endif
    } else {
	printf("out of memory.\n");
    }
}
