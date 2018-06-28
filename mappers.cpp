/*
 * mappers.c
 *
 * mapper emulation routines
 */

/* $Id: mappers.cpp,v 1.2 2005/01/15 20:32:00 guest Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include "nesStruct.h"
#include "nes.h"
#include "nes_ppu.h"
#include "mappers.h"
#include "ui.h"
#include "tool.h"


nes_mapper* map0_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map1_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map2_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map3_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map4_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map7_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map9_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* mapA_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* mapB_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* mapF_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map14_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map17_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map18_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map20_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map42_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map44_init(nes_ppu* ppu, nes_rom* romfile);
nes_mapper* map4E_init(nes_ppu* ppu, nes_rom* romfile);


mapper_support mappers[] = {
    {0,   "None",               map0_init,  2},
    {1,   "MMC1",               map1_init,  2},
    {2,   "UNROM",              map2_init,  2},
    {3,   "CNROM",              map3_init,  2},
    {4,   "MMC3",               map4_init,  1},
    {7,   "AOROM",              map7_init,  2},
    {9,   "MMC2",               map9_init,  2},
    {10,  "MMC4",               mapA_init,  2},
    {11,  "Color Dreams",       mapB_init,  2},
    {23,  "Konami VRC2 type B", map17_init, 1},
    {24,  "Konami VRC6",        map18_init, 1},
    {66,  "GNROM",              map42_init, 1},
    {68,  "Sunsoft Mapper 4",   map44_init, 2},
    {78,  "Irem 74HC161/32",    map4E_init, 1},
    {-1,  NULL,                 NULL,         0},
};

nes_mapper* create_mapper(nes_ppu* ppu, nes_rom* romfile)
{
    int i;
    
    for (i = 0; mappers[i].mapper != -1; i++) {
	if ((mappers[i].mapper == romfile->mapper) && (mappers[i].support)) {
	    printf("mappers: initializing%s completed mapper %d (%s).\n",
		   (mappers[i].support == 1)? " partially": "",
		   romfile->mapper, mappers[i].name);
	    return mappers[i].create(ppu, romfile);
	}
    }
    return NULL;
}

int mapper_supported(int mapper)
{
int i;

	for(i= 0; mappers[i].mapper != -1; i++)
		if (mappers[i].mapper == mapper)
			return mappers[i].support;

	return 0;
}


/* dummy routines for when a mapper doesn't need it. */

void dummy_hsync(nes_mapper* mapper, int display_active)
{
}


/* MAPPER 0 Implementation */

void map0_write(nes_mapper* mapper, unsigned short address, unsigned char value)
{
}

nes_mapper* map0_init(nes_ppu* ppu, nes_rom* romfile)
{
nes_mapper* retval= new nes_mapper;

    if (retval) {
	retval->write = map0_write;
	retval->hsync = dummy_hsync;
	
	ppu_latchfunc = NULL;
	nesppu_map_8k(ppu, 0, 0);

	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return retval;
}


/* MAPPER 1 Implementation */

struct mapper_1 {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
    nes_rom* romfile;
    int sequence;
    int accumulator;
    int data[4];
};

void map1_write(struct mapper_1 *mapper, unsigned short address, unsigned char value)
{
    int bank_select;
    
    if (value & 0x80) {
	mapper->data[0] |= 0x0c;
	mapper->accumulator = mapper->data[(address >> 13) & 3];
	mapper->sequence = 5;
    } else {
	mapper->accumulator |= ((value & 1) << (mapper->sequence));
	mapper->sequence++;
    }

    if ((mapper->sequence) == 5) {
	mapper->data[(address >> 13) & 3] = mapper->accumulator;
	mapper->sequence = 0;
	mapper->accumulator = 0;

	switch (mapper->data[0] & 3) {
 	case 0:
	    PPU_mirror_one_low();
	    break;
	case 1:
	    PPU_mirror_one_high();
	    break;
	case 2:
	    PPU_mirror_vertical();
	    break;
 	case 3:
	    PPU_mirror_horizontal();
	    break;
	}
	
	if (mapper->romfile->prg_size == 0x20) { /* 512k cart */
	    bank_select = mapper->data[1] & 0x10;
	} else { /* other carts */
	    bank_select = 0;
	}
	
	if (!(mapper->data[0] & 8)) {
	    nesprg_map_32k(0, (mapper->data[3] & 15) + (bank_select >> 1));
	} else if (mapper->data[0] & 4) {
	    nesprg_map_16k(0, (mapper->data[3] & 15) + bank_select);
	    nesprg_map_16k(1, -1);
	} else {
	    nesprg_map_16k(0, 0);
	    nesprg_map_16k(1, (mapper->data[3] & 15) + bank_select);
	}
	
	if (mapper->data[0] & 0x10) {
	    nesppu_map_4k(mapper->ppu, 0, mapper->data[1]);
	    nesppu_map_4k(mapper->ppu, 1, mapper->data[2]);
	} else {
	    nesppu_map_8k(mapper->ppu, 0, mapper->data[1] >> 1);
	}
    }
}

nes_mapper* map1_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_1 *retval= new struct mapper_1;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map1_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;
	retval->romfile = romfile;
	
	ppu_latchfunc = NULL;
	retval->data[0] = 0x1f; /* FIXME: may be incorrect */
	retval->data[3] = 0x00;

	/* FIXME: may want to set consistent internal state */
	/* NOTE: does nothing when not using CHR-ROM */
	nesppu_map_8k(ppu, 0, 0);
	
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper*)retval;
}


