/*
 * cd_unix.c
 *
 * UNIX(tm) CDROM interface
 */

/* $Id: cd_win32.cpp,v 1.1 2005/01/11 03:12:45 alainp Exp $ */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "types.h"
#include "cd.h"

int cd_drive;

void cd_set_sector_address(u32 sector)
{
}

void cd_read_next_sector(u8 *buf)
{
}

void cd_play_track(u8 track)
{
}

void cd_play_lba(u32 from, u32 to, int repeat)
{
}

void cd_get_tocheader(u8 *start_track, u8 *stop_track)
{
}

void cd_get_tocentry(u8 track, u32 *lba, u8 *flags)
{
}

void cd_init(void)
{
}
