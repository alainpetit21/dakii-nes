/*
 * sms9918.h
 *
 * sms custom vdp emulation.
 */

#ifndef SMS9918_H
#define SMS9918_H

/* $Id: sms9918.h,v 1.1 2005/01/11 03:12:50 alainp Exp $ */

#include "types.h"

typedef struct tagsms9918_namecache {
    u8 *front_tile;
    u8 *back_tile;
    u8 *palette;
    int is_vflip;
} sms9918_namecache;


/* vdp data structure */

typedef struct tagsms9918{
	u8 flags;
	u8 readahead;
	u8 addrsave;
	u8 status;
	u8 *memory;
	u8 regs[16];
	u8 pal[64];
	u8 palette_xlat[32];
	u16 address;
	u16 cur_scanline;
	u8 linecounter;

	int coarsescroll;
	int finescroll;
	sms9918_namecache *nametable;
}sms9918;

u8 sms9918_readport0(sms9918* vdp);
u8 sms9918_readport1(sms9918* vdp);
u8 sms9918_readscanline(sms9918* vdp);

void sms9918_writeport0(sms9918* vdp, u8 data);
void sms9918_writeport1(sms9918* vdp, u8 data);

int sms9918_periodic(sms9918* vdp);

sms9918* sms9918_create(int is_gamegear);

#endif /* SMS9918_H */