/* MAPPER 2 Implementation */

void map2_write(nes_mapper* mapper, unsigned short address, unsigned char value)
{
    nesprg_map_16k(0, value);
}

nes_mapper* map2_init(nes_ppu* ppu, nes_rom* romfile)
{
nes_mapper* retval= new nes_mapper;

    if (retval) {
	retval->write = map2_write;
	retval->hsync = dummy_hsync;

	/* NOTE: does nothing when not using CHR-ROM */
	nesppu_map_8k(ppu, 0, 0);
	
	ppu_latchfunc = NULL;
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return retval;
}


/* MAPPER 3 Implementation */

struct mapper_3 {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
};

void map3_write(struct mapper_3 *mapper, unsigned short address, unsigned char value)
{
    nesppu_map_8k(mapper->ppu, 0, value);
}

nes_mapper* map3_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_3 *retval= new struct mapper_3;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map3_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	map3_write(retval, 0, 0);
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper*)retval;
}


/* MAPPER 4 Implementation */

struct mapper_4 {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
    nes_rom* romfile;

    unsigned char command;
    
    unsigned char irq_count;
    unsigned char irq_latch;
    unsigned char irq_state;
    
    unsigned char chr_switch[6];
    unsigned char prg_switch[2];
};

void map4_sync_prg(struct mapper_4 *mapper)
{
    if (mapper->command & 0x40) {
	nesprg_map_8k(0, 0xfe);
	nesprg_map_8k(1, mapper->prg_switch[1]);
	nesprg_map_8k(2, mapper->prg_switch[0]);
	nesprg_map_8k(3, 0xff);
    } else {
	nesprg_map_8k(0, mapper->prg_switch[0]);
	nesprg_map_8k(1, mapper->prg_switch[1]);
	nesprg_map_16k(1, -1);
    }
}

void map4_sync_chr(struct mapper_4 *mapper)
{
    if (mapper->command & 0x80) {
	nesppu_map_1k(mapper->ppu, 0, mapper->chr_switch[2]);
	nesppu_map_1k(mapper->ppu, 1, mapper->chr_switch[3]);
	nesppu_map_1k(mapper->ppu, 2, mapper->chr_switch[4]);
	nesppu_map_1k(mapper->ppu, 3, mapper->chr_switch[5]);
	nesppu_map_1k(mapper->ppu, 4, mapper->chr_switch[0]);
	nesppu_map_1k(mapper->ppu, 5, mapper->chr_switch[0] + 1);
	nesppu_map_1k(mapper->ppu, 6, mapper->chr_switch[1]);
	nesppu_map_1k(mapper->ppu, 7, mapper->chr_switch[1] + 1);
    } else {
	nesppu_map_1k(mapper->ppu, 0, mapper->chr_switch[0]);
	nesppu_map_1k(mapper->ppu, 1, mapper->chr_switch[0] + 1);
	nesppu_map_1k(mapper->ppu, 2, mapper->chr_switch[1]);
	nesppu_map_1k(mapper->ppu, 3, mapper->chr_switch[1] + 1);
	nesppu_map_1k(mapper->ppu, 4, mapper->chr_switch[2]);
	nesppu_map_1k(mapper->ppu, 5, mapper->chr_switch[3]);
	nesppu_map_1k(mapper->ppu, 6, mapper->chr_switch[4]);
	nesppu_map_1k(mapper->ppu, 7, mapper->chr_switch[5]);
    }
}

