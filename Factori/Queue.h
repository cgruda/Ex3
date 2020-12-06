/**
 * Queue.h
 * Factori Project
 * ISP_HW_3_2020
 *
 * this is the header file of the Queue module
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__


/*
 ==============================================================================
 * INCLUDES
 ==============================================================================
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <Windows.h>

/*
 ==============================================================================
 * DEFINES
 ==============================================================================
 */
#define QUEUE_POP_MAX_WAIT_MS   2000

 /*
 ==============================================================================
 * STRUCTURES
 ==============================================================================
 */

struct Task
{
    struct Task *next;
    int offset;
};

struct Queue
{
    struct Task *head;
    struct Task *tail;
    int cnt;
};


/*
 ==============================================================================
 * DECLARATIONS
 ==============================================================================
 */

struct Queue *InitializeQueue();

struct Task *Top(struct Queue *p_queue);

struct Task *Pop(struct Queue *p_queue);

void Push(struct Queue *p_queue, struct Task *p_task);

bool Empty(struct Queue *p_queue);

int DestroyQueue(struct Queue *p_queue);

/**
 ******************************************************************************
 * @brief
 * @param
 * @param
 * @return
 ******************************************************************************
 */


#endif // __QUEUE_H__
