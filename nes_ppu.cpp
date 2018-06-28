#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nes_ppu.h"
#include "nes.h"
#include "tool.h"
#include "ui.h"
#include "video.h"
#include "mappers.h"
#include "nes_psg.h"
#include "tiledraw.h"
#include "palette.h"

#include "nespal.cpp"

nes_ppu nes_ppu_temp = {
    0, 0, 0, 0,
};

nes_ppu* nes_ppu_true = &nes_ppu_temp;

u8 PPU_sprite_ram[0x100];

/* ppu memory areas */
u8 *PPU_memory; /* PPU Ram */

u8 *PPU_banks[8];

u8 **PPU_patterns[2] = {
    &PPU_banks[0],
    &PPU_banks[4],
};

u8 *PPU_nametables[4];

ppulatch_t ppu_latchfunc; /* for mapper support */

long PPU_amode; /* $2006 */

void PPU_render_scan_line(nes_ppu* ppu);

int nesppu_periodic(nes_ppu* ppu)
{
	if (((ppu->scanline < 240) || (ppu->scanline == 262)) && (ppu->control_2 & 0x18))
		ppu->mapper->hsync(ppu->mapper, 1);
    else
		ppu->mapper->hsync(ppu->mapper, 0);
    
	if (ppu->scanline < 240)
	{
		ppu->refresh_data &= 0x7be0;
		ppu->refresh_data |= ppu->refresh_temp & 0x041f;
		PPU_render_scan_line(ppu);
    }
	else if (ppu->scanline == 240)
	{
		ppu->status = 0x80;
		video_BuildFrameBuffer();
    }

    ppu->scanline++;
    if(ppu->scanline == 262)
	{
		ppu->status = 0;
		ppu->scanline = 0;
		ppu->refresh_data = ppu->refresh_temp;
    }

    return (ppu->control_1 & 0x80) && (ppu->scanline == 243);
}

void nesppu_cache_tile_line(u8 pattern0, u8 pattern1, u8 *cache)
{
u8 chunks_0= ((pattern0 >> 1) & 0x55) | (pattern1 & 0xaa);
u8 chunks_1= (pattern0 & 0x55) | ((pattern1 << 1) & 0xaa);
u8 *cacheptr= cache;

    *cacheptr++ = (chunks_0 >> 6) & 3;
    *cacheptr++ = (chunks_1 >> 6) & 3;
    *cacheptr++ = (chunks_0 >> 4) & 3;
    *cacheptr++ = (chunks_1 >> 4) & 3;
    *cacheptr++ = (chunks_0 >> 2) & 3;
    *cacheptr++ = (chunks_1 >> 2) & 3;
    *cacheptr++ = chunks_0 & 3;
    *cacheptr++ = chunks_1 & 3;
}

void nesppu_cache_chr_rom(u8 *chr_data, int chr_size, u8 *tilecache)
{
u8 *cacheptr= tilecache;
u8 *dataptr;
int page;
int tile;
int line;

	for(page = 0; page < (chr_size << 3); page++)
	{
		for(line = 0; line < 8; line++)
		{
	    	dataptr = chr_data + (page * 0x400) + line;

			for(tile = 0; tile < 0x40; tile++)
			{
				nesppu_cache_tile_line(*dataptr, *(dataptr + 8), cacheptr);
				cacheptr += 8;
				dataptr += 0x10;
	    	}
		}
    }
}