void map4_write(struct mapper_4 *mapper, unsigned short address, unsigned char value)
{
    if (address == 0x8000) {
        mapper->command = value;
	map4_sync_chr(mapper);
	map4_sync_prg(mapper);
    } else if (address == 0x8001) {
        switch (mapper->command & 7) {
	case 0:
	    mapper->chr_switch[0] = value;
	    break;
	case 1:
	    mapper->chr_switch[1] = value;
	    break;
	case 2:
	    mapper->chr_switch[2] = value;
	    break;
	case 3:
	    mapper->chr_switch[3] = value;
	    break;
	case 4:
	    mapper->chr_switch[4] = value;
	    break;
	case 5:
	    mapper->chr_switch[5] = value;
	    break;
	case 6:
            mapper->prg_switch[0] = value;
	    break;
	case 7:
            mapper->prg_switch[1] = value;
	    break;
        }
	map4_sync_chr(mapper);
	map4_sync_prg(mapper);
    } else if (address == 0xa000) {
	if (value & 0x40) {
	    PPU_mirror_one_high();
	} else {
	    if (value & 0x01) {
		PPU_mirror_horizontal();
	    } else {
		PPU_mirror_vertical();
	    }
	}
    } else if (address == 0xc000) {
	mapper->irq_count = value;
    } else if (address == 0xc001) {
	mapper->irq_latch = value;
    } else if (address == 0xe000) {
	mapper->irq_state = 0;
    } else if (address == 0xe001) {
	mapper->irq_state = 1;
    } else {
        printf("map4_write: unknown address. (0x%04hx, 0x%02x)\n", address, value);
    }
}

void map4_hsync(struct mapper_4 *mapper, int display_active)
{
    if (display_active && mapper->irq_state && (mapper->irq_count-- == 0)) {
	mapper->irq_count = mapper->irq_latch;
	nes_external_irq();
    }
}

nes_mapper* map4_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_4 *retval= new struct mapper_4;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map4_write;
	retval->mapperInterface.hsync = (maphsync_t)map4_hsync;
	retval->ppu = ppu;
	retval->romfile = romfile;
	
	ppu_latchfunc = NULL;
	
	retval->command = 0;
	retval->prg_switch[0] = 0;
	retval->prg_switch[1] = 1;
	map4_sync_prg(retval);
	retval->chr_switch[0] = 0;
	retval->chr_switch[1] = 2;
	retval->chr_switch[2] = 4;
	retval->chr_switch[3] = 5;
	retval->chr_switch[4] = 6;
	retval->chr_switch[5] = 7;
	map4_sync_chr(retval);
    }

    return (nes_mapper*)retval;
}


/* MAPPER 7 Implementation */

struct mapper_7 {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
};

void map7_write(struct mapper_7 *mapper, unsigned short address, unsigned char value)
{
    if (value & 0x10) {
	PPU_mirror_one_high();
    } else {
	PPU_mirror_one_low();
    }

    nesprg_map_32k(0, value & 0x0f);
}

nes_mapper* map7_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_7 *retval= new struct mapper_7;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map7_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	nesprg_map_32k(0, 0);
    }

    return (nes_mapper*)retval;
}


/* MAPPER 9 Implementation */

struct map9_latch {
    unsigned char lo_bank;
    unsigned char hi_bank;
    unsigned char state;
};

struct mapper_9 {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
    struct map9_latch latch1;
    struct map9_latch latch2;
};

void map9_latchfunc(struct mapper_9 *mapper, unsigned short address)
{
    if ((address & 0x3ff0) == 0x0fd0) {
	mapper->latch1.state = 0;
	nesppu_map_4k(mapper->ppu, 0, mapper->latch1.lo_bank);
    } else if ((address & 0x3ff0) == 0x0fe0) {
	mapper->latch1.state = 1;
	nesppu_map_4k(mapper->ppu, 0, mapper->latch1.hi_bank);
    } else if ((address & 0x3ff0) == 0x1fd0) {
	mapper->latch2.state = 0;
	nesppu_map_4k(mapper->ppu, 1, mapper->latch2.lo_bank);
    } else if ((address & 0x3ff0) == 0x1fe0) {
	mapper->latch2.state = 1;
	nesppu_map_4k(mapper->ppu, 1, mapper->latch2.hi_bank);
    }
}

