/*
 * cpudefs.h
 *
 * CPU inclusion control.
 */

/* $Id: cpudefs.h,v 1.1 2005/01/11 03:12:46 alainp Exp $ */

#ifndef __CPUDEFS_H__
#define __CPUDEFS_H__

/*
 * choose which CPU cores to use.
 * this should probably be shifted out to a separate config program.
 */

#define CPU_MARATZ80
#define CPU_JUNK68K
/* #define CPU_TORR68K */
#define CPU_CPU6280
#define CPU_CPU6502

#endif /* __CPUDEFS_H__ */

/*
 * $Log: cpudefs.h,v $
 * Revision 1.1  2005/01/11 03:12:46  alainp
 * no message
 *
 * Revision 1.12  2000/11/29 05:43:29  nyef
 * disabled marat6502 support
 * removed all definitions related to marat 6502 and 6280 cores
 *
 * Revision 1.11  2000/05/01 00:36:26  nyef
 * added (enabled) cpu6502 support
 *
 * Revision 1.10  2000/01/24 05:04:22  nyef
 * disabled marat6280 core
 * enabled cpu6280 core
 *
 * Revision 1.9  2000/01/17 01:01:58  nyef
 * added (disabled) cpu6280 support
 *
 * Revision 1.8  1999/12/04 04:05:29  nyef
 * removed all definitions related to ASM CPU cores
 *
 * Revision 1.7  1999/11/24 03:00:47  nyef
 * disabled emu6280 support
 *
 * Revision 1.6  1999/08/15 02:28:01  nyef
 * added C_ONLY support
 * added (disabled) TORR68K support
 *
 * Revision 1.5  1999/08/02 00:05:25  nyef
 * added junk68k and marat6280 support
 *
 * Revision 1.4  1999/01/03 00:19:32  nyef
 * added maratz80 cupport.
 *
 * Revision 1.3  1999/01/01 07:00:25  nyef
 * added emuz80 support
 *
 * Revision 1.2  1998/12/12 20:35:41  nyef
 * enabled marat6502 support
 *
 * Revision 1.1  1998/11/29 18:29:03  nyef
 * Initial revision
 *
 */
