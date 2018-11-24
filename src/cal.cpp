/*
 * cal.c
 *
 * CPU Abstraction Layer implimentation.
 */

/* $Id: cal.cpp,v 1.1 2005/01/11 03:12:45 alainp Exp $ */

#include <stdlib.h>
#include <stdio.h>
#include "cal.h"
#include "ui.h"

cal_cpu* cal_create(cal_cpuinit_t initfunc)
{
//cal_cpu* retval= (cal_cpu*)calloc(1, sizeof(cal_cpu));
cal_cpu* retval= new cal_cpu;

    if(retval)
		initfunc(retval);
    else
		printf("Insufficient memory to create CPU.\n");

    return retval;
}

void cal_event_delay_callback(void *data, int cycles)
{
register cal_cpu* cpu = (cal_cpu*)data;

    cpu->runfor(cpu, cycles);
    cpu->run(cpu);
}
