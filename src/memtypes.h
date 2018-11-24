/*
 * memtypes.h
 *
 * CPU memory access definitions (function pointer and ranged memory types)
 */

/* $Id: memtypes.h,v 1.1 2005/01/11 03:12:47 alainp Exp $ */

#ifndef MEMTYPES_H
#define MEMTYPES_H

typedef unsigned char (*memread8_t)(void*, unsigned long);
typedef void (*memwrite8_t)(void*, unsigned long, unsigned char);
typedef unsigned short (*memread16_t)(void*, unsigned long);
typedef void (*memwrite16_t)(void*, unsigned long, unsigned short);
typedef unsigned long (*memread32_t)(void*, unsigned long);
typedef void (*memwrite32_t)(void*, unsigned long, unsigned long);

typedef struct {
    unsigned long start;
    unsigned long end;
    unsigned char *data;
    unsigned short flags;
    unsigned short tag;
} ranged_mmu;

#endif

/*
 * $Log: memtypes.h,v $
 * Revision 1.1  2005/01/11 03:12:47  alainp
 * no message
 *
 * Revision 1.1  2001/03/16 18:18:29  nyef
 * Initial revision
 *
 */
