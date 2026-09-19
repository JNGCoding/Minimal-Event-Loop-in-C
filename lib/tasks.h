#ifndef __DEVENT_TASKS_H__
#define __DEVENT_TASKS_H__

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Use this macro to easily define a runnable task
// name = name of the function
// param = name of the void*
#define define_task(name, param) void name(const void* param)

typedef void(*runnable)(const void*);

typedef enum task_priority_t
{
    TOP_PRIORITY,
    MID_PRIORITY,
    BOT_PRIORITY
} task_priority;

typedef struct task_t
{
    // Name of the task
    const char* name;

    // Function parameters
    runnable run;
    const void* parameters;

    // Priority of the task
    task_priority priority;

    // Timing variables of the task
    size_t time;
    clock_t start;

    // State variables of the task
    bool skip;
    bool pause;
} task;

task make_task(
    const char* name,
    runnable func,
    const void* param,
    size_t interval,
    task_priority priority
);

#define TASK_VECTOR_MULTIPLY_FACTOR (1.5f)
#define TASK_VECTOR_INITIAL_SIZE (32)

typedef struct task_vector_t {
    task** vector;
    size_t size, capacity;
} task_vector;

task_vector* create_task_vector();
void free_task_vector(task_vector* tvec);
bool add_task(task* t, task_vector* tvec);
bool remove_task(size_t ind, task_vector* tvec);
bool insert_task(task* t, size_t ind, task_vector* tvec);
task* get_task(size_t ind, task_vector* tvec);
void print_task_list(task_vector* tvec);
task* get_task_by_name(const char* name, task_vector* tvec);

#endif