void map9_write(struct mapper_9 *mapper, unsigned short address, unsigned char value)
{
    if (address < 0xa000) {
	/* Ignore it. It's not in spec. */
    } else if (address < 0xb000) {
	nesprg_map_8k(0, value);
    } else if (address < 0xc000) {
	mapper->latch1.lo_bank = value;
	if (mapper->latch1.state == 0) {
	    nesppu_map_4k(mapper->ppu, 0, value);
	}
    } else if (address < 0xd000) {
	mapper->latch1.hi_bank = value;
	if (mapper->latch1.state == 1) {
	    nesppu_map_4k(mapper->ppu, 0, value);
	}
    } else if (address < 0xe000) {
	mapper->latch2.lo_bank = value;
	if (mapper->latch2.state == 0) {
	    nesppu_map_4k(mapper->ppu, 1, value);
	}
    } else if (address < 0xf000) {
	mapper->latch2.hi_bank = value;
	if (mapper->latch2.state == 1) {
	    nesppu_map_4k(mapper->ppu, 1, value);
	}
    } else {
	if (value & 1) {
	    PPU_mirror_horizontal();
	} else {
	    PPU_mirror_vertical();
	}
    }
}

nes_mapper* map9_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_9 *retval= new struct mapper_9;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map9_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;

	retval->latch1.lo_bank = 0;
	retval->latch1.hi_bank = 0;
	retval->latch1.state = 0;
	retval->latch2.lo_bank = 0;
	retval->latch2.hi_bank = 0;
	retval->latch2.state = 0;
	ppu_latchfunc = (ppulatch_t)map9_latchfunc;
	nesprg_map_8k(0, 0);
	nesprg_map_8k(1, 0xfd);
	nesprg_map_8k(2, 0xfe);
	nesprg_map_8k(3, 0xff);
    }

    return (nes_mapper*)retval;
}


/* MAPPER A (10) Implementation */

struct mapA_latch {
    unsigned char lo_bank;
    unsigned char hi_bank;
    unsigned char state;
};

struct mapper_A {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
    struct mapA_latch latch1;
    struct mapA_latch latch2;
};

void mapA_latchfunc(struct mapper_A *mapper, unsigned short address)
{
    if ((address & 0x3ff0) == 0x0fd0) {
	mapper->latch1.state = 0;
	nesppu_map_4k(mapper->ppu, 0, mapper->latch1.lo_bank);
    } else if ((address & 0x3ff0) == 0x0fe0) {
	mapper->latch1.state = 1;
	nesppu_map_4k(mapper->ppu, 0, mapper->latch1.hi_bank);
    } else if ((address & 0x3ff0) == 0x1fd0) {
	mapper->latch2.state = 0;
	nesppu_map_4k(mapper->ppu, 1, mapper->latch2.lo_bank);
    } else if ((address & 0x3ff0) == 0x1fe0) {
	mapper->latch2.state = 1;
	nesppu_map_4k(mapper->ppu, 1, mapper->latch2.hi_bank);
    }
}

void mapA_write(struct mapper_A *mapper, unsigned short address, unsigned char value)
{
    if (address < 0xa000) {
	/* Ignore it. It's not in spec. */
    } else if (address < 0xb000) {
	nesprg_map_16k(0, value);
    } else if (address < 0xc000) {
	mapper->latch1.lo_bank = value;
	if (mapper->latch1.state == 0) {
	    nesppu_map_4k(mapper->ppu, 0, value);
	}
    } else if (address < 0xd000) {
	mapper->latch1.hi_bank = value;
	if (mapper->latch1.state == 1) {
	    nesppu_map_4k(mapper->ppu, 0, value);
	}
    } else if (address < 0xe000) {
	mapper->latch2.lo_bank = value;
	if (mapper->latch2.state == 0) {
	    nesppu_map_4k(mapper->ppu, 1, value);
	}
    } else if (address < 0xf000) {
	mapper->latch2.hi_bank = value;
	if (mapper->latch2.state == 1) {
	    nesppu_map_4k(mapper->ppu, 1, value);
	}
    } else {
	if (value & 1) {
	    PPU_mirror_horizontal();
	} else {
	    PPU_mirror_vertical();
	}
    }
}

nes_mapper* mapA_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_A *retval= new struct mapper_A;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)mapA_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;

	retval->latch1.lo_bank = 0;
	retval->latch1.hi_bank = 0;
	retval->latch1.state = 0;
	retval->latch2.lo_bank = 0;
	retval->latch2.hi_bank = 0;
	retval->latch2.state = 0;
	ppu_latchfunc = (ppulatch_t)mapA_latchfunc;
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper*)retval;
}


