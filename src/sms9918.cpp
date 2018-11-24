/*
 * sms9918.c
 *
 * sms custom VDP emulation.
 */

/* $Id: sms9918.cpp,v 1.2 2005/01/18 00:39:03 guest Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sms9918.h"
#include "ui.h"
#include "video.h"
#include "blitters.h"
#include "tiledraw.h"
#include "types.h"

/* manifest constants and flag defines */

#define SMS_VDP_RAMSIZE 0x4000

#define TF_ADDRWRITE 1
#define TF_GAMEGEAR 2


/* tilecache stuff */

u8 sms9918_tilecache[0x8000];
u8 sms9918_tilecache_rev[0x8000];
void sms9918_cache_tile(sms9918* vdp, u16 addr);


/* namecache stuff */
sms9918_namecache sms9918_namecached[0x2000];
void sms9918_cache_name(sms9918* vdp, u16 addr);


/* Palette definition */

int sms_palbase_red[64] = {
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
};

int sms_palbase_green[64] = {
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
};

int sms_palbase_blue[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};


/* port 0 (data) emulation */

u8 sms9918_readport0(sms9918* vdp)
{
    u8 retval;

    if (vdp->address & 0x4000) {
	if (!(vdp->flags & TF_GAMEGEAR)) {
	    retval = vdp->pal[(vdp->address++) & 0x1f];
	    vdp->address &= 0x401f;
	} else {
	    retval = vdp->pal[(vdp->address++) & 0x3f];
	    vdp->address &= 0x403f;
	}
    } else {
	retval = vdp->readahead;
	vdp->readahead = vdp->memory[vdp->address++];
	vdp->address &= 0x3fff;
    }
    vdp->flags &= ~TF_ADDRWRITE;
    return retval;
}

void sms9918_writeport0(sms9918* vdp, u8 data)
{
    if (vdp->address & 0x4000) {
	if (!(vdp->flags & TF_GAMEGEAR)) {
	    vdp->palette_xlat[(vdp->address) & 0x1f] = vid_pre_xlat[data&0x3f];
	    vdp->pal[(vdp->address++) & 0x1f] = data;
	    vdp->address &= 0x401f;
	} else {
	    u8 tmp;
	    vdp->pal[(vdp->address) & 0x3f] = data;

	    tmp = (vdp->pal[((vdp->address) & 0x3f) | 1] << 2) & 0x30;
	    tmp |= (vdp->pal[(vdp->address) & 0x3e] >> 4) & 0x0c;
	    tmp |= (vdp->pal[(vdp->address) & 0x3e] >> 2) & 0x03;

	    vdp->palette_xlat[((vdp->address) >> 1) & 0x1f] = vid_pre_xlat[tmp];
	    vdp->address++;
	    vdp->address &= 0x403f;
	}
    } else {
	vdp->readahead = data;
	vdp->memory[vdp->address] = data;
	sms9918_cache_tile(vdp, vdp->address);
	sms9918_cache_name(vdp, vdp->address);
	vdp->address++;
	vdp->address &= 0x3fff;
    }
    vdp->flags &= ~TF_ADDRWRITE;
}


/* port 1 (status/address/palette/registers) emulation */

u8 sms9918_readport1(sms9918* vdp)
{
    u8 retval;

    retval = vdp->status;
    vdp->status &= 0x3f;
    vdp->flags &= ~TF_ADDRWRITE;
    return retval;
}

void sms9918_write_register(sms9918* vdp, int reg, u8 data)
{
    vdp->regs[reg] = data;
    if (reg == 2) {
	vdp->nametable = &sms9918_namecached[(data & 14) << 9];
    } else if (reg == 8) {
	vdp->coarsescroll = (32 - ((data + 7) >> 3)) & 0x1f;
	vdp->finescroll = (8 - (data & 7)) & 7;
    }
}

