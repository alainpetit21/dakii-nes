#ifndef PCE_VDP_H
#define PCE_VDP_H

#include "types.h"

typedef struct tagpce_vdp{
    u8 flags;
    u8 status;
    u8 cur_reg;
    u8 *memory;
    u16 mem_read_addr;
    u16 mem_write_addr;
    u8 regs_l[32];
    u8 regs_h[32];
    u8 last_width;
    u16 last_height;
    u16 palette_addr;
    u8 palette_l[512];
    u8 palette_h[512];
    u8 palette_xlat[512];
    u16 cur_scanline;
    u16 y_scroll;
    int display_width;
}pce_vdp;

unsigned char pce_vce_read(pce_vdp* vdp, unsigned short addr);
void pce_vce_write(pce_vdp* vdp, unsigned short addr, unsigned char data);
unsigned char pce_vdp_read(pce_vdp* vdp, unsigned short addr);
void pce_vdp_write(pce_vdp* vdp, unsigned short addr, unsigned char data);
int pce_vdp_periodic(pce_vdp* vdp);

pce_vdp* pce_vdp_create(void);

#endif /* PCE_VDP_H */
