#ifndef MAPPERS_H
#define MAPPERS_H

#include "types.h"
#include "nesStruct.h"

/* mapper interface definition */

typedef void (* mapwrite_t)(nes_mapper* mapper, u16 address, u8 value);
typedef void (* maphsync_t)(nes_mapper* mapper, int display_active);

nes_mapper* create_mapper(nes_ppu* ppu, nes_rom* romfile);
int mapper_supported(int mapper);

#endif /* MAPPERS_H */
