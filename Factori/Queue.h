/**
 * Queue.h
 * Factori Project
 * ISP_HW_3_2020
 *
 * 
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */


 /*
 ******************************************************************************
 * STRUCTURES
 ******************************************************************************
 */

struct Task
{
    struct Task* next;
    int data;
};
typedef struct Task Task;

struct Queue
{
    struct Task* head;
    struct Task* tail;
    int cnt;
};
typedef struct Queue Queue;


/*
 ******************************************************************************
 * DECLARATIONS
 ******************************************************************************
 */

struct Queue *InitializeQueue();

struct Task *Top(struct Queue *p_queue);

struct Task *Pop(struct Queue *p_queue);

void Push(struct Queue *p_queue, struct Task *p_task);

bool Empty(struct Queue *p_queue);

void DestroyQueue(struct Queue *p_queue);

/**
 ******************************************************************************
 * @brief
 * @param
 * @param
 * @return
 ******************************************************************************
 */


#endif // __QUEUE_H__