void sms9918_writeport1(sms9918* vdp, u8 data)
{
    if (vdp->flags & TF_ADDRWRITE) {
	if (data & 0x80) {
	    if (data & 0x40) {
		if (!(vdp->flags & TF_GAMEGEAR)) {
		    vdp->address = (vdp->addrsave | (data << 8)) & 0x401f;
		} else {
		    vdp->address = (vdp->addrsave | (data << 8)) & 0x403f;
		}
	    } else {
		sms9918_write_register(vdp, data & 15, vdp->addrsave);
	    }
	} else {
	    vdp->address = (vdp->addrsave | (data << 8)) & 0x3fff;
	    if (!(data & 0x40)) {
		vdp->readahead = vdp->memory[vdp->address++];
		vdp->address &= 0x3fff;
	    }
	}
	vdp->flags &= ~TF_ADDRWRITE;
    } else {
	vdp->addrsave = data;
	vdp->flags |= TF_ADDRWRITE;
    }
}


/* scanline read emulation */

u8 sms9918_readscanline(sms9918* vdp)
{
    return (vdp->cur_scanline < 192)? vdp->cur_scanline: 255;
}

const u8 sms9918_cache_palette[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8,
				      9, 10, 11, 12, 13, 14, 15};
void sms9918_cache_tile(sms9918* vdp, u16 addr)
{
    u8 *cur_vbp;
    u8 *cur_vbp2;
    u8 *tiledata;
    u16 tile;
    int line;

    line = (addr << 1) & 0x38;
    tile = addr >> 5;
    
    cur_vbp = &sms9918_tilecache[(tile << 6) + line];
    cur_vbp2 = &sms9918_tilecache_rev[(tile << 6) + line];
    tiledata = &vdp->memory[addr & ~3];

    blit_4_8(tiledata[3], tiledata[2], tiledata[1], tiledata[0], 8, 0, sms9918_cache_palette, cur_vbp);
    blit_4_8_rev(tiledata[3], tiledata[2], tiledata[1], tiledata[0], 8, 0, sms9918_cache_palette, cur_vbp2);
}

void sms9918_cache_name(sms9918* vdp, unsigned short addr)
{
    u8 *tiledata;
    u16 cur_tile;
    sms9918_namecache *namecache;
    
    cur_tile = ltoh16(*((unsigned short *)(&vdp->memory[addr & ~1])));
    namecache = &sms9918_namecached[addr >> 1];
    
    if (cur_tile & 0x200) {
	tiledata = sms9918_tilecache_rev;
    } else {
	tiledata = sms9918_tilecache;
    }
    
    tiledata += ((cur_tile & 0x1ff) << 6);

    if (cur_tile & 0x400) {
	namecache->is_vflip = 1;
    } else {
	namecache->is_vflip = 0;
    }
    
    if (cur_tile & 0x1000) {
	namecache->back_tile = NULL;
	namecache->front_tile = tiledata;
    } else {
	namecache->back_tile = tiledata;
	namecache->front_tile = NULL;
    }
    
    if (cur_tile & 0x800) {
	namecache->palette = &vdp->palette_xlat[16];
    } else {
	namecache->palette = &vdp->palette_xlat[0];
    }
}

void sms9918_preload_namecache(sms9918* vdp)
{
    int i;
    for (i = 0; i < 0x2000; i++) {
	sms9918_cache_name(vdp, i << 1);
    }
}

/* video rendering */

u8 *sms_back_tiles[33];
u8 *sms_front_tiles[33];
u8 *sms_back_palettes[33];
u8 sms_back_finescroll;

void sms9918_init_backcache(sms9918* vdp)
{
    sms9918_namecache *cur_pos;
    sms9918_namecache *cur_tile;
    int cur_scanline;
    int cur_pattern_line;
    int pattern_offset;
    int pattern_offset_rev;
    int scroll;
    int i;

    cur_scanline = vdp->cur_scanline + vdp->regs[9];
    while (cur_scanline > 223) {
	cur_scanline -= 224;
    }
    
    cur_pattern_line = cur_scanline & 7;
    pattern_offset = cur_pattern_line << 3;
    pattern_offset_rev = (7 - cur_pattern_line) << 3;
    
    cur_pos = vdp->nametable;
    cur_pos += (cur_scanline >> 3) << 5;
    
    if ((vdp->regs[0] & 0x40) && (cur_scanline < 16)) {
	scroll = 0;
	sms_back_finescroll = 0;
    } else {
	scroll = vdp->coarsescroll;
	sms_back_finescroll = vdp->finescroll;
    }
    
    for (i = 0; i < 33; i++) {
	cur_tile = &cur_pos[scroll];
	scroll++;
	scroll &= 0x1f;

	sms_back_tiles[i] = cur_tile->back_tile;
	sms_front_tiles[i] = cur_tile->front_tile;
	sms_back_palettes[i] = cur_tile->palette;

	if (cur_tile->is_vflip) {
	    if (sms_back_tiles[i]) {
		sms_back_tiles[i] += pattern_offset_rev;
	    } else {
		sms_front_tiles[i] += pattern_offset_rev;
	    }
	} else {
	    if (sms_back_tiles[i]) {
		sms_back_tiles[i] += pattern_offset;
	    } else {
		sms_front_tiles[i] += pattern_offset;
	    }
	}
    }
}

