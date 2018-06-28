/*
 * cd.h
 *
 * CDROM interface
 */

/* $Id: cd.h,v 1.1 2005/01/11 03:12:45 alainp Exp $ */

#ifndef CD_H
#define CD_H

void cd_set_sector_address(u32 sector);
void cd_read_next_sector(u8 *buf);
void cd_get_tocheader(u8 *start_track, u8 *stop_track);
void cd_get_tocentry(u8 track, u32 *lba, u8 *flags);
void cd_play_lba(u32 from, u32 to, int repeat);
void cd_play_track(u8 track);
void cd_init(void);

#endif /* CD_H */

