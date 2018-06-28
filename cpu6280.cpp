/*
 * cpu6280.c
 *
 * CAL interface and non-generated code for the cpu6280 core
 */

/* $Id: cpu6280.cpp,v 1.2 2005/01/18 00:39:03 guest Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include "cal.h"
#include "tool.h"
#include "ui.h"

#include "cpu6280int.h"

void cal_cpu6280_reset(cal_cpu* cpu);
void cal_cpu6280_run(cal_cpu* cpu);
void cal_cpu6280_runfor(cal_cpu* cpu, int cycles);
void cal_cpu6280_irq(cal_cpu* cpu, int irqno);
void cal_cpu6280_nmi(cal_cpu* cpu);
void cal_cpu6280_setzpage(cal_cpu* cpu, void *page0);
int cal_cpu6280_timeleft(cal_cpu* cpu);
void cal_cpu6280_setmmu8(cal_cpu* cpu, int shift, int mask, memread8_t *rtbl, memwrite8_t *wtbl);

void cal_cpu6280_init(cal_cpu *cpu)
{
//	cpu->data.d_cpu6280 = (struct cpu6280_context*)calloc(1, sizeof(struct cpu6280_context));
	cpu->data.d_cpu6280 = new struct cpu6280_context;

	if (!cpu->data.d_cpu6280)
	{
		printf("Insufficient memory to create CPU.\n");
		delete cpu;
		cpu = NULL;
		return;
    }
    
	cpu->reset = cal_cpu6280_reset;
	cpu->run = cal_cpu6280_run;
	cpu->runfor = cal_cpu6280_runfor;
	cpu->irq = cal_cpu6280_irq;
	cpu->nmi = cal_cpu6280_nmi;
	cpu->setzpage = cal_cpu6280_setzpage;
	cpu->timeleft = cal_cpu6280_timeleft;
	cpu->setmmu8 = cal_cpu6280_setmmu8;
	cpu->data.d_cpu6280->cpu = cpu;
}

void cal_cpu6280_reset(cal_cpu* cpu)
{
struct cpu6280_context *context= cpu->data.d_cpu6280;

	context->reg_a = 0x00;
	context->reg_x = 0x00;
	context->reg_y = 0x00;
	context->reg_s = 0xff;
	
	context->flags = 0x00; /* FIXME: wrong */
	
	context->mpr[0] = 0x00;
	context->mpr[1] = 0x00;
	context->mpr[2] = 0x00;
	context->mpr[3] = 0x00;
	context->mpr[4] = 0x00;
	context->mpr[5] = 0x00;
	context->mpr[6] = 0x00;
	context->mpr[7] = 0x00;
	
	context->pc = context->readfunc(cpu, VECTOR_RESET_LO);
	context->pc |= context->readfunc(cpu, VECTOR_RESET_HI) << 8;
}

void cpu6280_step(struct cpu6280_context *context)
{
u8 opcode= OPFETCH(context);

	cpu6280_decode[opcode](context);
}

void cal_cpu6280_run(cal_cpu* cpu)
{
struct cpu6280_context *context= cpu->data.d_cpu6280;

	while(context->cycles_left > 0)
	{
		cpu6280_step(context);

		if (system_flags & F_UNIMPL)
			return;
	}
}

void cal_cpu6280_runfor(cal_cpu* cpu, int cycles)
{
	cpu->data.d_cpu6280->cycles_left += cycles;
}

void cal_cpu6280_irq(cal_cpu* cpu, int irqno)
{
struct cpu6280_context *context= cpu->data.d_cpu6280;

	if (context->flags & FLAG_I)
		return; /* interrupts disabled */

	PUSH_BYTE(context, context->pc >> 8);
	PUSH_BYTE(context, context->pc & 0xff);
	PUSH_BYTE(context, GET_FLAGS(context) & ~FLAG_B);
	
	context->flags |= FLAG_I;
	context->flags &= ~FLAG_D;
	
	context->pc = context->readfunc(context->cpu, VECTOR_IRQ2_LO + irqno);
	context->pc |= context->readfunc(context->cpu, VECTOR_IRQ2_HI + irqno) << 8;
}

void cal_cpu6280_nmi(cal_cpu* cpu)
{
}

void cal_cpu6280_setzpage(cal_cpu* cpu, void *page0)
{
	cpu->data.d_cpu6280->zpage = (unsigned char*)page0;
}

int cal_cpu6280_timeleft(cal_cpu* cpu)
{
	return cpu->data.d_cpu6280->cycles_left;
}

void cal_cpu6280_setmmu8(cal_cpu* cpu, int shift, int mask, memread8_t *rtbl, memwrite8_t *wtbl)
{
	cpu->data.d_cpu6280->readfunc = *rtbl;
	cpu->data.d_cpu6280->writefunc = *wtbl;
}

#ifdef LAZY_FLAG_EVALUATION
u8 cpu6280_get_flags(struct cpu6280_context *context)
{
u8 retval= context->flags;

	retval &= ~(FLAG_N | FLAG_Z | FLAG_V | FLAG_C);
	retval |= context->flag_n & FLAG_N;
	retval |= (!context->flag_z) << 1;
	retval |= context->flag_c;
	retval |= context->flag_v >> 1;
	
	return retval;
}

void cpu6280_set_flags(struct cpu6280_context *context, u8 flags)
{
	context->flags = flags & ~(FLAG_N | FLAG_Z | FLAG_V | FLAG_C);
	context->flag_n = flags;
	context->flag_z = !(flags & FLAG_Z);
	context->flag_c = flags & FLAG_C;
	context->flag_v = (flags & FLAG_V) << 1;
}
#endif

void cpu6280_op_0(struct cpu6280_context *context)
{
	context->pc -= 1;
	printf("unimplemented opcode 0x%02x.\n", OPFETCH(context));
	system_flags |= F_UNIMPL;
}
