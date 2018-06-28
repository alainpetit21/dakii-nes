/*
 * nes.h
 *
 * NES specific header.
 */

/* $Id: nes.h,v 1.1 2005/01/11 03:12:49 alainp Exp $ */

#ifndef NES_H
#define NES_H

#include "tool.h"
#include "types.h"
#include "mappers.h"

/* direct access to the NES PRG ROM bank handling for FDS */
extern u8 *bank_8;     /* Rom Page */
extern u8 *bank_A;     /* Rom Page */
extern u8 *bank_C;     /* Rom Page */
extern u8 *bank_E;     /* Rom Page */

/* PRG memory mapping */
void nesprg_map_4k(int bank, int page);
void nesprg_map_8k(int bank, int page);
void nesprg_map_16k(int bank, int page);
void nesprg_map_32k(int bank, int page);

/* IRQ generation (for mappers and such) */
void nes_external_irq(void);

typedef u8 (*readhook_t)(nes_mapper* mapper, u16 address);
typedef void (*writehook_t)(nes_mapper* mapper, u16 address, u8 data);

extern readhook_t nes_bank4_read_hook;
extern readhook_t nes_bank6_read_hook;
extern writehook_t nes_bank4_write_hook;
extern writehook_t nes_bank6_write_hook;

#endif /* NES_H */
