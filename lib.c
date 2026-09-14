#include "lib.h"

// Implementation of Task List

task_list* create_task_list()
{
    task_list* tlist = (task_list*) malloc(sizeof(task_list));
    if (tlist == NULL)
        return NULL;

    tlist->head = NULL;
    tlist->size = 0;

    return tlist;
}

void free_task_list(task_list* tlist)
{
    if (tlist != NULL)
    {
        while (tlist->size == 0)
            remove_task(0, tlist);
    
        free(tlist);
    }
}

bool submit_task(task* t, task_list* list)
{
    task_list_node* val = (task_list_node*) malloc(sizeof(task_list_node));
    if (val == NULL)
        return false;

    val->value = t;
    val->prev = NULL;

    if (list->head == NULL)
    {
        list->head = val;
        val->prev = NULL;
    }
    else
    {
        val->prev = list->head;
        list->head = val;
    }

    list->size++;

    return true;
}

bool remove_task(size_t index, task_list* tlist)
{
    if (index >= tlist->size)
        return false;
    
    if (index == tlist->size - 1)
    {
        task_list_node* node = tlist->head;
        tlist->head = tlist->head->prev;
        free(node);

        tlist->size--;

        return true;
    }

    task_list_node* next_node = get_node(index + 1, tlist);
    task_list_node* curr_node = next_node->prev;
    next_node->prev = curr_node->prev;
    free(curr_node);

    tlist->size--;

    return true;
}

task_list_node* get_node(size_t index, task_list* tlist)
{
    task_list_node* cur = tlist->head;
    for (size_t i = 0; i < (tlist->size - index - 1); i++)
        cur = cur->prev;
    return cur;
}

task* get_task(size_t index, task_list* tlist)
{
    task_list_node* cur = tlist->head;
    for (size_t i = 0; i < (tlist->size - index - 1); i++)
        cur = cur->prev;
    return cur->value;
}

// Implementation of Event queue

event_queue* create_event_queue()
{
    event_queue* queue = (event_queue*) malloc(sizeof(event_queue));
    if (queue == NULL)
        return NULL;

    queue->current_size = 0;
    queue->readptr = 0;
    queue->writeptr = 0;

    return queue;
}

void free_event_queue(event_queue* equeue)
{
    if (equeue != NULL)
        free(equeue);
}

bool push_event(event e, event_queue* equeue)
{
    if (equeue->current_size >= EVENT_QUEUE_SIZE) {
        return false;
    }

    equeue->events[equeue->writeptr] = e;
    equeue->writeptr = (equeue->writeptr + 1) % EVENT_QUEUE_SIZE;
    equeue->current_size++;

    return true;
}

event pop_event(event_queue* equeue)
{
    if (equeue->current_size <= 0)
        return NO_EVENT;

    event elem = equeue->events[equeue->readptr];
    equeue->readptr = (equeue->readptr + 1) % EVENT_QUEUE_SIZE;
    equeue->current_size--;

    return elem;
}

// Implementation of Event Loop

size_t millis(clock_t start)
{
    clock_t stop_time = clock();
    clock_t remaining_time = stop_time - start;
    return (size_t) (remaining_time * 1000 / CLOCKS_PER_SEC);
}

event_loop* create_event_loop(task_list* tlist, event_queue* equeue)
{
    event_loop* loop = (event_loop*) malloc(sizeof(event_loop));
    if (loop == NULL)
        return NULL;

    loop->list = tlist;
    loop->events = equeue;
    loop->start_time = clock();
    loop->running = false;

    return loop;
}

void free_event_loop(event_loop* loop)
{
    if (loop != NULL)
    {
        free_task_list(loop->list);
        free_event_queue(loop->events);
        free(loop);
    }
}

void loop_execute(event_loop* loop)
{
    task_list* tlist = loop->list;
    event_queue* equeue = loop->events;
    clock_t lstart_time = loop->start_time;

    loop->running = true;

    while (loop->running)
    {
        task_list_node* cur = tlist->head;
    
        for (size_t i = 0; i < tlist->size; i++)
        {
            if (millis(cur->value->start) >= cur->value->time)
            {
                cur->value->run();
                cur->value->start = clock();
            }

            event ev = pop_event(equeue);
            if (ev != NO_EVENT)
            {
                switch (ev)
                {
                    case DELETE_CURRENT_TASK: {
                        cur = cur->prev;
                        remove_task(tlist->size - i - 1, tlist);
                    } continue;

                    case BREAK_OUT_OF_LOOP: {
                        loop->running = false;
                    } break;
                }
            } else cur = cur->prev;
        }

        // Sleep for 1 milli-second to avoid busy-waiting
        Sleep(1);
    }
}