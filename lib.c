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

bool insert_task(task* t, size_t index, task_list* tlist)
{
    if (index > tlist->size)
        return false;

    task_list_node* val = (task_list_node*) malloc(sizeof(task_list_node));
    if (val == NULL)
        return false;

    val->value = t;
    val->prev = NULL;

    if (index == tlist->size || index == tlist->size - 1)
    {
        if (tlist->head == NULL)
        {
            tlist->head = val;
        }
        else
        {
            val->prev = tlist->head;
            tlist->head = val;
        }

        tlist->size++;
        return true;
    }

    task_list_node* next = get_node(index + 1, tlist);
    task_list_node* curr = next->prev;

    next->prev = val;
    val->prev = curr;

    tlist->size++;

    return true;
}

bool submit_task(task* t, task_list* tlist)
{
    // Since the event loop executes from the head of the list
    // <BOT_PRIORITY> <TOP_PRIORITY>

    if (tlist->head == NULL)
    {
        task_list_node* val = (task_list_node*) malloc(sizeof(task_list_node));
        if (val == NULL)
            return false;

        val->value = t;
        val->prev = NULL;

        tlist->head = val;
        tlist->size++;
    }
    else
    {
        task_list_node* cur = tlist->head;
        size_t insert_index = tlist->size - 1;

        // Insertion sort into the right priority index
        for (;; insert_index--)
        {
            if (t->priority <= cur->value->priority)
                break;

            if (insert_index == 0)
                break;

            cur = cur->prev;
        }

        insert_task(t, insert_index, tlist);
    }

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
    if (index >= tlist->size)
        return NULL;

    task_list_node* cur = tlist->head;
    for (size_t i = 0; i < (tlist->size - index - 1); i++)
        cur = cur->prev;
    return cur;
}

task* get_task(size_t index, task_list* tlist)
{
    if (index >= tlist->size)
        return NULL;
        
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

    loop->running = true;

    while (loop->running)
    {
        task_list_node* cur = tlist->head;
        size_t task_index = tlist->size - 1;

        while (cur != NULL)
        {
            if (millis(cur->value->start) >= cur->value->time)
            {
                if (!cur->value->skip)
                    cur->value->run(cur->value->parameters);

                cur->value->start = clock();
                if (cur->value->skip) cur->value->skip = false;
            }

            event ev = pop_event(equeue);
            if (ev != NO_EVENT)
            {
                switch (ev)
                {
                    case DELETE_CURRENT_TASK: {
                        cur = cur->prev;
                        remove_task(task_index, tlist);
                        task_index--;
                    } continue;

                    case BREAK_OUT_OF_LOOP: {
                        loop->running = false;
                    } break;

                    case SKIP_NEXT: {
                        cur->value->skip = true;
                    } break;
                }
            } else cur = cur->prev;

            task_index--;
        }
    
        // Sleep for 1 milli-second to avoid busy-waiting
        Sleep(1);
    }
}