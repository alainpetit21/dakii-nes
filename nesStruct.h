#ifndef NES_STRUCT_H
#define NES_STRUCT_H

#include "tool.h"
#include "types.h"
#include "palette.h"

typedef struct tagnes_rom {
    rom_file* file;
    u8 *prg_data;
    u8 *chr_data;
    int prg_size;
    int chr_size;
    int mapper;
    int uses_battery;
    int mirror_vertical;
}nes_rom;
/* defines for mapper interfaces */

typedef struct tagnes_mapper
{
	void (* write)(struct tagnes_mapper* mapper, u16 address, u8 value);
	void (* hsync)(struct tagnes_mapper* mapper, int display_active);
}nes_mapper;

typedef struct tagnes_ppu {
    u16 scanline;
    u16 refresh_data;
    u16 refresh_temp;
    int finescroll;

    u8 pal[0x20];
    palette* render_palette;

    u8 control_1; /* $2000 */
    u8 control_2; /* $2001 */
    u8 status; /* $2002 */
    u8 sprite_addy; /* $2003 */

    u8 address_latch;
    u8 read_latch;
    u16 address;
    
    u8 *tilecache;
    u8 *pattern_cache[8];

    int pageram_size;
    u8 *pageram_memory;
    u8 *pageram_tilecache;

    int chr_is_rom;
    
    nes_mapper* mapper;
    nes_rom* romfile;
}nes_ppu;

typedef struct tagmapper_support {
    int mapper;
    const char *name;
	nes_mapper* (*create)(nes_ppu* ppu, nes_rom* romfile);
    int support; /* mapper support. 0: none. 1: partial. 2: full. */
}mapper_support;

#endif // NES_STRUCT_H
