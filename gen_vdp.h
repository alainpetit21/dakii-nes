/*
 * gen_vdp.h
 *
 * genesis custom vdp emulation.
 */

#ifndef GEN_VDP_H
#define GEN_VDP_H

/* $Id: gen_vdp.h,v 1.1 2005/01/11 03:12:47 alainp Exp $ */

typedef struct taggen_vdp {
    unsigned short flags;
    unsigned short readahead;
    unsigned short addrsave;
    unsigned short status;
    unsigned char *memory;
    unsigned char regs[24];
    unsigned char pal[128];
    unsigned char palette_xlat[64];
    unsigned long address;
    unsigned short cur_scanline;
    unsigned char linecounter;
}gen_vdp;

unsigned short gen_vdp_readport0(gen_vdp* vdp);
unsigned short gen_vdp_readport1(gen_vdp* vdp);
void gen_vdp_writeport0(gen_vdp* vdp, unsigned short data);
void gen_vdp_writeport1(gen_vdp* vdp, unsigned short data);
int gen_vdp_periodic(gen_vdp* vdp);

gen_vdp* gen_vdp_create(void);

#endif /* GEN_VDP_H */

/*
 * $Log: gen_vdp.h,v $
 * Revision 1.1  2005/01/11 03:12:47  alainp
 * no message
 *
 * Revision 1.3  1999/12/04 16:13:03  nyef
 * moved the vdp data structure out to gen_vdp.c
 * published the prototypes for the functions that were behind procpointers
 *
 * Revision 1.2  1999/09/08 01:04:02  nyef
 * changed status register to be 16 bit
 *
 * Revision 1.1  1999/04/17 22:24:50  nyef
 * Initial revision
 *
 */
