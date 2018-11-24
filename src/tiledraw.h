/*
 * tiledraw.h
 *
 * drawing routines for tilecached VDPs
 */

#ifndef TILEDRAW_H
#define TILEDRAW_H

void tiledraw_8(u8 *cur_vbp, u8 **tiles, u8 **palettes, int num_tiles, int finescroll);

#endif /* TILEDRAW_H */
