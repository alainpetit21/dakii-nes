
#ifdef _LINUX
#include <unistd.h>
#endif

#include "lib.h"
#include "ui.h"
#include "sms_psg.h"
#include "snd.h"

typedef struct tagT_Sms_psg
{
	int	lastchan; /* for frequency writes */
	u8 	vols[4];
	u8 	freqls[4];
	u8 	freqhs[4];
	u32	index[4];
	u32	skip[4];
	u32	shiftreg;
} T_Sms_psg;


T_Sms_psg	sms_psg;
u8 			wave_buffers[4][NB_SAMPLE]; 
u32 		sms_psg_magic;
u32 		sms_psg_samples_per_sync;
u32 		sms_psg_sample_rate;

void sms_psg_calc_freqs(int channel)
{
u32 freq;
    
	if (channel != 3)
		freq = (sms_psg.freqhs[channel] << 4) + sms_psg.freqls[channel];
	else
	{
		if ((sms_psg.freqls[3] & 3) != 3)
			freq = (0x10 << (sms_psg.freqls[3] & 3));
		else
			freq = ((sms_psg.freqhs[2] << 4) + sms_psg.freqls[2]);
		
		sms_psg.shiftreg = 0x1f;
	}
	
	if (freq)
		sms_psg.skip[channel] = sms_psg_magic / freq;
	else
		sms_psg.skip[channel] = 0;
	
	if ((channel == 2) && ((sms_psg.freqls[3] & 3) == 3))
	{
		freq = ((sms_psg.freqhs[2] << 4) + sms_psg.freqls[2]);
		
		if (freq)
			sms_psg.skip[3] = sms_psg_magic / freq;
		else
			sms_psg.skip[3] = 0;
	}
}

void sms_psg_write(u8 data)
{
	if (data & 0x80)
	{
		if(data & 0x10)/* volume */
			sms_psg.vols[(data >> 5) & 3] = (15 - (data & 15)) * 0x11;
		else
		{	/* freq low */
			sms_psg.lastchan = (data >> 5) & 3;
			sms_psg.freqls[sms_psg.lastchan] = data & 15;
			sms_psg_calc_freqs(sms_psg.lastchan);
		}
	}
	else
	{	/* freq high */
		sms_psg.freqhs[sms_psg.lastchan] = data & 0x3f;
		sms_psg_calc_freqs(sms_psg.lastchan);
	}
}

void sms_psg_pulse(int channel)
{
u32 i;
	
	for(i= 0; i < sms_psg_samples_per_sync; i++)
	{
		sms_psg.index[channel] += sms_psg.skip[channel];
		sms_psg.index[channel] &= 0x1fffffff;
		wave_buffers[channel][i] = (sms_psg.index[channel] & 0x10000000)? sms_psg.vols[channel]: 0;
	}
}

void sms_psg_wave_4(void)
{
}

void sms_psg_vsync(void)
{
	sms_psg_pulse(0);
	sms_psg_pulse(1);
	sms_psg_pulse(2);
	sms_psg_wave_4();
	
	snd_Output4Waves(sms_psg_samples_per_sync, wave_buffers[0], wave_buffers[1], wave_buffers[2], wave_buffers[3]);
}

void sms_psg_init(void)
{
	snd_Init();
	
	sms_psg_magic			= 0x512b3400;
	sms_psg_samples_per_sync= NB_SAMPLE;
	sms_psg_sample_rate		= 44100;
	
	snd_Open(sms_psg_samples_per_sync, sms_psg_sample_rate);
}

void sms_psg_done(void)
{
    snd_Close();
}