void PPU_init(nes_rom* romfile)
{
int i;

	nes_ppu_true->romfile= romfile;
	
	video_SetSize(256, 240);
	video_SetPal(64, nes_palbase_red, nes_palbase_green, nes_palbase_blue);
	
	PPU_memory = new u8[0x3000];
	
	nes_ppu_true->render_palette = new_palette_8(8, 4);
	
	if (romfile->chr_size)
	{
		nes_ppu_true->tilecache = new u8[romfile->chr_size * 0x2000 * 4];
		nesppu_cache_chr_rom(romfile->chr_data, romfile->chr_size, nes_ppu_true->tilecache);
	}
	else
	{
//		nes_ppu_true->tilecache = (u8*)calloc(1, 0x2000 * 4);
		nes_ppu_true->tilecache = new u8[1 * 0x2000 * 4];
		nes_ppu_true->pattern_cache[0] = nes_ppu_true->tilecache + 0x0000;
		nes_ppu_true->pattern_cache[1] = nes_ppu_true->tilecache + 0x1000;
		nes_ppu_true->pattern_cache[2] = nes_ppu_true->tilecache + 0x2000;
		nes_ppu_true->pattern_cache[3] = nes_ppu_true->tilecache + 0x3000;
		nes_ppu_true->pattern_cache[4] = nes_ppu_true->tilecache + 0x4000;
		nes_ppu_true->pattern_cache[5] = nes_ppu_true->tilecache + 0x5000;
		nes_ppu_true->pattern_cache[6] = nes_ppu_true->tilecache + 0x6000;
		nes_ppu_true->pattern_cache[7] = nes_ppu_true->tilecache + 0x7000;
	}
	
	for (i = 0; i < 8; i++)
		PPU_banks[i] = PPU_memory + (i * 0x400);
	
	if(romfile->mirror_vertical)
		PPU_mirror_vertical();
	else
		PPU_mirror_horizontal();
	
	nes_ppu_true->chr_is_rom = romfile->chr_size;
}

void nesppu_set_mapper(nes_ppu* ppu, nes_mapper* mapper)
{
    ppu->mapper= mapper;
}

int nesppu_mask_page_to(int page, int mask)
{
    /*
     * NOTE: This is two cheap hacks piled one atop another. The first is
     * simply anding page with mask - 1, which relies on mask being a power
     * of two (I believe it is in all good dumps of non-pirate games). The
     * second is to not actually mask the page if the page number is less
     * than the mask. This fixes for the unfortunately rather popular bad
     * dump of Zelda II, which is missing some otherwise unused CHR-ROM
     * pages, "but it works with every other emulator". The _reason_ it
     * works with every other emulator, of course, is because every other
     * emulator author got fed up with hearing "but it works with NESticle",
     * and worked around the problem in some way, but people don't want to
     * hear that, they just want to play their thrice-damned "p1r473 R0MZ".
     */
	if (page < mask) 
		return page;
    
	return (page & (mask - 1));
}

void nesppu_map_1k(nes_ppu* ppu, int bank, int page)
{
int	masked_page;
int	num_banks;
u8	*mem_base;
u8	*cache_base;

	if(ppu->chr_is_rom)
	{
		num_banks = ppu->romfile->chr_size << 3;
		mem_base = ppu->romfile->chr_data;
		cache_base = ppu->tilecache;
	}
	else
	{
		num_banks = ppu->pageram_size << 3;
		mem_base = ppu->pageram_memory;
		cache_base = ppu->pageram_tilecache;
	}
	
	if(!num_banks)
		return;
	
	masked_page	= nesppu_mask_page_to(page, num_banks);
	PPU_banks[bank]	= mem_base + (masked_page * 0x400);
	ppu->pattern_cache[bank] = cache_base + (masked_page * 0x400 * 4);
}

void nesppu_map_2k(nes_ppu* ppu, int bank, int page)
{
	nesppu_map_1k(ppu, (bank << 1) + 0, (page << 1) + 0);
	nesppu_map_1k(ppu, (bank << 1) + 1, (page << 1) + 1);
}

void nesppu_map_4k(nes_ppu* ppu, int bank, int page)
{
	nesppu_map_2k(ppu, (bank << 1) + 0, (page << 1) + 0);
	nesppu_map_2k(ppu, (bank << 1) + 1, (page << 1) + 1);
}

void nesppu_map_8k(nes_ppu* ppu, int bank, int page)
{
	nesppu_map_4k(ppu, (bank << 1) + 0, (page << 1) + 0);
	nesppu_map_4k(ppu, (bank << 1) + 1, (page << 1) + 1);
}

