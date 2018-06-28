/*
 * pce_vdp.c
 *
 * HuC6270/HuC6260 emulation.
 */

/* $Id: pce_vdp.cpp,v 1.2 2005/01/18 00:39:03 guest Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pce_vdp.h"
#include "ui.h"
#include "video.h"
#include "blitters.h"
#include "tiledraw.h"
#include "types.h"

#define PCE_VDP_RAMSIZE 0x10000

/* FIXME: these sizes may be wrong */
u8 pce_tilecache[PCE_VDP_RAMSIZE << 1];
u8 pce_tilecache_dirty[PCE_VDP_RAMSIZE >> 5];

int pce_palbase_red[64] = {
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff,
};

int pce_palbase_blue[64] = {
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
    0x00, 0x55, 0xaa, 0xff, 0x00, 0x55, 0xaa, 0xff,
};

int pce_palbase_green[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

u8 pce_vce_read(pce_vdp* vdp, u16 addr)
{
u8 retval;

	if((addr & 6) == 4)
	{
		if(addr & 1)
		{
			retval= vdp->palette_h[vdp->palette_addr];
			vdp->palette_addr += 1;
			vdp->palette_addr &= 0x1ff;
			return retval;
		}
		else
			return vdp->palette_l[vdp->palette_addr];
	}
	return 0x00;
}

#define PCE_COLOR_XLAT(x) vid_pre_xlat[((vdp->palette_l[x] & 0x6) >> 1) | ((vdp->palette_l[x] & 0x30) >> 2) | ((vdp->palette_l[x] & 0x80) >> 3) | ((vdp->palette_h[x] & 1) << 5)]
void pce_vce_write(pce_vdp* vdp, u16 addr, u8 data)
{
	switch (addr & 7)
	{
	case 0:
		/* FIXME: add control register */
		break;
	case 2:
		vdp->palette_addr &= 0x100;
		vdp->palette_addr |= data;
		break;
	case 3:
		if (data & 1)
			vdp->palette_addr |= 0x100;
		else
			vdp->palette_addr &= 0xff;
		break;
	case 4:
		vdp->palette_l[vdp->palette_addr] = data;
		vdp->palette_xlat[vdp->palette_addr] = PCE_COLOR_XLAT(vdp->palette_addr);
		break;
	case 5:
		vdp->palette_h[vdp->palette_addr] = data & 1;
		vdp->palette_xlat[vdp->palette_addr] = PCE_COLOR_XLAT(vdp->palette_addr);
		vdp->palette_addr += 1;
		vdp->palette_addr &= 0x1ff;
		break;
	}
}

unsigned char pce_vdp_read(pce_vdp* vdp, u16 addr)
{
u8 retval;
    
	switch (addr & 3)
	{
	case 0:
		/* 	printf("pce_vdp: statread.\n"); */
		retval = vdp->status;
		vdp->status &= ~0x24; /* clear vblank bit */ /* FIXME: may be wrong */
		return retval;
	case 1:
		return 0x00;
	case 2:
		if(vdp->cur_reg != 2)
			printf("pce_vdp: r1l cur_reg %d.\n", vdp->cur_reg);
		else
			return vdp->memory[(vdp->mem_read_addr << 1) + 0];
		
		return 0x00;
	case 3:
		if (vdp->cur_reg != 2)
			printf("pce_vdp: r1h cur_reg %d.\n", vdp->cur_reg);
		else
			return vdp->memory[(vdp->mem_read_addr++ << 1) + 1];
		
		return 0x00;
	}
	
	return 0x00;
}

void pce_vdp_write(pce_vdp* vdp, u16 addr, u8 data)
{
	switch (addr & 3)
	{
	case 0:
		vdp->cur_reg = data & 0x1f;
		break;
	case 1:
		break;
	case 2:
		switch (vdp->cur_reg)
		{
		case 0:
			vdp->mem_write_addr &= 0xff00;
			vdp->mem_write_addr |= data;
			break;
		case 1:
			vdp->mem_read_addr &= 0xff00;
			vdp->mem_read_addr |= data;
			break;
		case 2:
			vdp->memory[((vdp->mem_write_addr & 0x7fff) << 1) + 0] = data;
			pce_tilecache_dirty[(vdp->mem_write_addr & 0x7fff) >> 4] = 1;
			break;
		case 5: /* Control */
			vdp->regs_l[5] = data;
			break;
		case 6: /* Raster counter */
			/*printf("pce_vdp: rcrl %d.\n", data); */
			vdp->regs_l[6] = data;
			break;
		case 7: /* BG X-Scroll */
			vdp->regs_l[7] = data;
			break;
		case 8: /* BG Y-Scroll */
			/*printf("pce_vdp: BYR low set 0x%02x, %d.\n", data, vdp->cur_scanline); */
			vdp->regs_l[8] = data;
			vdp->y_scroll &= 0xff00;
			vdp->y_scroll |= data;
			break;
		case 9: /* Memory width? */
			vdp->regs_l[9] = data;
			break;
		case 10: /* Horizontal Sync? */
			break;
		case 11: /* Horizontal Display? */
			vdp->regs_l[11] = data;
			/*printf("pce_vdp: hdw = %d.\n", data & 0x7f); */
			break;
		case 12: /* Vertical Sync? */
			break;
		case 13: /* Vertical Display? */
			break;
		case 14: /* Vertical Display End? */
			break;
		case 15: /* DMA Control */
			break;
		case 19: /* Sprite attributes? */
			vdp->regs_l[19] = data;
			break;
		default:
			printf("pce_vdp: regwrite %d l = 0x%02x.\n", vdp->cur_reg, data);
		}
		break;
	case 3:
		switch (vdp->cur_reg)
		{
		case 0:
			vdp->mem_write_addr &= 0x00ff;
			vdp->mem_write_addr |= (data << 8);
			break;
		case 1:
			vdp->mem_read_addr &= 0x00ff;
			vdp->mem_read_addr |= (data << 8);
			break;
		case 2:
			vdp->memory[((vdp->mem_write_addr & 0x7fff) << 1) + 1] = data;
			pce_tilecache_dirty[(vdp->mem_write_addr & 0x7fff) >> 4] = 1;
			
			switch (vdp->regs_h[5] & 0x18)
			{
			case 0x00:
				vdp->mem_write_addr += 1;
				break;
			case 0x08:
				vdp->mem_write_addr += 32;
				break;
			case 0x10:
				vdp->mem_write_addr += 64;
				break;
			case 0x18:
				vdp->mem_write_addr += 128;
				break;
			}
		    break;
		case 5: /* Control */
			vdp->regs_h[5] = data;
			break;
		case 6: /* Raster counter */
			vdp->regs_h[6] = data;
			break;
		case 7: /* BG X-Scroll */
			vdp->regs_h[7] = data;
			break;
		case 8: /* BG Y-Scroll */
			/*printf("pce_vdp: BYR high set 0x%02x, %d.\n", data, vdp->cur_scanline); */
			vdp->regs_h[8] = data;
			vdp->y_scroll &= 0x00ff;
			vdp->y_scroll |= (data << 8);
			break;
		case 9: /* Memory width? */
			break;
		case 10: /* Horizontal Sync? */
			break;
		case 11: /* Horizontal Display? */
			break;
		case 12: /* Vertical Sync? */
			break;
		case 13: /* Vertical Display? */
			break;
		case 14: /* Vertical Display End? */
			break;
		case 15: /* DMA Control */
			break;
		case 19: /* Sprite attributes? */
			vdp->regs_h[19] = data;
			break;
		default:
			printf("pce_vdp: regwrite %d h = 0x%02x.\n", vdp->cur_reg, data);
		}
		break;
	}
}

const u8 pce_cache_palette[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
void pce_cache_tile(pce_vdp* vdp, u32 tile)
{
u8 *cur_vbp= &pce_tilecache[tile << 6];
u8 *tiledata= &vdp->memory[tile << 5];
int i;

	pce_tilecache_dirty[tile] = 0;

	for (i = 8; i; i--)
	{
		blit_4_8(tiledata[17], tiledata[16], tiledata[1], tiledata[0], 8, 0, pce_cache_palette, cur_vbp);
		cur_vbp += 8;
		tiledata += 2;
	}
}

u8 *pce_cached_patterns[129];
u8 *pce_cached_palettes[129];

void pce_vdp_render_background(pce_vdp* vdp, u8 *cur_vbp)
{
u16	cur_scanline	= (vdp->y_scroll) & ((vdp->regs_l[9] & 0x40)? 511: 255);
int	cur_pattern_line= cur_scanline & 7;
u16	cur_pos			= (vdp->regs_l[7] >> 3) + ((vdp->regs_h[7] & 3) << 5);
int	finescroll		= vdp->regs_l[7] & 7;
u16	*linebase;
u32	cur_tile;
u8	*cur_pattern;
u8	*palette;
int	linemask;
int	i;

	if ((vdp->regs_l[9] & 0x30) == 0x00)
	{
		linebase = &((unsigned short *)vdp->memory)[(cur_scanline & 0x3f8) << 2];
		linemask = 0x1f;
	}
	else if ((vdp->regs_l[9] & 0x30) == 0x10)
	{
		linebase = &((unsigned short *)vdp->memory)[(cur_scanline & 0x3f8) << 3];
		linemask = 0x3f;
	}
	else
	{
		linebase = &((unsigned short *)vdp->memory)[(cur_scanline & 0x3f8) << 4];
		linemask = 0x7f;
	}

	for (i = 0; i <= vdp->last_width + 1; i++)
	{
		cur_pos &= linemask;
		cur_tile = ltoh16(linebase[cur_pos++]);
		palette = &vdp->palette_xlat[(cur_tile >> 8) & 0xf0];
		
		if (pce_tilecache_dirty[cur_tile & 0x7ff])
			pce_cache_tile(vdp, cur_tile & 0x7ff);
		
		cur_pattern = pce_tilecache + (cur_pattern_line << 3) + ((cur_tile & 0x7ff) << 6);
		pce_cached_patterns[i] = cur_pattern;
		pce_cached_palettes[i] = palette;
	}

	tiledraw_8(cur_vbp, pce_cached_patterns, pce_cached_palettes, vdp->last_width + 1, finescroll);
}

struct satb_entry {
	u16 ypos;
	u16 xpos;
	u16 pattern;
	u16 flags;
};

#define SF_VFLIP 0x8000
#define SF_VSIZEMASK 0x3000
#define SF_VSIZE1 0x0000
#define SF_VSIZE2 0x1000
#define SF_VSIZE4 0x3000
#define SF_HFLIP 0x0800
#define SF_HSIZE 0x0100
#define SF_SPBG 0x0080
#define SF_PALETTE 0x000f

void pce_vdp_blit_tile_clipped(u8 *vbp, u8 pat0, u8 pat1, u8 pat2, u8 pat3, u8 *palette, int position, int width)
{
	if(position < -8)
		return;
    
	if (position >= width)
		return;

	if (position < 0)
		blit_4_8_czt(pat0, pat1, pat2, pat3, 8 + position, -position, palette, vbp);
	else if ((position + 8) >= width)
		blit_4_8_czt(pat0, pat1, pat2, pat3, width - position, 0, palette, vbp + position);
	else
		blit_4_8_czt(pat0, pat1, pat2, pat3, 8, 0, palette, vbp + position);
}

void pce_vdp_blit_tile_clipped_rev(u8 *vbp, u8 pat0, u8 pat1, u8 pat2, u8 pat3, u8 *palette, int position, int width)
{
	if (position < -8)
		return;
    
	if (position >= width)
		return;

	if (position < 0)
		blit_4_8_czt_rev(pat0, pat1, pat2, pat3, 8 + position, -position, palette, vbp);
	else if ((position + 8) >= width)
		blit_4_8_czt_rev(pat0, pat1, pat2, pat3, width - position, 0, palette, vbp + position);
	else
		blit_4_8_czt_rev(pat0, pat1, pat2, pat3, 8, 0, palette, vbp + position);
}

void pce_vdp_draw_sprite(u8 *cur_vbp, u8 *pattern, u8 *palette, int position, int display_width)
{
	pce_vdp_blit_tile_clipped(cur_vbp, pattern[97], pattern[65], pattern[33], pattern[1], palette, position, display_width);
	pce_vdp_blit_tile_clipped(cur_vbp, pattern[96], pattern[64], pattern[32], pattern[0], palette, position + 8, display_width);
}

void pce_vdp_draw_sprite_rev(u8 *cur_vbp, u8 *pattern, u8 *palette, int position, int display_width)
{
	pce_vdp_blit_tile_clipped_rev(cur_vbp, pattern[97], pattern[65], pattern[33], pattern[1], palette, position + 8, display_width);
	pce_vdp_blit_tile_clipped_rev(cur_vbp, pattern[96], pattern[64], pattern[32], pattern[0], palette, position, display_width);
}

void pce_vdp_render_sprites(pce_vdp* vdp, unsigned char *cur_vbp, int behind)
{
struct satb_entry *satb= (struct satb_entry *)(((unsigned short *)vdp->memory) + (((vdp->regs_h[19] & 0x7f) << 8) + vdp->regs_l[19]));
struct satb_entry cur_sprite;
int cur_pattern_line;
u8 *cur_pattern;
u8 *palette;
int i;
int sprite_vsize;
    
	for(i = 63; i >= 0; i--)
	{
		cur_sprite.ypos = ltoh16(satb[i].ypos);
		cur_sprite.xpos = ltoh16(satb[i].xpos);
		cur_sprite.pattern = ltoh16(satb[i].pattern);
		cur_sprite.flags = ltoh16(satb[i].flags);
	
		/* sprite priority check */
		if ((!(cur_sprite.flags & SF_SPBG)) == (!behind))
			continue;
	
		/* upper bound visibility check */
		if ((cur_sprite.ypos & 0x3ff) > (vdp->cur_scanline + 64))
			continue;

		cur_pattern_line = (vdp->cur_scanline + 64) - (cur_sprite.ypos & 0x3ff);

		/* v-size check */
		switch (cur_sprite.flags & SF_VSIZEMASK)
		{
		case SF_VSIZE1:
			sprite_vsize = 16;
			break;
		case SF_VSIZE2:
			sprite_vsize = 32;
			break;
		case SF_VSIZE4:
			sprite_vsize = 64;
			break;
		default:
			/* Invalid size code */
			printf("pce_vdp: bogus sprite size code.\n");
			continue;
		}

		/* lower bound visibility check */
		if (cur_pattern_line >= sprite_vsize)
			continue;
	
		/* y-flip check */
		if (cur_sprite.flags & SF_VFLIP)
			cur_pattern_line = (sprite_vsize - 1) - cur_pattern_line;
	
		if (cur_sprite.pattern & 0x400) {
			printf("pce_vdp: sprite pattern 0x400 set.\n");
		}
	
		cur_pattern = &vdp->memory[(cur_sprite.pattern & 0x3ff) << 6];
		cur_pattern += (cur_pattern_line & 0x0f) << 1;
		cur_pattern += (cur_pattern_line & 0x30) << 4;

		palette = &vdp->palette_xlat[0x100 + ((cur_sprite.flags & SF_PALETTE) << 4)];
	
		if (!(cur_sprite.flags & SF_HFLIP))
		{
			pce_vdp_draw_sprite(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 32, vdp->display_width);
			
			cur_pattern += 0x80;
			
			if (cur_sprite.flags & SF_HSIZE)
				pce_vdp_draw_sprite(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 16, vdp->display_width);
		}
		else
		{
			if (!(cur_sprite.flags & SF_HSIZE))
				pce_vdp_draw_sprite_rev(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 32, vdp->display_width);
			else
			{
				pce_vdp_draw_sprite_rev(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 16, vdp->display_width);
		
				cur_pattern += 0x80;
	    
				pce_vdp_draw_sprite_rev(cur_vbp, cur_pattern, palette, (cur_sprite.xpos & 0x3ff) - 32, vdp->display_width);
			}
		}
    }
}

void pce_vdp_render_line(pce_vdp* vdp)
{
u8 *cur_vbp= video_GetScanline(vdp->cur_scanline);

	memset(cur_vbp, vdp->palette_xlat[0], vdp->display_width);
	
	if(vdp->regs_l[5] & 0x40)
		pce_vdp_render_sprites(vdp, cur_vbp, 1);
	
	if(vdp->regs_l[5] & 0x80)
		pce_vdp_render_background(vdp, cur_vbp);
	
	if(vdp->regs_l[5] & 0x40)
		pce_vdp_render_sprites(vdp, cur_vbp, 0);
}

int pce_vdp_periodic(pce_vdp* vdp)
{
	if ((vdp->cur_scanline + 0x40) == (vdp->regs_l[6] + (vdp->regs_h[6] << 8)))
	{
		/*printf("pce_vdp: RCR! (%d)\n", vdp->cur_scanline); */
		vdp->status |= 0x04;
    }
	else
		vdp->status &= ~0x04;
    
	if (vdp->cur_scanline < 256)
	{
		pce_vdp_render_line(vdp);
		vdp->y_scroll++;
    }
    
	if (vdp->cur_scanline == 256)
	{
		/*video_events(); */
		video_BuildFrameBuffer();
		vdp->status |= 0x20; /* FIXME: I'm not sure about this. */
    }
    
	if (vdp->cur_scanline == 261)
	{
		vdp->cur_scanline = 0;
		vdp->y_scroll = ((vdp->regs_h[8] << 8) + vdp->regs_l[8]);
		
		if ((vdp->regs_l[11] & 0x7f) != vdp->last_width)
		{
			vdp->last_width = vdp->regs_l[11] & 0x7f;
			vdp->display_width = (vdp->last_width + 1) << 3;
			video_SetSize(vdp->display_width, 256);
		}
    }
	else
		vdp->cur_scanline++;
    
	return ((vdp->regs_l[5] & 0x08) && (vdp->status & 0x20)) || ((vdp->regs_l[5] & 0x04) && (vdp->status & 0x04));

}

pce_vdp* pce_vdp_create(void)
{
//pce_vdp* retval= (pce_vdp*)calloc(1, sizeof(pce_vdp));
pce_vdp* retval= new pce_vdp;

	if (retval){
///		retval->memory = (unsigned char*)calloc(1, PCE_VDP_RAMSIZE);
		retval->memory = new u8[PCE_VDP_RAMSIZE];
		
		if (retval->memory){
			video_SetSize(256, 256);
			video_SetPal(64, pce_palbase_red, pce_palbase_green, pce_palbase_blue);
	
			retval->last_width = 31;
			retval->display_width = 256;
			retval->last_height = 0;
			retval->regs_l[11] = 31;
		}else{
			delete retval;
			retval = NULL;
		}
	}else{
		printf("pce_vdp_create(): out of memory.\n");
    }
	return retval;
}
