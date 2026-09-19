#ifndef __DEVENT_LOOP_H__
#define __DEVENT_LOOP_H__

#include "event_queue.h"
#include "tasks.h"

#include <stdbool.h>
#include <time.h>
#include <windows.h>

typedef struct event_loop_t
{
    event_queue* events;
    task_vector* tasks;
    bool running;
    size_t current_index;
    task* current_task;
} event_loop;

event_loop* create_event_loop();
void free_event_loop(event_loop* loop);

bool submit_task_to_loop(task* t, event_loop* loop);
bool remove_task_from_loop(const char* t, event_loop* loop);

bool push_event_to_loop(event_type type, const char* refer, event_loop* loop);

size_t millis(clock_t start);

void loop_check_event(event_loop* loop);

void loop_start(event_loop* loop);

#endif