void nesppu_paged_ram_init(int num_8k_pages)
{
	nes_ppu_true->pageram_memory	= new u8[num_8k_pages * 0x2000];
	nes_ppu_true->pageram_tilecache	= new u8[num_8k_pages * 0x8000];
	nes_ppu_true->pageram_size		= num_8k_pages;
}

void nesppu_paged_ram_mode(int enabled)
{
    if(enabled)
		nes_ppu_true->chr_is_rom = 0;
	else
	{
		nes_ppu_true->chr_is_rom = nes_ppu_true->romfile->chr_size;
		
		if(!nes_ppu_true->chr_is_rom)
		{
			nes_ppu_true->pattern_cache[0] = nes_ppu_true->tilecache + 0x0000;
			nes_ppu_true->pattern_cache[1] = nes_ppu_true->tilecache + 0x1000;
			nes_ppu_true->pattern_cache[2] = nes_ppu_true->tilecache + 0x2000;
			nes_ppu_true->pattern_cache[3] = nes_ppu_true->tilecache + 0x3000;
			nes_ppu_true->pattern_cache[4] = nes_ppu_true->tilecache + 0x4000;
			nes_ppu_true->pattern_cache[5] = nes_ppu_true->tilecache + 0x5000;
			nes_ppu_true->pattern_cache[6] = nes_ppu_true->tilecache + 0x6000;
			nes_ppu_true->pattern_cache[7] = nes_ppu_true->tilecache + 0x7000;
		}
    }
}

void PPU_mirror_horizontal()
{
	PPU_nametables[0] = &(PPU_memory[0x2000]);
	PPU_nametables[1] = &(PPU_memory[0x2000]);
	PPU_nametables[2] = &(PPU_memory[0x2400]);
	PPU_nametables[3] = &(PPU_memory[0x2400]);
}

void PPU_mirror_vertical()
{
	PPU_nametables[0] = &(PPU_memory[0x2000]);
	PPU_nametables[1] = &(PPU_memory[0x2400]);
	PPU_nametables[2] = &(PPU_memory[0x2000]);
	PPU_nametables[3] = &(PPU_memory[0x2400]);
}

void PPU_mirror_one_low()
{
	PPU_nametables[0] = &(PPU_memory[0x2000]);
	PPU_nametables[1] = &(PPU_memory[0x2000]);
	PPU_nametables[2] = &(PPU_memory[0x2000]);
	PPU_nametables[3] = &(PPU_memory[0x2000]);
}

void PPU_mirror_one_high()
{
	PPU_nametables[0] = &(PPU_memory[0x2400]);
	PPU_nametables[1] = &(PPU_memory[0x2400]);
	PPU_nametables[2] = &(PPU_memory[0x2400]);
	PPU_nametables[3] = &(PPU_memory[0x2400]);
}

void PPU_write(nes_ppu* ppu, u16 address, u8 value)
{
	address = address & 0x3fff;
    
	if(ppu_latchfunc)
		ppu_latchfunc(ppu->mapper, address);
    
	if (address >= 0x3f00)
	{
//		u8 screen_color= vid_pre_xlat[value & 0x3f];

		if ((address & 0x3f03) == 0x3f00) {
			ppu->pal[0x00 + (address & 0x0f)] = value;
			ppu->pal[0x10 + (address & 0x0f)] = value;
			ppu->render_palette->set(ppu->render_palette, 0x00 + (address & 0x0f), value);
			ppu->render_palette->set(ppu->render_palette, 0x10 + (address & 0x0f), value);
		} 
		else
		{
			ppu->pal[address & 0x1f] = value;
			ppu->render_palette->set(ppu->render_palette, address & 0x1f, value);
		}
    }
	else if (address > 0x1fff)
	{
		PPU_nametables[(address >> 10) & 3][address & 0x3ff] = value;
    }
	else if(!nes_ppu_true->chr_is_rom)
	{
		u8 *cur_bank= PPU_banks[(address >> 10)];
		u8 *cache_bank= ppu->pattern_cache[address >> 10];
		
		cur_bank[(address & 0x3ff)] = value;
		nesppu_cache_tile_line(cur_bank[(address & 0x3f7)], cur_bank[(address & 0x3ff) | 8], cache_bank + ((address & 0x3f0) >> 1) + ((address & 7) << 9));
    }
}

