/*
 * nes_psg.c
 *
 * NES sound emulation system
 */

#include "nes_psg.h"
#include "snd.h"
#include "types.h"

/*
 * volume envelopes
 */
typedef struct tagT_Envelope
{
	u8 decay_counter;
	u8 rate_counter;
}T_Envelope;

/*
 * sound register data
 */
struct channel {u8 a; u8 b; u8 c; u8 d;};
struct channel nes_psg_c1;
struct channel nes_psg_c2;
struct channel nes_psg_c3;
struct channel nes_psg_c4;

u8	nes_psg_control;

/*
 * Sync occurs every 7467 CPU cycles.
 * Therefore, at the rate calculated
 * for 44.1kHz (our highest rate),
 * there are 182 samples per sync.
 */
static u8 wave_buffers[4][NB_SAMPLE];

u32 nes_psg_pulse_magic;
u32 nes_psg_triangle_magic;
u32 nes_psg_noise_magic;
u32 nes_psg_samples_per_sync;
u32 nes_psg_sample_rate;

u8 pulse_25[0x20]=
{
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

u8 pulse_50[0x20] =
{
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

u8 pulse_75[0x20] = 
{
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

u8 pulse_87[0x20] = 
{
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x00, 0x00, 0x00, 0x00,
};

u8 triangle_50[0x20] = 
{
	0x00, 0x11, 0x22, 0x33,
	0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xaa, 0xbb,
	0xcc, 0xdd, 0xee, 0xff,
	0xff, 0xee, 0xdd, 0xcc,
	0xbb, 0xaa, 0x99, 0x88,
	0x77, 0x66, 0x55, 0x44,
	0x33, 0x22, 0x11, 0x00,
};

u8 *pulse_waves[4] = 
{
	pulse_50, pulse_50, pulse_50, pulse_50,
};

u8 nes_psg_atl[0x20] = 
{
	5, 127, 10, 1, 20,  2, 40,  3, 80,  4, 30,  5, 7,  6, 13,  7,
	6,   8, 12, 9, 24, 10, 48, 11, 96, 12, 36, 13, 8, 14, 16, 15,
};

static u32 			wave_1_index;
static int 			wave_1_sweep_clock;
static u8 			wave_1_length_counter;
static T_Envelope	wave_1_envelope;

static u32 			wave_2_index;
static int 			wave_2_sweep_clock;
static u8 			wave_2_length_counter;
static T_Envelope 	wave_2_envelope;

static u32 	wave_3_index;
static u8 	wave_3_length_counter;

int length_counter_clock;

/*
 * a psg write function is of the form c#$ where # is the channel number and
 *   $ is the channel register starting from a
 */
#define PSG_WRITEFUNC(channel, reg) 		\
void nes_psg_write_##channel##reg(u8 value)	\
{ 											\
    nes_psg_##channel.reg = value; 			\
}

void nes_psg_write_c1d(u8);
void nes_psg_write_c2d(u8);
void nes_psg_write_c3d(u8);

PSG_WRITEFUNC(c1, a); PSG_WRITEFUNC(c1, b); PSG_WRITEFUNC(c1, c);
PSG_WRITEFUNC(c2, a); PSG_WRITEFUNC(c2, b); PSG_WRITEFUNC(c2, c);
PSG_WRITEFUNC(c3, a); PSG_WRITEFUNC(c3, b); PSG_WRITEFUNC(c3, c);
PSG_WRITEFUNC(c4, a); PSG_WRITEFUNC(c4, b); PSG_WRITEFUNC(c4, c);
PSG_WRITEFUNC(c4, d);

void 
nes_psg_write_c1d(u8 value)
{
	nes_psg_c1.d = value;
	wave_1_envelope.decay_counter = 0x0f;
	wave_1_length_counter = nes_psg_atl[value >> 3];
}

void 
nes_psg_write_c2d(u8 value)
{
	nes_psg_c2.d = value;
	wave_2_envelope.decay_counter = 0x0f;
	wave_2_length_counter = nes_psg_atl[value >> 3];
}

void 
nes_psg_write_c3d(u8 value)
{
	nes_psg_c3.d = value;
	wave_3_length_counter = nes_psg_atl[value >> 3];
}

psg_writefunc sound_regs[16]=
{
	nes_psg_write_c1a, nes_psg_write_c1b, nes_psg_write_c1c, nes_psg_write_c1d,
	nes_psg_write_c2a, nes_psg_write_c2b, nes_psg_write_c2c, nes_psg_write_c2d,
	nes_psg_write_c3a, nes_psg_write_c3b, nes_psg_write_c3c, nes_psg_write_c3d,
	nes_psg_write_c4a, nes_psg_write_c4b, nes_psg_write_c4c, nes_psg_write_c4d,
};

void 
nes_psg_write_control(u8 value)
{
	nes_psg_control= value;
}

u8 
envelope_get_volume(struct channel *channel, T_Envelope *envelope)
{
	if(channel->a & 0x10)
		return channel->a & 0x0f;
	else
		return envelope->decay_counter;

}

void envelope_run_counter(struct channel *channel, T_Envelope *envelope)
{
	if(envelope->rate_counter--) 
		return;
	
	envelope->rate_counter = channel->a & 0x0f;
	
	if (envelope->decay_counter--) 
		return;
	
	envelope->decay_counter = (channel->a & 0x20)? 0x0f: 0;
}

/*
 * frequency sweeps
 */
static int 
sweep_unit_active(struct channel *channel)
{
	return channel->b & 0x80;
}

static int 
sweep_unit_shift_count(struct channel *channel)
{
	return channel->b & 7;
}

static int 
sweep_unit_should_sweep_down(struct channel *channel)
{
	return channel->b & 0x08;
}

static void 
sweep_unit_set_frequency(struct channel *channel, u16 freq)
{
	channel->c = freq;
	channel->d &= ~7;
	channel->d |= (freq >> 8) & 7;
}

static void 
sweep_unit_do_sweep(struct channel *channel, u16 freq)
{
	if (sweep_unit_should_sweep_down(channel))
	{
		freq -= freq >> sweep_unit_shift_count(channel);
		if (channel == &nes_psg_c1)
			freq--;
	}
	else
		freq += freq >> sweep_unit_shift_count(channel);

	sweep_unit_set_frequency(channel, freq);
	
	/* FIXME: sweep unit upper bound shutdown? */
}

void 
run_sweep_unit(struct channel *channel, int *sweep_clock, u16 freq)
{
	if (!sweep_unit_active(channel)) 
		return;
	
	if ((*sweep_clock)--) 
		return;
	
	*sweep_clock = (channel->b >> 3) & 0x0e;
	
	if (!sweep_unit_shift_count(channel)) 
		return;
	
	sweep_unit_do_sweep(channel, freq);
}

void nes_psg_wave_1(void)
{
u8	volume	= envelope_get_volume(&nes_psg_c1, &wave_1_envelope);
u16	freq	= ((nes_psg_c1.d & 0x07) << 8) + nes_psg_c1.c;
u8	*sample;
u32	step;
u32	i;

	wave_1_index= 0;
	if (freq >= 8)
		step= nes_psg_pulse_magic / freq;
	else
		step= 0;

	if(!wave_1_length_counter)
		step = 0;

	if(!(nes_psg_control & 0x01))
		step = 0;
 
	sample= pulse_waves[(nes_psg_c1.a >> 6)];
	
	for(i = 0; i < nes_psg_samples_per_sync; i++)
	{
		wave_1_index += step;
		wave_1_index &= 0x1fffffff;
		wave_buffers[0][i] = sample[wave_1_index >> 24] * volume;
	}
	
	envelope_run_counter(&nes_psg_c1, &wave_1_envelope);	
	run_sweep_unit(&nes_psg_c1, &wave_1_sweep_clock, freq);
}

void nes_psg_wave_2(void)
{
u8	volume	= envelope_get_volume(&nes_psg_c2, &wave_2_envelope);
u16	freq	= ((nes_psg_c2.d & 0x07) << 8) + nes_psg_c2.c;
u8	*sample;
u32	step;
u32	i;

	if (freq >= 8)
		step= nes_psg_pulse_magic / freq;
	else
		step= 0;

	if(!wave_2_length_counter)
		step = 0;

	if(!(nes_psg_control & 0x02))
		step = 0;
 
	sample= pulse_waves[(nes_psg_c2.a >> 6)];
	
	for(i = 0; i < nes_psg_samples_per_sync; i++)
	{
		wave_2_index += step;
		wave_2_index &= 0x1fffffff;
		wave_buffers[1][i] = sample[wave_2_index >> 24] * volume;
	}
	
	envelope_run_counter(&nes_psg_c2, &wave_2_envelope);	
	run_sweep_unit(&nes_psg_c2, &wave_2_sweep_clock, freq);
 }

void nes_psg_wave_3(void)
{
u16 freq= ((nes_psg_c3.d & 0x07) << 8) + nes_psg_c3.c;
u32 step;
u32 i;

	/* FIXME: Add linear counter */
    
	if(freq)
		step= nes_psg_triangle_magic / freq;
	else
		step= 0;
	
	if(!wave_3_length_counter)
		step = 0;
	
	if(!(nes_psg_control & 0x04))
		step = 0;

	for (i = 0; i < nes_psg_samples_per_sync; i++)
	{
		wave_3_index += step;
		wave_3_index &= 0x1fffffff;
		wave_buffers[2][i] = triangle_50[wave_3_index >> 24];
	}
}
void nes_psg_wave_4(void)
{
}

void nes_psg_frame(void)
{
	nes_psg_wave_1();
	nes_psg_wave_2();
	nes_psg_wave_3();
	nes_psg_wave_4();
	
	if(!length_counter_clock--)
	{
		length_counter_clock = 4; /* FIXME: This may want to be 3 */
		
		if(!(nes_psg_c1.a & 0x20))
			if(wave_1_length_counter)
				wave_1_length_counter--;
		
		if(!(nes_psg_c2.a & 0x20))
			if(wave_2_length_counter)
				wave_2_length_counter--;
		
		if(!(nes_psg_c3.a & 0x80))
			if(wave_3_length_counter)
				wave_3_length_counter--;
	}
	
	snd_Output4Waves(nes_psg_samples_per_sync, wave_buffers[0], wave_buffers[1], wave_buffers[2], wave_buffers[3]);
}

void nes_psg_init(void)
{
	snd_Init();

	nes_psg_pulse_magic			= 0x512b3800;
	nes_psg_triangle_magic		= 0x289d9c00;
	nes_psg_noise_magic			= 0x289d9c00;
	nes_psg_samples_per_sync	= NB_SAMPLE;
	nes_psg_sample_rate			= 44100;

	snd_Open(nes_psg_samples_per_sync, nes_psg_sample_rate);
}

void nes_psg_done(void)
{
	snd_Close();
}
