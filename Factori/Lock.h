/**
 * Lock.h
 * Factori Project
 * ISP_HW_3_2020
 *
 * this is the header file of Lock module, which solves the
 * readers-writer sync problem.
 *
 * by: Chaim Gruda
 *     Nir Beiber
 */

#ifndef __LOCK_H__
#define __LOCK_H__

/*
 ==============================================================================
 * INCLUDES
 ==============================================================================
 */
#include <windows.h>
#include <stdio.h>

/*
 ==============================================================================
 * DEFINES
 ==============================================================================
 */
#define MAX_WAIT_MS_R   (20 * SEC2MS)
#define MAX_WAIT_MS_W   (20 * SEC2MS)

#define MUTX_R "Factori_Lock_Mutex_Read"
#define SMPR_G "Factori_Lock_Semaphore_Global"

 /*
 ==============================================================================
 * STRUCTURES
 ==============================================================================
 */
struct Lock
{
    HANDLE h_read_mtx;
    HANDLE h_global_smpr;
    int    readers_count;
};

/*
 ==============================================================================
 * MACROS
 ==============================================================================
 */
/**
 ******************************************************************************
 * @brief better readability for wait code handling. use in Lock module only.
 *        handle different wait codes returnd by WaitForSingleObject().
 *        in case of any result other than WAIT_OBJECT_0 we print an error
 *        message and return ERR code.
 * @param wait_code
 ******************************************************************************
 */
#define LOCK_WAIT_CODE_CHECK(wait_code)          \
{                                                \
    switch (wait_code)                           \
    {                                            \
    case WAIT_OBJECT_0:                          \
        break;                                   \
    case WAIT_FAILED:                            \
        PRINT_ERROR(E_WINAPI, E_MSG_NULL_MSG);                \
        return ERR;                              \
    case WAIT_TIMEOUT:                           \
        PRINT_ERROR(E_INTERNAL, E_MSG_MAX_WAIT); \
        return ERR;                              \
    case WAIT_ABANDONED:                         \
        return ERR;                              \
    }                                            \
}


/*
 ==============================================================================
 * FUNCTION DECLARATIONS
 ==============================================================================
 */
/**
 ******************************************************************************
 * @brief  initialize a Lock object, containing a Mutex, Semaphore, and counter
 * @return success: pointer to Lock struct, failure: NULL
 ******************************************************************************
 */
struct Lock *InitializeLock();

/**
 ******************************************************************************
 * @brief  locks for conccurent reading, not allowing write.
 * @param  p_lock pointer to lock struct
 * @return success: OK, failure: ERR
 ******************************************************************************
 */
int read_lock(struct Lock *p_lock);

/**
 ******************************************************************************
 * @brief  frees file from concurrent reading, allowing write (or new again)
 * @param  p_lock pointer to lock struct
 * @return success: OK, failure: ERR
 ******************************************************************************
 */
int read_release(struct Lock *p_lock);

/**
 ******************************************************************************
 * @brief  lock file for exclusive writing, not aloowing other write or read
 * @param  p_lock pointer to lock struct
 * @return success: OK, failure: ERR
 ******************************************************************************
 */
int write_lock(struct Lock *p_lock);

/**
 ******************************************************************************
 * @brief  frees file for write or read
 * @param  p_lock pointer to lock struct
 * @return success: OK, failure: ERR
 ******************************************************************************
 */
int write_release(struct Lock *p_lock);

/**
 ******************************************************************************
 * @brief  frees all Lock struct resources
 * @param  p_lock pointer to lock pointer, lock will be set to NULL
 * @return success: OK, failure (close handle failed): ERR
 ******************************************************************************
 */
int DestroyLock(struct Lock **p_lock);

#endif // __LOCK_H__
