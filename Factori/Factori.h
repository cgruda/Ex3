/**
 * Factori.h
 * Factori Project
 * ISP_HW_3_2020
 *
 * this is the header file of Factori module
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

#ifndef __FACTORI_H__
#define __FACTORI_H__


/*
 ==============================================================================
 * INCLUDES
 ==============================================================================
 */

#include <windows.h>
#include "tasks.h"


/*
 ==============================================================================
 * DEFINES
 ==============================================================================
 */

#define QUEUE_POP_MAX_WAIT_MS   2000


/*
 ==============================================================================
 * ENUMERATIONS
 ==============================================================================
 */

enum thread_status
{
    THREAD_STATUS_ERR       = ERR,
    THREAD_STATUS_CONTINUE  = OK,
    THREAD_STATUS_ABORT,
    THREAD_STATUS_TIMEOUT,
    THREAD_STATUS_SUCCESS
};


/*
 ==============================================================================
 * MACROS
 ==============================================================================
 */

// local macro for status checking
#define CHECK_STATUS()  if (status != THREAD_STATUS_CONTINUE) break


/*
 ==============================================================================
 * DECLARATIONS
 ==============================================================================
 */

/**
 ******************************************************************************
 * @brief factori thread function
 * @param param pointer to thread arguments struct
 * @return thread_status enum
 ******************************************************************************
 */
DWORD WINAPI factori_thread(LPVOID param);


#endif // __FACTORI_H__