int sms9918_cache_sprites(sms9918* vdp, u8 **tiledata, u8 *sprite_x)
{
    u8 *sprite_table;
    u16 cur_tile;
    int cur_pattern_line;
    int sprite_8x16;
    int i;
    int num_sprites;

    num_sprites = 0;
    sprite_table = &vdp->memory[(vdp->regs[5] & 0x7e) << 7];
    sprite_8x16 = (vdp->regs[1] & 2)? 16: 8;
    
    for (i = 0; i < 64; i++) {
	if (sprite_table[i] == 208) {
	    break;
	}

	cur_pattern_line = vdp->cur_scanline - sprite_table[i];
	cur_pattern_line &= 0xff;

	if (!(cur_pattern_line < sprite_8x16)) {
	    continue;
	}
	
	if (num_sprites == 8) {
	    /* FIXME: Sprite overflow flag? */
	    break;
	}

	cur_tile = sprite_table[128 + (i << 1) + 1] | ((vdp->regs[6] & 4) << 6);
	if (vdp->regs[1] & 2) {
	    cur_tile &= 0x1fe;
	}
	tiledata[num_sprites] = &sms9918_tilecache[(cur_tile << 6) + (cur_pattern_line << 3)];
	sprite_x[num_sprites] = sprite_table[128 + (i << 1) + 0];
	num_sprites++;
    }

    return num_sprites;
}

void sms9918_draw_sprite_line(u8 *vbp, u8 *cache, u8 *palette, int num_pels)
{
    int i;
    
    for (i = 0; i < num_pels; i++) {
	if (*cache) {
	    *vbp = palette[*cache];
	}
	cache++; vbp++;
    }
}

void sms9918_render_sprites(sms9918* vdp, u8 *cur_vbp)
{
    u8 *tiledata[8];
    u8 sprite_x[8];
    int i;
    int num_sprites;
    
    num_sprites = sms9918_cache_sprites(vdp, tiledata, sprite_x);
    
    for (i = num_sprites - 1; i >= 0; i--) {
	if (sprite_x[i] > 248) {
	    sms9918_draw_sprite_line(&cur_vbp[sprite_x[i]], tiledata[i],
				     &vdp->palette_xlat[16], 256-sprite_x[i]);
	    sms9918_draw_sprite_line(&cur_vbp[0], tiledata[i]+(256-sprite_x[i]),
				     &vdp->palette_xlat[16], 8-(256-sprite_x[i]));
	} else {
	    sms9918_draw_sprite_line(&cur_vbp[sprite_x[i]], tiledata[i],
				     &vdp->palette_xlat[16], 8);
	}
    }
}

void sms9918_render_sprites_gg(sms9918* vdp, unsigned char *cur_vbp)
{
    u8 *tiledata[8];
    u8 sprite_x[8];
    int i;
    int num_sprites;

    num_sprites = sms9918_cache_sprites(vdp, tiledata, sprite_x);
    
    for (i = num_sprites - 1; i >= 0; i--) {
	if (sprite_x[i] < 40) {
	    continue;
	} else if (sprite_x[i] < 48) {
	    sms9918_draw_sprite_line(&cur_vbp[0], tiledata[i]-(sprite_x[i]-48),
				     &vdp->palette_xlat[16], sprite_x[i]-40);
	} else if (sprite_x[i] < (160 + 40)) {
	    sms9918_draw_sprite_line(&cur_vbp[sprite_x[i]-48], tiledata[i],
				     &vdp->palette_xlat[16], 8);
	} else if (sprite_x[i] < (160 + 48)) {
	    sms9918_draw_sprite_line(&cur_vbp[sprite_x[i]-48], tiledata[i],
				     &vdp->palette_xlat[16], 208-sprite_x[i]);
	}
    }
}

