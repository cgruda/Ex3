/**
 * Queue.c
 * Factori Project
 * ISP_HW_3_2020
 *
 * this module implements a queue using a linked list
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

/*
 ==============================================================================
 * INCLUDES
 ==============================================================================
 */
#include "Queue.h"
#include "tasks.h"


/*
 ==============================================================================
 * FUNCTION DEFENITIONS
 ==============================================================================
 */

struct Task *InitializeTask(int offset)
{
    struct Task *p_task;

    p_task = calloc(1, sizeof(*p_task));
    if (!p_task)
    {
        PRINT_ERROR(E_STDLIB, 0);
        return NULL;
    }

    p_task->next = NULL;
    p_task->offset = offset;

    return p_task;;
}

//==============================================================================

struct Queue *InitializeQueue()
{
    struct Queue *p_queue;

    p_queue = calloc(1, sizeof(*p_queue));
    if (!p_queue)
    {
        PRINT_ERROR(E_STDLIB, 0);
        return NULL;
    }

    p_queue->head = NULL;
    p_queue->tail = NULL;
    p_queue->cnt  = 0;

    return p_queue;
}

//==============================================================================

struct Task *Top(struct Queue *p_queue)
{
    if (!p_queue)
    {
        return NULL;
    }

    return p_queue->head;
}

//==============================================================================

struct Task *Pop(struct Queue *p_queue)
{
    struct Task *p_task = NULL;

    if (!p_queue)
    {
        return NULL;
    }

    if (Empty(p_queue))
    {
        return NULL;
    }

    p_task = p_queue->head;
    p_queue->head = p_task->next;
    p_queue->cnt--;

    return p_task;
}

//==============================================================================

void Push(struct Queue *p_queue, struct Task *p_task)
{
    if (!p_queue || !p_task)
    {
        return NULL;
    }

    p_task->next = NULL;

    if (Empty(p_queue))
    {
        p_queue->head = p_task;
    }
    else
    {
        p_queue->tail->next = p_task;
    }

    p_queue->tail = p_task;
    p_queue->cnt++;
}

//==============================================================================

bool Empty(struct Queue *p_queue)
{
    if (!p_queue)
    {
        return true;
    }

    return (p_queue->cnt == 0);
}

//==============================================================================

int DestroyQueue(struct Queue *p_queue)
{
    struct Task *p_task;

    if (!p_task)
    {
        return ERR;
    }

    while (!Empty(p_queue))
    {
        p_task = Pop(p_queue);
        free(p_queue);
    }

    free(p_queue);
    p_queue = NULL;
}
