/*
 * mtypes.h
 *
 * contains type definitions common to m6502 and mz80.
 */

#ifndef MTYPES_H
#define MTYPES_H

/* $Id: mtypes.h,v 1.1 2005/01/11 03:12:48 alainp Exp $ */

typedef unsigned char byte;
typedef unsigned short word;
typedef signed char offset;

typedef union
{
  word W;
  struct { byte l,h; } B;
} pair;

#endif /* MTYPES_H */

/*
 * $Log: mtypes.h,v $
 * Revision 1.1  2005/01/11 03:12:48  alainp
 * no message
 *
 * Revision 1.2  1999/08/07 16:19:37  nyef
 * removed definition of LSB_FIRST
 *
 * Revision 1.1  1999/01/03 02:20:58  nyef
 * Initial revision
 *
 */
