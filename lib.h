#ifndef __LIB_ABCD_H__
#define __LIB_ABCD_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

typedef void(*runnable)(void);

typedef struct
{
    runnable run;
    size_t time;

    clock_t start;
} task;

typedef struct tln
{
    task* value;
    struct tln* prev;
} task_list_node;

typedef struct
{
    task_list_node* head;
    size_t size;
} task_list;

// Task list
task_list* create_task_list();
void free_task_list(task_list* tlist);
bool submit_task(task* t, task_list* tlist);
bool remove_task(size_t index, task_list* tlist);
task_list_node* get_node(size_t index, task_list* tlist);
task* get_task(size_t index, task_list* tlist);

// Event Queue
typedef enum
{
    NO_EVENT,
    DELETE_CURRENT_TASK,
    BREAK_OUT_OF_LOOP
} event;

#define EVENT_QUEUE_SIZE 4096
typedef struct
{
    event events[EVENT_QUEUE_SIZE];
    size_t current_size;
    size_t readptr;
    size_t writeptr;
} event_queue;

event_queue* create_event_queue();
void free_event_queue(event_queue* equeue);
bool push_event(event e, event_queue* equeue);
event pop_event(event_queue* equeue);

// Event Loop

typedef struct
{
    task_list* list;
    event_queue* events;
    clock_t start_time;
    bool running;
} event_loop;

size_t millis(clock_t start);

event_loop* create_event_loop(task_list* tlist, event_queue* equeue);
void free_event_loop(event_loop* loop);
void loop_execute(event_loop* loop);


#endif