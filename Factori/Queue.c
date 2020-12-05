/**
 * Queue.c
 * Factori Project
 * ISP_HW_3_2020
 *
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "Queue.h"

/*
 ******************************************************************************
 * FUNCTION DEFENITIONS
 ******************************************************************************
 */

struct Task* InitializeTask(int line)
{
    Task* T;
    T = malloc(sizeof(Task));
    T->next = NULL;
    T->data = line;
    return T;
}

struct Queue* InitializeQueue()
{
    Queue* q;
    q = malloc(sizeof(Queue));
    q->cnt = 0;
    q->head = NULL;
    q->tail = NULL;
    return q;
}

bool Empty(struct Queue* p_queue)
{
    return (p_queue->cnt == 0);
}

struct Task* Top(struct Queue* p_queue)
{
    return p_queue->head;
}

struct Task* Pop(struct Queue* p_queue)
{
    if (!Empty(p_queue)) {
        Task* tmp;
        tmp = p_queue->head;
        p_queue->head = p_queue->head->next;
        p_queue->cnt--;
        return tmp;
    }
    else
        return NULL;
}

void Push(struct Queue* p_queue, struct Task* p_task)
{
    if (!Empty(p_queue)) {
        p_queue->tail->next = p_task;
        p_queue->tail = p_task;
    }
    else {
        p_queue->head = p_task;
        p_queue->tail = p_task;
    }
    p_queue->cnt++;
}

void DestroyQueue(struct Queue* p_queue)
{
    Task* tmp;
    while (!Empty(p_queue)) {
        tmp = p_queue->head->next;
        free(p_queue->head);
        p_queue->head = tmp;
        p_queue->cnt--;
    }
    free(p_queue);
    p_queue = NULL;
}

void display(struct Queue* p_queue) { //for debuging only

    Task* tmp;
    tmp = p_queue->head;
    printf("Number of tasks to print: %d\n", p_queue->cnt);
    while (tmp != NULL) {
        printf("%d\n", tmp->data);
        tmp = tmp->next;
    }
    printf("DONE\n");
}