#include "lib.h"
#include "libMngSound.h"

#include "types.h"

int g_posToWrite;
int g_posToRead;
void *g_soundRendered;

#define NB_SAMPLE	2048

u16 g_data[NB_SAMPLE*8];

void
snd_UserFetchSound(u32 p_nbSample)
{
CMngSound	*mngSound= GetMngSound();
u16			*pData;
u32			sizeData;

	if(g_posToRead+NB_SAMPLE > g_posToWrite)
		return;

	mngSound->LockSound(g_soundRendered, &pData, &sizeData);

	memcpy(&pData[g_posToRead % (NB_SAMPLE*8)], &g_data[g_posToRead % (NB_SAMPLE*8)], p_nbSample<<1);

	mngSound->UnlockSound(g_soundRendered, pData, sizeData);

	g_posToRead+= NB_SAMPLE;

	if((g_posToRead > (1<<20)) && (g_posToWrite > (1<<20)))
	{
		g_posToRead%= (NB_SAMPLE*8);
		g_posToWrite%= (NB_SAMPLE*8);
	}
}

void snd_Output4Waves(int p_nbSample, u8 *wave1, u8 *wave2, u8 *wave3, u8 *wave4)
{
int i= 0;

	for(i= 0; i < p_nbSample; ++i)
		g_data[g_posToWrite++ % (NB_SAMPLE*8)]= ((u32)wave1[i] + (u32)wave2[i] + (u32)wave3[i] + (u32)wave4[i]) << 5;

}

void snd_Init(void)
{
CMngSound	*mngSound= GetMngSound();

	g_posToWrite	= 0;
	g_soundRendered	= mngSound->NewSound((NB_SAMPLE*8)<<1);

	mngSound->PlaySound(g_soundRendered, true);
}

int snd_Open(int samples_per_sync, int sample_rate)
{
    return 0;
}

void snd_Close(void)
{
}
