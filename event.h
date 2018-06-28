#ifndef EVENT_H
#define EVENT_H

typedef void (*event_delay_callback_t)(void *, int);

typedef struct tagevent {
    struct tagevent *next;
    int timeleft;
    int duration;
    void (*callback)(void);
}event;

typedef struct tagevent_scheduler {
    event *top_event;
    event_delay_callback_t delay_callback;
    void *delay_callback_data;
}event_scheduler;

void event_register(event_scheduler* scheduler, event *new_event);
void event_timeslice(event_scheduler* scheduler);
event_scheduler* new_event_scheduler(event_delay_callback_t callback, void *callback_data);

#endif // EVENT_H