u8 PPU_read(nes_ppu* ppu, u16 address)
{
u8 value;
    
	address &= 0x3fff;

	if(ppu_latchfunc)
		ppu_latchfunc(ppu->mapper, address);

	if(address >= 0x3f00)
		value = ppu->pal[address & 0x1f];
	else if (address >= 0x2000)
		value = PPU_nametables[(address >> 10) & 3][address & 0x3ff];
	else
		value = PPU_banks[(address >> 10)][(address & 0x3ff)];

	return value;
}

typedef struct tagspritedata
{
    u8 vpos;
    u8 tile;
    u8 flags;
    u8 xpos;
}spritedata;

typedef struct tagTSpriteCache
{
    u8 xpos;
    u8 pattern0;
    u8 pattern8;
    u8 flags;
    u8 *pattern;
}TSpriteCache;

TSpriteCache spritecache[8];

#define SFL_VFLIP 0x80
#define SFL_HFLIP 0x40
#define SFL_PRIO  0x20
#define SFL_COLOR 0x03

int num_sprites_line;

void PPU_init_spritecache(nes_ppu* ppu, u16 line, int is_8x16)
{
spritedata 	*sprites= (spritedata *)PPU_sprite_ram;
int 		cur_ptbl= (ppu->control_1 & 0x08)? 1: 0;
u8 			cur_tile;
u8 			cur_pattern_line;
u8 			*cur_pattern;
int 		i;
  
	num_sprites_line = 0;
    
	/* FIXME: clear sprite count */
    
	for (i = 0; i < 64; i++)
	{
		if ((sprites[i].vpos + 1 <= line) && (sprites[i].vpos + (is_8x16? 17: 9) > line))
		{
	    	if (num_sprites_line > 7)
			{
				/* FIXME: set sprite count */
				break;
			}
	
			spritecache[num_sprites_line].xpos = sprites[i].xpos;
			spritecache[num_sprites_line].flags = sprites[i].flags;

			cur_pattern_line = (line - (sprites[i].vpos+1)) & (is_8x16? 15: 7);
			
			if (sprites[i].flags & SFL_VFLIP)
				cur_pattern_line = (is_8x16? 15: 7) - cur_pattern_line;
	    
			cur_tile = sprites[i].tile;
	    
			if (is_8x16)
			{
				cur_ptbl = cur_tile & 1;
				cur_tile = (cur_pattern_line & 8)?
				(cur_tile | 1): (cur_tile & 0xfe);
				cur_pattern_line &= 7;
			}
   
			if (ppu_latchfunc)
			{
				ppu_latchfunc(ppu->mapper, (cur_ptbl << 12) | (cur_tile << 4) | (cur_pattern_line));
				ppu_latchfunc(ppu->mapper, (cur_ptbl << 12) | (cur_tile << 4) | (cur_pattern_line) | 8);
			}

			cur_pattern = &(PPU_patterns[cur_ptbl][(cur_tile >> 6)][((cur_tile & 0x3f) << 4) + cur_pattern_line]);

			spritecache[num_sprites_line].pattern0 = cur_pattern[0];
			spritecache[num_sprites_line].pattern8 = cur_pattern[8];

			spritecache[num_sprites_line].pattern = &(ppu->pattern_cache[(cur_ptbl << 2) + (cur_tile >> 6)][((cur_tile & 0x3f) << 3) + (cur_pattern_line << 9)]);

			if ((i == 0) && ((cur_pattern[0]) || cur_pattern[8]))
				ppu->status |= 0x40;

			num_sprites_line++;
		}
    }
}

void spritedraw_8(u8 *vbp, int offset, u8 *pattern, u8 *palette)
{
u8	*cur_vbp	= vbp + offset;
u8	*tiledata	= pattern;
int i;

	for(i = 0; i < 8; ++i)
	{
		if (*tiledata)
			*cur_vbp = palette[*tiledata];

		tiledata++;
		cur_vbp++;
    }
}

