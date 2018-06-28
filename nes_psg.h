/*
 * nes_psg.h
 *
 * NES sound emulation
 */

#ifndef NES_PSG_H
#define NES_PSG_H

#include "types.h"

typedef void (*psg_writefunc)(u8 value);

void nes_psg_init(void);
void nes_psg_done(void);
void nes_psg_frame(void);
void nes_psg_write_control(u8 value);

extern psg_writefunc sound_regs[16];


#endif /* NES_PSG_H */
