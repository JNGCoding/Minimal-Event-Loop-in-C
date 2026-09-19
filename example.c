#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "lib/event_loop.h"

event_loop* loop;

define_task(print_hello, params) { printf("print_hello()\n"); }
define_task(print_world, params) { printf("print_world()\n"); }
define_task(print_races, params) { printf("print_races()\n"); }
define_task(print_place, params) { printf("print_place()\n"); }

task t1;
task t2;
task t3;
task t4;

int main(int argc, const char* argv[])
{
    t1 = make_task("Task1", &print_hello, NULL, 1000, MID_PRIORITY);
    t2 = make_task("Task2", &print_world, NULL, 1000, TOP_PRIORITY);
    t3 = make_task("Task3", &print_races, NULL, 1000, TOP_PRIORITY);
    t4 = make_task("Task4", &print_place, NULL, 1000, BOT_PRIORITY);

    loop = create_event_loop();
    if (loop == NULL)
    {
        printf("Failed to initialize event loop, exiting....\n");
        return -1;
    }

    bool res;

    res = submit_task_to_loop(&t1, loop);
    res &= submit_task_to_loop(&t2, loop);
    res &= submit_task_to_loop(&t3, loop);
    res &= submit_task_to_loop(&t4, loop);

    if (!res)
    {
        printf("Failed to add every task, exiting....");
        return -1;
    }
    
    printf("Task Vector is ");
    print_task_list(loop->tasks);
    printf("\n");

    loop_start(loop);

    return 0;
}