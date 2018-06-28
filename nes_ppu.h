/*
 * nes_ppu.h
 *
 * emulation interface for NES PPU
 */

/* $Id: nes_ppu.h,v 1.1 2005/01/11 03:12:49 alainp Exp $ */

#ifndef NES_PPU_H
#define NES_PPU_H

#ifndef NES_H
#include "nes.h"
#endif

extern nes_ppu* nes_ppu_true;

/* this include must come after the typedef for nes_ppu */
#ifndef MAPPERS_H
#include "mappers.h"
#endif

/* PPU control */
void PPU_mirror_horizontal(void);
void PPU_mirror_vertical(void);
void PPU_mirror_one_low(void);
void PPU_mirror_one_high(void);
int nesppu_periodic(nes_ppu* ppu);
void nesppu_set_mapper(nes_ppu* ppu, nes_mapper* mapper);

/* PPU memory initialization */
void PPU_init(nes_rom* romfile);

/* PPU memory mapping */
void nesppu_map_1k(nes_ppu* ppu, int bank, int page);
void nesppu_map_2k(nes_ppu* ppu, int bank, int page);
void nesppu_map_4k(nes_ppu* ppu, int bank, int page);
void nesppu_map_8k(nes_ppu* ppu, int bank, int page);

/* PPU functions */
void PPU_render_video_frame(void);

/* PPU I/O */
void ppu_io_write(nes_ppu* ppu, unsigned short addr, unsigned char value);
unsigned char ppu_io_read(nes_ppu* ppu, unsigned short addr);

/* PPU debug output */
void PPU_dump_memory(short address);
void PPU_dump_pattern(short address);

/* PPU nametables */
extern unsigned char *PPU_nametables[4];

/* PPU sprite RAM */
extern unsigned char PPU_sprite_ram[0x100];

/* support for mappers that use latches */
typedef void (* ppulatch_t)(nes_mapper* mapper, unsigned short address);
extern ppulatch_t ppu_latchfunc;

/* support for paged CHR RAM */
void nesppu_paged_ram_init(int num_8k_pages);
void nesppu_paged_ram_mode(int enabled);

#endif /* NES_PPU_H */