void spritedraw_rev_8(u8 *vbp, int offset, u8 *pattern, u8 *palette)
{
u8	*cur_vbp	= vbp + offset;;
u8	*tiledata= pattern + 7;;
int	i;

	for(i = 0; i < 8; ++i)
	{
		if (*tiledata)
			*cur_vbp = palette[*tiledata];

		tiledata--;
		cur_vbp++;
    }
}

void PPU_render_sprites(nes_ppu* ppu, u8 *cur_vbp, int behind)
{
u8	*colors;
int	i;

	if(!num_sprites_line)
		return;
    
    for(i = (num_sprites_line - 1); i >= 0; i--)
	{
		if((behind)?!(spritecache[i].flags & SFL_PRIO):(spritecache[i].flags & SFL_PRIO))
			continue;
	
		colors = (u8*)ppu->render_palette->palettes[4 + (spritecache[i].flags & SFL_COLOR)];
	
		if (!(spritecache[i].flags & SFL_HFLIP))
			spritedraw_8(cur_vbp, spritecache[i].xpos, spritecache[i].pattern, colors);
		else
			spritedraw_rev_8(cur_vbp, spritecache[i].xpos, spritecache[i].pattern, colors);
	}
}

void nesppu_cache_background(nes_ppu* ppu, u8 **patterns, u8 **attrs)
{
u8	tile_offset		= ppu->refresh_data & 0x1f;
u8	*cur_bank		= PPU_nametables[(ppu->refresh_data >> 10) & 3];
u8	*cur_tile		= &cur_bank[(ppu->refresh_data & 0x3e0) + tile_offset];
u8	*cur_attrline	= cur_bank + ((ppu->refresh_data >> 4) & 0x38) + 0x3c0 + (tile_offset >> 2);
u16	cur_patternline	= (ppu->refresh_data >> 3) & 0x0e00;
u8	cur_attrbase	= (ppu->refresh_data >> 4) & 4;
u8	**cur_ptbl		= &ppu->pattern_cache[(ppu->control_1 & 0x10) >> 2];
int i;

	cur_attrbase |= tile_offset & 2;
	
	for(i = 0; i < 33; i++)
	{
		patterns[i] = &(cur_ptbl[(*cur_tile >> 6)][((*cur_tile & 0x3f) << 3) + cur_patternline]);
	
		if(ppu_latchfunc)
		{
			ppu_latchfunc(ppu->mapper, ((ppu->control_1 & 0x10) << 8) | ((*cur_tile) << 4) | cur_patternline);
			ppu_latchfunc(ppu->mapper, ((ppu->control_1 & 0x10) << 8) | ((*cur_tile) << 4) | cur_patternline | 8);
		}

		attrs[i] = (u8*)ppu->render_palette->palettes[(*cur_attrline >> cur_attrbase) & 3];
	
		if(tile_offset & 1)
			cur_attrbase ^= 2;

		if ((tile_offset & 3) == 3)
			cur_attrline++;

		cur_tile++;
		tile_offset++;
	
		if (tile_offset > 31)
		{
			tile_offset	= 0;
			cur_bank	= PPU_nametables[((ppu->refresh_data >> 10) & 3) ^ 1];
			cur_tile	= &cur_bank[ppu->refresh_data & 0x3e0];
			cur_attrline= cur_bank + ((ppu->refresh_data >> 4) & 0x38) + 0x3c0;
		}
	}
}

void PPU_render_background(nes_ppu* ppu, char *cur_vbp)
{
	u8 *patterns[33];
	u8 *attrs[33];
	int i;
    
	nesppu_cache_background(ppu, patterns, attrs);
    
	if (ppu->control_2 & 0x02)
		i = 0;
	else
	{
		for (i = 0; i < 8; i++)
		    *cur_vbp++ = vid_pre_xlat[0x0f];
		
		i = 1;
	}

	tiledraw_8((u8*)cur_vbp, patterns + i, attrs + i, 32 - i, ppu->finescroll);
}

