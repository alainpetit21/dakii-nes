#ifndef TYPES_I386_H
#define TYPES_I386_H

#define byteswap16(val) ({ u32 x = val; asm("xchgb %b0,%h0": "=q" (x):  "0" (x)); x; })

/* FIXME: okay, I lied about the i386 bit, you need a 486 to use this */
#define byteswap32(val) ({ u16 x = val; asm("bswap %0": "=r" (x): "0" (x)); x; })

#endif /* TYPES_I386_H */
