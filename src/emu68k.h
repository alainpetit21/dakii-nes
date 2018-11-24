/*
 * emu68k.h
 *
 * emulation of the 68000
 */

/* $Id: emu68k.h,v 1.1 2005/01/11 03:12:46 alainp Exp $ */

#ifndef EMU68K_H
#define EMU68K_H

#include "types.h"

struct emu68k_context {
    u32 regs_a[8];
    u32 regs_d[8];
    u32 other_sp;
    u32 pc;
    u16 flags;
    u8 flag_n;
    u8 flag_c;
    u8 flag_x;
    u32 flag_z;
    u32 flag_v;
    int cycles_left;
    memread8_t *read8table;
    memwrite8_t *write8table;
    memread16_t *read16table;
    memwrite16_t *write16table;
    int memshift;
    u32 memmask;
    cal_cpu* cpu;
};

void emu68k_step(struct emu68k_context *context);
void emu68k_run(struct emu68k_context *context);
void emu68k_reset(struct emu68k_context *context);

u16 emu68k_get_flags(struct emu68k_context *context);
void emu68k_set_flags(struct emu68k_context *context, u16 flags);

#endif /* EMU68K_H */