void floodfill_8(u8 *vbp, int width, u8 *palette, int index)
{
	memset(vbp, palette[index], width);
}

void PPU_render_scan_line(nes_ppu* ppu)
{
u8 *cur_vbp= video_GetScanline(ppu->scanline);
    
	floodfill_8((u8*)cur_vbp, 256, (u8*)ppu->render_palette->palettes[0], 0);

	if (ppu->control_2 & 0x10)
	{
		PPU_init_spritecache(ppu, ppu->scanline, ppu->control_1 & 0x20);
		PPU_render_sprites(ppu, cur_vbp, 1);
    }
    
	if (ppu->control_2 & 0x08)
		PPU_render_background(ppu, (char*)cur_vbp);
    
	if (ppu->control_2 & 0x10)
		PPU_render_sprites(ppu, cur_vbp, 0);
    
	ppu->refresh_data += 0x1000;

	if (ppu->refresh_data & 0x8000)
	{
		u16 tmp= (ppu->refresh_data & 0x03e0) + 0x20;

		ppu->refresh_data &= 0x7c1f;
		
		if (tmp == 0x03c0)
	    	ppu->refresh_data ^= 0x0800;
		else
	    	ppu->refresh_data |= (tmp & 0x03e0);
    }
}

void ppu_io_write(nes_ppu* ppu, u16 addr, u8 value)
{

	switch(addr & 7)
	{
	case 0:
		ppu->control_1 = value;
		ppu->refresh_temp &= 0x73ff;
		ppu->refresh_temp |= (value & 3) << 10;
		break;
	case 1:
		ppu->control_2 = value;
		break;
	case 2:
		printf("ppu_io_write(2, %02x): Unknown register.\n", value);
		break;
	case 3:
		ppu->sprite_addy = value;
		break;
	case 4:
		PPU_sprite_ram[ppu->sprite_addy++] = value;
		break;
	case 5:
		if(PPU_amode & 1)
		{
			PPU_amode &= ~1;
			ppu->refresh_temp &= 0x0c1f;
			ppu->refresh_temp |= (value & 7) << 12;
			ppu->refresh_temp |= (value << 2) & 0x03e0;
		}
		else
		{
			PPU_amode |= 1;
			ppu->refresh_temp &= 0x7fe0;
			ppu->refresh_temp |= value >> 3;
			ppu->finescroll = value & 7;
		}
		break;
	case 6:
		if (PPU_amode & 1)
		{
			PPU_amode &= ~1;
			ppu->address = (ppu->address_latch << 8) | value;
		
			ppu->refresh_temp &= 0xff00;
			ppu->refresh_temp |= value;
			ppu->refresh_data = ppu->refresh_temp;
		}else{
			PPU_amode |= 1;
			ppu->address_latch = value;
		
			ppu->refresh_temp &= 0x00ff;
			ppu->refresh_temp |= (value & 0x3f) << 8;
		}
		break;
	case 7:
		if (PPU_amode & 1)
			printf("ppu_w_2007(): ppu write during addr sequence.\n");

		PPU_write(ppu, ppu->address, value);
		ppu->address += ((ppu->control_1 & 0x04)? 0x20: 1);
		break;
    }
}

u8 ppu_io_read(nes_ppu* ppu, u16 addr)
{
u8 retval;
    
    switch (addr & 7)
	{
	/* NOTE: to the best of my knowledge, these ports are synonymous */
	case 0: case 1: case 2: case 3: case 4: case 5: case 6:
		retval = ppu->status;
		ppu->status &= 0x7f;
		PPU_amode &= ~1;
		return retval;
		break;
    case 7:
		retval = ppu->read_latch;
		ppu->read_latch = PPU_read(ppu, ppu->address);
		ppu->address += ((ppu->control_1 & 0x04)? 0x20: 1);
		return retval;
		break;
	}
	
	return 0; /* gcc -O2 is a chode. */
}
