#ifndef __LIB_ABCD_H__
#define __LIB_ABCD_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

// Macro to easily define a task
// task = pointer to the task
// time = time interval
// params = parameters to pass to the function each call by pointer
// priority = task priority
#define make_task(task, time, params, priority) (struct task_t) {task, params, priority, time, 0, false}

// Use this macro to easily define a runnable task
// name = name of the function
// param = name of the void*
#define task_define(name, param) void name(void* param)

typedef void(*runnable)(void*);

// Basic enum specifying three task priorities
// priorities with lower enum numbers are higher and that with higher enum numbers are lower
typedef enum task_priority_t
{
    TOP_PRIORITY,
    MID_PRIORITY,
    BOT_PRIORITY
} task_priority;

// Task control block
// Holds important information for scheduling
typedef struct task_t
{
    runnable run;
    void* parameters;
    task_priority priority;
    size_t time;
    clock_t start;
    bool skip;
} task;

// A wrapper around task for insertion/deletion operation in task_list
typedef struct task_list_node_t
{
    task* value;
    struct task_list_node_t* prev;
} task_list_node;

// Priority sorted singly linked list to store all list
typedef struct task_list_t
{
    task_list_node* head;
    size_t size;
} task_list;

// Task list

/**
 * @brief Initializes a new task list on the heap and returns it
 * @returns an empty instance of task_list by pointer, if failed returns NULL
 */
task_list* create_task_list();

/**
 * @brief Deallocates a task list allocated on the heap free-ing all its nodes, the value that each node hold may individually be freed by the programmer
 */
void free_task_list(task_list* tlist);

/**
 * @brief Appends a task to the task list
 * @return true if operation was successful else false
 */
bool submit_task(task* t, task_list* tlist);

/**
 * @brief Inserts a task to the task list at the specified index
 * @return true if operation was successful else false
 * @warning This function is used in the inner workings of the library, don't use it as it might break the priority system
 */
bool insert_task(task* t, size_t index, task_list* tlist);

/**
 * @brief Removes a task from the task list at a provided `index`
 * @return true if operation was successful else false
 */
bool remove_task(size_t index, task_list* tlist);

/**
 * @brief Retrieves a node at `index` and returns it
 * @return returns the node by pointer if present else NULL
 */
task_list_node* get_node(size_t index, task_list* tlist);

/**
 * @brief Retrieves a task in the task list at `index` and returns it
 * @return returns the task by pointer if present else NULL
 */
task* get_task(size_t index, task_list* tlist);

// Event Queue

/**
 * @brief Holds all the possible events that can be passed to the event queue
 */
typedef enum event_t
{
    NO_EVENT,
    DELETE_CURRENT_TASK,
    BREAK_OUT_OF_LOOP,
    SKIP_NEXT
} event;

// Modify this macro, to increase the total events that the event_queue can hold
#define EVENT_QUEUE_SIZE 4096

/**
 * @brief Basic implementation of normal queue meant to handle events
 */
typedef struct event_queue_t
{
    event events[EVENT_QUEUE_SIZE];
    size_t current_size;
    size_t readptr;
    size_t writeptr;
} event_queue;

/**
 * @brief Initializes a new event_queue on the heap and returns it
 * @return a new empty instance of event_queue by pointer, if failed returns NULL
 */
event_queue* create_event_queue();

/**
 * @brief Deallocates an event_queue allocated on the heap
 */
void free_event_queue(event_queue* equeue);

/**
 * @brief Pushes an event onto the queue
 * @return true if the operation was successful else false
 */
bool push_event(event e, event_queue* equeue);

/**
 * @brief pops an event out of the queue
 * @return event if an event was present else NO_EVENT 
 */
event pop_event(event_queue* equeue);

// Event Loop

/**
 * @brief basic data structure to maintain an event loop
 */
typedef struct event_loop_t
{
    task_list* list;
    event_queue* events;
    bool running;
} event_loop;

/**
 * @brief calculates and returns the total milli-seconds passed from a start point
 * @return the milli-seconds passed as size_t
 */
size_t millis(clock_t start);

/**
 * @brief Allocates an event_loop with the task list and event queue provided on the heap and returns it
 * @return an instance of event_loop by pointer, if failed returns NULL
 */
event_loop* create_event_loop(task_list* tlist, event_queue* equeue);

/**
 * @brief Deallocates an event_loop allocated on the heap
 */
void free_event_loop(event_loop* loop);

/**
 * @brief Executes the loop. It is blocking so any code after this function
 * will not be executed unless explicitly broke out of using `BREAK_OUT_OF_LOOP` event
 */
void loop_execute(event_loop* loop);

#endif