/*
 * blit_c.c
 *
 * C version of blitters.S
 */

/* $Id: blit_c.cpp,v 1.1 2005/01/11 03:12:44 alainp Exp $ */

#include "blitters.h"

/*
 * Basic merges
 */

#define MERGE(reg1, reg2, shift, mask) \
  temp = reg2;    \
  temp >>= shift; \
  temp ^= reg1;   \
  temp &= mask;   \
  reg1 ^= temp;   \
  temp <<= shift; \
  reg2 ^= temp

#define MERGE4(reg1, reg2) MERGE(reg1, reg2, 4, 0x0f)
#define MERGE2(reg1, reg2) MERGE(reg1, reg2, 2, 0x33)
#define MERGE1(reg1, reg2) MERGE(reg1, reg2, 1, 0x55)


/*
 * Special merges
 */

#define SPECIAL_MERGE(reg1, reg2, shift, mask) \
  reg1 = reg2;  \
  reg1 &= mask; \
  reg2 >>= shift

#define MERGE_2_8(reg1, reg2) SPECIAL_MERGE(reg1, reg2, 2, 0x03)
#define MERGE_4_8(reg1, reg2) SPECIAL_MERGE(reg1, reg2, 4, 0x0f)


/*
 * common code fragments
 */

#define ARGS_2 unsigned char pattern0, unsigned char pattern1, \
	       unsigned char count, unsigned char skip,        \
	       const unsigned char *colors, unsigned char *vbp

#define ARGS_4 unsigned char pattern0, unsigned char pattern1, \
	       unsigned char pattern2, unsigned char pattern3, \
	       unsigned char count, unsigned char skip,        \
	       const unsigned char *colors, unsigned char *vbp

#define VARIABLES_2    unsigned char pattern2, pattern3, pattern4, \
                                     pattern5, pattern6, pattern7, temp

#define VARIABLES_4    unsigned char pattern4, pattern5, \
                                     pattern6, pattern7, temp

#define SKIP_2         pattern0 <<= skip; pattern1 <<= skip
#define SKIP_4 SKIP_2; pattern2 <<= skip; pattern3 <<= skip

#define SKIP_2_REV             pattern0 >>= skip; pattern1 >>= skip
#define SKIP_4_REV SKIP_2_REV; pattern2 >>= skip; pattern3 >>= skip

#define COMBINE_2		   \
    MERGE1(pattern0, pattern1);    \
    MERGE_2_8(pattern6, pattern0); \
    MERGE_2_8(pattern7, pattern1); \
    MERGE_2_8(pattern4, pattern0); \
    MERGE_2_8(pattern5, pattern1); \
    MERGE_2_8(pattern2, pattern0); \
    MERGE_2_8(pattern3, pattern1)

#define COMBINE_4		   \
    MERGE1(pattern0, pattern1);    \
    MERGE1(pattern2, pattern3);    \
    MERGE2(pattern0, pattern2);    \
    MERGE2(pattern1, pattern3);    \
    MERGE_4_8(pattern4, pattern0); \
    MERGE_4_8(pattern5, pattern1); \
    MERGE_4_8(pattern6, pattern2); \
    MERGE_4_8(pattern7, pattern3)

/*
 * NOTE: These next might need the "do { ... } while (0)" trick when not
 * using gcc to compile.
 */
#define OUTPUT	                         \
    switch (count) {                     \
      case 8: vbp[7] = colors[pattern7]; \
      case 7: vbp[6] = colors[pattern6]; \
      case 6: vbp[5] = colors[pattern5]; \
      case 5: vbp[4] = colors[pattern4]; \
      case 4: vbp[3] = colors[pattern3]; \
      case 3: vbp[2] = colors[pattern2]; \
      case 2: vbp[1] = colors[pattern1]; \
      case 1: vbp[0] = colors[pattern0]; \
      default: break;                    \
    }

#define OUTPUT_REV			 \
    switch (count) {			 \
      case 8: vbp[7] = colors[pattern0]; \
      case 7: vbp[6] = colors[pattern1]; \
      case 6: vbp[5] = colors[pattern2]; \
      case 5: vbp[4] = colors[pattern3]; \
      case 4: vbp[3] = colors[pattern4]; \
      case 3: vbp[2] = colors[pattern5]; \
      case 2: vbp[1] = colors[pattern6]; \
      case 1: vbp[0] = colors[pattern7]; \
      default: break;                    \
    }

#define OUTPUT_CZT				       \
    switch (count) {			 	       \
      case 8: if (pattern7) vbp[7] = colors[pattern7]; \
      case 7: if (pattern6) vbp[6] = colors[pattern6]; \
      case 6: if (pattern5) vbp[5] = colors[pattern5]; \
      case 5: if (pattern4) vbp[4] = colors[pattern4]; \
      case 4: if (pattern3) vbp[3] = colors[pattern3]; \
      case 3: if (pattern2) vbp[2] = colors[pattern2]; \
      case 2: if (pattern1) vbp[1] = colors[pattern1]; \
      case 1: if (pattern0) vbp[0] = colors[pattern0]; \
      default: break; 			               \
    }

#define OUTPUT_CZT_REV				       \
    switch (count) {			 	       \
      case 8: if (pattern0) vbp[7] = colors[pattern0]; \
      case 7: if (pattern1) vbp[6] = colors[pattern1]; \
      case 6: if (pattern2) vbp[5] = colors[pattern2]; \
      case 5: if (pattern3) vbp[4] = colors[pattern3]; \
      case 4: if (pattern4) vbp[3] = colors[pattern4]; \
      case 3: if (pattern5) vbp[2] = colors[pattern5]; \
      case 2: if (pattern6) vbp[1] = colors[pattern6]; \
      case 1: if (pattern7) vbp[0] = colors[pattern7]; \
      default: break; 			               \
    }


/*
 * The blitter functions themselves.
 */

#if 0 /* not actually used anywhere */
/*
 * NOTE: the 2-plane blitters are CZT without being declared so in their
 * names, which might want to be changed at some point (they were originally
 * used for NES rendering, which required CZT, and was before support for the
 * SMS and PCE was added, which required both CZT and non-CZT 4-plane blits).
 */
void blit_2_8(ARGS_2)
{ VARIABLES_2; SKIP_2;     COMBINE_2; OUTPUT_CZT; }

void blit_2_8_rev(ARGS_2)
{ VARIABLES_2; SKIP_2_REV; COMBINE_2; OUTPUT_CZT_REV; }
#endif

void blit_4_8(ARGS_4)
{ VARIABLES_4; SKIP_4;     COMBINE_4; OUTPUT; }

void blit_4_8_rev(ARGS_4)
{ VARIABLES_4; SKIP_4_REV; COMBINE_4; OUTPUT_REV; }

void blit_4_8_czt(ARGS_4)
{ VARIABLES_4; SKIP_4;     COMBINE_4; OUTPUT_CZT; }

void blit_4_8_czt_rev(ARGS_4)
{ VARIABLES_4; SKIP_4_REV; COMBINE_4; OUTPUT_CZT_REV; }

/*
 * $Log: blit_c.cpp,v $
 * Revision 1.1  2005/01/11 03:12:44  alainp
 * no message
 *
 * Revision 1.2  2001/03/16 21:54:48  nyef
 * completely rebuilt based on code from AmiDog and some preprocessor abuse
 *
 * Revision 1.1  1999/08/15 02:17:19  nyef
 * Initial revision
 *
 */
