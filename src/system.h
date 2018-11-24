/*
 * system.h
 *
 * definitions for system emulation control
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "tool.h"

#define ST_NONE			0
#define ST_NES			1
#define ST_PCENGINE		2 /* also the CD versions */
#define ST_GENESIS		3
#define ST_MASTERSYS	4
#define ST_GAMEGEAR		5
#define ST_GAMEBOY		6
#define ST_SNES			7
#define ST_LYNX			8
#define ST_JAGUAR		9
#define ST_SEGACD		10
#define ST_APPLE2		11
#define ST_COLECO		12
#define ST_MSX			13
#define ST_SG1000		14
#define ST_SC3000		15
#define ST_MACINTOSH	16
#define ST_PLAYSTATION	17
#define ST_N64			18
#define ST_SATURN		19
#define ST_END			20

void nes_run(rom_file* romfile);
void pce_run(rom_file* romfile);
void sms_run(rom_file* romfile);
void gg_run(rom_file* romfile);
void genesis_run(rom_file* romfile);
void cv_run(rom_file* romfile);
void sg1k_run(rom_file* romfile);
void sc3k_run(rom_file* romfile);
void msx_run(rom_file* romfile);

#endif /* __SYSTEM_H__ */
