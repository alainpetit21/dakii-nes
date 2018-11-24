#ifndef TYPES_H
#define TYPES_H

#include "libTypes.h"

#ifdef MACH_TYPES
#include MACH_TYPES
#endif

static inline u32 
byteswap32(u32 val)
{
u32 x = val; 

	x = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24); 
	return x;
}

static inline u16 
byteswap16(u16 val)
{
u16 x = val; 

	x = (x << 8) | (x >> 8); 
	return x;
}

#define htol16(x) (x)
#define htol32(x) (x)
#define htom16(x) byteswap16(x)
#define htom32(x) byteswap32(x)

#define ltoh16(x) htol16(x)
#define ltoh32(x) htol32(x)
#define mtoh16(x) htom16(x)
#define mtoh32(x) htom32(x)

#endif /* TYPES_H */
