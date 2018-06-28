#include "event.h"
#include <stdlib.h>
#include <stdio.h>
#include "tool.h"
#include "ui.h"


void 
event_reschedule(event_scheduler* scheduler)
{
event	*prev_event	= NULL;
event	*cur_event	= scheduler->top_event;
event	*next_event	= cur_event->next;
int		done;

	scheduler->top_event->timeleft= scheduler->top_event->duration;
	scheduler->top_event= next_event;
	done= 0;

	for(;;)
	{
		if(!next_event)
			done = 1;
		else if (cur_event->timeleft <= next_event->timeleft)
		{
			next_event->timeleft -= cur_event->timeleft;
			done = 1;
		}

		if(done)
		{
			if (prev_event)
				prev_event->next = cur_event;
			else
				scheduler->top_event = cur_event;
			
			cur_event->next = next_event;
			return;
		}

		cur_event->timeleft -= next_event->timeleft;

		prev_event = next_event;
		next_event = next_event->next;
	}
}

void 
event_register(event_scheduler* scheduler, event *new_event)
{
	new_event->next = scheduler->top_event;
	scheduler->top_event = new_event;
	event_reschedule(scheduler);
}

void 
event_timeslice(event_scheduler* scheduler)
{
	for(;;)
	{
		event_reschedule(scheduler);

		scheduler->delay_callback(scheduler->delay_callback_data, scheduler->top_event->timeleft);

		if(system_flags & F_UNIMPL)
		{
			printf("Unimplimented Instruction.\n");
			system_flags|= F_QUIT;
			return;
		}

		if(!scheduler->top_event->callback)
			return;

		scheduler->top_event->callback();
    }
}

event_scheduler* 
new_event_scheduler(event_delay_callback_t callback, void *callback_data)
{
event_scheduler* retval= new event_scheduler;

	if(!retval)
		return NULL;

	retval->delay_callback = callback;
	retval->delay_callback_data = callback_data;
	retval->top_event = NULL;

	return retval;
}
