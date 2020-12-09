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
#include <stdbool.h>

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
/**
 ******************************************************************************
 * @brief init a task struct
 * @return pointer to task struct
 ******************************************************************************
 */
struct Task *InitializeTask(int offset);

/**
 ******************************************************************************
 * @brief init a queue struct
 * @return pointer to queue struct
 ******************************************************************************
 */
struct Queue *InitializeQueue();

/**
 ******************************************************************************
 * @brief peek into queue
 * @param p_queue pointer to queue
 * @return pointer to head
 ******************************************************************************
 */
struct Task *Top(struct Queue *p_queue);

/**
 ******************************************************************************
 * @brief pop element from queue
 * @param p_queue pointer to queue
 * @return pointer to popped element (the head)
 ******************************************************************************
 */
struct Task *Pop(struct Queue *p_queue);

/**
 ******************************************************************************
 * @brief push an element into the queue
 * @param p_queue pointer to queue
 * @param p_task pointer to task to be pushed into queue
 * @return OK or ERR
 ******************************************************************************
 */
int Push(struct Queue *p_queue, struct Task *p_task);

/**
 ******************************************************************************
 * @brief check if queue is empty
 * @param p_queue pointer to queue
 * @return true or false
 ******************************************************************************
 */
bool Empty(struct Queue *p_queue);

/**
 ******************************************************************************
 * @brief destroy queue struct and all elements in it
 * @param p_queue pointer to queue pointer, queue will be set to NULL
 * @return OK or ERR
 ******************************************************************************
 */
int DestroyQueue(struct Queue **p_queue);

#endif // __QUEUE_H__
