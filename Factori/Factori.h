/**
 * Factori.h
 * Factori Project
 * ISP_HW_3_2020
 *
 * // TODO:
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

enum thread_status
{
    THREAD_STATUS_ERR       = ERR,
    THREAD_STATUS_CONTINUE  = OK,
    THREAD_STATUS_ABORT,
    THREAD_STATUS_TIMEOUT,
    THREAD_STATUS_SUCCESS
};

#define READ_BUFFER_LEN 35

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