/* MAPPER B (11) Implementation */

struct mapper_B {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
};

void mapB_write(struct mapper_B *mapper, unsigned short address, unsigned char value)
{
    unsigned char vromptr;
    unsigned char romptr;

    vromptr = (value >> 4) & 0x0f;
    romptr = value & 0x0f;

    nesppu_map_8k(mapper->ppu, 0, vromptr);

    nesprg_map_32k(0, romptr);
}

nes_mapper* mapB_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_B *retval= new struct mapper_B;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)mapB_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	mapB_write(retval, 0, 0);
    }

    return (nes_mapper*)retval;
}


/* MAPPER 17 (23) Implementation */

struct mapper_17 {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
    unsigned char bank_regs[16];
};

void map17_sync_vrom(struct mapper_17 *mapper, int bank)
{
    unsigned char value;

    value = mapper->bank_regs[bank];
    value |= mapper->bank_regs[bank + 1] << 4;

    nesppu_map_1k(mapper->ppu, bank >> 1, value);
}

void map17_write(struct mapper_17 *mapper, unsigned short address, unsigned char value)
{
    if (address == 0x8000) {
	nesprg_map_8k(0, value);
    } else if (address == 0x9000) {
	switch (value & 3) {
	case 0:
	    PPU_mirror_vertical();
	    break;
	case 1:
	    PPU_mirror_horizontal();
	    break;
	case 2:
	    PPU_mirror_one_low();
	    break;
	case 3:
	    PPU_mirror_one_high();
	    break;
	}
    } else if (address == 0xa000) {
	nesprg_map_8k(1, value);
    } else if (((address & 0x8ffc) == 0x8000)
	       && (address >= 0xb000)
	       && (address < 0xf000)) {
	u16 base;

	base = (address >> 10) - 0x2c;
	mapper->bank_regs[base + (address & 3)] = value & 15;
	map17_sync_vrom(mapper, base + (address & 2));
    } else {
	printf("map23: unknown addr 0x%04hx = 0x%02x.\n", address, value);
    }
}

nes_mapper* map17_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_17 *retval= new struct mapper_17;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map17_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;

	ppu_latchfunc = NULL;
#if 0
	nesppu_map_8k(ppu, 0, 0);
#endif
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper*)retval;
}


/* MAPPER 18 (24) Implementation */

struct mapper_18 {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
    unsigned char irq_latch;
    unsigned char irq_count;
    unsigned char irq_state;
};

void map18_write(struct mapper_18 *mapper, unsigned short address, unsigned char value)
{
    if (address == 0x8000) {
	nesprg_map_16k(0, value);
    } else if (address < 0x9000) {
	printf("map18_write: unknown write in 0x8001-0x8fff range.\n");
    } else if (address == 0xb003) {
	switch (value & 0x0c) {
	case 0x00:
	    PPU_mirror_vertical();
	    break;
	case 0x04:
	    PPU_mirror_horizontal();
	    break;
	case 0x08:
	    PPU_mirror_one_low();
	    break;
	case 0x0c:
	    PPU_mirror_one_high();
	    break;
	}
    } else if ((address & 0xfffc) == 0xd000) {
	nesppu_map_1k(mapper->ppu, address & 3, value);
    } else if ((address & 0xfffc) == 0xe000) {
	nesppu_map_1k(mapper->ppu, 4 + (address & 3), value);
    } else if ((address & 0xfffc) == 0xf000) {
	switch (address & 3) {
	case 0:
	    mapper->irq_latch = value;
	    break;
	case 1:
	    mapper->irq_state = value;
	    mapper->irq_count = mapper->irq_latch;
	    break;
	case 2:
	    mapper->irq_state = (mapper->irq_state << 1) | (mapper->irq_state & 1);
	    break;
	case 3:
	    printf("map18: unknown write 0x%02x to 0xf003.\n", value);
	    break;
	}
    }
}

void map18_hsync(struct mapper_18 *mapper, int display_active)
{
    if ((mapper->irq_state & 2) && (++mapper->irq_count == 0)) {
	nes_external_irq();
	mapper->irq_count = mapper->irq_latch;
    }
}

nes_mapper* map18_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_18 *retval= new struct mapper_18;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map18_write;
	retval->mapperInterface.hsync = (maphsync_t)map18_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper*)retval;
}

