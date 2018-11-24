/*
 * cpu6502.c
 *
 * CAL interface and non-generated code for the cpu6502 core
 */

/* $Id: cpu6502.cpp,v 1.2 2005/01/15 20:32:00 guest Exp $ */

#include "cpu6502.h"

#include <stdlib.h>
#include <stdio.h>
#include "cal.h"
#include "tool.h"
#include "ui.h"

#include "cpu6502int.h"

struct cpu6502_context *
cpu6502_create(void)
{
//	return (cpu6502_context *)calloc(1, sizeof(struct cpu6502_context));
	return new struct cpu6502_context;
}

void 
cpu6502_setuserdata(struct cpu6502_context *context, void *userdata)
{
	context->cpu = (cal_cpu*)userdata;
}

void 
cpu6502_setmemhandlers(struct cpu6502_context *context, memread8_t readfunc, memwrite8_t writefunc)
{
	context->readfunc = readfunc;
	context->writefunc = writefunc;
}

void 
cpu6502_setzpage(struct cpu6502_context *context, void *page0)
{
	context->zpage = (unsigned char*)page0;
}

void 
cpu6502_reset(struct cpu6502_context *context)
{
	context->reg_a = 0x00;
	context->reg_x = 0x00;
	context->reg_y = 0x00;
	context->reg_s = 0xff;
	
	SET_FLAGS(context, 0x00); /* FIXME: wrong */
	
	context->pc = context->readfunc(context->cpu, VECTOR_RESET_LO);
	context->pc |= context->readfunc(context->cpu, VECTOR_RESET_HI) << 8;
}

void 
cpu6502_step(struct cpu6502_context *context)
{
u8 opcode= OPFETCH(context);

	cpu6502_decode[opcode](context);
}

void 
cpu6502_run(struct cpu6502_context *context)
{
	while(context->cycles_left > 0)
	{
		cpu6502_step(context);

		if (system_flags & F_UNIMPL)
			return;
	}
}

void 
cpu6502_runfor(struct cpu6502_context *context, int cycles)
{
	context->cycles_left += cycles;
}

void 
cpu6502_event_delay_callback(void *context, int cycles)
{
	cpu6502_runfor((struct cpu6502_context *)context, cycles);
	cpu6502_run((struct cpu6502_context *)context);
}

void 
cpu6502_irq(struct cpu6502_context *context)
{
	if (context->flags & FLAG_I)
		return; /* interrupts disabled */

	PUSH_BYTE(context, context->pc >> 8);
	PUSH_BYTE(context, context->pc & 0xff);
	PUSH_BYTE(context, (GET_FLAGS(context)) & ~FLAG_B);
	
	context->flags |= FLAG_I;
	context->flags &= ~FLAG_D;
	
	context->pc = context->readfunc(context->cpu, VECTOR_IRQ_LO);
	context->pc |= context->readfunc(context->cpu, VECTOR_IRQ_HI) << 8;
	
	context->cycles_left -= 7;
}

void 
cpu6502_nmi(struct cpu6502_context *context)
{
	PUSH_BYTE(context, context->pc >> 8);
	PUSH_BYTE(context, context->pc & 0xff);
	PUSH_BYTE(context, (GET_FLAGS(context)) & ~FLAG_B);
	
	context->pc = context->readfunc(context->cpu, VECTOR_NMI_LO);
	context->pc |= context->readfunc(context->cpu, VECTOR_NMI_HI) << 8;
	
	context->cycles_left -= 7;
}

#ifdef LAZY_FLAG_EVALUATION
u8 
cpu6502_get_flags(struct cpu6502_context *context)
{
u8 retval= context->flags;

	retval &= ~(FLAG_N | FLAG_Z | FLAG_V | FLAG_C);
	retval |= context->flag_n & FLAG_N;
	retval |= (!context->flag_z) << 1;
	retval |= context->flag_c;
	retval |= context->flag_v >> 1;
	
	return retval;
}

void cpu6502_set_flags(struct cpu6502_context *context, u8 flags)
{
	context->flags = flags & ~(FLAG_N | FLAG_Z | FLAG_V | FLAG_C);
	context->flag_n = flags;
	context->flag_z = !(flags & FLAG_Z);
	context->flag_c = flags & FLAG_C;
	context->flag_v = (flags & FLAG_V) << 1;
}
#endif

void cpu6502_op_0(struct cpu6502_context *context)
{
	context->pc -= 1;
	printf("unimplemented opcode 0x%02x.\n", OPFETCH(context));
	system_flags |= F_UNIMPL;
}

/* CAL interface code */

void 	cal_cpu6502_reset	(cal_cpu* cpu);
void 	cal_cpu6502_run		(cal_cpu* cpu);
void 	cal_cpu6502_runfor	(cal_cpu* cpu, int cycles);
void 	cal_cpu6502_irq		(cal_cpu* cpu, int irqno);
void 	cal_cpu6502_nmi		(cal_cpu* cpu);
void 	cal_cpu6502_setzpage(cal_cpu* cpu, void *page0);
int 	cal_cpu6502_timeleft(cal_cpu* cpu);
void 	cal_cpu6502_setmmu8	(cal_cpu* cpu, int shift, int mask, memread8_t *rtbl, memwrite8_t *wtbl);

void 
cal_cpu6502_init(cal_cpu *cpu)
{
//	cpu->data.d_cpu6502= (struct cpu6502_context *)calloc(1, sizeof(struct cpu6502_context));
	cpu->data.d_cpu6502= new struct cpu6502_context;
	
	if(!cpu->data.d_cpu6502)
	{
		printf("Insufficient memory to create CPU.\n");
		delete cpu;
		cpu = NULL;
		return;
	}

	cpu->reset = cal_cpu6502_reset;
	cpu->run = cal_cpu6502_run;
	cpu->runfor = cal_cpu6502_runfor;
	cpu->irq = cal_cpu6502_irq;
	cpu->nmi = cal_cpu6502_nmi;
	cpu->setzpage = cal_cpu6502_setzpage;
	cpu->timeleft = cal_cpu6502_timeleft;
	cpu->setmmu8 = cal_cpu6502_setmmu8;
	cpu->data.d_cpu6502->cpu = cpu;
}

void 
cal_cpu6502_reset(cal_cpu* cpu)
{
	cpu6502_reset(cpu->data.d_cpu6502);
}

void 
cal_cpu6502_run(cal_cpu* cpu)
{
	cpu6502_run(cpu->data.d_cpu6502);
}

void 
cal_cpu6502_runfor(cal_cpu* cpu, int cycles)
{
	cpu6502_runfor(cpu->data.d_cpu6502, cycles);
}

void 
cal_cpu6502_irq(cal_cpu* cpu, int irqno)
{
	cpu6502_irq(cpu->data.d_cpu6502);
}

void 
cal_cpu6502_nmi(cal_cpu* cpu)
{
	cpu6502_nmi(cpu->data.d_cpu6502);
}

void 
cal_cpu6502_setzpage(cal_cpu* cpu, void *page0)
{
	cpu6502_setzpage(cpu->data.d_cpu6502, page0);
}

int 
cal_cpu6502_timeleft(cal_cpu* cpu)
{
	return cpu->data.d_cpu6502->cycles_left;
}

void 
cal_cpu6502_setmmu8(cal_cpu* cpu, int shift, int mask, memread8_t *rtbl, memwrite8_t *wtbl)
{
	cpu6502_setmemhandlers(cpu->data.d_cpu6502, *rtbl, *wtbl);
}
