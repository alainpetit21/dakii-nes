/** M6502: portable 6502 emulator ****************************/
/**                                                         **/
/**                         M6502.h                         **/
/**                                                         **/
/** This file contains declarations relevant to emulation   **/
/** of 6502 CPU.                                            **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996                      **/
/**               Alex Krasivsky  1996                      **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/   
/**     changes to this file.                               **/
/*************************************************************/
#ifndef M6502_H
#define M6502_H

/*
 * this file has been modified for use with the cal.
 */

/* $Id: m6502.h,v 1.1 2005/01/11 03:12:47 alainp Exp $ */

#include "cal.h"
#include "mtypes.h"

                               /* Compilation options:       */
/* #define FAST_RDOP */        /* Separate Op6502()/Rd6502() */
/* #define DEBUG */            /* Compile debugging version  */
/* #define LSB_FIRST */        /* Compile for low-endian CPU */

                               /* Loop6502() returns:        */
#define INT_NONE  0xFFFF       /* No interrupt required      */
#define INT_IRQ	  0x0038       /* Standard IRQ interrupt     */
#define INT_NMI	  0x0066       /* Non-maskable interrupt     */
#define INT_QUIT  0xFFFE       /* Exit the emulation         */

                               /* 6502 status flags:         */
#define	M6502_C_FLAG	  0x01         /* 1: Carry occured           */
#define	M6502_Z_FLAG	  0x02         /* 1: Result is zero          */
#define	M6502_I_FLAG	  0x04         /* 1: Interrupts disabled     */
#define	M6502_D_FLAG	  0x08         /* 1: Decimal mode            */
#define	M6502_B_FLAG	  0x10         /* Break [0 on stk after int] */
#define	M6502_R_FLAG	  0x20         /* Always 1                   */
#define	M6502_V_FLAG	  0x40         /* 1: Overflow occured        */
#define	M6502_N_FLAG	  0x80         /* 1: Result is negative      */

/** Simple Datatypes *****************************************/
/** NOTICE: sizeof(byte)=1 and sizeof(word)=2               **/
/*************************************************************/
/* typedef unsigned char byte; */
/* typedef unsigned short word; */
/* typedef signed char offset; */

/** Structured Datatypes *************************************/
/** NOTICE: #define LSB_FIRST for machines where least      **/
/**         signifcant byte goes first.                     **/
/*************************************************************/
/* typedef union */
/* { */
/*   word W; */
/* #ifdef LSB_FIRST */
/*   struct { byte l,h; } B; */
/* #else */
/*   struct { byte h,l; } B; */
/* #endif */
/* } pair; */

typedef struct M6502
{
  byte A,P,X,Y,S;     /* CPU registers and program counter   */
  pair PC;

  int IPeriod,ICount; /* Set IPeriod to number of CPU cycles */
                      /* between calls to Loop6502()         */
  word IRequest;      /* Set to the INT_IRQ when pending IRQ */
  byte AfterCLI;      /* Private, don't touch                */
  int IBackup;        /* Private, don't touch                */
  cal_cpu* User;       /* Arbitrary user data (ID,RAM*,etc.)  */
  byte TrapBadOps;    /* Set to 1 to warn of illegal opcodes */
  word Trap;          /* Set Trap to address to trace from   */
  byte Trace;         /* Set Trace=1 to start tracing        */
    byte memshift;
    word memmask;
    memread8_t *readtable;
    memwrite8_t *writetable;
    ranged_mmu *mmu;
    unsigned char *cur_ip;
    unsigned short ip_left;
} M6502;

/** Reset6502() **********************************************/
/** This function can be used to reset the registers before **/
/** starting execution with Run6502(). It sets registers to **/
/** their initial values.                                   **/
/*************************************************************/
void Reset6502(register M6502 *R);

/** Int6502() ************************************************/
/** This function will generate interrupt of a given type.  **/
/** INT_NMI will cause a non-maskable interrupt. INT_IRQ    **/
/** will cause a normal interrupt, unless M6502_I_FLAG set in R.  **/
/*************************************************************/
void Int6502(register M6502 *R,register word Type);

/** Run6502() ************************************************/
/** This function will run 6502 code until Loop6502() call  **/
/** returns INT_QUIT. It will return the PC at which        **/
/** emulation stopped, and current register values in R.    **/
/*************************************************************/
word Run6502(register M6502 *R);

/** Debug6502() **********************************************/
/** This function should exist if DEBUG is #defined. When   **/
/** Trace!=0, it is called after each command executed by   **/
/** the CPU, and given the 6502 registers. Emulation exits  **/
/** if Debug6502() returns 0.                               **/
/*************************************************************/
byte Debug6502(register M6502 *R);

/** Loop6502() ***********************************************/
/** 6502 emulation calls this function periodically to      **/
/** check if the system hardware requires any interrupts.   **/
/** This function must return one of following values:      **/
/** INT_NONE, INT_IRQ, INT_NMI, or INT_QUIT to exit the     **/
/** emulation loop.                                         **/
/************************************ TO BE WRITTEN BY USER **/
byte Loop6502(register M6502 *R);

#endif /* M6502_H */

/*
 * $Log: m6502.h,v $
 * Revision 1.1  2005/01/11 03:12:47  alainp
 * no message
 *
 * Revision 1.6  1999/11/21 04:39:22  nyef
 * added preliminary implementation of ranged mmu
 *
 * Revision 1.5  1999/08/07 16:19:25  nyef
 * removed definition of LSB_FIRST
 *
 * Revision 1.4  1999/04/17 20:07:58  nyef
 * fixed for new version of the CAL.
 *
 * Revision 1.3  1999/01/03 02:24:33  nyef
 * modified interrupt handling to not conflict with mz80.
 * moved type definitions out to mtypes.h.
 *
 * Revision 1.2  1998/12/22 04:56:55  nyef
 * changed x_FLAG to M6502_x_FLAG.
 *
 * Revision 1.1  1998/12/16 03:50:26  nyef
 * Initial revision
 *
 */