/* MAPPER 42 (66) Implementation */

struct mapper_42 {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
};

void map42_write(struct mapper_42 *mapper, unsigned short address, unsigned char value)
{
    unsigned char vromptr;
    unsigned char romptr;

    vromptr = value & 0x03;
    romptr = (value >> 4) & 0x03;

    nesppu_map_8k(mapper->ppu, 0, vromptr);
    nesprg_map_32k(0, romptr);
}

nes_mapper* map42_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_42 *retval= new struct mapper_42;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map42_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;
	
	ppu_latchfunc = NULL;
	map42_write(retval, 0, 0);
    }

    return (nes_mapper*)retval;
}


/* MAPPER 44 (68) Implementation */

struct mapper_44 {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
    nes_rom* romfile;
    unsigned char ppu_a10l;
    unsigned char ppu_a10h;
    unsigned char mirror;
};

void map44_sync_mirrors(struct mapper_44 *mapper)
{
    unsigned char *namebase;
    
    if (mapper->mirror & 0x10) {
	namebase = mapper->romfile->chr_data + 0x20000;
	switch (mapper->mirror & 3) {
	case 0:
	    PPU_nametables[0] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[1] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[2] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[3] = namebase + (mapper->ppu_a10h << 10);
	    break;
	case 1:
	    PPU_nametables[0] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[1] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[2] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[3] = namebase + (mapper->ppu_a10h << 10);
	    break;
	case 2:
	    PPU_nametables[0] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[1] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[2] = namebase + (mapper->ppu_a10l << 10);
	    PPU_nametables[3] = namebase + (mapper->ppu_a10l << 10);
	    break;
	case 3:
	    PPU_nametables[0] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[1] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[2] = namebase + (mapper->ppu_a10h << 10);
	    PPU_nametables[3] = namebase + (mapper->ppu_a10h << 10);
	    break;
	}
    } else {
	switch (mapper->mirror & 3) {
	case 0:
	    PPU_mirror_horizontal();
	    break;
	case 1:
	    PPU_mirror_vertical();
	    break;
	case 2:
	    PPU_mirror_one_low();
	    break;
	case 3:
	    PPU_mirror_one_high();
	    break;
	}
    }
}

void map44_write(struct mapper_44 *mapper, unsigned short address, unsigned char value)
{
    switch ((address & 0x7000) >> 12) {
    case 0:
	nesppu_map_2k(mapper->ppu, 0, value);
	break;
    case 1:
	nesppu_map_2k(mapper->ppu, 1, value);
	break;
    case 2:
	nesppu_map_2k(mapper->ppu, 2, value);
	break;
    case 3:
	nesppu_map_2k(mapper->ppu, 3, value);
	break;
    case 4:
	mapper->ppu_a10l = value;
	map44_sync_mirrors(mapper);
	break;
    case 5:
	mapper->ppu_a10h = value;
	map44_sync_mirrors(mapper);
	break;
    case 6:
	mapper->mirror = value;
	map44_sync_mirrors(mapper);
	break;
    case 7:
	nesprg_map_16k(0, value);
	break;
    }
}

nes_mapper* map44_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_44 *retval= new struct mapper_44;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map44_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;
	retval->romfile = romfile;
	
	ppu_latchfunc = NULL;
	nesprg_map_16k(0, 0);
	nesprg_map_16k(1, -1);
    }

    return (nes_mapper*)retval;
}


/* MAPPER 4E (78) Implementation */

struct mapper_4E {
    nes_mapper mapperInterface;
    nes_ppu* ppu;
};

void map4E_write(struct mapper_4E *mapper, unsigned short address, unsigned char value)
{
    unsigned char vromptr;
    unsigned char romptr;

    vromptr = (value >> 4) & 0x0f;
    romptr = value & 0x0f;

    nesppu_map_8k(mapper->ppu, 0, vromptr);
    nesprg_map_16k(0, romptr);
}

nes_mapper* map4E_init(nes_ppu* ppu, nes_rom* romfile)
{
struct mapper_4E *retval= new struct mapper_4E;

    if (retval) {
	retval->mapperInterface.write = (mapwrite_t)map4E_write;
	retval->mapperInterface.hsync = dummy_hsync;
	retval->ppu = ppu;

	ppu_latchfunc = NULL;
	
	nesprg_map_16k(1, -1);
	map4E_write(retval, 0, 0);
    }

    return (nes_mapper*)retval;
}

