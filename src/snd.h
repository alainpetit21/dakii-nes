/*
 * snd.h
 *
 * os-dependant sound routines
 */

#ifndef SND_H
#define SND_H

#include "types.h"

void 	snd_Init(void);
int		snd_Open(int samples_per_sync, int sample_rate);
void 	snd_Close(void);
void 	snd_Output4Waves(int samples, u8 *wave1, u8 *wave2, u8 *wave3, u8 *wave4);
void	snd_UserFetchSound(u32);

#define NB_SAMPLE	2048

#endif /* SND_H */
