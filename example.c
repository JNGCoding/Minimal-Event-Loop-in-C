#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "lib.h"

// Global variables
task_list* tasks;
event_queue* events;
event_loop* loop;

// Some tasks
define_task(print_goodbye, param) {
    printf("print_goodbye() - goodbye, world\n");
    push_event(make_event(DELETE_CURRENT_TASK, NULL), events);
}

define_task(print_heaven, param) {
    printf("print_heaven() - world is heaven\n");
    push_event(make_event(DELETE_CURRENT_TASK, NULL), events);
}

define_task(print_chaos, param) {
    printf("print_chaos() - chaos everywhere\n");
    push_event(make_event(DELETE_CURRENT_TASK, NULL), events);
}

define_task(print_peace, param) {
    printf("print_peace() - peace restored\n");
    push_event(make_event(DELETE_CURRENT_TASK, NULL), events);
}

// Creating tasks, (func, timer, parameters, priority)
// higher priority tasks will be checked and launched before lower priority ones
// same timers have been given to demostrate that
task task1 = make_task("task1", &print_goodbye, 1000, NULL, TOP_PRIORITY);
task task2 = make_task("task2", &print_heaven, 1000, NULL, TOP_PRIORITY);
task task3 = make_task("task3", &print_chaos, 1000, NULL, BOT_PRIORITY);
task task4 = make_task("task4", &print_peace, 1000, NULL, BOT_PRIORITY);

int main(int argc, const char* argv[])
{
    // Initialize
    tasks = create_task_list();
    assert(tasks != NULL && "tasks is null");

    events = create_event_queue();
    assert(events != NULL && "events is null");

    loop = create_event_loop(tasks, events);
    assert(loop != NULL && "loop is null");

    // Create the tasks
    submit_task(&task1, tasks);
    submit_task(&task2, tasks);
    submit_task(&task3, tasks);
    submit_task(&task4, tasks);

    print_task_list(tasks);
    printf("\n");

    // Execute
    loop_execute(loop);

    // Get out of the loop
    printf("Got out of the loop\n");

    // Also frees the event_queue and task_list associated
    // with the event_loop
    free_event_loop(loop);

    return 0;
}