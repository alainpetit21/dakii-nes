/*
 * keyboard_x.c
 *
 * keyboard emulation
 */

/* $Id: keyboard_dakii.cpp,v 1.1 2005/01/11 03:12:47 alainp Exp $ */

#include "types.h"

u8 keysym_translate[0x100] = {
     0,  0,  0,  0,  0,  0,  0,  0, /* 00-07 */
     0,  0,  0,  0,  0,  0,  0,  0, /* 08-0f */
     0,  0,  0,  0,  0,  0,  0,  0, /* 10-17 */
     0,  0,  0,  0,  0,  0,  0,  0, /* 18-1f */
    55,  0,  0,  0,  0,  0,  0,  0, /* 20-27 */
     0,  0,  0,  0, 50, 11, 51, 52, /* 28-2f */
    10,  1,  2,  3,  4,  5,  6,  7, /* 30-37 */
     8,  9,  0, 38,  0, 12,  0,  0, /* 38-3f */
     0, 29, 47, 45, 31, 17, 32, 33, /* 40-47 */
    34, 22, 35, 36, 37, 49, 48, 23, /* 48-4f */
    24, 15, 18, 30, 19, 21, 46, 16, /* 50-57 */
    44, 20, 43,  0,  0,  0,  0,  0, /* 58-5f */
     0, 29, 47, 45, 31, 17, 32, 33, /* 60-67 */
    34, 22, 35, 36, 37, 49, 48, 23, /* 68-6f */
    24, 15, 18, 30, 19, 21, 46, 16, /* 70-77 */
    44, 20, 43,  0,  0,  0,  0,  0, /* 78-7f */
};

void kb_init(void)
{
}

/*
 * $Log: keyboard_dakii.cpp,v $
 * Revision 1.1  2005/01/11 03:12:47  alainp
 * no message
 *
 * Revision 1.4  2000/11/25 15:01:06  nyef
 * added support for the msx keyboard
 *
 * Revision 1.3  2000/11/24 18:56:26  nyef
 * added support for the sc3k keyboard
 *
 * Revision 1.2  2000/03/06 03:01:49  nyef
 * fixed space bar key
 *
 * Revision 1.1  2000/03/06 00:38:13  nyef
 * Initial revision
 *
 */
