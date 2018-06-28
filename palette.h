/*
 * palette.h
 *
 * generic palette system
 */

/* $Id: palette.h,v 1.1 2005/01/11 03:12:49 alainp Exp $ */

#ifndef PALETTE_H
#define PALETTE_H

typedef struct tagpalette {
    void (*set)(struct tagpalette *ceci, int index, int value);
    void *translate;
    void *base_palette;
    void *palettes[1];
} palette;

palette* new_palette_8(int num_palettes, int palette_size);
palette* new_palette_16(int num_palettes, int palette_size);
palette* new_palette_32(int num_palettes, int palette_size);

#endif /* PALETTE_H */

/*
 * $Log: palette.h,v $
 * Revision 1.1  2005/01/11 03:12:49  alainp
 * no message
 *
 * Revision 1.1  2000/08/04 23:34:46  nyef
 * Initial revision
 *
 */
