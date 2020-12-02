/**
 * Lock.h
 * Factori Project
 * ISP_HW_3_2020
 *
 * 
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

#ifndef __LOCK_H__
#define __LOCK_H__

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

enum status
{
    ERR,
    OK,
    WINAPI_ERR,
    STDLIB_ERR
};

enum lock_status
{
    LOCK_ERROR,
    LOCK_SUCCESS,
    LOCK_TIMEOUT,
};

#define MAX_WAIT_MS_R   (20 * SEC2MS)
#define MAX_WAIT_MS_W   (20 * SEC2MS)

#define MUTX_NAME "Lock Mutex"
#define SMPR_NAME "Lock Semaphore"

 /*
 ******************************************************************************
 * STRUCTURES
 ******************************************************************************
 */

struct Lock
{
    HANDLE h_read_smpr;
    HANDLE h_write_mtx;
    int smpr_cnt;
    // implement
};


/*
 ******************************************************************************
 * DECLARATIONS
 ******************************************************************************
 */

/**
 ******************************************************************************
 * @brief
 * @param
 * @param
 * @return
 ******************************************************************************
 */
struct Lock *InitializeLock();

int read_lock(struct Lock *p_lock);

int read_release(struct Lock *p_lock);

int write_lock(struct Lock *p_lock);

int write_release(struct Lock *p_lock);

int DestroyLock(struct Lock *p_lock);



#endif // __LOCK_H__
