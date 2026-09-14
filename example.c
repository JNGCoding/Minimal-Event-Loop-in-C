#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "lib.h"

// Global variables
task_list* tasks;
event_queue* equeue;
event_loop* loop;

// Some tasks
void print_hello() {
    static int i = 5;
    printf("print_hello() - hello, world\n");

    i--;
    if (i == 0)
        push_event(DELETE_CURRENT_TASK, equeue);
}

void print_hell()  {
    static int i = 10;
    printf("print_hell() - world is hell\n");

    i--;
    if (i == 0)
        push_event(BREAK_OUT_OF_LOOP, equeue);
}

task task1 = {&print_hello, 500, 0};
task task2 = {&print_hell, 1000, 0};

int main(int argc, const char* argv[])
{
    // Initialize
    tasks = create_task_list();
    assert (tasks != NULL && "tasks is null");

    equeue = create_event_queue();
    assert (equeue != NULL && "equeue is null");

    loop = create_event_loop(tasks, equeue);
    assert (loop != NULL && "loop is null");


    // Create the tasks
    submit_task(&task1, tasks);
    submit_task(&task2, tasks);

    // Execute
    loop_execute(loop);

    // Get out of the loop
    printf("Got out of the loop\n");

    // Also frees the event_queue and task_list associated
    // with the event_loop
    free_event_loop(loop);

    return 0;
}