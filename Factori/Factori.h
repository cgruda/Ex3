/**
 * Factori.h
 * Factori Project
 * ISP_HW_3_2020
 *
 *
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

#ifndef __FACTORI_H__
#define __FACTORI_H__

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

enum thread_status
{
    THREAD_STATUS_CONTINUE,
    THREAD_STATUS_ABORT,
    THREAD_STATUS_ERR,
    THREAD_STATUS_TIMEOUT,
    THREAD_STATUS_SUCCESS
};

#define READ_BUFFER_LEN 35

/*
 ******************************************************************************
 * STRUCTURES
 ******************************************************************************
 */



/*
 ******************************************************************************
 * DECLARATIONS
 ******************************************************************************
 */


DWORD WINAPI factori_thread(LPVOID param);



#endif // __FACTORI_H__
