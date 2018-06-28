/*
 * cal.h
 *
 * CPU Abstraction Layer interface.
 */

/* $Id: cal.h,v 1.1 2005/01/11 03:12:45 alainp Exp $ */

#ifndef __CAL_H__
#define __CAL_H__

#include "memtypes.h"



typedef struct tagcal_cpu {
	void *userdata;
	void (*reset)(struct tagcal_cpu*);
	void (*run)(struct tagcal_cpu*);
	void (*runfor)(struct tagcal_cpu*, int);
	void (*irq)(struct tagcal_cpu*, int);
	void (*nmi)(struct tagcal_cpu*);
	void (*setzpage)(struct tagcal_cpu*, void *);
	int (*timeleft)(struct tagcal_cpu*);
	void (*setmmu0)(struct tagcal_cpu*, ranged_mmu *);
	void (*setmmu8)(struct tagcal_cpu*, int, int, memread8_t *, memwrite8_t *);
	void (*setmmu16)(struct tagcal_cpu*, int, int, memread16_t *, memwrite16_t *);
	void (*setmmu32)(struct tagcal_cpu*, int, int, memread32_t *, memwrite32_t *);
	void (*setiou)(struct tagcal_cpu*, memread8_t, memwrite8_t);

	union {
		struct M6502 *d_marat6502;
		struct emu6502_context *d_emu6502;
		struct emu6502_context *d_emu6502a;
		struct emu6502_context *d_emu65c02;
		struct emu6280_context *d_emu6280;
		struct Z80 *d_maratz80;
		struct emuz80_context *d_emuz80;
		struct emu68k_context *d_junk68k;
		struct M6280 *d_marat6280;
		struct cpu6280_context *d_cpu6280;
		struct cpu6502_context *d_cpu6502;
	} data;
}cal_cpu;


void cal_maratz80_init(cal_cpu *cpu);
void cal_junk68k_init(cal_cpu *cpu);
void cal_torr68k_init(cal_cpu *cpu);
void cal_cpu6280_init(cal_cpu *cpu);
void cal_cpu6502_init(cal_cpu *cpu);

#define CPUT_MARATZ80  cal_maratz80_init
#define CPUT_JUNK68K   cal_junk68k_init
#define CPUT_TORR68K   cal_torr68k_init
#define CPUT_CPU6280   cal_cpu6280_init
#define CPUT_CPU6502   cal_cpu6502_init

void cal_event_delay_callback(void *, int);

typedef void (*cal_cpuinit_t)(cal_cpu *cpu);
typedef void (*cpureset_t)(cal_cpu*);
typedef void (*cpurun_t)(cal_cpu*);
typedef void (*cpurunfor_t)(cal_cpu*, int);
typedef void (*cpuirq_t)(cal_cpu*, int);
typedef void (*cpunmi_t)(cal_cpu*);
typedef void (*cpusetzpage_t)(cal_cpu*, void *);
typedef int (*cputimeleft_t)(cal_cpu*);
typedef void (*cpusetmmu0_t)(cal_cpu*, ranged_mmu *);
typedef void (*cpusetmmu8_t)(cal_cpu*, int, int, memread8_t *, memwrite8_t *);
typedef void (*cpusetmmu16_t)(cal_cpu*, int, int, memread16_t *, memwrite16_t *);
typedef void (*cpusetmmu32_t)(cal_cpu*, int, int, memread32_t *, memwrite32_t *);
typedef void (*cpusetiou_t)(cal_cpu*, memread8_t, memwrite8_t);

cal_cpu* cal_create(cal_cpuinit_t initfunc);

#endif /* __CAL_H__ */