void sms9918_render_line_sms(sms9918* vdp)
{
    u8 *video_buffer;
    int i;

    video_buffer = video_GetScanline(vdp->cur_scanline);
    
    if (vdp->regs[1] & 0x40) {
	sms9918_init_backcache(vdp);
	memset(video_buffer, sms_back_palettes[0][0], 256);
	tiledraw_8(video_buffer, sms_back_tiles, sms_back_palettes, 32, sms_back_finescroll);
	sms9918_render_sprites(vdp, video_buffer);
	tiledraw_8(video_buffer, sms_front_tiles, sms_back_palettes, 32, sms_back_finescroll);
	if (vdp->regs[0] & 0x20) {
	    for (i = 0; i < 8; i++) {
		video_buffer[i] = vid_pre_xlat[0];
	    }
	}
    } else {
	/* FIXME: floodfill with black */
    }
}

void sms9918_render_line_gg(sms9918* vdp)
{
    u8 *video_buffer;

    video_buffer = video_GetScanline(vdp->cur_scanline - 24);
    
    if (vdp->regs[1] & 0x40) {
	sms9918_init_backcache(vdp);
	memset(video_buffer, sms_back_palettes[0][0], 160);
	tiledraw_8(video_buffer, sms_back_tiles + 6, sms_back_palettes + 6, 20, sms_back_finescroll);
	sms9918_render_sprites_gg(vdp, video_buffer);
	tiledraw_8(video_buffer, sms_front_tiles + 6, sms_back_palettes + 6, 20, sms_back_finescroll);
    } else {
	/* FIXME: floodfill with black */
    }
}


/* periodic task */

int sms9918_periodic(sms9918* vdp)
{
    /* FIXME: does not account for overscan. */
    if ((!(vdp->flags & TF_GAMEGEAR)) && (vdp->cur_scanline < 192)) {
	sms9918_render_line_sms(vdp);
    } else if ((vdp->flags & TF_GAMEGEAR) && (vdp->cur_scanline >= 24) && (vdp->cur_scanline < 168)) {
	sms9918_render_line_gg(vdp);
    } else if (vdp->cur_scanline == 192) {
/* 	video_events(); */
	video_BuildFrameBuffer();
	vdp->status |= 0x80;
    }
    if (vdp->cur_scanline == 261) {
	vdp->cur_scanline = 0;
	vdp->linecounter = vdp->regs[10];
    } else {
	vdp->cur_scanline++;
	if ((vdp->cur_scanline < 192) && (!(vdp->linecounter--))) {
	    vdp->status |= 0x40;
	    vdp->linecounter = vdp->regs[10];
	}
    }
    return (((vdp->status & 0x80) && (vdp->regs[1] & 0x20)) ||
	    ((vdp->status & 0x40) && (vdp->regs[0] & 0x10)));
}


/* initialization */

u8 sms9918_initial_register_values[11] = {
    0x36, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xfb, 0x00, 0x00, 0x00, 0xff,
};

void sms9918_init_registers(sms9918* vdp)
{
    int i;

    for (i = 0; i < 11; i++) {
	sms9918_write_register(vdp, i, sms9918_initial_register_values[i]);
    }
}

void sms9918_init(sms9918* vdp, int is_gamegear)
{
    if (!is_gamegear) {
	video_SetSize(256, 192);
    } else {
	vdp->flags = TF_GAMEGEAR;
	video_SetSize(160, 144);
    }
    sms9918_preload_namecache(vdp);
    sms9918_init_registers(vdp);
    video_SetPal(64, sms_palbase_red, sms_palbase_green, sms_palbase_blue);
}

sms9918* sms9918_create(int is_gamegear)
{
//sms9918* retval= (sms9918*)calloc(1, sizeof(sms9918));
sms9918* retval= new sms9918;

	if(retval){
//		retval->memory = (unsigned char*)calloc(1, SMS_VDP_RAMSIZE);
		retval->memory = new u8[SMS_VDP_RAMSIZE];

		if (!retval->memory) {
			delete retval;
			retval = NULL;
		}
	}

	if (!retval) {
		printf("sms9918_create(): out of memory.\n");
		return NULL;
	}

	sms9918_init(retval, is_gamegear);

	return retval;
}
